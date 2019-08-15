/*
 * histimage.cc
 *
 *  Created on: Jan 25, 2019
 *      Author: Hu XP
 */

#include "histimage.h"

using namespace std;
using namespace cv;
using namespace walgo;


bool walgo::hist(const cv::Mat& input,  std::vector<double>& output, std::map<std::string, int>& config,int ct,bool doMediumBlur)
{
    int th = 15;
    getConfigEntry(config, "PEAK1D_THRESH", th);
    ;
    int nc= input.cols, nr= input.rows;
    float* p;
    uchar* jp;
    uchar mi;
    //output.resize(nr, 0);
    Mat max1c= Mat::zeros(1,nr,CV_8UC1);
    reduce(input,max1c,0,2);


    //float a= 15,b= 256;
    int histSize = 256-th;
    float range[] = { float(th),256 };
    const float *histRanges = { range };
    Mat hist;
    calcHist(&max1c,1, 0, Mat(), hist, 1, &histSize, &histRanges, true, false);
    int s= sum(hist)[0];
    //cout << hist.depth() << "s= " << s << endl;
    if( ct == 0)
    {
        int count= 0;
        int nt= round(s/20);
        for(int i= 0;i < hist.rows;i++)
        {
            p= hist.ptr<float>(i);
            //cout << (*p) << endl;

            if(count < nt && count > -1)
            {
                count+= *p;
            }
            else if(count > 0)
            {
                th= i-1+th;
                count= -3;
            }
        }
        count= nr-s;
        cout << th << " " << count << " " << nt << endl;
    }

    Mat tempMean, tempStddv;
    double MEAN, STDDV;// mean and standard deviation of the flame region
    double m = mean(hist)[0];
    cout << "mean=" << m << endl;
    meanStdDev(hist, tempMean, tempStddv);

    MEAN = tempMean.at<double>(0, 0);
    STDDV = tempStddv.at<double>(0, 0);
    if(s == 0) m= 0;
    else m= sum(max1c)[0]/s;
    output.clear();
    output.push_back(s);
    output.push_back(m);
    output.push_back(STDDV);
    for(auto o:output) cout << o << endl;

    return true;

}
