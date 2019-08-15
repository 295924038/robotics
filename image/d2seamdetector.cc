/*
 * d2seamdetector.cc
 *
 *  Created on: Apr 13, 2018
 *      Author: zhian
 */
#include "d2seamdetector.h"
#include "detectors.h"
#include "utils.h"
using namespace cv;
using namespace std;
using namespace walgo;

// image is the original image
// maskRaw is the detected laster signals,
// this function construct a line by averaging along column
void CalcCOG(const Mat& image, Mat& maskRaw, vector<float>& line, int& count)
{
	int nc = image.cols;
	line.resize(nc, 0);
	vector<int> sumv(nc, 0);
	for ( int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (maskRaw.at<uchar>(i,j) > 0) {
                                //int iv = image.at<uchar>(i,j);
                                line[j] += i;
                                sumv[j] ++;
			}
		}
	}
	for ( int i = 0; i < nc; i++)
	{
		if ( sumv[i] > 0)
			line[i] = line[i]/sumv[i];
		else
			{
				line[i] = -100000;
				count+= 1;
			}
	}
}

inline int findNext(const vector<float>& line, int i)
{
	for ( int j = i+1; j < line.size(); j++ )
		if ( line[j] >= 0 )
			return j;
	return -1;     // to line end
}

inline int findLast(const vector<float>& line, int i)
{
	for ( int j = i-1; j > 0; j--)
		if ( line[j] >= 0)
			return j;
	return -1;
}

// returns true if there n gap points from i to i+n [i, i+n)
inline bool hasNGapsRight(const vector<float>& line, int i, int n)
{
    if ( i+n >= line.size()) return false;
	for ( int j = i; j < i+n; j++)
		if ( line[j] >= 0 ) return false;
	return true;
}

// returns true if there are n values from i to i+n [i, i+n)
inline bool hasNVals(const vector<float>& line, int i, int n)
{
	if ( i+n >= line.size()) return false;
	for ( int j = i; j < i+n; j++ )
		if ( line[j] < 0) return false;
	return true;
}

//  is right continuous at i
inline bool isRightCont(const vector<float>& line, int i, int n, float threshDelta)
{
	assert(n>1);
	if (i+n >= line.size()) return false;
	float sum=0;
	for ( int j = i+1; j < i+n; j++)
		sum += line[j];
	sum = sum/(n-1);
	if ( fabs(sum-line[i]) < threshDelta ) return true;
	return false;
}

inline bool isLeftCont(const vector<float>& line, int i, int n, float threshDelta)
{
	assert(n>1);
	if ( i < n-1) return false;
	float sum;
	for ( int j = i-1; j > i-n; j-- )
		sum += line[j];
	sum = sum/(n-1);
	if ( fabs(sum-line[i]) < threshDelta ) return true;
	else return false;
}

// is right discontinuous
inline bool isRightDiscont(const vector<float>& line, int i, int n, float threshDelta)
{
	if (i+n >= line.size()) return false;
	for ( int j = i+1; j < i+n; j++)
	{
		if ( fabs(line[j]-line[i]) < threshDelta ) return false;
	}
	return true;
}

// is left discontinuous
inline bool isLeftDiscont(const vector<float>& line, int i, int n, float threshDelta)
{
	if (i < n-1 )  return false;
	for ( int j = i-1; j > i-n; j--)
	{
		if ( fabs(line[j]-line[i]) < threshDelta ) return false;
	}
	return true;
}

// second derivative
inline float der2(const vector<float>& line, int i, int n)
{
	int m = (n-1)/2;
	int m2 = m/2;
	float d2 = line[i+m]+line[i-m]-2.0*line[i];//+2.0*line[i+m2]+2.0*line[i-m2]
	d2 = d2/((float)m);
	return d2;
}



void smooth(const vector<float>& line,
			vector<float>& sline,
			int l, int imin, int imax, float medianTh)
{
	int n = (l-1)/2;
	float l1 = 1.0/l;
	vector<int> v(l);
	sline = line;

	for ( int i = imin; i < imax; i++) {
		float sum = 0;
		int j = 0;
		for ( int m = i-n; m <= i+n; m++,j++ ) {
			v[j] = line[m];
		}
		std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
		float median = v[v.size()/2];
		int numVal=0;
		for ( int m = i-n; m <= i+n; m++ ) {
				if ( fabs(line[m]-median) <= medianTh) {
					sum += line[m];
					numVal++;
				}
		}
		sline[i] = 1.0/numVal*sum;
	}
}


void processGaps(const vector<float>& line,
				 vector<float>& filledLine,
				 vector<int>& gaps,
			     int minGapLength,
				 int gapDilate,
				 int imin,
				 int imax)
{
	int i = imin;
	int iprev = imin;
	filledLine.resize(line.size());
	gaps.resize(line.size());
	std::fill(gaps.begin(), gaps.end(), 0);
	for ( int j = 0; j < imin; j++)
		filledLine[j] = 0;
	for ( int j = imax+1; j < line.size(); j++)
		filledLine[j] = 0;
	filledLine[iprev] = line[iprev];
	float yprev = line[iprev];
	while ( i >= 0 )
	{
		i = findNext(line, iprev);
		if ( i < 0 ) break;
		filledLine[i] = line[i];
		// gap length
		int xlength = i - iprev;
		float ylength = std::abs(line[i]-yprev);
		float length = sqrt(xlength*xlength+ylength*ylength);
		if ( length > minGapLength)
		{
			int gapMin = iprev-gapDilate;
			if ( gapMin < imin )
				gapMin = imin;
			int gapMax = i+gapDilate;
			if ( gapMax >= imax)
				gapMax = imax;
			for (int k = gapMin; k < gapMax; k++)
				gaps[k] = -1;
		}
		if ( i > iprev+1)
		{
			float s = (line[i]-line[iprev])/(i-iprev);
			for ( int k = iprev+1; k < i; k++)
				filledLine[k] = line[iprev]+ s*(k-iprev);
		}
		iprev = i;
		yprev = line[i];
	}
}

void detectLineTopos(vector<float>& line,
           			vector<TopoPoint>& topos,
			        float threshDelta,           // threshold for delta difference
			        float threshD,               // threshold for derivative jump
			        float threshGap,             // threshold for gap length
			        float threshLine)             // threshold for line length
{
	int iprev = findNext(line, 0);
	if ( iprev < 0)
	{
		cout <<"No line topos find!" << endl;
		return;
	}
	int i = iprev;
	while (i >= 0)
	{
		i = findNext(line, iprev+1);
		if ( i < 0 )  break;
		// first for gap case
		if ( (i - iprev) > threshGap && hasNVals(line, i, threshLine))
		{
			if ( isLeftCont(line, iprev, threshLine, threshDelta) && isRightCont(line, i, threshLine, threshDelta))
			{
				Point2f p1(iprev, line[iprev]);
				Point2f p2(i, line[i]);
				TopoPoint tp1(LINE_END, p1, i-iprev, true, 0, false);
				topos.push_back(tp1);
				TopoPoint tp2(LINE_END, p2, i-iprev, false, 0, true);
				topos.push_back(tp2);
				iprev = i;
				continue;
			}
		}
		else if ( isLeftCont(line, iprev, threshLine, threshDelta) && isRightDiscont(line, iprev, threshLine, threshDelta) &&
				  isRightCont(line, i, threshLine, threshDelta) && isLeftDiscont(line, i, threshLine, threshDelta))
		{
			Point2f p1(iprev, line[iprev]);
			Point2f p2(i, line[i]);
			TopoPoint tp1(LINE_JUMP, p1, i-iprev, true, 0, false);
			topos.push_back(tp1);
			TopoPoint tp2(LINE_JUMP, p2, i-iprev, false, 0, true);
			topos.push_back(tp2);
			iprev = i;
			continue;
		}
		iprev = i;
	}
	int imin = 100;
	int imax = line.size()-imin;
	vector<D2> d2v;
	vector<float> line1=line;
	smooth(line, line1, 11, imin, imax, 3);
	line = line1;
	int d2length = 2*threshLine+1;
	for ( int i = imin; i < imax; i++) {
		if ( hasNVals(line, i-2, d2length))
		{
			float d = der2(line, i, d2length);
			D2 d2(i, d);
			d2v.push_back(d2);
		}
	}
	std::sort(d2v.begin(), d2v.end(), d2less);
	for ( auto & it : d2v ) cout << " d2 = " <<  it._d2 << endl;
	size_t s = d2v.size();
	D2& d1 = d2v[0];
	D2& d2 = d2v[s-1];
	if (fabs(d1._d2)> threshD && fabs(d2._d2) > threshD)
	{
		Point2f p1(d1._i, line[d1._i]);
		Point2f p2(d2._i, line[d2._i]);
		TopoPoint tp1(LINE_BEND, p1, 0, true, d1._d2, false);
		TopoPoint tp2(LINE_BEND, p2, 0, false, d2._d2, true);
		topos.push_back(tp1);
		topos.push_back(tp2);
	}
}

void medianFilter(const vector<float>& line, vector<float>& line1, int ml) //中值滤波，宽度为ml
{
	int n = (ml-1)/2;
	vector<float> v(ml);
	for ( int i = n+1; i < line.size()-n-1; i++)
	{
		int j = 0;
		for ( int m = i-n; m <= i+n; m++,j++ )
			v[j] = line[m];
		std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
		line1[i] = v[v.size()/2];
	}
}

void detectD2LineTopos(const vector<float>& cog,
					  vector<float>& smoothCog,
			          vector<D2>& d2v,
					  vector<TopoPoint>& topos,
					  int& xmin,
					  int& xmax,
					  int medianL,
					  int smoothL,
					  int d2L,
					  float threshD2,
					  float medianTh,
					  int minGapLength,
					  int gapDilate)
{
	d2v.clear();
	vector<float> line = cog;
	//去掉尖峰。 （seam1/291.jpg)
	medianFilter(cog, line, medianL);
	//找到第一个激光点
	int imin = findNext(line, xmin);

	// 找到最后激光点
	int imax = findLast(line, xmax);
	if ( imin < 0 || imax < 0)
	{
		cout <<"No line topos find!" << endl;
		Point2f p(0,0);
		TopoPoint tp1(LINE_BEND, p, 0, true, 0, false);
		topos.push_back(tp1);
		TopoPoint tp2(LINE_BEND, p, 0, false, 0, true);
		topos.push_back(tp2);
		return;
	}

	vector<float> filledLine;
	// 在断线处用线性插值填充
	vector<int> gaps;
	processGaps(line, filledLine, gaps, minGapLength, gapDilate, imin, imax);
	int erosion = smoothL/2;
	imin= imin + erosion;
	imax= imax - erosion;
	cout << "imin = " << imin << "  imax = " << imax << endl;
	// 用平均滤波， medianTh是和median值的差别阈值, medianTh很大就是一般平均
	// 用以排除杂光和增强跳点信号
	//vector<float> smoothLine;
	smooth(filledLine, smoothCog, smoothL, imin, imax, medianTh);
	// 计算二阶倒数
	erosion = (d2L+1)/2;
	imin += erosion;
	imax -= erosion;
	for ( int i = imin; i < imax; i++) {
		if ( gaps[i] == 0)
		{
			float d = der2(smoothCog, i, d2L);
			D2 d2(i, d);
			d2v.push_back(d2);
		}
	}
	//第一输出点是下折点， 第二是上折点
	D2& d1 = *std::min_element(d2v.begin(), d2v.end(), d2less);
	D2& d2 = *std::max_element(d2v.begin(), d2v.end(), d2less);
	Point2f p1(d1._i, smoothCog[d1._i]);
	cout << "p1: " << p1.x << "  " << p1.y << "  " << endl;
	TopoPoint tp1(LINE_BEND, p1, 0, true, d1._d2, false);
	topos.push_back(tp1);
	Point2f p2(d2._i, smoothCog[d2._i]);
	cout << "p2: " << p2.x << "  " << p2.y << "  " << endl;
	TopoPoint tp2(LINE_BEND, p2, 0, false, d2._d2, true);
	topos.push_back(tp2);

	xmin = imin;
	xmax = imax;
	return;
}

void walgo::findD2Seam(const Mat& image,
                       vector<float>& smoothCog,
                       vector<D2>& d2v,
                       vector<Vec4i>& lines,
                       //vector<Point>& seam,
                       vector<TopoPoint>& topos,
                       map<string, int>& config,
                       const int p)
{
	LASER_DETECTOR_TYPE t = (LASER_DETECTOR_TYPE) config["LASER_DETECTOR_TYPE"];
	detector* det = detector::getDetector(t);
        //Mat edges, maskRaw, mask;
        int avx2= 1;
        getConfigEntry(config, "AVX2", avx2);
	int doMediumBlur = 1;
	getConfigEntry(config, "SEAM_DETECTOR_MEDIUM_BLUR", doMediumBlur);
        Mat image1;//.clone();
        if(avx2 != 0) image1 = image;//.clone();
        else transpose(image,image1);//image1 = image;//

	if ( doMediumBlur )
	{
                int mediumBlurSize = 5;
		getConfigEntry(config, "SEAM_DETECTOR_MEDIUM_BLUR_SIZE", mediumBlurSize);
		medianBlur(image, image1, mediumBlurSize);
                //imwrite(to_string(p)+"o.jpg",image);
	}



        int count= 0;
        vector<float> line;
        //vector<vector<int> > maskRawVec;
        if(avx2 == 0) detect0(image1, line, count, config);
        else
        {
            Mat maskRaw;
            det->detect(image1, maskRaw, config);
            CalcCOG(image1, maskRaw, line, count);
            int mediumBlurSize = 5;
            getConfigEntry(config, "SEAM_DETECTOR_MEDIUM_BLUR_SIZE", mediumBlurSize);
            if (mediumBlurSize == 3) imwrite(to_string(p)+"m.jpg", maskRaw);
        }
	//for ( int i = 0; i < line.size(); i++ )  cout << i<< " " << line[i] << endl;
	//vector<TopoPoint> topos;

        if(line.size()-count < 150)
        {
            int cs= image.cols;
            int rs= image.rows;
            for(int i= 0;i < cs;i++)
            {
                smoothCog.push_back(0);
                D2 d2(i, -10);
                d2v.push_back(d2);
            }
            cout << "black rand !!!" << endl;
            Point2f p1(rand()%cs, rand()%rs);
            cout << "p1: " << p1.x << "  " << p1.y << "  " << endl;
            TopoPoint tp1(LINE_BEND, p1, 0, true, -10, false);
            topos.push_back(tp1);
            Point2f p2(rand()%cs, rand()%rs);
            cout << "p2: " << p2.x << "  " << p2.y << "  " << endl;
            TopoPoint tp2(LINE_BEND, p2, 0, false, -10, true);
            topos.push_back(tp2);

            return;
        }
	int medianL = 9;
	int smoothL = 15;
	int d2L = 80;
	float threshD2 = 1;
	int medianTh = 8;
	getConfigEntry(config, "D2_MEDIAN_LENGTH", medianL);
	getConfigEntry(config, "D2_SMOOTH_LENGTH", smoothL);
	getConfigEntry(config, "D2_LENGTH", d2L);
	getConfigEntry(config, "D2_MEDIAN_THRESH", medianTh);
	int xmin = 0;;
        int xmax = line.size();//image.cols;
	getConfigEntry(config, "D2_UMIN", xmin);
	getConfigEntry(config, "D2_UMAX", xmax);
        int minGapLength = 60;
	int gapDilate = d2L/2;
	getConfigEntry(config, "D2_MIN_GAPLENGTH", minGapLength);
        getConfigEntry(config, "D2_GAPDILATE", gapDilate);
	detectD2LineTopos(line,
			smoothCog,
			d2v,
			topos,
			xmin,               // min x (u) location
			xmax,               // max x (u) location
			medianL,           // length for median filter for spike removal
			smoothL,           // smoothing length
			d2L,               // length for d2 filter
			threshD2,          // threshold for d2
			medianTh,          // threshold for median rejection
			minGapLength,       // minimum gap length to suppress
			gapDilate           // gap dilate length
	);
	cout <<" found " << topos.size() << " topo points" << endl;
	// 第一返回点是下折点， 第二返回点是上折点，
	// 根据情况选点作为焊缝特征点。
	/*
	for ( auto && it : topos )
	{
		cout << "topo point type " << it._type << endl;
		Point p(it._point.x, it._point.y);
		seam.push_back(p);
	}
	*/
	vector<Point> pixels;
	for ( int i = 0; i < line.size(); i++)
		if ( line[i] > 0) pixels.push_back(Point(i, round(line[i])));
	for ( int i = 0; i < pixels.size()-1; i++ ) {
		Vec4i v;
		v[0] = pixels[i].x;
		v[1] = pixels[i].y;
		v[2] = pixels[i+1].x;
		v[3] = pixels[i+1].y;
		lines.push_back(v);
	}

	return;
}


//bool walgo::findD2Seam(cv::Mat& image,int thresh,std::vector<TopoPoint>& seam,bool rl)
//{
//    //cout << "image.size(): " << image.size() << endl;
//    cv::Mat img;
///*
//    for (int i=0; i < 459; i++)
//    {
//        for (int j=500; j < 555; j++) image.at<uchar>(j,i)= 255;
//    } */
//    threshold(image,img,thresh,255,cv::THRESH_BINARY);
//    //imwrite("threshold.png",img);
//    vector<Point> pts255;
//    findNonZero(img,pts255);
//    vector<Point> left,right;
//    int convex= 0,tm= 2;
//    if(thresh%2 == 1) convex= 1;
//    if(thresh%3 == 2) tm= 4;
//    for (auto p:pts255)
//    {
//        if (p.x < seam[convex]._point.x-15*tm && p.x > seam[convex]._point.x-150)
//        {
//            int cc= image.at<uchar>(p.y,p.x)/10;
//            for(int i= 0; i < cc;i++) left.push_back(p);
//        }
//        else if (p.x > seam[convex]._point.x+15*tm && p.x < seam[convex]._point.x+250)
//        {
//            int cc= image.at<uchar>(p.y,p.x)/10;
//            for(int i= 0; i < cc;i++)  right.push_back(p);
//        }
//    }
//    cout << "lr size: " << left.size() << " " << right.size() << endl;
//    Vec4d lineLeft,lineRight;
//    if(left.size() > 100)
//        fitLine(left, lineLeft, DIST_HUBER, 0, 0.01,0.01);
//    else return false;
//    if(right.size() > 100)
//    fitLine(right, lineRight, DIST_HUBER, 0, 0.01,0.01);
//    else return false;
//    float vx = lineLeft[0];
//    float vy = lineLeft[1];
//    float x = lineLeft[2];
//    float y = lineLeft[3];
//    float lk = vy/vx;
//    float lc = y - lk*x;
//    float k1 = 100*sqrt(1.0+lk*lk)+lc;
//    vx = lineRight[0];
//    vy = lineRight[1];
//    x = lineRight[2];
//    y = lineRight[3];
//    float rk = vy/vx;
//    float rc = y - rk*x;
//    float k2 = 100*sqrt(1.0+rk*rk)+rc;
//    seam[0]._point.x= (rc-lc)/(lk-rk);
//    seam[0]._point.y= seam[0]._point.x * lk +lc;
//    seam[1]._point.x= (k1-k2)/(rk-lk);
//    seam[1]._point.y= lk*seam[1]._point.x+k1;
//    return true;
//}

bool walgo::findD2Seam(cv::Mat& image,int thresh,std::vector<TopoPoint>& seam,int l,int r)
{
    cv::Mat img;
    threshold(image,img,thresh,255,cv::THRESH_BINARY);
    //imwrite("threshold.png",img);
    vector<Point> pts255;
    findNonZero(img,pts255);
    vector<Point> left,right;
    int convex= 0,tm= 2;
    if(thresh%2 == 1) convex= 1;
    if(thresh%3 == 2) tm= 4;
    for (auto p:pts255)
    {
        if (p.x < seam[convex]._point.x-10*tm && p.x > seam[convex]._point.x-l)
        {
            int cc= image.at<uchar>(p.y,p.x)/10;
            for(int i= 0; i < cc;i++) left.push_back(p);
        }
        else if (p.x > seam[convex]._point.x+10*tm && p.x < seam[convex]._point.x+r)
        {
            int cc= image.at<uchar>(p.y,p.x)/10;
            for(int i= 0; i < cc;i++)  right.push_back(p);
        }
    }
    cout << "lr size: " << left.size() << " " << right.size() << endl;
    Vec4d lineLeft,lineRight;
    if(left.size() > 100)
        fitLine(left, lineLeft, DIST_HUBER, 0, 0.01,0.01);
    else return false;
    if(right.size() > 100)
    fitLine(right, lineRight, DIST_HUBER, 0, 0.01,0.01);
    else return false;
    float vx = lineLeft[0];
    float vy = lineLeft[1];
    float x = lineLeft[2];
    float y = lineLeft[3];
    float lk = vy/vx;
    float lc = y - lk*x;
    float k1 = 100*sqrt(1.0+lk*lk)+lc;
    vx = lineRight[0];
    vy = lineRight[1];
    x = lineRight[2];
    y = lineRight[3];
    float rk = vy/vx;
    float rc = y - rk*x;
    float k2 = 100*sqrt(1.0+rk*rk)+rc;
    seam[0]._point.x= (rc-lc)/(lk-rk);
    seam[0]._point.y= seam[0]._point.x * lk +lc;
    seam[1]._point.x= lk;
    seam[1]._point.y= rk;
    return true;
}
