#ifndef GETXUVA_H
#define GETXUVA_H
#include "utils.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
using namespace walgo;

namespace walgo
{
static uchar elementArray55[5][5]= {{0, 0, 1, 0, 0},
                                    {0, 1, 1, 1, 0},
                                    {1, 1, 1, 1, 1},
                                    {0, 1, 1, 1, 0},
                                    {0, 0, 1, 0, 0},};
static uchar elementArray35[3][5]= {{0, 0, 1, 0, 0},
                                    {1, 1, 1, 1, 1},
                                    {0, 0, 1, 0, 0},};
static uchar elementArray79[7][9]= {{1, 1, 1, 1, 1, 1, 1, 1, 1},
                                    {0, 1, 1, 1, 1, 1, 1, 1, 0},
                                    {0, 0, 1, 1, 1, 1, 1, 0, 0},
                                    {0, 0, 0, 1, 1, 1, 0, 0, 0},
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0},};

static uchar elementArray33[3][3]= {{0, 1, 0},
                                    {1, 1, 1},
                                    {0, 1, 0},};
static uchar elementArray57[5][7]= {{0, 0, 0, 1, 0, 0, 0},
                                    {0, 1, 1, 1, 1, 1, 0},
                                    {1, 1, 1, 1, 1, 1, 1},
                                    {0, 1, 1, 1, 1, 1, 0},
                                    {0, 0, 0, 1, 0, 0, 0},};

int getTh(const Mat& input,int& th,int& count);
int getTth(const Mat& input,int& th,int& count);

static const int Width= 8;
static Mat gg= getGaussianKernel(2*Width+1,Width/3.0);

enum LINE_TYPE
{
    LT_D, //debug
    LT_V, //v
    //LT_VT,
    LT_W, //w
    //LT_WT,
    //LT_WTT,
    LT_U,  //u
    LT_N, //n
    NUM_LINE_TYPES
};

class getXuvA
{
public:
    static getXuvA* getLineType(LINE_TYPE t);
    static vector<getXuvA*> _obj;

    getXuvA(LINE_TYPE t);

    virtual ~getXuvA() {}

    virtual int addimg(const Mat& image)= 0;

    virtual bool confirm(int s);

    virtual void clear();

    //virtual int getxuv(vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Mat& image,Point2d& cp);
    //virtual int getxuv(vector<Vec4i>& selectedLinesM,vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Point2d& uv)= 0;

    bool getuv(map<int,Point2d>& luv);
//    {
//        //luv= _uv;
//        callTimes++;
//        if(_uv.size() == _imgNum)
//        {
//            for(int i= 0;i < _imgNum;i++)
//            {
//                if(_uv[i].x > 100)
//                {
//                    luv[i]= _uv[i];
//                    _log << i << " " << _uv[i].x << " " << _uv[i].y << endl;
//                }
//            }
//            return true;
//        }
//        else return false;
//    }

    //bool getuv(map<int,Point2d>& luv,vector<Mat>& iv);

    int eraseWeldedParts(int& start,int s= 138);

    string logpd;
    Mat kernel;
    int center_x_p;
    int callTimes;
    int adjuster;
    double raserLineWidth;
    vector<double> GV;

protected:
    virtual void init();
    //int getxuv(vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Point2d& uv,Vec4i& lLine,Vec4i& rLine);
    //virtual int getXuvW(Mat& img,Mat& maskf,vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Point2d& uv,Vec4i& lLine,Vec4i& rLine);
    //int getxuv3(vector<Vec4i>& selectedLinesM,Point2d& uv,Vec4i& mLine);
    //int getxuvdg(const Mat& input,int num);
    virtual int getRoi(const Mat& image,int th,Rect2i& roi);
    void secondHough(Mat& rimg,vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR);

    LINE_TYPE _lineType;
    int _endORstart;
    int _start;
    int _end;
    std::map<std::string, int> _config;
    fstream _log;
    const std::string getuv_num= "getuv1_2019.04.23!";
    int _imgCols,_imgRows;
    int _imgNum,_debugNum;
    int _save;
    int _d2d;
//    vector<int> _debugVec;
//    vector<Mat> _imgRoivec;
//    vector<Vec4i> _bottomLineVec;
    map<int,double> _gapLen;
    map<int,Point2d> _uv;
    Mat _img_,_imgp;
    int _lowerBound;

};

class getXuvW: virtual public getXuvA
{
public:
    getXuvW(LINE_TYPE t,int endORstart= 0,int start= 100,int end= 300) : getXuvA(t)
    {
        cout << " W type." << endl;
        _endORstart= endORstart;
        _start= start;
        _end= end;
    }

    int addimg(const Mat& input);

protected:
    int getxuv(vector<Vec4i>& selectedLinesL, vector<Vec4i>& selectedLinesR, Mat& img, Point2d& cp,Point2d& center,Point2d& lwp,Point2d& rwp,double& c0l,double& c1279r);
    int getxuv2(vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Mat& img,Point2d& cp,Point2d& center,double& c0l,double& c1279r);
    //int getxuv(vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Mat& image,Point2d& cp);
    map<int,Point2d> _luv;
    map<int,Point2d> _ruv;
    bool getluv(map<int,Point2d>& luv);
    bool getruv(map<int,Point2d>& luv);
};

class getXuvU: virtual public getXuvA
{
public:
    getXuvU(LINE_TYPE t) : getXuvA(t) {cout << " U type." << endl;_endORstart= 0;}

    int addimg(const Mat& input);

protected:
    int getxuv(vector<Vec4i>& selectedLinesM,vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Point2d& uv);
};

class getXuvV: virtual public getXuvA
{
public:
    getXuvV(LINE_TYPE t,int endORstart= 0,int start= 100,int end= 300) : getXuvA(t)
      //endORstart= 0无起始检测，1 仅起头，-1 仅末尾,2 两头都检测；start 起头的张数； end 总张数-末尾张数；
    {
        cout << " V type." << endl;
        _endORstart= endORstart;
        _start= start;
        _end= end;
    }

    int addimg(const Mat& input);

protected:
    int getxuv(vector<Vec4i>& selectedLinesL, vector<Vec4i>& selectedLinesR, Mat& img, Point2d& cp,double& c0l,double& c1279r);
    //int getxuv(vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Mat& image,Point2d& cp);
};


//getXuvV(LINE_TYPE t,int endORstart= 0,int start= 100,int end= 300);//endORstart= 0无起始检测，1 仅起头，-1 仅末尾,2 两头都检测；start 起头的张数； end 总张数-末尾张数；

}
#endif // GETXUVA_H
