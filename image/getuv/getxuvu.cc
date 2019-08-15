#include "getxuva.h"
#include "lineanalysis.h"
#include <unistd.h>

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
}

int getXuvU::addimg(const Mat& input)
{
    cout << input.cols << " " << input.rows << "U-addimg" << endl;
    _log << "pic" << to_string(_imgNum) << ": \n";
    cout << "pic" << to_string(_imgNum) << ": \n";
    if(input.cols == _imgCols && input.rows == _imgRows)
    {
        int th= 10,count= 60;
        int nt= 0;
        Mat image= input.clone();
        medianBlur(image,image,5);
        //nt= getTh(image,th,count);
        _log << "thresh: " << th << " count and nt: " << count << " " << nt << endl;

        Rect2i roi;
        if(getRoi(image,th,roi) == -1)
        {
            cout << "wrong roi!!!" << endl;
            _imgNum++;
            return -2;
        }
//        if(_lowerBound < 888)
//        {
//            if(_lowerBound-(roi.y+roi.height) > -30)
//            {
//                roi.y= (roi.y+roi.height-150);
//                roi.height= 150;
//            }
//            else
//            {
//                roi.height= 150;
//                roi.y= _lowerBound+10-roi.height;
//            }
//        }
        cout << roi << endl;
        vector<Vec4i> selectedLinesL,selectedLinesR,selectedLinesM;
        Mat img;
        threshold(image(roi),img,th,255,0);

        vector<Vec4i> lines,linesl,linesr,linesm;
        int houghThresh = 60;
        int houghMinLineLength = 100;
        int houghMaxLineGap = 30;
        getConfigEntry(_config, "HOUGH_THRESH", houghThresh);
        getConfigEntry(_config, "HOUGH_MINLINELENGTH", houghMinLineLength);
        getConfigEntry(_config, "HOUGH_MAXLINEGAP", houghMaxLineGap);
        double rho= 12;
        double theta= 1;
        //imwrite("hask"+to_string(_imgNum)+".jpg",maskh);
        dilate(img,img,kernel);
        HoughLinesP(img,lines,1*rho,CV_PI*theta/180,houghThresh,houghMinLineLength,houghMaxLineGap);
        cvtColor(img,_img_, CV_GRAY2BGR);

        for(auto it:lines)
        {
            if(it[0] == it[2]) continue;
            double k= (it[3]-it[1]*1.0)/(it[2]-it[0]);
            _log << "k: " << k << endl;
            //if((it[1]+it[3])/2)
            if(k > -0.1 && k < 0.1) linesm.push_back(it);
            else if( k > 1.08) linesl.push_back(it);
            else if(k < -1.08) linesr.push_back(it);
            else k= 1;
            if(k != 1 && _save == 2)  line(_img_,Point(it[0],it[1]),Point(it[2],it[3]),Scalar(0,205,205));

        }
        //imwrite(logpd+to_string(_imgNum)+".png",_img);
        if(linesm.size() > 0 && linesr.size() > 0 && linesl.size() > 0)
        {
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

            vector<double> weightsM(numbins);
            vector<vector<Vec4i> > histM(numbins);
            calcAngleHistogram(linesm, histM, weightsM, binsize);
            selectMaxAngles(histM, weightsM, numbins,
                    selectedLinesM, angleRange/binsize);
        }
        else
        {
            cout << linesm.size() << " && " << linesr.size() << " && " << linesl.size() << endl;
            _log << linesm.size() << " && " << linesr.size() << " && " << linesl.size() << endl;
            _uv[_imgNum]= Point2d(0,0);
            _imgNum++;
            return -1;
        }

        Point2d center= Point2d(0,0);
        Vec4i lLine{0,0,0,0};
        Vec4i rLine{0,0,0,0};
        Vec4i mLine{0,0,0,0};

        int rt;
        rt= getxuv(selectedLinesM,selectedLinesL,selectedLinesR,center);
        if(rt == 0)
        {
            _uv[_imgNum].x= center.x;
            _uv[_imgNum].y= center.y+roi.y;
            _lowerBound+= roi.y;
        }
        else _uv[_imgNum]= Point2d(0,0);

        _imgNum++;
        return rt;

    }
    else
    {
        _log << "wrong image size!!! " << _imgNum << endl;
        cout << "wrong image size!!! " << _imgNum << endl;
        _imgNum++;
        _log.close();
        return -4;
    }

}

int getXuvU::getxuv(vector<Vec4i>& selectedLinesM,vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Point2d& uv)
{
    vector<Vec4i> lp,rp;
    for(auto m:selectedLinesM)
    {
        for(auto l:selectedLinesL)
        {
            Vec4i near{m[0],m[1],l[2],l[3]};
            if(dist(near) < 35) lp.push_back(near);
        }
        for(auto r:selectedLinesR)
        {
            Vec4i near{r[0],r[1],m[2],m[3]};
            if(dist(near) < 35) rp.push_back(near);
        }
    }
    if(lp.size() > 1 && rp.size() > 1)
    {
        double lx= 0;
        double ly= 0;
        double rx= 0;
        double ry= 0;

        for(auto l:lp)
        {
            lx+= l[0]+l[2];
            ly+= l[1]+l[3];
        }
        lx= 0.5*lx/lp.size();
        ly= 0.5*ly/lp.size();
        for(auto r:rp)
        {
            rx+= r[0]+r[2];
            ry+= r[1]+r[3];
        }
        rx= 0.5*rx/rp.size();
        ry= 0.5*ry/rp.size();
        uv.x= (lx+rx)/2;
        uv.y= (ly+ry)/2;
        if(_save >= 0)
        {
            circle(_img_,Point(lx,ly),15,Scalar(0,5,250),2);
            circle(_img_,Point(rx,ry),15,Scalar(0,5,250),2);
            circle(_img_,uv,15,Scalar(0,255,0),2);
            imwrite(logpd+to_string(_imgNum)+"c.png",_img_);
        }
        _lowerBound= (ly > ry)? ly : ry;
        return 0;
    }
    else return -1;
}
