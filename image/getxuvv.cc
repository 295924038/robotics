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

int walgo::getXuvV::addimg(const Mat& input)
{
    cout << input.cols << " " << input.rows << "A-addimg" << endl;
    _log << "pic" << to_string(_imgNum) << ": \n";
    cout << "pic" << to_string(_imgNum) << ": \n";
    _uv[_imgNum]= Point2d(0,0);
    if(input.cols != _imgCols || input.rows != _imgRows)
    {
        _log << "wrong image size!!! " << _imgNum << endl;
        cout << "wrong image size!!! " << _imgNum << endl;
        _imgNum++;
        return -1;
    }

    int th= 15,count= 30;
    int nt= 0;
    Mat image;//= input.clone();
    //int doMediumBlur = 1;
    //getConfigEntry(_config, "SEAM_DETECTOR_MEDIUM_BLUR", doMediumBlur);
    medianBlur(input,image,5);
    nt= getTh(image,th,count);
    _log << "thresh: " << th << " count and nt: " << count << " " << nt << endl;

    Rect2i roi;
    if(getRoi(image,th,roi) == -1)
    {
        cout << "bad roi!!!" << endl;
        _log << "bad roi!!!" << endl;
        _imgNum++;
        return -2;
    }
    cout << roi << endl;
    _log << "roi: " << roi << endl;

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
    int houghThresh = 190;
    int houghMinLineLength = 365;
    int houghMaxLineGap = 176;
    //getConfigEntry(_config, "HOUGH_THRESH", houghThresh);
    //getConfigEntry(_config, "HOUGH_MINLINELENGTH", houghMinLineLength);
    //getConfigEntry(_config, "HOUGH_MAXLINEGAP", houghMaxLineGap);
    double rho= 2/adjuster;
    double theta= 2/adjuster;
    //imwrite("hask"+to_string(_imgNum)+".jpg",maskh);
    houghThresh/= adjuster;
    houghMinLineLength/= adjuster;
    houghMaxLineGap/= adjuster;
    HoughLinesP(maskh,lines,1*rho,CV_PI*theta/180,houghThresh,houghMinLineLength,houghMaxLineGap);
    for(auto it:lines)
    {
        double k= (it[3]-it[1]*1.0)/(it[2]-it[0]);
        //_log << "k: " << k << endl;
        if( k < -0.08) linesl.push_back(it);
        else if(k > 0.08) linesr.push_back(it);
    }
    _log << "linesl: \n";
    for(auto line:linesl)
    {
        for(int i= 0;i < 4;i++) _log << line[i] << " ";
        _log << endl;
    }
    _log << "linesr: \n";
    for(auto line:linesr)
    {
        for(int i= 0;i < 4;i++) _log << line[i] << " ";
        _log << endl;
    }
    int angleRange = 0;
    getConfigEntry(_config, "PATH_ANGLE_RANGE", angleRange);
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

    int SH= 0;
    if(selectedLinesL.size() < 2 || selectedLinesR.size() < 2)
    {
        //cout << "bad laser line !!!" << endl;
        _log << "bad laser " << _imgNum << " line !!!" << " thresh: " << th << endl;
        imwrite(logpd+to_string(_imgNum)+"t0.png",maskh);

        if(selectedLinesL.size() < 2 && selectedLinesR.size() < 2)
        {
            _log << "all bad lasers line" << endl;
            cout << "all bad lasers line" << endl;
            _imgNum++;
            return -3;
        }
        //Mat rimg= image(roi);
        //secondHough(rimg,selectedLinesL,selectedLinesR,SH);
        if(selectedLinesL.size() < 2 || selectedLinesR.size() < 2)
        {
            _log << "one bad laser line" << endl;
            cout << "one bad laser line" << endl;
            _imgNum++;
            return -3;
        }
    }

//    else
//    {
        Point2d cp;
        double c0l,c1279r;
        Mat img2;
        img2= input(roi).clone();//.copyTo(img,maskh);
        int rt= getxuv(selectedLinesL,selectedLinesR,img2,cp,c0l,c1279r,SH);
        if(rt == 0)
        {
            cp.y+= roi.y;
            _uv[_imgNum]= cp;//Point2d(cp.x,cp.y+roi.y);
            if(_save == 2)
            {
                Mat cimg;
                cvtColor(input,cimg,CV_GRAY2BGR);
                line(cimg,Point2d(0,c0l+roi.y),cp,Scalar(0,255,0),1,LINE_AA);
                line(cimg,Point2d(1279,c1279r+roi.y),cp,Scalar(0,255,0),1,LINE_AA);
                circle(cimg,cp,15,Scalar(0,255,0),2);
                imwrite(logpd+to_string(_imgNum)+"sl.jpg",cimg);
            }
        }
        _imgNum++;
        return rt;
    //}
}

void walgo::getXuvV::secondHough(Mat& rimg,vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,int& SH)
{
    //Mat img= 0;
    vector<int> terminalVec;
    int median= 8642;
    for(int i= _imgNum-1;i >= 0;i--)
    {
        if(_uv[i].x > 300/adjuster)
        {
            median= _uv[i].x;
            break;
        }
    }
    Rect2i roi;
    if(selectedLinesL.size() < selectedLinesR.size())
    {
        selectedLinesL.clear();
        if(median == 8642)
        {
            for(auto it:selectedLinesR) terminalVec.push_back(it[0]);
            std::nth_element(terminalVec.begin(),terminalVec.begin()+terminalVec.size()/2,terminalVec.end());
            median= terminalVec[terminalVec.size()/2];
            if(median > 386/adjuster) median-= 64/adjuster;
            else return;
        }
        roi= Rect2i(median,0,rimg.cols-median,rimg.rows);
        rimg(roi)= 0;
        SH= -1;
    }
    else
    {
        selectedLinesR.clear();
        if(median == 8642)
        {
            for(auto it:selectedLinesL) terminalVec.push_back(it[2]);
            std::nth_element(terminalVec.begin(),terminalVec.begin()+terminalVec.size()/2,terminalVec.end());
            median= terminalVec[terminalVec.size()/2];
            if(median < 894/adjuster) median+= 64/adjuster;
            else return;
        }
        roi= Rect2i(0,0,median,rimg.rows);
        rimg(roi)= 0;
        SH= 1;
    }
    Mat maskh,maskf;
    threshold(rimg,maskh,15,255,0);
    imwrite(logpd+to_string(_imgNum)+"tt.png",maskh);
    kernel= Mat(3,5,CV_8UC1,elementArray35);//Mat::ones(5,7,CV_8UC1);//
    dilate(maskh,maskh,kernel);
    imwrite(logpd+to_string(_imgNum)+"td.png",maskh);
    vector<Vec4i> lines,linesl,linesr;


    int houghThresh = 58;
    int houghMinLineLength = 320;
    int houghMaxLineGap = 175;

    double rho= 2;
    double theta= 2;
    HoughLinesP(maskh,lines,1*rho,CV_PI*theta/180,houghThresh,houghMinLineLength,houghMaxLineGap);

    for(auto it:lines)
    {
        double k= (it[3]-it[1]*1.0)/(it[2]-it[0]);
        //_log << "k: " << k << endl;
        if(k < -0.05 && k > -6) linesl.push_back(it);
        else if(k > 0.05 && k < 6) linesr.push_back(it);
    }
    if((linesl.size() < 2 && SH == -1) || (linesr.size() < 2 && SH == 1))
    {
        linesl.clear();
        linesr.clear();
        houghThresh = 90;
        houghMinLineLength = 139;
        houghMaxLineGap = 39;
        lines.clear();
        maskh= 0;
        threshold(rimg,maskh,30,255,0);
        HoughLinesP(maskh,lines,1*rho,CV_PI*theta/180,houghThresh,houghMinLineLength,houghMaxLineGap);
        for(auto it:lines)
        {
            double k= (it[3]-it[1]*1.0)/(it[2]-it[0]);
            //_log << "k: " << k << endl;
            if(k < -0.05 && k > -3) linesl.push_back(it);
            else if(k > 0.05 && k < 3) linesr.push_back(it);
        }
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

int getXuvV::getxuv(vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Mat& img,Point2d& cp,double& c0l,double& c1279r,int SH)
{
    Point2d center= Point2d(0,0);
//    Vec4i lLine{0,0,0,0};
//    Vec4i rLine{0,0,0,0};
//    double cx,cy,vxl,vyl,vxr,vyr,dl,dr;
    Point2d vl,vln,vr,vrn;

    if(0)
    {

    }
    else
    {
        vector<double> lru,rlu,lrv,rlv;
        if(_save == 2) cvtColor(img,_img_, CV_GRAY2BGR);
        vector<double> kv,cv;
        double k,c,kl,cl,kr,cr;
        _log << to_string(_imgNum)+"leftline: ";
        for(auto it:selectedLinesL)
        {
            _log  << it[0] << " " << it[1] << " " << it[2] << " " << it[3] << endl;
            lru.push_back(it[2]); lrv.push_back(it[3]);
            k= (it[1]-it[3]*1.0)/(it[0]-it[2]);
            kv.push_back(k);
            c= it[1]-k*it[0];
            cv.push_back(c);
            _log << "k,c= " << k << " " << c << endl;
            if(_save == 2) line(_img_, Point(it[0],it[1]),  Point(it[2],it[3]), Scalar(0,195,0), 1, LINE_AA);
        }
        sort(kv.begin(),kv.end());
        if(kv.size()%2 == 1) kl= kv[(kv.size()-1)/2];
        else kl= (kv[(kv.size())/2]+kv[(kv.size()-2)/2])/2;
        sort(cv.begin(),cv.end());
        if(cv.size()%2 == 1) cl= cv[(cv.size()-1)/2];
        else cl= (cv[(cv.size())/2]+cv[(cv.size()-2)/2])/2;
        _log << "median k,c= " << kl << " " << cl << endl;
        kv.clear();
        cv.clear();

        _log << to_string(_imgNum)+"rightline: ";
        for(auto it:selectedLinesR)
        {
            _log << it[0] << " " << it[1] << " " << it[2] << " " << it[3] << endl;
            rlu.push_back(it[0]);rlv.push_back(it[1]);
            k= (it[1]-it[3]*1.0)/(it[0]-it[2]);
            kv.push_back(k);
            c= it[1]-k*it[0];
            cv.push_back(c);
            _log << "k,c= " << k << " " << c << endl;;
            if(_save == 2) line(_img_, Point(it[0],it[1]),  Point(it[2],it[3]), Scalar(0,195,0), 1, LINE_AA);
        }
        sort(kv.begin(),kv.end());
        if(kv.size()%2 == 1) kr= kv[(kv.size()-1)/2];
        else kr= (kv[(kv.size())/2]+kv[(kv.size()-2)/2])/2;
        sort(cv.begin(),cv.end());
        if(cv.size()%2 == 1) cr= cv[(cv.size()-1)/2];
        else cr= (cv[(cv.size())/2]+cv[(cv.size()-2)/2])/2;
        _log << "median k,c= " << kr << " " << cr << endl;
        kv.clear();
        cv.clear();


        center.x= (cr-cl)/(kl-kr);
        center.y= center.x*kl +cl;
        if(center.x < 380 || center.x > 900)
        {
            cout << "maybe laser line inclined to one side!!!\n";
            return -9;
        }

        vl.x= -1/sqrt(kl*kl+1);
        vl.y= kl*vl.x;
        //rotHalfPi(vl.x,vl.y);
        vr.x= 1/sqrt(kr*kr+1);
        vr.y= kr*vr.x;

        double leftCut1= 108,rightCut1= 108;
        double leftCut2= 361,rightCut2= 361;
        if(SH == -1)
        {
            leftCut1= 18;
            leftCut2= 500;
        }
        else if(SH == 1)
        {
            rightCut1= 18;
            rightCut2= 500;
        }

        leftCut1*= vl.x/adjuster;
        rightCut1*= vr.x/adjuster;
        leftCut2*= vl.x/adjuster;
        rightCut2*= vr.x/adjuster;
        Point2d lc1,lc2,rc1,rc2;
        lc1.x= center.x+leftCut1;
        lc2.x= (center.x+leftCut2 > 22/adjuster)? center.x+leftCut2 : 22/adjuster;
        rc1.x= center.x+rightCut1;
        rc2.x= (center.x+rightCut2 < 1257/adjuster)? center.x+rightCut2 : 1257/adjuster;

        lc1.y= lc1.x*kl+cl;
        lc2.y= lc2.x*kl+cl;
        rc1.y= rc1.x*kr+cr;
        rc2.y= rc2.x*kr+cr;

        double kln= -vl.x/vl.y;
        double krn= -vr.x/vr.y;
        Point2d cln1= Point2d(0,lc1.y-kln*lc1.x);
        Point2d cln2= Point2d(0,lc2.y-kln*lc2.x);
        Point2d crn1= Point2d(0,rc1.y-krn*rc1.x);
        Point2d crn2= Point2d(0,rc2.y-krn*rc2.x);



        //swap_cs(vxr,vyr);
//        cvtColor(img,_img,CV_GRAY2BGR);
//        line(_img,Point2d(0,cl),center,Scalar(255,55,255));
//        line(_img,Point2d(1279,cr+kr*1279),center,Scalar(255,55,255));
//        line(_img,cln1,lc1,Scalar(0,255,0));
//        line(_img,cln2,lc2,Scalar(0,255,0));
//        line(_img,crn1,rc1,Scalar(0,255,0));
//        line(_img,crn2,rc2,Scalar(0,255,0));
//        imwrite(logpd+to_string(_imgNum)+"ll.png",_img);


        vector<Point> left,right;
        Point2d clv1= lc1-cln1;
        Point2d clv2= lc2-cln2;
        Point2d crv1= rc1-crn1;
        Point2d crv2= rc2-crn2;

        Point2d lpv= lc1-lc2;
        Point2d rpv= rc1-rc2;

        double lenthl= norm(lpv);
        double lenthr= norm(rpv);
        int x2= 0,y2= 0;
        double dist;
        Point2d dp;
        int pv= 1;

//        Rect2i roi= Rect2i(0,0,int(center.x),img.rows);
//        threshold(img(roi),img(roi),15,255,THRESH_OTSU);
//        roi= Rect2i(int(center.x),0,img.cols-int(center.x),img.rows);
//        threshold(img(roi),img(roi),15,255,THRESH_OTSU);
        int ac1= 8;
        int ac2= 12;
        ac1/= adjuster;
        ac2/= adjuster;
        Rect2i roi= Rect2i(0,0,int(lc2.x)-ac2,img.rows);
        img(roi)= 0;
        roi= Rect2i(int(lc2.x-ac2),0,int(lc1.x+ac1)-int(lc2.x-ac2),img.rows);
        threshold(img(roi),img(roi),15,255,THRESH_OTSU);

        roi= Rect2i(int(rc1.x-ac1),0,int(rc2.x+ac2)-int(rc1.x-ac1),img.rows);
        threshold(img(roi),img(roi),15,255,THRESH_OTSU);
        roi= Rect2i(int(rc2.x+ac2),0,img.cols-int(rc2.x+ac2),img.rows);
        img(roi)= 0;

        roi= Rect2i(int(lc1.x+ac1),0,int(rc1.x-ac1)-int(lc1.x+ac1),img.rows);
        Mat rimg= img(roi).clone();
        img(roi)= 0;

        //imwrite(logpd+to_string(_imgNum)+"ri.png",img);
        //kernel= Mat(5,5,CV_8UC1,elementArray55);
        //erode(img,img,kernel);
        //imwrite(logpd+to_string(_imgNum)+"er.png",img);

        vector<Point> pts255;
        findNonZero(img,pts255); cout << "pts255.size(): " << pts255.size() << endl;
        for (auto p:pts255)
        {
            dp.x= double(p.x);
            dp.y= double(p.y);
            if(abs(lpv.cross(dp-lc2)/lenthl) < raserLineWidth
                    && clv1.cross(dp-cln1) > 0 && clv2.cross(dp-cln2) < 0) left.push_back(p);
//            {
//                pv= img.at<uchar>(p.y,p.x);
//                if(pv < 100) left.push_back(p);
//                else if(pv < 200)
//                {
//                    left.push_back(p);
//                    left.push_back(p);
//                }
//                else
//                {
//                    left.push_back(p);
//                    left.push_back(p);
//                    left.push_back(p);
//                }
//            }
            else if(abs(rpv.cross(dp-rc2)/lenthr) < raserLineWidth
                    && crv1.cross(dp-crn1) > 0 && crv2.cross(dp-crn2) < 0) right.push_back(p);
//            {
//                pv= img.at<uchar>(p.y,p.x);
//                if(pv < 100) right.push_back(p);
//                else if(pv < 200)
//                {
//                    right.push_back(p);
//                    right.push_back(p);
//                }
//                else
//                {
//                    right.push_back(p);
//                    right.push_back(p);
//                    right.push_back(p);
//                }
//            }
            //else img.at<uchar>(p.y,p.x)= 0;
        }

        cout << "lr size: " << left.size() << " " << right.size() << endl;
//        Mat limg= Mat::zeros(img.rows,img.cols,CV_8UC1);
//        Mat rimg= Mat::zeros(img.rows,img.cols,CV_8UC1);
//        for(auto p:left) limg.at<uchar>(p.y,p.x)= 255;
//        for(auto p:right) rimg.at<uchar>(p.y,p.x)= 255;
//        imwrite(logpd+to_string(_imgNum)+"li.png",limg);
//        imwrite(logpd+to_string(_imgNum)+"ri.png",rimg);

        Vec4d lineLeft,lineRight;
        if(left.size() > 256/adjuster) fitLine(left, lineLeft, DIST_L2, 0, 0.01,0.01);
        else return -4;
        if(right.size() > 256/adjuster) fitLine(right, lineRight, DIST_L2, 0, 0.01,0.01);
        else return -4;
        if(SH == -1) fitLine(left, lineLeft, DIST_HUBER, 0, 0.01,0.01);
        if(SH == 1) fitLine(right, lineRight, DIST_HUBER, 0, 0.01,0.01);
        double vx = lineLeft[0];
        double vy = lineLeft[1];
        double x = lineLeft[2];
        double y = lineLeft[3];
        kl = vy/vx;
        cl = y - kl*x;

        vx = lineRight[0];
        vy = lineRight[1];
        x = lineRight[2];
        y = lineRight[3];
        kr = vy/vx;
        cr = y - kr*x;

        center.x= (cr-cl)/(kl-kr);
        center.y= center.x*kl +cl;
        cp= center;

        c0l= cl;
        c1279r= cr+kr*1279/adjuster;

//        line(_img,Point2d(0,c0l),cp,Scalar(0,255,0),1,LINE_AA);
//        line(_img,Point2d(1279,c1279r),cp,Scalar(0,255,0),1,LINE_AA);

        //_endORstart= -1;
        if(_endORstart)
        {
            lc1.x= center.x+leftCut1*2;
            rc1.x= center.x+rightCut1*2;
            lc1.y= lc1.x*kl+cl;
            rc1.y= rc1.x*kr+cr;
            Point2d c_l= center-lc1;
            double lenthc_l= norm(c_l);
            Point2d r_c= rc1-center;
            double lenthr_c= norm(r_c);
            Point2d r_l= rc1-lc1;
            double lenthr_l= norm(r_l);

            double shl= 0;
            double shr= 0;
            double dl= raserLineWidth;
            int ct= 0;
            //Mat limg= Mat::zeros(rimg.rows,rimg.cols,CV_8UC1);

            int roi_x= roi.x;
            roi= Rect2i(0,0,int(center.x-roi_x),rimg.rows);
            threshold(rimg(roi),rimg(roi),15,255,THRESH_OTSU);
            //imwrite(logpd+to_string(_imgNum)+"lr.png",rimg);
            roi= Rect2i(int(center.x-roi_x),0,rimg.cols-int(center.x-roi_x),rimg.rows);
            threshold(rimg(roi),rimg(roi),15,255,THRESH_OTSU);
            //imwrite(logpd+to_string(_imgNum)+"rr.png",rimg);
            pts255.clear();
            findNonZero(rimg,pts255);//imwrite(logpd+to_string(_imgNum)+"r0.png",rimg);
            double heightOnLeft= 0,heightOnRight= 0;
            for(auto p:pts255)
            {
                dp.x= double(p.x+roi_x);
                dp.y= double(p.y);
                shl= c_l.cross(dp-lc1)/lenthc_l;
                shr= r_c.cross(dp-center)/lenthr_c;
                if(shl > dl && shr > dl && r_l.cross(dp-lc1) < 0)
                {
                    heightOnLeft+= shl;
                    heightOnRight+= shr;
                    rimg.at<uchar>(p.y,p.x)= 168;
                    ct++;
                }
            }
            //cout << heightOnLeft << " heightOnLeft ct " << ct  << " ct heightOnRight " << heightOnRight << endl;
            if(ct > 250) _gapLen[_imgNum]= -0.5*(heightOnLeft+heightOnRight)/ct;
            else _gapLen[_imgNum]= 0;
            cvtColor(rimg,rimg,CV_GRAY2BGR);
            circle(rimg,Point2d(center.x-roi_x,center.y),15,Scalar(0,255,0),2);
            imwrite(logpd+to_string(_imgNum)+"r1.png",rimg);
        }
        return 0;
    }
    return -1;

}
