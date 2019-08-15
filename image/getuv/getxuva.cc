#include "getxuva.h"
#include "lineanalysis.h"
#include "d2seamdetector.h"
#include <unistd.h>
#include <algorithm>

using namespace cv;
using namespace std;
using namespace walgo;

namespace walgo
{
//static uchar elementArray55[5][5]= {{0, 0, 1, 0, 0},
//                                    {0, 1, 1, 1, 0},
//                                    {1, 1, 1, 1, 1},
//                                    {0, 1, 1, 1, 0},
//                                    {0, 0, 1, 0, 0},};
//static uchar elementArray35[3][5]= {{0, 0, 1, 0, 0},
//                                    {1, 1, 1, 1, 1},
//                                    {0, 0, 1, 0, 0},};
//static uchar elementArray33[3][3]= {{0, 1, 0},
//                                    {1, 1, 1},
//                                    {0, 1, 0},};
//static uchar elementArray57[5][7]= {{0, 0, 0, 1, 0, 0, 0},
//                                    {0, 1, 1, 1, 1, 1, 0},
//                                    {1, 1, 1, 1, 1, 1, 1},
//                                    {0, 1, 1, 1, 1, 1, 0},
//                                    {0, 0, 0, 1, 0, 0, 0},};

static double dist(const Vec4i &line4i)
{
    return sqrt(pow(line4i[2]-line4i[0],2)+pow(line4i[3]-line4i[1],2));
}

//static bool cmp(const Vec4i &x,const Vec4i &y)
//{
//    return dist(x) > dist(y);//sqrt(pow(x[2]-x[0],2)+pow(x[3]-x[1],2)) > sqrt(pow(y[2]-y[0],2)+pow(y[3]-y[1],2));
//}

//const int Width= 8;
//Mat gg= getGaussianKernel(2*Width+1,Width/2);
//MatIterator_<double> ggit= gg.begin<double>();
//for(int d= 0;d < 18;d++) cout << *(ggit++) << " ";
//static vector<double> GV;
//{printf("%10.20f ",d);});

}

vector<getXuvA*> getXuvA::_obj(NUM_LINE_TYPES);

getXuvA* getXuvA::getLineType(LINE_TYPE t)
{
    if ( _obj[t]== NULL)
    {
        if ( t == LT_W )//|| t == LT_WT || t == LT_WTT)
            _obj[t] = new getXuvW(t);
        else if ( t == LT_U)
            _obj[t]= new getXuvU(t);
        else if ( t == LT_V )//|| t == LT_VT)
            _obj[t]= new getXuvV(t);
    }
    return _obj[t];
}
//int getXuvU::callTimes= 0;

int walgo::getTh(const Mat& input,int& th,int& count)
{
    int d= count;
    count= input.cols;
    if(d < 30) d= 30;
    vector<uchar> maxv;
    reduce(input,maxv,0,2);
    vector<uchar> tmaxv= maxv;
    sort(tmaxv.begin(),tmaxv.end());
    for(int i= 0;i < tmaxv.size();i++)
    {
        if(tmaxv[i] >= th)
        {
            count= i;
            break;
        }
    }
    if(count > 300) return -1;
    int nt= round((tmaxv.size()-count*1.0)/d);
    if(nt < 1) th= 15;
    else th= tmaxv[count-1+nt];
    if(th > 250) th= 222;
    if(th < 15) th= 15;
    //cout << "thresh: " << th << " " << count << " " << nt << endl;
    return nt;
}

int walgo::getTth(const Mat& input,int& th,int& count)
{
    Mat maxv= Mat::zeros(input.rows,1,CV_8UC1);
    reduce(input,maxv,1,2);
    vector<uchar> tmaxv;
    for(int i= 0;i < input.rows;i++) tmaxv.push_back(maxv.at<uchar>(i,0));
    sort(tmaxv.begin(),tmaxv.end());
    for(int i= 0;i < tmaxv.size();i++)
    {
        if(tmaxv[i] >= th)
        {
            count= i;
            break;
        }
    }
    int nt= round((tmaxv.size()-count)/2.2);
    th= tmaxv[count-1+nt];
    if(th > 250) th= 222;
    cout << "thresh: " << th << " " << count << " " << nt << endl;
    return nt;
}

walgo::getXuvA::getXuvA(LINE_TYPE t): _lineType(t)
{
    if (!readConfig("getUvByHough"+to_string(t)+".info", _config))  cout <<"warning! No getUvBy"+to_string(t)+".info !!!" << endl;
    init();
    cout << "initialized "+getuv_num;
    //_log << "initialized "+getuv_num;
}

void walgo::getXuvA::init()
{
    string path= "uvlog/";
    if(access(path.c_str(),0))
    {
        cout << "uvlog/ is nonexistent\n";
        system("mkdir uvlog");
    }

    std::time_t now= time(0);
    std::tm *dhms= std::localtime(&now);
    logpd= "uvlog/"+to_string(dhms->tm_mon)+"_"+to_string(dhms->tm_mday)+"_"+to_string(dhms->tm_hour)+"_"+\
            to_string(dhms->tm_min)+"_"+to_string(dhms->tm_sec)+"/";
    //cout << pd << endl;
    string bb= "mkdir "+logpd;
    const char* tpd = bb.c_str();
    system(tpd);
    _log.open(logpd+"log.txt",ios::out);

    _imgNum= 0;
    _debugNum= 0;
    _save= 1;

    _imgCols= 1280;
    _imgRows= 1024;
    getConfigEntry(_config, "ROI_WIDTH", _imgCols);
    getConfigEntry(_config, "ROI_HEIGHT", _imgRows);
    adjuster= 1280/_imgCols;

    getConfigEntry(_config, "SAVE_DEBUG_IMAGE", _save);
    _d2d= 2;
    getConfigEntry(_config, "DO_2ND_DETECT", _d2d);

    center_x_p= 0;
    _lowerBound= _imgCols-1;
    kernel= Mat(3,5,CV_8UC1, elementArray35);

    callTimes= 0;
    raserLineWidth= 12.0/adjuster;

    cout << kernel << " " << callTimes << endl;
    vector<double> gv;
    //for_each(gg.begin<double>(),gg.end<double>(),[](const double& d) {gv.push_back(d);});//{printf("%10.20f ",d);});//
    for(int i= 0;i < gg.rows;i++)
    {
        GV.push_back(gg.at<double>(0,i));
        cout << gg.at<double>(0,i) << endl;
    }
}

void walgo::getXuvA::clear()
{
    //upts.clear();
    //vpts.clear();
    if(callTimes == 0) return;
    cout << "callTimes: " << callTimes << endl;
    _imgNum= 0;
    _debugNum= 0;
    _uv.clear();
    //_imgRoivec.clear();
    //_log.close();
    std::time_t now= time(0);
    std::tm *dhms= std::localtime(&now);
    logpd= "uvlog/"+to_string(dhms->tm_mon)+"_"+to_string(dhms->tm_mday)+"_"+to_string(dhms->tm_hour)+"_"+\
            to_string(dhms->tm_min)+"_"+to_string(dhms->tm_sec)+"/";
    //cout << pd << endl;
    string bb= "mkdir "+logpd;
    const char* tpd = bb.c_str();
    system(tpd);
    //fstream log;
    _log.open(logpd+"log.txt",ios::out);
    _gapLen.clear();
    center_x_p= 0;
    _lowerBound= _imgCols-1;
    kernel= Mat(3,5,CV_8UC1, elementArray35);
}

bool walgo::getXuvA::confirm(int s)
{
    //for(int i= 0;i < _debugVec.size()/2;i++) //_log << _debugVec[2*i] << " " << _debugVec[2*i+1] << endl;
    for(auto &g:_gapLen) cout << g.first << " " << g.second << endl;
    if(_uv.size() == s && _imgNum == s)
    {
        return true;
    }
    else
    {
        cout << "nonconfirm! " << _uv.size() << " " << s << " " << _imgNum << endl;
        return false;
    }
}


int walgo::getXuvA::eraseWeldedParts(int& start,int s)
{
    int m = 3;

    //map<int,double> temp;
    map<int,double>::iterator it= _gapLen.begin();
    int v[2*m+1],temp[2*m+1];
    vector<int> medianGapLen,mmgld;

//        vector<int> sls,temp;
//        vector<int>::iterator bi;
//        for(int i= 0;i < 2*m+1;i++) sls.push_back((it++)->second);

//        for(int i= 0;i < s-(2*m+1);i++)
//        {
//            temp= sls;
//            std::nth_element(temp.begin(), temp.begin()+m, temp.end());
//            medianGapLen.push_back(temp[m]);
//            bi= sls.begin();
//            bi= sls.erase(bi);
//            sls.push_back(it->second);
//            it++;
//        }
//        for(auto it:sls) cout << it << " ";
//        cout  << "sls.size(): " << sls.size() << endl;

    for(int i= 0;i < 2*m+1;i++) v[i]= (it++)->second;
    for(int i= 0;i < s;i++)
    {
        for(int j= 0;j < 2*m+1;j++) temp[j]= v[j];
        std::nth_element(temp,temp+m, temp+2*m+1);//sort()
        medianGapLen.push_back(temp[m]);
        for(int j= 0;j < 2*m;j++) v[j]= v[j+1];
        v[2*m]= it->second;
        it++;
    }

    for(int i= 0;i < 2*m+1;i++) cout << v[i] << " ";
    cout << "gaplenV ";

    int ms= medianGapLen.size();
    mmgld.assign(ms,0);
    int dmargin= 0;
    for(int i= 0;i < m;i++) dmargin+= medianGapLen[i]-medianGapLen[m+i];

    for(int n= m;n < ms-m-1;n++)
    {
        //cout << dmargin << " ";
        mmgld[n]= dmargin;
        dmargin+= 2*medianGapLen[n]-medianGapLen[n-m]-medianGapLen[n+m];
    }
    mmgld[ms-m-1]= dmargin;

    it= _gapLen.begin();
    for(int i= 0;i < m;i++) it++;
    for(auto &m:medianGapLen) cout << (it++)->first << " " << m << endl;

    it= _gapLen.begin();
    for(int i= 0;i < m;i++) it++;
    for(auto &m:mmgld)
    {
        cout << (it++)->first << " " << m << endl;
    }
    start= 0;
    vector<int>::iterator maxm= max_element(mmgld.begin(),mmgld.end());
    while(maxm-- != mmgld.begin()) start++;
    it= _gapLen.begin();
    for(int i= m+start;i > 0;i--) it++;
    start= it->first;
//        cout << "eraseWeldedParts size: " << temp.size() << endl;
//        for(auto &t:temp) cout << t.first << " " << t.second << endl;
    return 0;
}


int walgo::getXuvA::getRoi(const Mat& image,int th,Rect2i& roi)
{
    //cout << "getRoi: " << endl;
    int s= image.rows;
    Mat maxc= Mat::zeros(s,1,CV_8UC1);
    reduce(image,maxc,1,2);
    threshold(maxc,maxc,th,255,0);
    //medianBlur(maxc,maxc,9);
    //cout  << maxc << endl;
    int eu= 0,ed= 0;
    for(int i= 0;i < s;i++)
    {
        if(maxc.at<uchar>(i,0) > 0)
        {
            eu= i;
            break;
        }
    }
    for(int i= s-1;i > 0 ;i--)
    {
        if(maxc.at<uchar>(i,0) > 0)
        {
            ed= i;
            break;
        }
    }
    //int addition= (ed+20 < image.rows)? 20:0;
    //if(addition == 0) addition= (ed+10 < image.rows)? 10:0;
    //if(addition == 0) addition= (ed+ 15 < image.rows)?  15:0;
    //cout << "eu ed: " << eu << " " << ed << endl;
    if(ed-eu < 100 || image.rows-ed < 60) { return -1;}

    roi= Rect2i(0,eu,image.cols,ed-eu+20);
    return 0;
}

void walgo::getXuvA::secondHough(Mat& rimg,vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR)
{
    //Mat img= 0;
    vector<int> terminalVec;
    int median= 640;
    Rect2i roi;
    if(selectedLinesL.size() < selectedLinesR.size())
    {
        selectedLinesL.clear();
        for(auto it:selectedLinesR) terminalVec.push_back(it[0]);
        std::nth_element(terminalVec.begin(),terminalVec.begin()+terminalVec.size()/2,terminalVec.end());
        median= terminalVec[terminalVec.size()/2];
        if(median > 386) median-= 128;
        else return;
        roi= Rect2i(median,0,rimg.cols-median,rimg.rows);
        rimg(roi)= 0;
        {

        }
    }
    else
    {
        selectedLinesR.clear();
        for(auto it:selectedLinesL) terminalVec.push_back(it[2]);
        std::nth_element(terminalVec.begin(),terminalVec.begin()+terminalVec.size()/2,terminalVec.end());
        median= terminalVec[terminalVec.size()/2];
        if(median < 968) median+= 128;
        else return;
        roi= Rect2i(0,0,median,rimg.rows);
        rimg(roi)= 0;
        {

        }
    }
    Mat maskh,maskf;
    threshold(rimg,maskh,15,255,0);
    imwrite(logpd+to_string(_imgNum)+"tt.png",maskh);
    kernel= Mat(3,5,CV_8UC1,elementArray35);//Mat::ones(5,7,CV_8UC1);//
    dilate(maskh,maskh,kernel);
    imwrite(logpd+to_string(_imgNum)+"t2.png",maskh);
    vector<Vec4i> lines,linesl,linesr;


    int houghThresh = 36;
    int houghMinLineLength = 200;
    int houghMaxLineGap = 155;

    double rho= 2;
    double theta= 2;
    HoughLinesP(maskh,lines,1*rho,CV_PI*theta/180,houghThresh,houghMinLineLength,houghMaxLineGap);

    for(auto it:lines)
    {
        double k= (it[3]-it[1]*1.0)/(it[2]-it[0]);
        //_log << "k: " << k << endl;
        if(k < -0.05) linesr.push_back(it);
        else if(k > 0.05) linesl.push_back(it);
    }
    int angleRange = 0;
    //getConfigEntry(_config, "PATH_ANGLE_RANGE", angleRange);
    int binsize = _config["ANGLE_BIN_SIZE"];
    int numbins = (int) floor((360.0/(double)binsize)+0.5);
    vector<double> weightsL(numbins);
    vector<vector<Vec4i> > histL(numbins);
    if(roi.x == 0)
    {
        selectedLinesR.clear();
        calcAngleHistogram(linesr, histL, weightsL, binsize);
        selectMaxAngles(histL, weightsL, numbins,
                        selectedLinesR, angleRange/binsize);
    }
    else
    {
        selectedLinesL.clear();
        calcAngleHistogram(linesl, histL, weightsL, binsize);
        selectMaxAngles(histL, weightsL, numbins,
                        selectedLinesL, angleRange/binsize);
    }

//    vector<double> weightsR(numbins);
//    vector<vector<Vec4i> > histR(numbins);
//    calcAngleHistogram(linesr, histR, weightsR, binsize);
//    selectMaxAngles(histR, weightsR, numbins,
//            selectedLinesR, angleRange/binsize);

    _log << selectedLinesL.size() << " " << selectedLinesR.size() << endl;
}


int walgo::getXuvW::addimg(const Mat& input)
{
    //cout << input.cols << " " << input.rows << "A-addimg" << endl;
    //_log << "pic" << to_string(_imgNum) << ": \n";
    //cout << "pic" << to_string(_imgNum) << ": \n";
    Point2d lwp= Point2d(0,0),rwp= Point2d(0,0);
    if(input.cols != _imgCols || input.rows != _imgRows)
    {
        //_log << "wrong image size!!! " << _imgNum << endl;
        //cout << "wrong image size!!! " << _imgNum << endl;
        _imgNum++;
        //_log.close();
        return -1;
    }

    int th= 15,count= 30;
    int nt= -1;
    Mat image;//= input.clone();
    //int doMediumBlur = 1;
    //getConfigEntry(_config, "SEAM_DETECTOR_MEDIUM_BLUR", doMediumBlur);
    medianBlur(input,image,5);
    nt= getTh(image,th,count);
    _log << "thresh: " << th << " count and nt: " << count << " " << nt << endl;

    Rect2i roi;
    if(getRoi(image,th,roi) == -1)
    {
        //cout << "wrong roi!!!" << endl;
        return -2;
    }
    //cout << roi << endl;
    //_log << "roi: " << roi << endl;
    vector<Vec4i> selectedLinesL,selectedLinesR;
    Mat img,maskh,maskf;
    threshold(image(roi),maskh,th,255,0); //imwrite("threshed.png",img);
    // mophology transform
    //image(roi).copyTo(img,maskh);
    //cvtColor(img,_img,CV_GRAY2BGR);
    if(1)
    {
        erode(maskh,maskh,kernel);
        maskf= maskh.clone();
        kernel= Mat(5,5,CV_8UC1,elementArray55);//Mat::ones(5,7,CV_8UC1);//
        dilate(maskh,maskh,kernel);
        //kernel= Mat(3,3,CV_8UC1,elementArray33);
        //erode(maskh,maskf,kernel);
        //imwrite(logpd+"ct"+to_string(_imgNum)+".png",img);
    }

    vector<Vec4i> lines,linesl,linesr;
    int houghThresh = 100;
    int houghMinLineLength = 230;
    int houghMaxLineGap = 55;
    //getConfigEntry(_config, "HOUGH_THRESH", houghThresh);
    //getConfigEntry(_config, "HOUGH_MINLINELENGTH", houghMinLineLength);
    //getConfigEntry(_config, "HOUGH_MAXLINEGAP", houghMaxLineGap);
    double rho= 2;
    double theta= 2;
    //imwrite("hask"+to_string(_imgNum)+".jpg",maskh);
    HoughLinesP(maskh,lines,1*rho,CV_PI*theta/180,houghThresh,houghMinLineLength,houghMaxLineGap);
    for(auto it:lines)
    {
        double k= (it[3]-it[1]*1.0)/(it[2]-it[0]);
        //_log << "k: " << k << endl;
        if( k < -0.05) linesr.push_back(it);
        else if(k > 0.05) linesl.push_back(it);
    }
    //_log << "linesl: \n";
//    for(auto line:linesl)
//    {
//        for(int i= 0;i < 4;i++) _log << line[i] << " ";
//        _log << endl;
//    }
//    _log << "linesr: \n";
//    for(auto line:linesr)
//    {
//        for(int i= 0;i < 4;i++) _log << line[i] << " ";
//        _log << endl;
//    }
    int angleRange = 0;
    //getConfigEntry(_config, "PATH_ANGLE_RANGE", angleRange);
    int binsize = _config["ANGLE_BIN_SIZE"];
    int numbins = (int) floor((360.0/(double)binsize)+0.5);
    vector<double> weightsL(numbins);
    vector<vector<Vec4i> > histL(numbins);
    calcAngleHistogram(linesl, histL, weightsL, binsize);
    selectMaxAngles(histL, weightsL, numbins,
            selectedLinesL, angleRange/binsize);

    vector<double> weightsR(numbins);
    vector<vector<Vec4i> > histR(numbins);
    calcAngleHistogram(linesr, histR, weightsR, binsize);
    selectMaxAngles(histR, weightsR, numbins,
            selectedLinesR, angleRange/binsize);

    if(selectedLinesL.size() < 2 || selectedLinesR.size() < 2)
    {
        //cout << "bad laser line !!!" << endl;
        _log << "bad laser " << _imgNum << " line !!!" << " thresh: " << th << endl;
        imwrite(logpd+to_string(_imgNum)+"t1.png",maskh);

        if(selectedLinesL.size() < 2 && selectedLinesR.size() < 2)
        {
            _log << "all bad lasers " << endl;
            _uv[_imgNum]= Point2d(0,0);
            return -2;
        }
        Mat rimg= image(roi);
        secondHough(rimg,selectedLinesL,selectedLinesR);
        if(selectedLinesL.size() < 2 || selectedLinesR.size() < 2)
        {
            _uv[_imgNum]= Point2d(0,0);
            _imgNum++;
            return -2;
        }
    }
//    else
//    {
        Point2d cp,center;
        double c0l,c1279r;
        Mat img2;
        img2= input(roi).clone();//.copyTo(img,maskh);
        roi.height= input.rows-roi.y;
        _imgp= input(roi).clone();

        int rt= -1;
        if(rt == -1) rt= getxuv(selectedLinesL,selectedLinesR,img2,cp,center,lwp,rwp,c0l,c1279r);
        if(rt == -11) rt= getxuv2(selectedLinesL,selectedLinesR,img2,cp,center,c0l,c1279r);
        if(rt == 0)
        {
            cp.y+= roi.y;
            center.y+= roi.y;
            lwp.y+= roi.y;
            rwp.y+= roi.y;
            _uv[_imgNum]= cp;//Point2d(cp.x,cp.y+roi.y);
            _luv[_imgNum]= lwp;
            _ruv[_imgNum]= rwp;
            if(_save > 0)
            {
                Mat cimg;
                cvtColor(input,cimg,CV_GRAY2BGR);
                line(cimg,Point2d(0,c0l+roi.y),center,Scalar(0,255,0),1,LINE_AA);
                line(cimg,Point2d(1279,c1279r+roi.y),center,Scalar(0,255,0),1,LINE_AA);
                circle(cimg,cp,15,Scalar(0,255,0),2);
                circle(cimg,lwp,15,Scalar(0,255,0),2);
                circle(cimg,rwp,15,Scalar(0,255,0),2);
                imwrite(logpd+to_string(_imgNum)+"sl.png",cimg);
            }
        }
        else _uv[_imgNum]= Point2d(0,0);
        _imgNum++;
        return rt;
//    }
}

bool walgo::getXuvW::getluv(map<int,Point2d>& luv)
{
    //luv= _uv;
    callTimes++;
    if(_luv.size() == _imgNum)
    {
        for(int i= 0;i < _imgNum;i++)
        {
            if(_luv[i].x > 100)
            {
                luv[i]= _luv[i];
                _log << i << " " << _luv[i].x << " " << _luv[i].y << endl;
            }
        }
        return true;
    }
    else return false;
}

bool walgo::getXuvW::getruv(map<int,Point2d>& luv)
{
    //luv= _uv;
    callTimes++;
    if(_ruv.size() == _imgNum)
    {
        for(int i= 0;i < _imgNum;i++)
        {
            if(_ruv[i].x > 100)
            {
                luv[i]= _ruv[i];
                _log << i << " " << _ruv[i].x << " " << _ruv[i].y << endl;
            }
        }
        return true;
    }
    else return false;
}

bool walgo::getXuvA::getuv(map<int,Point2d>& luv)
{
    //luv= _uv;
    int s= 0;
    int e= 0;
    if(_endORstart == 1 || _endORstart ==2) eraseWeldedParts(s,_start);
    if(_endORstart == -1 || _endORstart ==2)
    {
        map<int,double> temp;
        for(int i= 0;i < _imgNum-_end;i++) temp[i]= _gapLen[_imgNum-i];
        for(int i= 0;i < _imgNum-_end;i++) _gapLen[i]= temp[i];
        eraseWeldedParts(e,_imgNum-_end);
    }
    callTimes++;
    if(_uv.size() == _imgNum)
    {
        for(int i= s;i < _imgNum-e;i++)
        {
            if(_uv[i].x > 100)
            {
                luv[i]= _uv[i];
                _log << i << " " << _uv[i].x << " " << _uv[i].y << endl;
            }
        }
        return true;
    }
    else return false;
}

