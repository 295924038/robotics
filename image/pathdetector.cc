/*
 * pathdetector.cc
 *
 *  Created on: Aug 12, 2017
 *      Author: zhian
 */

#include "pathdetector.h"
#include "d2seamdetector.h"
#include "linemodel.h"
#include "bsplinemodel.h"
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <map>
#include <algorithm>
#include "utils.h"
#include <unistd.h>
#include "detectors.h"
#include "bench.h"
#include "linefeature.h"
#include "lineanalysis.h"
#include "findpeaks.h"
#include <unistd.h>


using namespace cv;
using namespace std;
using namespace walgo;

namespace walgo {
class ptclose
{
public:
	ptclose(int th) : _th(th) {}
	bool operator() (const Point& a, const Point& b) const
	{
		int dist = std::abs(a.x-b.x);
		if ( dist < _th ) return true;
		return false;
	}
	int _th;
};

class cmp
{
public:

    bool operator() (const Point& a, const Point& b) const
	{
		return a.x < b.x;
	}
};

int gsize(const vector<Point>& gr)
{
	cmp c;
	const Point& gmin = *std::min_element(gr.begin(), gr.end(), c);
	const Point& gmax = *std::max_element(gr.begin(), gr.end(), c);
	cout << "gsize: gmin " << gmin << " gmax "<< gmax << endl;
	return gmax.x-gmin.x;
}

Point leftEnd(const vector<Point>& gr)
{
	cmp c;
	return *std::min_element(gr.begin(), gr.end(), c);
}

Point rightEnd(const vector<Point>& gr)
{
	cmp c;
	return *std::max_element(gr.begin(), gr.end(), c);
}

bool cmpHist(const pair<int,int> &x,const pair<int,int> &y)
{
    return x.second > y.second;
}

}

walgo::PathDetector::PathDetector(int lmax,
									walgo::PathDetector::D2PointType d2Type,
									std::string name) :
									_lmax(lmax), _d2PointType(d2Type)
{
	if (!readConfig(name, _config)) {
		cout <<"WARNING! No config.4 file " << endl;
	}
	init();
        cout << "initialized "+pd_num;
        _log << "initialized "+pd_num;
}

walgo::PathDetector::PathDetector(int lmax,
                                                                D2PointType d2Type,
								const map<string, int>& config) :
								_lmax(lmax), _d2PointType(d2Type)
{
	_config = config;
	init();
        cout << "initialized "+pd_num;
        _log << "initialized "+pd_num;
}

void walgo::PathDetector::init()
{
    string path= "log";
    if(access(path.c_str(),0))
    {
        cout << "log/ is nonexistent\n";
        system("mkdir log");
    }
        int xc = 640;
	int yc = 512;
	int w = 1280;
	int h = 1024;
	_cullMethod = 0;
	getConfigEntry(_config, "CULL_METHOD", _cullMethod);
	// must have ROI entries.
	getConfigEntry(_config, "ROI_CENTER_X", xc);
	getConfigEntry(_config, "ROI_CENTER_Y", yc);
	getConfigEntry(_config, "ROI_WIDTH", w);
	getConfigEntry(_config, "ROI_HEIGHT", h);

	//转换成opencv适用的方框格式
	int x = xc - w/2;
	int y = yc -h/2;
        cout << "ROI: "<< x <<" "<< y << "  "<< w << "  "<< h << endl;
        _roi = Rect2d(x,y,w,h);
	_showImage = 0;
	_smin = 0;
	_smax = _lmax;
	getConfigEntry(_config, "SHOW_IMAGE", _showImage);
	_saveImage = 0;
	getConfigEntry(_config, "SAVE_DEBUG_IMAGE", _saveImage);
	_d2mu = Mat::zeros(1280, _lmax, CV_8U);
	_d2mv = Mat::zeros(1024, _lmax, CV_8U);
	_d2mw = Mat::zeros(1000, _lmax, CV_8U);
	_d2mu2nd = Mat::zeros(1280, _lmax, CV_8U);
	_d2mv2nd = Mat::zeros(1024, _lmax, CV_8U);
	_d2mw2nd = Mat::zeros(1000, _lmax, CV_8U);
	_nmax = 0;
	_nmin = _lmax;
	_subMin = 10;
	_subMax = 0;
	_d2vs.resize(_lmax, {});
	_cogs.resize(_lmax, {});
	_uvec.resize(_lmax, 0);
	_vvec.resize(_lmax, 0);
	_wvec.resize(_lmax, 0);
        //_subMap.resize(_lmax, 0);
	_subLMin.resize(10, 1000000);
	_subLMax.resize(10, -1);

        std::time_t now= time(0);
        std::tm *dhms= std::localtime(&now);
        pd= "log/"+to_string(dhms->tm_mon)+"_"+to_string(dhms->tm_mday)+"_"+to_string(dhms->tm_hour)+"_"+\
                to_string(dhms->tm_min)+"_"+to_string(dhms->tm_sec)+"/";
        //cout << pd << endl;
        string bb= "mkdir "+pd;
        const char *tpd = bb.c_str();
        system(tpd);
        _log.open(pd+"log.txt",ios::out);

	int minSig = 300;
	int maxSig = 330;
	getConfigEntry(_config, "SIGNATURE_MIN", minSig);
	getConfigEntry(_config, "SIGNATURE_MAX", maxSig);
	_minSignal = 0.0001*minSig;
	_maxSignal = 0.0001*maxSig;

        int imin = 100;
        int imax = 84;
        getConfigEntry(_config, "SIGNATURE_LOWER",imin);
        getConfigEntry(_config, "SIGNATURE_UPPER", imax);
        float div = 0.001*(_maxSignal-_minSignal);
        _minS= _minSignal+imin*div;
        _maxS= _minSignal+imax*div;
        _minmaxS= 666;
        _blackpics.clear();
        _weldbool= 0;
        maskRaw= Mat::zeros(Size(w,_lmax),CV_8UC1);
}


bool walgo::PathDetector::addImage(const cv::Mat& image, int l, int isub)
{
	if ( l < _nmin) _nmin = l;
	if ( l > _nmax) _nmax = l;
	if ( l > _subLMax[isub]) _subLMax[isub] = l;
	if ( l < _subLMin[isub]) _subLMin[isub] = l;
	if ( isub > _subMax) _subMax = isub;
	if ( isub < _subMin) _subMin = isub;
        Mat img;
        if(_roi.x+_roi.width <= image.cols && _roi.y+_roi.height <= image.rows) img = image(_roi);
        else
        {
            {
                _log << "wrong image size!!! " << l << endl;
                cout << "wrong image size!!! " << l << endl;
                _log.close();
                return false;
            }
        }
	vector<Vec4i> lines;
	vector<TopoPoint> seam;
	vector<D2> d2v;
	vector<float> cog;
        findD2Seam(img, cog, d2v, lines, seam, _config,l);

        if(lines.empty())
        {
            _blackpics.push_back(l);
            _log << "black rand !!! " << l << endl;
            cout << "black rand !!! " << l << endl;
        }

        else if ( _showImage && l >= _smin && l <= _smax)
	{
		Mat cimage;
		cvtColor(image, cimage, CV_GRAY2BGR);
		for ( int i = 0; i < seam.size(); i++) {
			cout << "Detected Seam at: " << seam[i]._point.x << " " << seam[i]._point.y << endl;
		}
		int xoff = _roi.x;
		int yoff = _roi.y;
		//加上线特征标注用蓝色
		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];
			line( cimage, Point(l[0]+xoff, l[1]+yoff), Point(l[2]+xoff, l[3]+yoff), Scalar(255,0,0), 1, CV_AA);
		}
		for ( int i = 0; i < seam.size(); i++)
			circle(cimage, seam[i]._point+Point2f(xoff,yoff), 10, Scalar(0,255,0));
		namedWindow("SeamImage", WINDOW_AUTOSIZE);
		//显示蓝色方框，因为颜色格式是BGR,所以第一个数255代表蓝色
		rectangle(cimage, _roi, Scalar(255,0,0), 2,1);
		imshow("SeamImage", cimage);
		waitKey();
	}
        //imwrite(to_string(l)+"m.jpg",img);
        float minSig = _minS;
        float maxSig = _maxS;
        int n= d2v.size();
        for (int i= 0;i < n; i++)
        {
                if ( d2v[i]._d2 > maxSig || d2v[i]._d2 < minSig ) d2v[i]._d2= -10;
        }

	_d2vs[l] = d2v;
	_cogs[l] = cog;

        if (_saveImage == 2)
        {
            fstream cd;
            cd.open(pd+to_string(l)+"cd.txt",ios::out);
            for(auto c:cog) cd  << c << " ";
            cd << "$$$$$$$$" << endl;
            for(auto d:d2v) cd << d._i << " " << d._d2 << endl;
            cd.close();
        }

	if ( seam.size() < 2) {
		cout << "findD2Seam did not return two points!" << endl;
		return false;
	}
	if ( _d2PointType == D2POINT_2)
	{
		_d2mu.at<uchar>(round(seam[1]._point.x), l) = 255;
		_d2mv.at<uchar>(round(seam[1]._point.y), l) = 255;
		_uvec[l] = seam[1]._point.x;
		_vvec[l] = seam[1]._point.y;
		_wvec[l] = seam[1]._signal;
	}
	else if ( _d2PointType == D2POINT_1)
	{
		_d2mu.at<uchar>(round(seam[0]._point.x), l) = 255;
		_d2mv.at<uchar>(round(seam[0]._point.y), l) = 255;
		_uvec[l] = seam[0]._point.x;
		_vvec[l] = seam[0]._point.y;
		_wvec[l] = seam[0]._signal;
	}
	else if ( _d2PointType == D2POINT_MID )
	{
		_d2mu.at<uchar>(round((seam[0]._point.x+seam[1]._point.x)*0.5), l) = 255;
		_d2mv.at<uchar>(round((seam[0]._point.y+seam[1]._point.y)*0.5), l) = 255;
		_uvec[l] = (seam[0]._point.x+seam[1]._point.x)*0.5;
		_vvec[l] = (seam[0]._point.y+seam[1]._point.y)*0.5;
		_wvec[l] = seam[0]._signal;
	}
	return true;
}


bool walgo::PathDetector::detect(cv::Mat& uimg,
                                 vector<float>& uvec,
                                 AbsLineModel*& model,
                                 vector<Point>& linePoints,
                                 vector<vector<Point> >& lineSegments,
                                 vector<Point2f>& endPoints)
{
	endPoints.clear();
	bench b;
	b.start();
	if ( _cullMethod == 0 )
	{
		LineFeature* linefeature = LineFeature::getLineFeature(HOUGH);
		vector<Vec4i> lines;
		int doEdgeDetCopy = _config["DO_EDGE_DETECTION"];
		// 不做edgedetection
		_config["DO_EDGE_DETECTION"] = 0;
		linefeature->extract(uimg, uimg, lines, _config);
		_config["DO_EDGE_DETECTION"] = doEdgeDetCopy;
		//找到线最集中的角度. 要将线长设够大， 避免将短线计入。
		int angleRange = 0;
		getConfigEntry(_config, "PATH_ANGLE_RANGE", angleRange);
		int binsize = _config["ANGLE_BIN_SIZE"];
		int numbins = (int) floor((360.0/(double)binsize)+0.5);
		vector<double> weights(numbins);
		vector<vector<Vec4i> > hist(numbins);
		calcAngleHistogram(lines, hist, weights, binsize);
		vector<Vec4i>  selectedLines;
		selectMaxAngles(hist, weights, numbins,
				selectedLines, angleRange/binsize);
		//用linemodel找线上的点， 到线模型距离小于 eps
		int lineModelType = 0;
		getConfigEntry(_config, "LINE_MODEL_TYPE", lineModelType);
		if ( lineModelType == 0)
			model = new LineModel(uimg, selectedLines, 2);
		else if ( lineModelType == 1 )
		{
			int nDiv = 4;
			getConfigEntry(_config, "LINE_MODEL_BSPLINE_NDIV", nDiv);
			model = new BsplineModel(uimg, selectedLines, 4, nDiv);
		}
	}
	else
	{
		int imin = 100;
		int imax = 84;
		getConfigEntry(_config, "SIGNATURE_LOWER",imin);
		getConfigEntry(_config, "SIGNATURE_UPPER", imax);
		float div = 0.001*(_maxSignal-_minSignal);
		float minSig = _minSignal + imin*div;
		float maxSig = _minSignal + imax*div;

                if(_minmaxS != 666 )
                {
                    minSig = _minS;
                    maxSig = _maxS;
                }
                _log << "minSig = " << minSig << "  maxSig = " << maxSig << endl;

		for ( int i = _nmin; i < _nmax; i++) {
                        //_log << i << "  " << _wvec[i] << endl;
			if ( _wvec[i] > maxSig || _wvec[i] < minSig ) {
				uimg.at<uchar>(round(uvec[i]), i)= 0;
			}
		}

                if (_minmaxS != 666 )
                {
                    imwrite(pd+"filteredU0.5"+to_string(_minmaxS)+"th.png", uimg);
                    vector<Point> locations;
                    cv::findNonZero(uimg, locations);
                    int medianPoint= locations.size()/2;
                    int medianU= locations[medianPoint].y;
                    vector<Point> badPts;
                    int lineMedianFilter= 150;
                    getConfigEntry(_config, "LINEMEDIANFILTER", lineMedianFilter);
                    lineMedianFilter= lineMedianFilter*_minmaxS/150; _log << "lineMedianFilter: " << lineMedianFilter << endl;
                    for(auto p:locations)
                    {
                        _log << p.y << " ";
                        if(p.y-medianU > lineMedianFilter || p.y-medianU < -lineMedianFilter) badPts.push_back(p);
                    }
                    _log << endl;
                    for(auto it:badPts)
                    {
                        _log << it.y << " ";
                        uimg.at<uchar>(it.y,it.x)= 0;
                    }
                    _log << endl;
                    //_minmaxS= 666;
                }

		int lineModelType = 0;
		int niter = 4;
		getConfigEntry(_config, "LINE_MODEL_TYPE", lineModelType);
		if ( lineModelType == 0)
                {
                    vector<Point> locations;
                    cv::findNonZero(uimg, locations);
                    cmp c;
                    const Point& lmin = *std::min_element(locations.begin(), locations.end(), c);
                    const Point& lmax = *std::max_element(locations.begin(), locations.end(), c);
                    if(locations.size() < 5 && lmax.x-lmin.x < 5) return false;
                    model = new LineModel(uimg, niter);
                }
		else if ( lineModelType == 1 )
		{
			int nDiv = 4;
			getConfigEntry(_config, "LINE_MODEL_BSPLINE_NDIV", nDiv);
			model = new BsplineModel(uimg, nDiv, niter);
		}
	}
	//lm.setUseOrigModel();
	int eps = 3;
	getConfigEntry(_config, "LINE_MODEL_EPS",eps);
        if (_minmaxS != 666 )
        {
            if (_minmaxS > 0) eps= eps/3;
            _minmaxS= 666;
        }
	model->build((float) eps);
        float c= model->getC();
        if(c < 25 || c > (_roi.width-25)) return false;
	//int xmin, xmax, ymin, ymax;
	//lm.getRange(xmin, xmax, ymin, ymax);
	std::vector<cv::Point> pts;
	model->getAllPoints(pts);
	linePoints = pts;   // return points within eps from model line
	//用opencv的partition将线分为线段，距离定义为时间距离，参见ptclose类
	int distTh = 20;   // can be smaller
	getConfigEntry(_config, "PARTITION_DIST", distTh);
	ptclose ptc(distTh);
	vector<int> labels;
	partition(pts, labels, ptc);
	map<int, vector<Point> > groups;
	for ( int i = 0; i < labels.size(); i++ ) {
		groups[labels[i]].push_back(pts[i]);
	}
	vector<vector<Point>> vgr;
	for ( auto && it : groups)
		vgr.push_back(it.second);
	// 把线段按左右排序。
	auto gcmp = [] (const vector<Point>& a, const vector<Point>& b)
	{
		return a[0].x < b[0].x;
	};
	std::sort(vgr.begin(), vgr.end(), gcmp);
        _log << "there are total " << vgr.size() << " groups " << endl;

	if ( vgr.size() == 0) return false;
	//最小长度，和最小点数
	int minEndSection = 30;
	int minEndNumPoints = 5;
	getConfigEntry(_config, "MIN_END_SECTION", minEndSection);
	getConfigEntry(_config, "MIN_END_NUM_POINTS", minEndNumPoints);
        _log << "culling segments from left" << endl;
	auto it = vgr.begin();
	while (it < vgr.end())
	{
		int g = gsize(*it);
		int s = (*it).size();
                _log << "g = " << g << " s = "<< s << endl;
		if (g<minEndSection || s < minEndNumPoints )
			it = vgr.erase(it);
		else
			break;
	}
        _log << "culling segments from right" << endl;
	it = vgr.end();
	while (it > vgr.begin())
	{
		it --;
		int g = gsize(*it);
		int s = (*it).size();
                _log << "g = " << g << " s = "<< s << endl;
		if (g<minEndSection || s < minEndNumPoints )
			it = vgr.erase(it);
		else
			break;
	}
	b.stop();
	lineSegments = vgr;  // return line segments
        _log << "end detection used: " << b.duration() << endl;
        _log << " there are " << vgr.size() << " groups left " << endl;
	if (vgr.size() == 0) return false;
	Point le = leftEnd(vgr[0]);
	cv::Point2f p;
	p.x = le.x;
	p.y = le.y;
	endPoints.push_back(p);
	Point re = rightEnd(vgr[vgr.size()-1]);
	p.x = re.x;
        p.y = re.y;
	endPoints.push_back(p);

	return true;
}

bool walgo::PathDetector::build2ndUVImages(float u0, float v0, float u1, float v1, int lmin, int lmax)
{
	float uavg = (u0+u1)*0.5;
	int range = 150;
	getConfigEntry(_config, "2ND_DETECT_RANGE", range);
	int useAvg = 0;
	getConfigEntry(_config, "2ND_DETECT_USE_AVG", useAvg);
	int umin = uavg-range;
	int umax = uavg+range;
	if ( umin < 0) umin=0;
	if ( umax > 1279) umax = 1279;
        _log << "umin = " << umin << " umax = " << umax << endl;
	if ( lmax == lmin) {
                _log << "Error less than 1 seam point detected!" << std::endl;
		return false;
	}
        float ku = (u1-u0)/(lmax-lmin);
	for ( int l = _nmin; l <= _nmax; l++ )
	{
		if ( !useAvg )
		{
			uavg = ku*(l-lmin)+u0;
			umin  = uavg-range;
			umax = uavg + range;
			if ( umin < 0) umin=0;
			if ( umax > 1279) umax = 1279;
		}
                const vector<D2>& d2v = _d2vs.at(l);
//                int s = d2v.size();
//                if ( s == 0) continue;
//                int offset = d2v[0]._i;
//                int dumin = umin-offset;
//                if ( dumin < 0) dumin = 0;
//                int dumax = umax-offset;
//                if ( dumax < 0) dumax = 0;
//                if ( dumin > (s-1)) dumin = s-1;
//                if ( dumax > (s-1)) dumax = s-1;
//                if ( dumax == dumin ) continue;
//                _log <<  " offset = " << offset << " dumin = " << dumin << " dumax = " << dumax << endl;
//                const D2& d1 = *std::min_element(d2v.begin()+dumin, d2v.begin()+dumax, d2less);
//                const D2& d2 = *std::max_element(d2v.begin()+dumin, d2v.begin()+dumax, d2less);
                D2 d1(uavg,0),d2(uavg,0);
                for(auto it:d2v)
                {
                    if(it._i > umin && it._i < umax)
                    {
                        if(it._d2 > d2._d2) d2= it;
                        else if(it._d2 < d1._d2) d1= it;
                    }
                }
                if(d2._d2 < 0.05) continue;
		const vector<float>& cog = _cogs.at(l);
		int u1 = d1._i;
		int u2 = d2._i;
                _log << " u1 = " << u1 << " u2 = " << u2 << endl;
		int um = (u1+u2+1)/2;
                _log << " cog[u1] " << cog[u1] << endl;
		int v1 = round(cog[u1]);
		int v2 = round(cog[u2]);
		int vm = (v1+v2+1)/2;
		float sRange = 1.0/(_maxSignal-_minSignal);
		if ( _d2PointType == D2POINT_2)
		{
			_d2mu2nd.at<uchar>(u2,l) = 255;
			_d2mv2nd.at<uchar>(v2,l) = 255;
			_uvec[l] = u2;
			_vvec[l] = v2;
			_wvec[l] = d2._d2;
			int iw = (d2._d2-_minSignal)*sRange;
			if ( iw < 0) iw = 0; if ( iw > 999 ) iw = 999;
			_d2mw2nd.at<uchar>(iw,l) = 255;
		}
		else if ( _d2PointType == D2POINT_1)
		{
			_d2mu2nd.at<uchar>(u1, l) = 255;
			_d2mv2nd.at<uchar>(v1, l) = 255;
			_uvec[l] = u1;
			_vvec[l] = v1;
			_wvec[l] = d1._d2;
			int iw = (d1._d2-_minSignal)*sRange;
			if ( iw < 0) iw = 0; if ( iw >999 ) iw = 999;
			_d2mw2nd.at<uchar>(iw,l) = 255;
		}
		else if ( _d2PointType == D2POINT_MID )
		{
			_d2mu2nd.at<uchar>(um, l) = 255;
			_d2mv2nd.at<uchar>(vm, l) = 255;
			_uvec[l] = um;
			_vvec[l] = vm;
			_wvec[l] = d1._d2;
			int iw = (d1._d2-_minSignal)*sRange;
			if ( iw < 0) iw = 0; if ( iw >999 ) iw = 999;
			_d2mw2nd.at<uchar>(iw,l) = 255;
		}
	}

        return true;

}

bool walgo::PathDetector::eraseSpargePts(int eraseSparge,vector<int>& L,AbsLineModel* umodel,AbsLineModel* vmodel)
{
        vector<int> sparge;
        fstream fuv;
        fuv.open(pd+"fuvinpd1.txt",ios::out);
        for(auto it : L)
        {
            double vlsum= 0;
            double vrsum= 0;
            int Ns= 10;
            int Ne= 28;
            int count= 0;
            int u= (int)(umodel->model(it)+0.50001);

            for(int i= Ns;i < Ne;i++)
            {   count++;
                vlsum= _cogs[it][u-i]+vlsum;
                vrsum= _cogs[it][u+i]+vrsum;
            }
            _log << it << " " << vlsum/count-vmodel->model(it) << " " << vrsum/count-vmodel->model(it) << " " << count << endl;// "delta_v: " << _cogs[it][umodel->model(it)] - vmodel->model(it) << endl;
            if (eraseSparge >0 && (vlsum/count-vmodel->model(it) < -eraseSparge/3 || vrsum/count-vmodel->model(it) < -eraseSparge/3))
            {
                sparge.push_back(it);
                fuv << it << " : " << vlsum/count-vmodel->model(it) << " , " << vrsum/count-vmodel->model(it) << endl;
            }

        }

        if (eraseSparge > 0)
        {
            for(auto it : sparge)
            {
                remove(L.begin(),L.end(),it);
                L.pop_back();
            }
        }
        for(auto it : L) _log << it << " "; _log << "L.size(): " << L.size() << endl;
        return true;
}

bool walgo::PathDetector::eraseSpargePts(vector<int>& L,AbsLineModel* umodel,AbsLineModel* vmodel)
{
        vector<int> sparge;
        int eraseSparge = 5;
        getConfigEntry(_config, "ERASE_SPARGE", eraseSparge);
        fstream fuv;
        fuv.open(pd+"fuvinpd2.txt",ios::out);
        for(auto it : L)
        {
            double vlsum= 0;
            double vrsum= 0;
            int Ns= 1;
            int Ne= 5;
            int count= 0;
            int u= (int)(umodel->model(it)+0.50001);

            for(int i= Ns;i < Ne;i++)
            {   count++;
                vlsum= _cogs[it][u-i]+vlsum;
                vrsum= _cogs[it][u+i]+vrsum;
            }
            _log << it << " " << vlsum/count-vmodel->model(it) << " " << vrsum/count-vmodel->model(it) << " " << count << endl;// "delta_v: " << _cogs[it][umodel->model(it)] - vmodel->model(it) << endl;
            if (eraseSparge >0 && (vlsum/count-vmodel->model(it) > eraseSparge/0.5 || vrsum/count-vmodel->model(it) > eraseSparge/0.5))
            {
                sparge.push_back(it);
                fuv << it << " : " << vlsum/count-vmodel->model(it) << " , " << vrsum/count-vmodel->model(it) << endl;
            }

        }

        if (eraseSparge > 0)
        {
            for(auto it : sparge)
            {
                remove(L.begin(),L.end(),it);
                L.pop_back();
            }
        }
        for(auto it : L) _log << it << " "; _log << "L.size(): " << L.size() << endl;
        return true;
}

bool walgo::PathDetector::eraseSpargePts(vector<int>& L)
{
        vector<int> sparge;
        int eraseSparge = 5;
        getConfigEntry(_config, "ERASE_SPARGE", eraseSparge);
        for(auto it : L)
        {
            double vlsum= 0;
            double vrsum= 0;
            int Ns= 1;
            int Ne= 50;
            int count= 0;
            int u= (int)(_uvec[it]+0.50001);

            for(int i= Ns;i < Ne;i++)
            {   count++;
                vlsum= _cogs[it][u-i]+vlsum;
                vrsum= _cogs[it][u+i]+vrsum;
            }
            _log << it << " " << vlsum/count-_cogs[it][u] << " " << vrsum/count-_cogs[it][u] << " " << count << endl;// "delta_v: " << _cogs[it][umodel->model(it)] - vmodel->model(it) << endl;
            if (eraseSparge >0 && (vlsum/count-_cogs[it][u] < eraseSparge/-35.0 || vrsum/count-_cogs[it][u] < eraseSparge/-35.0))
            {
                sparge.push_back(it);
            }

        }

        if (eraseSparge > 0)
        {
            for(auto it : sparge)
            {
                remove(L.begin(),L.end(),it);
                L.pop_back();
            }
        }
        for(auto it : L) _log << it << " "; _log << "L.size(): " << L.size() << endl;
        return true;

}

void walgo::PathDetector::findWeldPoints(const int i,AbsLineModel* umodel,AbsLineModel* vmodel,vector<int>& endSteps)
{
       cout << "weldPoits! ";
       int ucols= 25;
       //double vrsum= 0;
       int start= subParts[2*i];
       int end= subParts[2*i+1];


       std::vector<float> toposNearCog;
       std::vector<double> weldLiner,weldLinel;
       weldLiner.resize(end-start,0);
       weldLinel.resize(end-start,0);
       toposNearCog.resize(2*ucols,0);

       cv::Mat GK= cv::getGaussianKernel(2*ucols,18);

       double* p= GK.ptr<double>(0);

       for(int i= 0;i < end-start;i++)
       {
           int it= i+start;
           int u= (int)(umodel->model(it)+0.50001);
           float v= vmodel->model(it);

           for(int cols= 0;cols < ucols;cols++)
           {
               toposNearCog[cols]= (_cogs[it][u-ucols+cols]);
               toposNearCog[ucols+cols]= (_cogs[it][u+cols]);
           }
           for(int c= 0;c < ucols;c++)
           {
               weldLiner[i]+= 2*p[c]*toposNearCog[c];
               weldLinel[i]+= 2*p[ucols+c]*toposNearCog[ucols+c];
           }

       }

       std::vector<double> tempw= weldLiner;
       std::vector<double> temp;
       temp.resize(5,0);
       for(int i=2;i < end-start-2;i++)
       {
           for(int j= 0;j < 5;j++) temp[j]= tempw[i-2+j];
           sort(temp.begin(),temp.end());
           weldLiner[i]= temp[2];
       }
       weldLiner[0]= weldLiner[1]= weldLiner[2];
       weldLiner[end-start-1]= weldLiner[end-start-2]= weldLiner[end-start-3];
       for(int i= 0;i < end-start;i++) _log << weldLiner[i] << " " << tempw[i] << endl;

       tempw.clear();
       tempw= weldLinel;
       temp.resize(5,0);
       for(int i=2;i < end-start-2;i++)
       {
           for(int j= 0;j < 5;j++) temp[j]= tempw[i-2+j];
           sort(temp.begin(),temp.end());
           weldLinel[i]= temp[2];
       }
       weldLinel[0]= weldLinel[1]= weldLinel[2];
       weldLinel[end-start-1]= weldLinel[end-start-2]= weldLinel[end-start-3];
       for(int i= 0;i < end-start;i++) _log << weldLinel[i] << " " << tempw[i] << endl;
       _log << endl;

       fstream ptrtxt;
       ptrtxt.open(pd+"/ptr"+to_string(i)+".txt",ios::out);
       double ptr[end-start];
       for(int i= 0;i < end-start;i++)
       {
           int it= i+start;
           float v= vmodel->model(it);
           if(weldLiner[i] < v+2*weldPoits || weldLinel[i] < v+2*weldPoits) ptr[i]= 0;
           else ptr[i]= (weldLiner[i]+weldLinel[i])/2-v;
           ptrtxt << ptr[i] << " " << v << " " << weldLiner[i] << " " << weldLinel[i] << endl;
       }

       //for(int i= 0;i < end-start;i++) ptrtxt << ptr[i] << " " << weldLiner[i] << " " << weldLinel[i] << endl;


       vector<double> tempeaks;
       for(auto p:ptr) tempeaks.push_back(p);
       sort(tempeaks.begin(),tempeaks.end());
       double maxaver= 0;
       for(int i= 2;i < 8;i++)
       {
           ptrtxt << tempeaks[end-start-1-i] << " ";
           maxaver+= tempeaks[end-start-1-i];
       }

       maxaver/= (8-2)*4;
       ptrtxt << "maxaver: " << maxaver << endl;
       ptrtxt.close();
       for(int i= 0;i < end-start;i++)
       {
           if(ptr[i] < maxaver) ptr[i] =0;
       }
       vector<int> peaks;
       findPeaks(ptr,end-start,peaks);

       int found= 0;
       for(auto p:peaks)
       {
           _log << p << " ";
           //if(p == *peaks.begin() || p == *(peaks.end()-1))
           {
               if(p + start < endSteps[2*i] && endSteps[2*i] - (p+start) < 50)
               {
                   int s= p;
                   while(ptr[s] > 2.72*weldPoits && s > 10) s--;
                   endSteps[2*i]= s+start;
                   found= 5;
               }
               else if(p + start > endSteps[2*i+1] && endSteps[2*i+1]-(p+start) > -50)
               {
                   int e= p;
                   while(ptr[e] > 2.72*weldPoits && e < end-start-10) e++;
                   endSteps[2*i+1]= e+start;
                   found= 9;
               }
           }
       }
       if(found !=0 ) _weldbool+= pow(2,i);
       _log << endl;
   }


bool walgo::PathDetector::detect(vector<Point2f>& endPoints,
                                                                 vector<int>& endSteps,
                                                                 std::vector<cv::Point>& uLinePts,
                                                                 std::vector<cv::Point>& vLinePts,
                                                                 std::vector<std::vector<cv::Point>>& uSegments,
                                                                 std::vector<std::vector<cv::Point>>& vSegments)
{
        if ( _showImage )
        {
                string d2mustring = string("D2 U vs T");
                string d2mvstring = string("D2 V vs T");

                namedWindow(d2mustring, WINDOW_NORMAL);
                imshow(d2mustring, _d2mu);
                namedWindow(d2mvstring, WINDOW_NORMAL);
                imshow(d2mvstring, _d2mv);
                waitKey(0);
        }
        int lmin = _nmin;
        int lmax = _nmax;
        _log <<"**** minSignal = " << _minSignal << " maxSignal = " << _maxSignal << endl;

        float sRange = 1000.0/(_maxSignal - _minSignal);
        for ( int nl = _nmin; nl < _nmax; nl++)
        {
                float wval = _wvec[nl];
                int iw = round((wval-_minSignal)*sRange);
                if ( iw < 0) iw =  0;
                if ( iw >= 1000) iw  = 999;
                _d2mw.at<uchar>(iw, nl) = 255;
        }
        endPoints.clear();
        vector<Point2f> upoints, vpoints;
        AbsLineModel* umodel = nullptr;
        AbsLineModel* vmodel = nullptr;

        if ( _saveImage ) {
                imwrite("d2mu.png", _d2mu);
                imwrite("d2mv.png", _d2mv);
                imwrite("d2mw.png", _d2mw);
        }
        Mat filteredU,filteredV;
        _d2mu.copyTo(filteredU);
        _d2mv.copyTo(filteredV);
        bool usuccess = detect(filteredU, _uvec, umodel, uLinePts, uSegments,  upoints);
        if ( ! usuccess ) {
                if ( umodel) {
                        delete umodel;
                        umodel = nullptr;
                }
                return false;
        }
        bool vsuccess = detect(filteredV, _vvec, vmodel, vLinePts, vSegments, vpoints);
        if ( ! usuccess ) {
                if ( umodel) {
                        delete umodel;
                        umodel = nullptr;
                }
                return false;
        }
        // we can tolerate vsuccess false
        if ( _saveImage)
        {
                imwrite("filteredU.png", filteredU);
                imwrite("filteredV.png", filteredV);
        }
        lmin = round(upoints[0].x);
        float u0 = umodel->model(lmin);
        float v0 = vmodel->model(lmin);
        lmax = round(upoints[1].x);
        float u1 = umodel->model(lmax);
        float v1 = vmodel->model(lmax);
        int do2ndDetect = 1;
        getConfigEntry(_config, "DO_2ND_DETECT", do2ndDetect);

        if ( do2ndDetect ){
                build2ndUVImages(u0, v0, u1, v1, lmin, lmax);
                if ( _saveImage ) {
                        imwrite("d2mu2nd.png", _d2mu2nd);
                        imwrite("d2mv2nd.png", _d2mv2nd);
                        imwrite("d2mw2nd.png", _d2mw2nd);
                }
                if (umodel) {delete umodel; umodel = nullptr;}
                if (vmodel) {delete vmodel; vmodel = nullptr;}
                upoints.clear();
                vpoints.clear();
                Mat filteredU, filteredV;
                _d2mu2nd.copyTo(filteredU);
                _d2mv2nd.copyTo(filteredV);
                usuccess = detect(filteredU, _uvec, umodel, uLinePts, uSegments, upoints);
                if ( ! usuccess )   {
                        if ( umodel ) {delete umodel; umodel = nullptr; }
                        return false;
                }
                vsuccess = detect(filteredV, _vvec, vmodel, vLinePts, vSegments, vpoints);
                if ( _saveImage)
                {
                        imwrite("filteredU2nd.png", filteredU);
                        imwrite("filteredV2nd.png", filteredV);
                }
                lmin = round(upoints[0].x);
                u0 = umodel->model(lmin);
                float v0 = vmodel->model(lmin);
                lmax = round(upoints[1].x);
                u1 = umodel->model(lmax);
                float v1 = vmodel->model(lmax);

        }
        int numSubs = _subMax-_subMin+1;
        endSteps.resize(numSubs*2);

        for ( int i = 0; i < numSubs; i++)
        {
                endSteps[i*2] = lmax;
                endSteps[i*2+1] = lmin;
        }

        for ( int i = 0; i< uSegments.size(); i++)
        {
                const vector<Point>& seg = uSegments[i];
                int l1 = leftEnd(seg).x;
                int l2 = rightEnd(seg).x;
                int mysub = -1;
                for ( int j = _subMin; j<=_subMax; j++)
                {
                        if ( l1 >=  _subLMin[j] && l2 <= _subLMax[j] )
                        {
                                mysub = j;
                                break;
                        }
                }

                if ( mysub == -1 )
                {
                        _log << "WARNING: u segment #" << i << " does not belong to any sub seam! " << endl;
                        continue;
                }
                else
                        _log << " segment " << l1 << " "<< l2 << " belongs to subseam " << mysub << endl;
                mysub = mysub-_subMin;
                if ( l1 < endSteps[mysub*2]) endSteps[mysub*2] = l1;
                if ( l2 > endSteps[mysub*2+1]) endSteps[mysub*2+1] = l2;
        }
        endPoints.clear();
        for ( int i = 0; i < numSubs; i++)
        {
                int l1 = endSteps[i*2];
                int l2 = endSteps[i*2+1];
                float u1 = umodel->model(l1);
                float v1 = vmodel->model(l1);
                float u2 = umodel->model(l2);
                float v2 = vmodel->model(l2);
                endPoints.push_back(Point2f(u1,v1));
                endPoints.push_back(Point2f(u2,v2));
                _log << "subSeam _weldbool#" << i << " left step " << l1 << " right step " << l2 << endl;
                _log << "Left point " << u1 << "  " << v1 << endl;
                _log << "Right point " <<  u2 << "  " << v2 << endl;
        }

        if ( umodel ) { delete umodel; umodel = nullptr;}
        if ( vmodel ) { delete vmodel; vmodel = nullptr;}

        return true;
}

bool walgo::PathDetector::confirm(int m)
{
//    int mcu= 0,mcb= 255;
//    getConfigEntry(_config, "MCU", mcu);
//    getConfigEntry(_config, "MCB", mcb);
//    for(int i= 0;i < _lmax;i++)
//    {
//        uchar* p= maskRaw.ptr<uchar>(i);
//        for(int j= 0;j < 800;j++)
//        {
//            int c= 0;
//            if(_cogs[i][j] > mcu && _cogs[i][j] < mcb)
//                c= round((_cogs[i][j]-mcu)/(mcb-mcu)*255);
//            else if(_cogs[i][j] >= mcb)
//                c= 255;

//            *p= c;
//            p++;
//        }
//    }
//    imwrite("maskRawCog.png",maskRaw);


    subParts.push_back(0);
    int endORstart= 1;
    for(int i= 0;i < _lmax;i++)
    {
        auto c= _cogs[i];
        if(endORstart-c.size() == 1)
        {
            subParts.push_back(i);
            endORstart= 0;
        }
        else if(endORstart == 0 && c.size() > 10)
        {
            subParts.push_back(i);
            endORstart= 1;
        }
    }
    if(subParts.size()%2 == 1) subParts.push_back(_lmax);
    _log << "subParts: ";
    //int pics= 0;
    for(int i= 0;i < subParts.size()/2;i++)
    {
        _log << subParts[2*i] << " " << subParts[2*i+1] << " " << endl;
        pics+= subParts[2*i+1]-subParts[2*i];
    }
    _log << endl;
    if(pics != m) return false;

    if(_blackpics.size() > 0.05*pics)
    {
        _log << "too much black images !!! ";
        cout << "too much black images !!! ";
        for(auto b:_blackpics)
        {
            _log << b << " ";
            cout << b << " ";
        }
        cout << endl;
        _log.close();
        return false;
    }

    else return true;
}

bool walgo::PathDetector::detect(vector<Point2f>& endPoints,
                                 vector<int>& endSteps,
                                 std::vector<cv::Point>& uLinePts,
                                 std::vector<cv::Point>& vLinePts,
                                 std::vector<std::vector<cv::Point>>& uSegments,
                                 std::vector<std::vector<cv::Point>>& vSegments,
                                 std::vector<float>& jjmm)
{

        if ( _showImage )
	{
		string d2mustring = string("D2 U vs T");
		string d2mvstring = string("D2 V vs T");

		namedWindow(d2mustring, WINDOW_NORMAL);
		imshow(d2mustring, _d2mu);
		namedWindow(d2mvstring, WINDOW_NORMAL);
		imshow(d2mvstring, _d2mv);
		waitKey(0);
	}
	int lmin = _nmin;
	int lmax = _nmax;
        _log <<"**** minSignal = " << _minSignal << " maxSignal = " << _maxSignal << endl;

	float sRange = 1000.0/(_maxSignal - _minSignal);
	for ( int nl = _nmin; nl < _nmax; nl++)
	{
		float wval = _wvec[nl];
		int iw = round((wval-_minSignal)*sRange);
		if ( iw < 0) iw =  0;
		if ( iw >= 1000) iw  = 999;
		_d2mw.at<uchar>(iw, nl) = 255;
	}
	endPoints.clear();
	vector<Point2f> upoints, vpoints;
	AbsLineModel* umodel = nullptr;
	AbsLineModel* vmodel = nullptr;

	if ( _saveImage ) {
                imwrite(pd+"d2mu.png", _d2mu);
                imwrite(pd+"d2mv.png", _d2mv);
                imwrite(pd+"d2mw.png", _d2mw);
	}

        int pts_d2mu= 666;
        float d2PeakPortion= 0,finalPtsPortion= 0,finalPtsStd= 0;
        int ni= 0;
        getConfigEntry(_config, "NEW_ITER", ni);
        if(ni)
        {
                vector<Point> locations;
                cv::findNonZero(_d2mu, locations);
                pts_d2mu= locations.size();
                fstream fd2topos;
                fd2topos.open(pd+"fd2_0.txt",ios::out);
                vector<D2> filterd2;

                for(auto p : locations)
                {
                    for(auto d:_d2vs[p.x])
                    {
                        if(d._i == p.y)
                        {
                            fd2topos << p.x << " " << d._i << " " << d._d2 << endl;
                            D2 fd2(d._i,d._d2);
                            filterd2.push_back(fd2);
                        }
                    }
                }
                fd2topos.close();
                _log << _maxSignal << _minSignal << endl;
                int imin = 400;
                int imax = 1000;
                getConfigEntry(_config, "SIGNATURE_LOWER",imin);
                getConfigEntry(_config, "SIGNATURE_UPPER", imax);
                _log << " imin imax: " << imin << " " << imax << endl;
                float div = 0.001*(_maxSignal-_minSignal);
                float minSig = _minSignal + imin*div;
                float maxSig = _minSignal + imax*div;
                float gap= 0.00125;
                int multiple= 3;
                getConfigEntry(_config, "GAP_MULTIPLE", multiple);
                gap*= multiple;
                //_log << minSig << " " << maxSig << " " << gap << endl;
                vector<int> topFive;
                _log << " calc d2 histogram: ";
                int gapNum = (int)((maxSig-minSig)/gap);
                _log << minSig << " " << maxSig << " " << gap << "gapNum= " << gapNum << endl;
                vector<pair<int,int>> pairVec;
                pair<int,int> pairArray[gapNum];
                vector<int> histVec;
                histVec.resize(gapNum,0);
                for(auto d:filterd2)
                {
                    int cc= (int)((d._d2-minSig)/gap);
                    if(cc > 0 && cc < gapNum) histVec[cc]= histVec[cc]+1;
                }

                for(int j= 1; j < gapNum; j++)
                {
                    _log << histVec[j] << " ";
                    pairArray[j].first= j;
                    pairArray[j].second= histVec[j];
                    pairVec.push_back(pairArray[j]);
                }
                sort(pairVec.begin(),pairVec.end(),cmpHist);
                int len= 5;
                float count= 0;
                for(int i= 0; i < len; i++)
                {
                    _log << pairVec[i].first << " " << pairVec[i].second << endl;
                    topFive.push_back(pairVec[i].first);
                }
                sort(topFive.begin(),topFive.end());
                int peakPoint= topFive[(len-1)/2];
                topFive.clear();
                for(auto p:pairVec)
                {
                    if(p.first-peakPoint > -7 && p.first-peakPoint < 7 && p.second > pairVec[0].second/5)
                    {
                        topFive.push_back(p.first);
                        _log << p.first << " " << p.second << endl;
                        count+= p.second;
                    }
                }
                sort(topFive.begin(),topFive.end());
                d2PeakPortion= count/pts_d2mu;// _log << "count" << count << "pts_d2mu" << pts_d2mu << endl;
                len= topFive.size();
                int ntc= 100;
                getConfigEntry(_config, "NEW_ITER_COUNT", ntc);
                if( count > ntc)
                {
                    _log << "_minS: " << _minS << " _maxS: " << _maxS << endl;
                    _maxS= minSig+(topFive[len-1]+1)*gap;
                    _minS= minSig+topFive[0]*gap;
                    _log << "new_minS: " << _minS << " new_maxS: " << _maxS << endl;
                    _minmaxS= 150;
                }
                else _log << "min: " << minSig << " max: " << maxSig << endl;
        }

	Mat filteredU,filteredV;
	_d2mu.copyTo(filteredU);
	_d2mv.copyTo(filteredV);
	bool usuccess = detect(filteredU, _uvec, umodel, uLinePts, uSegments,  upoints);
	if ( ! usuccess ) {
		if ( umodel) {
			delete umodel;
			umodel = nullptr;
		}
		return false;
	}
	bool vsuccess = detect(filteredV, _vvec, vmodel, vLinePts, vSegments, vpoints);
        if ( ! vsuccess ) {
                if ( vmodel) {
                        delete vmodel;
                        vmodel = nullptr;
                }
                return false;
        }
	// we can tolerate vsuccess false
	if ( _saveImage)
	{
                imwrite(pd+"filteredU.png", filteredU);
                imwrite(pd+"filteredV.png", filteredV);
	}
	lmin = round(upoints[0].x);
	float u0 = umodel->model(lmin);
	float v0 = vmodel->model(lmin);
	lmax = round(upoints[1].x);
	float u1 = umodel->model(lmax);
	float v1 = vmodel->model(lmax);
	int do2ndDetect = 1;
	getConfigEntry(_config, "DO_2ND_DETECT", do2ndDetect);

	if ( do2ndDetect ){
                if(!build2ndUVImages(u0, v0, u1, v1, lmin, lmax)) return false;
		if ( _saveImage ) {
                        imwrite(pd+"d2mu2nd.png", _d2mu2nd);
                        imwrite(pd+"d2mv2nd.png", _d2mv2nd);
                        imwrite(pd+"d2mw2nd.png", _d2mw2nd);
		}
		if (umodel) {delete umodel; umodel = nullptr;}
		if (vmodel) {delete vmodel; vmodel = nullptr;}
		upoints.clear();
		vpoints.clear();
		Mat filteredU, filteredV;
		_d2mu2nd.copyTo(filteredU);
		_d2mv2nd.copyTo(filteredV);
		usuccess = detect(filteredU, _uvec, umodel, uLinePts, uSegments, upoints);
		if ( ! usuccess )   {
			if ( umodel ) {delete umodel; umodel = nullptr; }
			return false;
		}
		vsuccess = detect(filteredV, _vvec, vmodel, vLinePts, vSegments, vpoints);
                if ( ! vsuccess ) {
                        if ( vmodel) {
                                delete vmodel;
                                vmodel = nullptr;
                        }
                        return false;
                }
		if ( _saveImage)
		{
                        imwrite(pd+"filteredU2nd.png", filteredU);
                        imwrite(pd+"filteredV2nd.png", filteredV);
		}
		lmin = round(upoints[0].x);
		u0 = umodel->model(lmin);
		float v0 = vmodel->model(lmin);
		lmax = round(upoints[1].x);
		u1 = umodel->model(lmax);
		float v1 = vmodel->model(lmax);
	}

	int numSubs = _subMax-_subMin+1;
	endSteps.resize(numSubs*2);

	for ( int i = 0; i < numSubs; i++)
	{
		endSteps[i*2] = lmax;
		endSteps[i*2+1] = lmin;
	}

	for ( int i = 0; i< uSegments.size(); i++)
	{
		const vector<Point>& seg = uSegments[i];
		int l1 = leftEnd(seg).x;
		int l2 = rightEnd(seg).x;
		int mysub = -1;
		for ( int j = _subMin; j<=_subMax; j++)
		{
			if ( l1 >=  _subLMin[j] && l2 <= _subLMax[j] )
			{
				mysub = j;
				break;
			}
		}

		if ( mysub == -1 )
		{
                        _log << "WARNING: u segment #" << i << " does not belong to any sub seam! " << endl;
			continue;
		}
		else
                        _log << " segment " << l1 << " "<< l2 << " belongs to subseam " << mysub << endl;
		mysub = mysub-_subMin;
		if ( l1 < endSteps[mysub*2]) endSteps[mysub*2] = l1;
		if ( l2 > endSteps[mysub*2+1]) endSteps[mysub*2+1] = l2;
	}
	endPoints.clear();
	for ( int i = 0; i < numSubs; i++)
	{
		int l1 = endSteps[i*2];
		int l2 = endSteps[i*2+1];
		float u1 = umodel->model(l1);
		float v1 = vmodel->model(l1);
		float u2 = umodel->model(l2);
		float v2 = vmodel->model(l2);
		endPoints.push_back(Point2f(u1,v1));
		endPoints.push_back(Point2f(u2,v2));
                _log << "subSeam #" << i << " left step " << l1 << " right step " << l2 << endl;
                _log << "Left point " << u1 << "  " << v1 << endl;
                _log << "Right point " <<  u2 << "  " << v2 << endl;
	}

        vector<int> L;
        for(auto it : uSegments )
        {
            for(auto io : it) L.push_back(io.x);
        }
        sort(L.begin(),L.end());
        for(auto it : L) _log << it << " "; _log << "L.size(): " << L.size() << endl;
        eraseSpargePts(L,umodel,vmodel);
        eraseSpargePts(L);
        //eraseSpargePts(5,L,umodel,vmodel);
        fstream fd2topos;
        fd2topos.open(pd+"fd2_1.txt",ios::out);
        finalPtsPortion= L.size()/(pts_d2mu+.0000001);
        for(auto it:L)
        {
            int u= (int)(umodel->model(it)+0.50001);
            for(auto p:uLinePts)
            {
                if(p.x == it)
                {
                    finalPtsStd+= (p.y-u)*(p.y-u);
                    for(auto d:_d2vs[p.x])
                    {if(d._i == p.y) fd2topos << p.x  << " " << _cogs[it][d._i] << " " << d._i << " " << d._d2 << endl;}
                    break;
                }
            }

        }

        if(subParts.size() == 2)
        {
            vector<Point> locations;

            vector<int>::iterator iter;
            if (umodel) {delete umodel; umodel = nullptr;}
            if (vmodel) {delete vmodel; vmodel = nullptr;}
            upoints.clear();
            vpoints.clear();
            cv::Mat filteredUa= Mat::zeros(_d2mu2nd.rows,_d2mu2nd.cols,CV_8U);
            cv::Mat filteredVa= Mat::zeros(_d2mv2nd.rows,_d2mv2nd.cols,CV_8U);
            cv::findNonZero(_d2mu2nd, locations);
            for(auto p : locations)
            {
                iter= find(L.begin(),L.end(),p.x);
                if(iter != L.end()) filteredUa.at<uchar>(p.y,p.x)= 255;
            }
            usuccess = detect(filteredUa, _uvec, umodel, uLinePts, uSegments, upoints);
            if ( ! usuccess )   {
                    if ( umodel ) {delete umodel; umodel = nullptr; }
                    return false;
            }

            cv::findNonZero(_d2mv2nd, locations);
            for(auto p : locations)
            {
                iter= find(L.begin(),L.end(),p.x);
                if(iter != L.end()) filteredVa.at<uchar>(p.y,p.x)= 255;
            }
            vsuccess = detect(filteredVa, _vvec, vmodel, vLinePts, vSegments, vpoints);
            if ( ! vsuccess ) {
                    if ( vmodel) {
                            delete vmodel;
                            vmodel = nullptr;
                    }
                    return false;
            }
            eraseSpargePts(5,L,umodel,vmodel);
            cv::Point2i ustart(*L.begin(),int(umodel->model(*L.begin())));
            cv::Point2i uend(*(L.end()-1),int(umodel->model(*(L.end()-1))));
            cv::Point2i vstart(*L.begin(),int(vmodel->model(*L.begin())));
            cv::Point2i vend(*(L.end()-1),int(vmodel->model(*(L.end()-1))));
            cvtColor(filteredUa,filteredUa, CV_GRAY2BGR);
            cvtColor(filteredVa,filteredVa, CV_GRAY2BGR);
            cv::line(filteredUa,ustart,uend,cv::Scalar(0,255,0),1,8);
            cv::line(filteredVa,vstart,vend,cv::Scalar(0,255,0),1,8);
            imwrite(pd+"filteredUa.png", filteredUa);
            imwrite(pd+"filteredVa.png", filteredVa);
        }

        finalPtsStd= finalPtsStd/L.size();
        endSteps.clear();
        endSteps.push_back(*L.begin());
        for(int i= 1; i < subParts.size()/2; i++)
        {
            endSteps.push_back(*(lower_bound(L.begin(),L.end(),subParts[2*i])-1));
            endSteps.push_back(*(lower_bound(L.begin(),L.end(),subParts[2*i]))) ;
        }
        endSteps.push_back(*(L.end()-1));

        weldPoits= 1;
        getConfigEntry(_config, "WELDPOINTS", weldPoits);
        if(endSteps.size() == 2 && weldPoits != 0) findWeldPoints(0,umodel,vmodel,endSteps);

        endPoints.clear();
        for ( int i = 0; i < endSteps.size()/2; i++)
        {
        _log << "part" << i << " start/end: " << endSteps[2*i] << " , " << endSteps[2*i+1] << endl;
        //_log << " UV: " << uv_kc[0]*endSteps[2*i]+uv_kc[1] <<  " , " << uv_kc[2]*endSteps[2*i]+uv_kc[3] << " , " << uv_kc[0]*endSteps[2*i+1]+uv_kc[1] << " , " << uv_kc[2]*endSteps[2*i+1]+uv_kc[3] << endl;
        float u1 = umodel->model(endSteps[2*i]);
        float v1 = vmodel->model(endSteps[2*i]);
        float u2 = umodel->model(endSteps[2*i+1]);
        float v2 = vmodel->model(endSteps[2*i+1]);
        endPoints.push_back(Point2f(u1,v1));
        endPoints.push_back(Point2f(u2,v2));
        _log << " UV: " << u1 <<  " , " << v1 << " , " << u2 << " , " << v2 << endl;
        }

        _log << "d2PeakPortion: " << d2PeakPortion << " finalPtsPortion: " << finalPtsPortion << " finalPtsStd: " << finalPtsStd << endl;

        int seamPts= 0;
        for(int i=0;i < endSteps.size()/2;i++) seamPts+= endSteps[2*i+1]-endSteps[2*i];
        std::vector<float> jm;
        jm.push_back(d2PeakPortion*pts_d2mu/seamPts);
        jm.push_back(finalPtsPortion*pts_d2mu/seamPts);
        jm.push_back(finalPtsStd);
        for(auto j:jm) cout << "jm: " << j << endl;

        if(finalPtsStd > 0 && subParts.size() > 2 && jjmm.size() >0)
        {
            vector<Mat> filteredUs,filteredVs;
            vector<int> workPieces;
            //workPieces.push_back();
            int tempi= 0;
            for(auto i:jjmm)
            {
                Mat subd2u= Mat::zeros(_d2mu2nd.rows,_d2mu2nd.cols,CV_8U);
                Mat subd2v= Mat::zeros(_d2mv2nd.rows,_d2mv2nd.cols,CV_8U);
                filteredUs.push_back(subd2u);
                filteredVs.push_back(subd2v);
                workPieces.push_back(subParts[tempi]);
                workPieces.push_back(subParts[tempi+2*i-1]);
                tempi+= 2*i;
            }
            cout << "workPieces: ";
            for(auto w:workPieces) cout << w << " ";
            cout << endl;

            vector<Point> locations;
            cv::findNonZero(_d2mu2nd, locations);
            vector<int>::iterator iter;
            if(workPieces.size()/2 == jjmm.size())
            {
                int wpn;
                for(auto p : locations)
                {
                    for(int i= 0;i < jjmm.size();i++)
                    {
                        if(p.x >= workPieces[2*i] && p.x < workPieces[2*i+1])
                        {
                            wpn= i;
                            break;
                        }
                    }
                    iter= find(L.begin(),L.end(),p.x);
                    if(iter != L.end()) filteredUs[wpn].at<uchar>(p.y,p.x)= 255;
                    else _log << p << endl;
                }

                locations.clear();
                cv::findNonZero(_d2mv2nd, locations);
                for(auto p : locations)
                {
                    for(int i= 0;i < jjmm.size();i++)
                    {
                        if(p.x > workPieces[2*i] && p.x < workPieces[2*i+1])
                        {
                            wpn= i;
                            break;
                        }
                    }
                    iter= find(L.begin(),L.end(),p.x);
                    if(iter != L.end()) filteredVs[wpn].at<uchar>(p.y,p.x)= 255;
                    else _log << p << endl;
                }

            for(int i=1;i < jjmm.size();i++)
            {jm.push_back(0);jm.push_back(0);jm.push_back(0);}

            endSteps.clear();

            int minmax= 0;
            getConfigEntry(_config, "MINMAX", minmax);
            if(minmax == 0)
            {
                float dltS= _maxS-_minS;
                _minS-= 3.5*dltS;
                _maxS+= 3.5*dltS;
            }
            else
            {
                int imin = 100;
                int imax = 84;
                getConfigEntry(_config, "SIGNATURE_LOWER",imin);
                getConfigEntry(_config, "SIGNATURE_UPPER", imax);
                float div = 0.001*(_maxSignal-_minSignal);
                _minS= _minSignal+imin*div;
                _maxS= _minSignal+imax*div;
            }

            int allLenth= *(subParts.end()-1)-*subParts.begin();

            for(int j;j < jjmm.size();j++)
            {

                _minmaxS= 7+150*(workPieces[2*j+1]-workPieces[2*j])/allLenth;
                //_minS-= (3-_minmaxS)*dltS;
                //_maxS+= (3-_minmaxS)*dltS;
                upoints.clear();
                vpoints.clear();
                if (umodel) {delete umodel; umodel = nullptr;}
                if (vmodel) {delete vmodel; vmodel = nullptr;}
                usuccess = detect(filteredUs[j], _uvec, umodel, uLinePts, uSegments, upoints);
                if ( ! usuccess ) {
                        if ( umodel) {
                                delete umodel;
                                umodel = nullptr;
                        }
                        return false;
                }
                vsuccess = detect(filteredVs[j], _vvec, vmodel, vLinePts, vSegments, vpoints);
                if ( ! vsuccess ) {
                        if ( vmodel) {
                                delete vmodel;
                                vmodel = nullptr;
                        }
                        return false;
                }
                imwrite(pd+"filteredU2"+to_string(j)+".png", filteredUs[j]);
                imwrite(pd+"filteredV2"+to_string(j)+".png", filteredVs[j]);

                vector<int> L;
                for(auto it : uLinePts ) L.push_back(it.x);
                sort(L.begin(),L.end());
                for(auto it : L) _log << it << " "; _log << "Ll.size(): " << L.size() << endl;
                eraseSpargePts(5,L,umodel,vmodel);
                int d2gap= 2025;
                //getConfigEntry(_config,"D2_MIN_GAPLENGTH",d2gap);
                if(d2gap == 2025)
                {
                    vector<Point> Upts;
                    vector<Point> Lpts;
                    cv::findNonZero(filteredUs[j], Upts);
                    cv::findNonZero(filteredVs[j], Lpts);
                    for(auto p:Upts)
                    {
                        vector<int>::iterator result = find(L.begin(),L.end(),p.x);
                        if(result == L.end()) filteredUs[j].at<uchar>(p.y,p.x)= 0;
                    }
                    for(auto p:Lpts)
                    {
                        vector<int>::iterator result = find(L.begin(),L.end(),p.x);
                        if(result == L.end()) filteredVs[j].at<uchar>(p.y,p.x)= 0;
                    }
                    usuccess = detect(filteredUs[j], _uvec, umodel, uLinePts, uSegments, upoints);
                    if ( ! usuccess ) {
                            if ( umodel) {
                                    delete umodel;
                                    umodel = nullptr;
                            }
                            return false;
                    }
                    imwrite(pd+"filteredU2"+to_string(j)+"+.png", filteredUs[j]);
                    vsuccess = detect(filteredVs[j], _vvec, vmodel, vLinePts, vSegments, vpoints);
                    if ( ! vsuccess ) {
                            if ( vmodel) {
                                    delete vmodel;
                                    vmodel = nullptr;
                            }
                            return false;
                    }
                    imwrite(pd+"filteredV2"+to_string(j)+"+.png", filteredVs[j]);
                }
                eraseSpargePts(5,L,umodel,vmodel);
                finalPtsStd= 0;
                for(auto it:L)
                {
                    int u= (int)(umodel->model(it)+0.50001);
                    for(auto p:uLinePts)
                    {
                        if(p.x == it) finalPtsStd+= (p.y-u)*(p.y-u);
                    }
                }
                float count= 0;

                int partsCount= 0;
                for(int i= 0;i < j;i++) partsCount+= jjmm[i];
                for(int i= partsCount;i < partsCount+jjmm[j];i++) count+= subParts[2*i+1]-subParts[2*i];
                jm[3*j+1]= L.size()/count;
                jm[3*j+2]= finalPtsStd/L.size();
                jm[3*j]= jm[0];

                endSteps.push_back(*L.begin());
                for(int i= partsCount+1;i < partsCount+jjmm[j];i++)
                {
                    endSteps.push_back(*(lower_bound(L.begin(),L.end(),subParts[2*i])-1));
                    endSteps.push_back(*(lower_bound(L.begin(),L.end(),subParts[2*i]))) ;
                }
                endSteps.push_back(*(L.end()-1));

                for ( int i= partsCount; i < partsCount+jjmm[j]; i++)
                {
                    _log << "nwp part" << i << " start/end: " << endSteps[2*i] << " , " << endSteps[2*i+1] << endl;
                    if(weldPoits != 0)  findWeldPoints(i,umodel,vmodel,endSteps);


                _log << "part" << i << " start/end: " << endSteps[2*i] << " , " << endSteps[2*i+1] << endl;
                float u1 = umodel->model(endSteps[2*i]);
                float v1 = vmodel->model(endSteps[2*i]);
                float u2 = umodel->model(endSteps[2*i+1]);
                float v2 = vmodel->model(endSteps[2*i+1]);
                Point2f delta= endPoints[2*i]-(Point2f(u1,v1));
                _log << "delta " << delta << " " << delta.ddot(delta) << endl;
                delta= endPoints[2*i+1]-(Point2f(u2,v2));
                _log << "delta " << delta << " " << delta.ddot(delta) << endl;
                endPoints[2*i]= (Point2f(u1,v1));
                endPoints[2*i+1]= (Point2f(u2,v2));
                _log << " UV: " << endPoints[2*i] << " , " << endPoints[2*i+1] << endl;

                }
            }


            for(auto j:jm) _log << j << " ";

            }
        }

        for(int i= 0;i < endPoints.size();i++)
        {
            endPoints[i].x+= _roi.x;
            endPoints[i].y+= _roi.y;
        }
        int grade= 25;
        getConfigEntry(_config, "GRADE",grade);
        _log << " grade= " << grade << endl;
        jjmm.clear();
        //cout << "jjmm.size(): " << jjmm.size() << endl;

        for(int i=0;i < jm.size();i++)
        {
            int imod= i%3;
            if( imod!= 2) jjmm.push_back(100*(jm[i]*100-10*(1+imod))/(100-grade-10*(2-imod*2)));
            else jjmm.push_back((20-sqrt(jm[i]))*5);
        }
        for(auto j:jm) jjmm.push_back(j);
        _log << "jjmm.size(): " << jjmm.size() << endl;
        jjmm.push_back(_blackpics.size()/(pics+.000001));
        jjmm.push_back(_weldbool);
        int js= jm.size();
        for(int j=0;j < js;j++) _log << jjmm[j] << " " << jjmm[js+j] << endl;
        _log << "_blackpics.size()/pics: " << jjmm[jjmm.size()-2] << endl;
        fd2topos.close();

	if ( umodel ) { delete umodel; umodel = nullptr;}
	if ( vmodel ) { delete vmodel; vmodel = nullptr;}
        cout << "get out "+pd_num<< endl;
        _log  << "get out "+pd_num<< endl;
        _log.close();
	return true;
}


