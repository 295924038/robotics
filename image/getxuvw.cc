#include "getxuva.h"
#include "lineanalysis.h"
#include <unistd.h>

using namespace cv;
using namespace std;
using namespace walgo;

namespace walgo
{
static double dist(const Vec4i &line4i)
{
    return sqrt(pow(line4i[2]-line4i[0],2)+pow(line4i[3]-line4i[1],2));
}

}


int getXuvW::getxuv(vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Mat& img,Point2d& cp,Point2d& center,Point2d& lwp,Point2d& rwp,double& c0l,double& c1279r)
{
    center= Point2d(0,0);
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
        if(_save == 2 || _imgNum == 372) cvtColor(img,_img_, CV_GRAY2BGR);
        vector<double> kv,cv;
        double k,c,kl,cl,kr,cr;
        //_log << to_string(_imgNum)+"leftline: ";
        for(auto it:selectedLinesL)
        {
            //_log  << it[0] << " " << it[1] << " " << it[2] << " " << it[3] << endl;
            lru.push_back(it[2]); lrv.push_back(it[3]);
            k= (it[1]-it[3]*1.0)/(it[0]-it[2]);
            kv.push_back(k);
            c= it[1]-k*it[0];
            cv.push_back(c);
            //_log << "k,c= " << k << " " << c << endl;
            if(_save == 2 || _imgNum == 372) line(_img_, Point(it[0],it[1]),  Point(it[2],it[3]), Scalar(0,195,0), 1, LINE_AA);
        }
        sort(kv.begin(),kv.end());
        if(kv.size()%2 == 1) kl= kv[(kv.size()-1)/2];
        else kl= (kv[(kv.size())/2]+kv[(kv.size()-2)/2])/2;
        sort(cv.begin(),cv.end());
        if(cv.size()%2 == 1) cl= cv[(cv.size()-1)/2];
        else cl= (cv[(cv.size())/2]+cv[(cv.size()-2)/2])/2;
        //_log << "median k,c= " << kl << " " << cl << endl;
        kv.clear();
        cv.clear();

        //_log << to_string(_imgNum)+"rightline: ";
        for(auto it:selectedLinesR)
        {
            //_log << it[0] << " " << it[1] << " " << it[2] << " " << it[3] << endl;
            rlu.push_back(it[0]);rlv.push_back(it[1]);
            k= (it[1]-it[3]*1.0)/(it[0]-it[2]);
            kv.push_back(k);
            c= it[1]-k*it[0];
            cv.push_back(c);
            //_log << "k,c= " << k << " " << c << endl;;
            if(_save == 2 || _imgNum == 372) line(_img_, Point(it[0],it[1]),  Point(it[2],it[3]), Scalar(0,195,0), 1, LINE_AA);
        }
        if(_save == 2 || _imgNum == 372) imwrite(logpd+to_string(_imgNum)+"_.png",_img_);
        sort(kv.begin(),kv.end());
        if(kv.size()%2 == 1) kr= kv[(kv.size()-1)/2];
        else kr= (kv[(kv.size())/2]+kv[(kv.size()-2)/2])/2;
        sort(cv.begin(),cv.end());
        if(cv.size()%2 == 1) cr= cv[(cv.size()-1)/2];
        else cr= (cv[(cv.size())/2]+cv[(cv.size()-2)/2])/2;
        //_log << "median k,c= " << kr << " " << cr << endl;
        kv.clear();
        cv.clear();


        center.x= (cr-cl)/(kl-kr);
        center.y= center.x*kl +cl;
        if(center.x < 320 || center.x > 960)
        {
            cout << "maybe laser line inclined to one side!!!\n";
            return -9;
        }


        vl.x= -1/sqrt(kl*kl+1);
        vl.y= kl*vl.x;
        //rotHalfPi(vl.x,vl.y);
        vr.x= 1/sqrt(kr*kr+1);
        vr.y= kr*vr.x;
        double leftCut1= 175*vl.x,rightCut1= 175*vr.x;
        double leftCut2= 421*vl.x,rightCut2= 421*vr.x;
        Point2d lc1,lc2,rc1,rc2;
        lc1.x= center.x+leftCut1;
        lc2.x= (center.x+leftCut2 > 22)? center.x+leftCut2 : 22;
        rc1.x= center.x+rightCut1;
        rc2.x= (center.x+rightCut2 < 1257)? center.x+rightCut2 : 1257;

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
//        imwrite(logpd+to_string(_imgNum)+"l.png",_img);


        vector<Point> left,right;
        Point2d clv1= lc1-cln1;
        Point2d clv2= lc2-cln2;
        Point2d crv1= rc1-crn1;
        Point2d crv2= rc2-crn2;

        Point2d lpv= lc1-lc2;
        Point2d rpv= rc1-rc2;

        double lenthl= norm(lpv);
        double lenthr= norm(rpv);
        //int x2= 0,y2= 0;
        //double dist;
        Point2d dp;
        //int pv= 1;
        Mat rimg,simg,mask= Mat::zeros(img.rows,img.cols,CV_8UC1);
        //cvtColor(img,simg,CV_GRAY2BGR);
        simg= img.clone();

        Rect2i roi= Rect2i(0,0,int(lc2.x)-12,img.rows);
        img(roi)= 0;
        roi= Rect2i(int(lc2.x-12),0,int(lc1.x+8)-int(lc2.x-12),img.rows);
        threshold(img(roi),img(roi),15,255,THRESH_OTSU);

        roi= Rect2i(int(rc1.x-8),0,int(rc2.x+12)-int(rc1.x-8),img.rows);
        threshold(img(roi),img(roi),15,255,THRESH_OTSU);
        roi= Rect2i(int(rc2.x+12),0,img.cols-int(rc2.x+12),img.rows);
        img(roi)= 0;

        roi= Rect2i(int(lc1.x+8),0,int(rc1.x-8)-int(lc1.x+8),img.rows);
        rimg= img(roi).clone();
        img(roi)= 0;
        //imwrite(logpd+to_string(_imgNum)+"mi.png",img);
        //kernel= Mat(3,5,CV_8UC1,elementArray35);
        //erode(img,img,kernel);

        vector<Point> pts255;
        findNonZero(img,pts255); //cout << "pts255.size(): " << pts255.size() << endl;
        for (auto p:pts255)
        {
            dp.x= double(p.x);
            dp.y= double(p.y);
            if(abs(lpv.cross(dp-lc2)/lenthl) < raserLineWidth
                    && clv1.cross(dp-cln1) < 0 && clv2.cross(dp-cln2) > 0) left.push_back(p);
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
                    && crv1.cross(dp-crn1) < 0 && crv2.cross(dp-crn2) > 0) right.push_back(p);
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
//        Mat mi;
//        cvtColor(img,mi,CV_GRAY2BGR);
//        line(mi,lc1,lc2,Scalar(0,255,0));
//        line(mi,rc1,rc2,Scalar(0,255,0));
//        imwrite(logpd+to_string(_imgNum)+"mi.png",mi);
        //cout << "lr size: " << left.size() << " " << right.size() << endl;


        Vec4d lineLeft,lineRight;
        if(left.size() > 300) fitLine(left, lineLeft, DIST_L2, 0, 0.01,0.01);
        else return -4;
        if(right.size() > 300) fitLine(right, lineRight, DIST_L2, 0, 0.01,0.01);
        else return -4;
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

        c0l= cl;
        c1279r= cr+kr*1279;
        int wpOpt= 0;
        getConfigEntry(_config, "WPOINT_OPTION", wpOpt);
        double meanl= 0,meanr= 0;
        //cp= center;
        if(wpOpt == 0)
        {
            cp= center;
            return 0;
        }
        else if(wpOpt == 1)  _log << "cross_point"+to_string(_imgNum)+" :" << center << endl;
//        else if(wpOpt == 3)
//        {
//            Point2d lPoint= Point2d(0,0);
//            Point2d rPoint= Point2d(0,0);
//            int rt= getxuvS(const Point2d& center,Point2d& lPoint,Point2d& rPoint);
//            return rt;
//        }


        //cvtColor(img,_img,CV_GRAY2BGR);
//        line(_img,Point2d(0,cl),center,Scalar(0,255,0),1,LINE_AA);
//        line(_img,Point2d(1279,cr+kr*1279),center,Scalar(0,255,0),1,LINE_AA);
//        circle(_img,cp,15,Scalar(0,255,0),2);
//        imwrite(logpd+to_string(_imgNum)+"sl.png",_img);




        //return 0;

        else if(wpOpt == 3)
        {
            Mat dstl,dstr;
            //Point center(src.cols/2,src.rows/2); //旋转中心

            double scale = 1.0;  //缩放系数

            //cout << center << endl;
            Rect2i roi= Rect2i(0,0,img.cols,ceil(center.y)+4*Width);
            //reduce(,,,REDUCE_SUM);
            simg= _imgp(roi).clone();
            //imshow("src",src);
            //imshow("dst",dst);
            //waitKey();

    //        mask= Mat::zeros(simg.rows,simg.cols,CV_8UC1);
    //        line(mask,lc1,center,Scalar(255),Width*4,LINE_8);
    //        Mat simgl,simgr;
    //        simg.copyTo(simgl,mask);
    //        imwrite(logpd+to_string(_imgNum)+"il.png",simgl);

            //warpAffine(img,dstl,rotMat,Size(img.cols,center.y+20),INTER_CUBIC);//*3/4));
            double angle = atan(kl)*180/CV_PI;  //cout << "anglel: " << angle << endl;
            Mat rotMat = getRotationMatrix2D(center,angle,scale);
            warpAffine(simg,dstl,rotMat,simg.size(),INTER_CUBIC);
            angle= atan(kr)*180/CV_PI;  //cout << "angler: " << angle << endl;
            rotMat= getRotationMatrix2D(center,angle,scale);
            warpAffine(simg,dstr,rotMat,simg.size(),INTER_CUBIC);
            //imwrite(logpd+to_string(_imgNum)+"dstl.png",dstl);
            medianBlur(dstl,dstl,5);
            medianBlur(dstr,dstr,5);
            //return -22;
            int wyc= int(center.y)-Width*2;
            if(int(center.x)-320 < 0 || int(center.x)+320 > 1280) return -6;
            Rect2i leftRoi= Rect2i(int(center.x)-320,wyc,320,Width*6);
            int lth= 15,count= 30;
            int nt= 0;
            nt= getTh(dstl(leftRoi),lth,count);
            cout << "th,nt: " << lth << " " << nt << endl;
            if(lth < 25) lth= 15;
            threshold(dstl(leftRoi),dstl(leftRoi),lth,255,THRESH_BINARY);
            //threshold(dstl(leftRoi),dstl(leftRoi),15,255,THRESH_OTSU);
            //kernel= Mat(7,9,CV_8UC1,elementArray79);
            //erode(dstl(leftRoi),dstl(leftRoi),kernel);
            if(_save > 0) imwrite(logpd+to_string(_imgNum)+"dstl.png",dstl);
            Mat maxv= Mat::zeros(Width*6,1,CV_32FC1);
            Mat cc;
            dstl(leftRoi).convertTo(cc,CV_32FC1);
            reduce(cc,maxv,1,1);//cv::ReduceTypes;
            //maxv= maxv/255;
            double max, min;
            cv::Point min_loc, max_loc;
            cv::minMaxLoc(maxv, &min, &max, &min_loc, &max_loc);
            cout << maxv << " " << max << " " << max_loc;

            max*= 0.39;
            int tBound= 0,bBound= Width*6;
            for(int i= Width*6-1;i > Width*1.5;i--)
            {
                if(maxv.at<float>(0,i) > max)
                {
                    bBound= i;
                    break;
                }
            }
            for(int i= bBound;i > 0;i--)
            {
                if(maxv.at<float>(0,i) < max)
                {
                    tBound= i+1;
                    break;
                }
            }

            vector<int> tEnd,bEnd;
            int tBound_9= (tBound-9 > 0) ? tBound-9 : 0;
            cout << tBound_9 << tBound << " bound " << bBound << endl;

            for(int i= tBound-2;i > tBound_9;i--)
            {
                auto it= cc.ptr<float>(i)+110;
                auto end= cc.ptr<float>(i)+319;
                while(it != end && *it == 0) it++;
                if(it != end)
                {
                    tEnd.push_back(0);
                    auto ct= tEnd.end()-1;
                    while(it++ != end) (*ct)+= 1;
                }
            }
            for(int i= tBound+1;i < bBound;i++)
            {
                //auto it= cc.ptr<float>(i)+319;
                auto start= cc.ptr<float>(i)+119;
                for(int j= 200;j > 0;j--)
                {
                    if(start[j] > 0)  {tEnd.push_back(200-j);break;}
                }
            }

            //threshold(img,dst,th,255,0);

            //Mat temp= dstl(leftRoi).clone;
            //mask51= 255-mask15;
            //Moments mm= moments(dstl(leftRoi),true);
            //if(mm.m00 < 500) mmVec.push_back(0);
            //else mmVec.push_back(mm.m01/mm.m00);

            if(tEnd.size() == 0) return -5;
            std::nth_element(tEnd.begin(),tEnd.begin()+tEnd.size()/2,tEnd.end());
            int medianl = tEnd[tEnd.size()/2];
            meanl= 0;
            int counts= 0;
            for(auto &i:tEnd)
            {
                //cout << i << " ";
                if(abs(i-medianl) < 4*Width)
                {
                    meanl+= i;
                    counts++;
                }
            }
            meanl/= counts;
            cout << tEnd.size() << " lend\n";

            Rect2i rightRoi= Rect2i(int(center.x),wyc,320,Width*6);
            //threshold(dstr(rightRoi),dstr(rightRoi),15,255,THRESH_OTSU);
            int rth= 15;count= 30;
            nt= getTh(dstr(rightRoi),rth,count);
            cout << "th,nt: " << rth << " " << nt << endl;
            if(rth < 25) rth= 15;
            threshold(dstr(rightRoi),dstr(rightRoi),rth,255,THRESH_BINARY);
            if(_save > 0) imwrite(logpd+to_string(_imgNum)+"dstr.png",dstr);
            maxv= 0;
            cc= 0;
            dstr(rightRoi).convertTo(cc,CV_32FC1);
            reduce(cc,maxv,1,1);
            max, min;
            min_loc, max_loc;
            cv::minMaxLoc(maxv, &min, &max, &min_loc, &max_loc);
            cout << maxv << " " << max << " " << max_loc << endl;
            max*= 0.39;

            //line(dstr,center,Point2d(center.x+333,center.y),Scalar(0),1,LINE_AA);
            tBound= 0;bBound= Width*6;
            for(int i= Width*6-1;i > Width*1.5;i--)
            {
                if(maxv.at<float>(0,i) > max)
                {
                    bBound= i;
                    break;
                }
            }
            for(int i= bBound;i > 0;i--)
            {
                if(maxv.at<float>(0,i) < max)
                {
                    tBound= i+1;
                    break;
                }
            }
            cout << tBound << " bound " << bBound << endl;

            tEnd.clear();
            tBound_9= (tBound-9 > 0) ? tBound-9 : 0;
            cout << tBound_9 << endl;
            for(int i= tBound-2;i > tBound_9;i--)
            {
                auto it= cc.ptr<float>(i)+200;
                auto end= cc.ptr<float>(i);
                while(it != end && *it == 0) it--;
                if(it != end)
                {
                    tEnd.push_back(0);
                    auto ct= tEnd.end()-1;
                    while(it-- != end) (*ct)+= 1;
                }
            }
            for(int i= tBound+1;i < bBound;i++)
            {
                //auto it= cc.ptr<float>(i)+319;
                auto start= cc.ptr<float>(i);
                for(int j= 0;j < 200;j++)
                {
                    if(start[j] > 0)  {tEnd.push_back(j);break;}
                }
            }

            if(tEnd.size() == 0) return -5;
            std::nth_element(tEnd.begin(),tEnd.begin()+tEnd.size()/2,tEnd.end());
            int medianr = tEnd[tEnd.size()/2];
            meanr= 0;
            counts= 0;
            for(auto &i:tEnd)
            {
                //cout << i << " ";
                if(abs(i-medianr) < 4*Width)
                {
                    meanr+= i;
                    counts++;
                }
            }
            meanr/= counts;
            cout << tEnd.size() << " rend\n";

            double lcount= meanl;
            //leftCut1= lcount*vl.x;//,rightCut1= rcount*vr.x;
            lc1.x= center.x+lcount*vl.x;
            lc1.y= lc1.x*kl+cl;


            double rcount= meanr;
            //rightCut1= rcount*vr.x;
            rc1.x= center.x+rcount*vr.x;
            rc1.y= rc1.x*kr+cr;

            //cp= lc1+rc1-center;
            if(_save > 0 || 1)
            {
            cvtColor(simg,simg,CV_GRAY2BGR);
            circle(simg,lc1,1,Scalar(0,0,255),2);
            circle(simg,lc1,15,Scalar(0,255,0),2,LINE_AA);
            circle(simg,rc1,1,Scalar(0,0,255),2);
            circle(simg,rc1,15,Scalar(0,255,0),2,LINE_AA);
            circle(simg,lc1+rc1-center,15,Scalar(0,255,0),2,LINE_AA);
            //imwrite(logpd+to_string(_imgNum)+"csimg.png",simg);
            }
            lwp= lc1;
            rwp= rc1;
            if(_endORstart || 1) _gapLen[_imgNum]= meanl+meanr;
        }

        lc1.x= center.x+leftCut1;
        rc1.x= center.x+rightCut1;
        lc1.y= lc1.x*kl+cl;
        rc1.y= rc1.x*kr+cr;
        Point2d l_c= lc1-center;
        Point2d c_r= center-rc1;
        double lenthl_c= norm(l_c);
        double lenthc_r= norm(c_r);
        double sh= 0;
//        double ht= 0;
//        int ct= 0;

//        roi= Rect2i(lc1.x,0,rc1.x-lc1.x,img.rows);
//        Mat rimg= s_img(roi);
        medianBlur(rimg,rimg,5);
        //for(auto &g:GV) cout << g << " "; cout << endl;
        int th= 15,count= 60;
        int nt= 0;
        if(0)
        {
            nt= getTh(rimg,th,count);
            //_log << "thresh: " << th << " count and nt: " << count << " " << nt << endl;
            threshold(rimg,rimg,th,255,THRESH_BINARY);
        }
        else
        {
            Mat mask15,rrimg;
            //threshold(rimg,mask15,th,255,THRESH_BINARY);
            //
            threshold(rimg,mask15,th,255,THRESH_OTSU);
            Mat mask51= 255-mask15;
            rimg.copyTo(rrimg,mask51);
            threshold(rrimg,mask51,th,255,THRESH_OTSU);
            rimg= mask15+mask51;
        }
        pts255.clear();
        findNonZero(rimg,pts255); //cout << "pts255.size(): " << pts255.size() << endl;
        //rimg= Mat::zeros(img.rows,img.cols,CV_8UC1);
        if(_save > 0) imwrite(logpd+to_string(_imgNum)+"o.png",rimg);

        vector<int> shortLinel,shortLiner;
        vector<double> shortLineL,shortLineR;
//        for(int i= 0;i < s_img.rows;i++)
//        {
//            for(int j= 0;j < rc1.x; j++)
//        }
        int dl= Width,dh= 186;
        shortLinel.resize(dh,0);
        shortLiner.resize(dh,0);
        shortLineL.resize(dh,0);
        shortLineR.resize(dh,0);
        vector<double> shortLineL2,shortLineR2;
        shortLineL2.resize(dl*5,0);
        shortLineR2.resize(dl*5,0);
//        for (auto p:pts255)
//        {
//            dp.x= double(p.x+roi.x);
//            dp.y= double(p.y);
//            sh= l_c.cross(dp-center)/lenthl_c;
//            if(sh > dl && sh < dh)
//            {
//                shortLinel[int(sh-dl)]+= 1;
//                //rimg.at<uchar>(p.y,p.x+roi.x)= 255;
//            }

//            sh= c_r.cross(dp-rc1)/lenthc_r;
//            if(sh > dl && sh < dh)
//            {
//                shortLiner[int(sh-dl)]+= 1;
//                //rimg.at<uchar>(p.y,p.x+roi.x)= 255;
//            }
//        }
        vector<Point2d> pts25;
        double shl,shr;
        int dl_3= -3*dl;
        for (auto p:pts255)
        {
            dp.x= double(p.x+roi.x);
            dp.y= double(p.y);
            shl= l_c.cross(dp-center)/lenthl_c*adjuster;
            shr= c_r.cross(dp-rc1)/lenthc_r*adjuster;
            if(shl > dl && shl < dh && shr > dl && shr < dh)
            {
                pts25.push_back(Point2d(dp));
                shortLinel[int(shl)]++;
                shortLiner[int(shr)]++;
                //rimg.at<uchar>(p.y,p.x+roi.x)= 255;
            }
            else if(shl > dl_3 && shr > dl_3)
            {
                if(shl <= dl) shortLineL2[int(dl-shl)]++;
                if(shr <= dl) shortLineR2[int(dl-shr)]++;
            }

        }
        for(auto &s:shortLineL2) _log << s << " ";
        _log << endl;
        for(auto &s:shortLineR2) _log << s << " ";
        _log << endl;
        int leftWidth= 0;
        int rightWidth= 0;
        double lmax= *(max_element(shortLineL2.begin(),shortLineL2.end()))/2;
        double rmax= *(max_element(shortLineR2.begin(),shortLineR2.end()))/2;
        _log << lmax << " lmax rmax " << rmax << endl;
        for(auto &s:shortLineL2)
        {
            if(s > lmax) leftWidth++;
        }
        for(auto &s:shortLineR2)
        {
            if(s > rmax) rightWidth++;
        }
        for(int i= dl;i < 2*dl;i++)
        {
            _log << shortLinel[i] << endl;
            if(shortLinel[i] <= lmax) break;
            else leftWidth++;
        }
        for(int i= dl;i < 2*dl;i++)
        {
            _log << shortLiner[i] << endl;
            if(shortLiner[i] <= rmax) break;
            else rightWidth++;
        }
        _log << leftWidth << " lwidth rwidth " << rightWidth << endl;
        _log << "pic" << _imgNum << ":\n";

        if(lmax < 50 || rmax < 50)
        {
            leftWidth= (lmax > rmax) ? leftWidth : rightWidth;
            rightWidth= leftWidth;
        }
        _log << leftWidth << " lwidth rwidth " << rightWidth << endl;
//        for(int i= 0;i < shortLinel.size();i++) _log << i << ">>>" << shortLinel[i] << " ";
//        _log << "\nlr\n";
//        for(int i= 0;i < shortLiner.size();i++) _log << i << ">>>" << shortLiner[i] << " ";

        //int breadth= 6;
        int cw= 0;
        double convolution= 0;
        for(int i= Width;i < dh-2*Width-1;i++)
        {
            cw= 0;
            convolution= 0;
            for(auto &g:GV) convolution+= g*shortLinel[i+(cw++)];
            shortLineL[i+Width]= (convolution);
        }
        for(int i= Width;i < dh-2*Width-1;i++)
        {
            cw= 0;
            convolution= 0;
            for(auto &g:GV) convolution+= g*shortLiner[i+(cw++)];
            shortLineR[i+Width]= (convolution);
        }
        int nc= 0;
        int ccl= dl;
        int ccr= dl;
        double m= 0;
        for(auto &it:shortLineL)
        {
            _log << "[" << nc << "]: " << it << " " << shortLinel[nc] << " ";
            //printf("[%d]: %9.3f %d",nc,it,shortLinel[nc]);
            if(it > m && nc > 2*Width)
            {
                m= it;
                ccl= nc;
            }
            nc++;
        }
        _log << endl;
        nc= 0;m= 0;
        for(auto &it:shortLineR) //cout << it << " ";
        {
            _log << "[" << nc << "]: " << it << " " << shortLiner[nc] << " ";
            //printf("[%d]: %9.3f %d",nc,it,shortLiner[nc]);
            if(it > m && nc > 2*Width)
            {
                m= it;
                ccr= nc;
            }
            nc++;
        }
        _log << endl;

        shortLinel.clear();
        shortLiner.clear();
        shortLineL.clear();
        shortLineR.clear();
        shortLinel.resize(dh,0);
        shortLiner.resize(dh,0);
        shortLineL.resize(dh,0);
        shortLineR.resize(dh,0);

        //img= 0;
        //Mat Rimg= img.clone();
        for (auto dp:pts25)
        {
            shl= l_c.cross(dp-center)/lenthl_c*adjuster;
            shr= c_r.cross(dp-rc1)/lenthc_r*adjuster;
            if(shl < dh && shr < ccr-Width)
            {
                shortLinel[int(shl)]+= 1;
                //img.at<uchar>(dp.y,dp.x)= 255;
            }
            if(shl < ccl-Width && shr < dh)
            {
                shortLiner[int(shr)]+= 1;
                //Rimg.at<uchar>(dp.y,dp.x)= 255;
            }
        }
        //imwrite(logpd+to_string(_imgNum)+"l.png",img);
        //imwrite(logpd+to_string(_imgNum)+"r.png",Rimg);
        //cout << endl << gg << endl;
        //cp= center;
        //for_each(shortLinel.begin(),shortLinel.end(),[](const int& i) {cout << i << " ";});
        //cout << endl;

        for(int i= Width;i < dh-2*Width-1;i++)
        {
            cw= 0;
            convolution= 0;
            for(auto &g:GV) convolution+= g*shortLinel[i+(cw++)];
            shortLineL[i+Width]= (convolution);
        }
        for(int i= Width;i < dh-2*Width-1;i++)
        {
            cw= 0;
            convolution= 0;
            for(auto &g:GV) convolution+= g*shortLiner[i+(cw++)];
            shortLineR[i+Width]= (convolution);
        }

        shortLineL2.clear();
        shortLineR2.clear();
        nc= 0;m= 0;
        for(auto &it:shortLineL)
        {
            //_log << "[" << nc << "]: " << it << " " << shortLinel[nc] << endl;
            printf("[%d]: %9.3f %d\n",nc,it,shortLinel[nc]);
            if(it > m && nc > 2*Width)
            {
                m= it;
                ccl= nc;
            }
            if(it > 2) shortLineL2.push_back(it);
            nc++;
        }
        int ssl= 3;
        if(shortLineL2.size() < 3) ssl= 0;
        //std::nth_element(shortLineL2.begin(),shortLineL2.begin()+shortLineL2.size()/2,shortLineL2.end());
        //double median = shortLineL2[shortLineL2.size()/2];
        auto max= max_element(shortLineL2.begin(),shortLineL2.end());double max_l= *max;
        double halfMax= (*max)/2;
        int lcount= 1;
        auto it= max;
        while (*(++it) > halfMax) lcount++;
        it= max;
        while (*(--it) > halfMax) lcount++;

        _log << "\nLmax: " << halfMax << " at " << ccl << "cc lcount: " << lcount << endl;

        //if(max < 2*median) return -3;



        cout << _imgNum << "p: " << endl;
        int ew= 0;
        getConfigEntry(_config, "ERASE_WELDED", ew);

        if(lcount > 2*leftWidth && ew) return -4;

        nc= 0;m= 0;
        for(auto &it:shortLineR)
        {
            //_log << "[" << nc << "]: " << it << " " << shortLiner[nc] << endl;
            printf("[%d]: %9.3f %d\n",nc,it,shortLiner[nc]);
            if(it > m && nc > 2*Width)
            {
                m= it;
                ccr= nc;
            }
            if(it > 2) shortLineR2.push_back(it);
            nc++;
        }
        cout << endl;
        int ssr= 3;
        if(shortLineR2.size() < 3) ssr= 0;
        //std::nth_element(shortLineR2.begin(),shortLineR2.begin()+shortLineR2.size()/2,shortLineR2.end());
        //median = shortLineR2[shortLineR2.size()/2];
        max= max_element(shortLineR2.begin(),shortLineR2.end());double max_r= *max;
        halfMax= (*max)/2;
        int rcount= 1;
        it= max;
        while (*(++it) > halfMax) rcount++;
        it= max;
        while (*(--it) > halfMax) rcount++;

        _log << "\nRmax: " << halfMax << " at " << ccr << "cc rcount: " << rcount << endl;

        if(rcount > 2*rightWidth && ew) return -4;

        int ph= (ceil(center.y)+Width < _imgp.rows) ? ceil(center.y)+Width : _imgp.rows;
        Rect2i roip= Rect2i(0,0,img.cols,ph);
        simg= _imgp(roip).clone();

        if(max_l > 22 && ssl)
        {
            double clc= cl-ccl*sqrt(kl*kl+1);
            rc1.x= (cr-clc)/(kl-kr);
            rc1.y= rc1.x*kr +cr;
            //circle(simg,rc1,1,Scalar(0,0,255),2);
            //circle(simg,rc1,15,Scalar(255,0,0),1,LINE_AA);
            rwp= rc1;

        }
        else
        {
            Mat mask= Mat::zeros(simg.rows,simg.cols,CV_8UC1);
            line(mask,rc1,center,Scalar(255),Width,LINE_8);
            Mat simgr;
            simg.copyTo(simgr,mask);

            medianBlur(simgr,simgr,5);
            threshold(simgr,simgr,15,255,THRESH_OTSU);
            if(_save > 0) imwrite(logpd+to_string(_imgNum)+"ir.png",simgr);
            Moments mm= moments(simgr,true);
            if(mm.m00 > 50)
            {
                Point2d deltaw= (rc1-center)/norm(rc1-center);
                rc1.x-= (rc1.x-mm.m10/mm.m00)*2;
                rc1.y= rc1.x*kr +cr;
                rwp= rc1-Width*deltaw;
            }
        }

        if(max_r > 22 && ssr)
        {
            double crc= cr-ccr*sqrt(kr*kr+1);
            lc1.x= (crc-cl)/(kl-kr);
            lc1.y= lc1.x*kl +cl;
            //circle(simg,lc1,1,Scalar(0,0,255),2);
            //circle(simg,lc1,15,Scalar(255,0,0),1,LINE_AA);
            lwp= lc1;
        }
        else
        {
            Mat mask= Mat::zeros(simg.rows,simg.cols,CV_8UC1);
            line(mask,lc1,center,Scalar(255),Width,LINE_8);
            Mat simgl;
            simg.copyTo(simgl,mask);

            medianBlur(simgl,simgl,5);
            threshold(simgl,simgl,15,255,THRESH_OTSU);
            if(_save > 0) imwrite(logpd+to_string(_imgNum)+"il.png",simgl);
            Moments mm= moments(simgl,true);
            if(mm.m00 > 50)
            {
                Point2d deltaw= (center-lc1)/norm(center-lc1);
                lc1.x+= (mm.m10/mm.m00-lc1.x)*2;
                lc1.y= lc1.x*kl +cl;
                lwp= lc1+Width*deltaw;
            }
        }

//        if(_endORstart || 1) _gapLen[_imgNum]= meanl+meanr;
        //if(_save > 0) imwrite(logpd+to_string(_imgNum)+"csimg.png",simg);
//        cl-= ccl*sqrt(kl*kl+1);
//        cr-= ccr*sqrt(kr*kr+1);
//        cp.x= (cr-cl)/(kl-kr);
//        cp.y= cp.x*kl +cl;

        //cvtColor(rimg,rimg,CV_GRAY2BGR);
        //line(s_img,cp,Point2d(1279,1279*kl+cl),Scalar(0,255,0),1,LINE_AA);
        //line(s_img,cp,Point2d(0,cr),Scalar(0,255,0),1,LINE_AA);
        //circle(simg,cp,15,Scalar(0,255,0),2);
        //imwrite(logpd+to_string(_imgNum)+"s.png",simg);




//        Point2d l_r= lc1-rc1;
//        double lenthl_r= norm(l_r);
//        double sht= 0;
//        double ht= 0;
//        int ct= 0;
        //Mat limg= Mat::zeros(img.rows,img.cols,CV_8UC1);

//        for(auto p:pts25)
//        {

//            ht= l_r.cross(p-rc1);
//            if(ht < 0)
//            {
//                sht+= ht;
//                ct++;
//                //limg.at<uchar>(p.y,p.x)= 255;
//            }
//        }
//        _gapLen[_imgNum]= sht/lenthl_r/ct;
        //imwrite(logpd+to_string(_imgNum)+"li.png",limg);
        //cout << sh/lenthl_r << " sh ct " << ct  << " lenthl_r :" << lenthl_r << endl;

        //shortLinel.resize(dh,0);
        //shortLiner.resize(dh,0);
//        double ht= 0;
//        int ct= 0;
//        for (auto p:pts25)
//        {
//            ht+= norm(p-center);
//            ct++;
//        }
//        _gapLen[_imgNum]= ht/ct;
//        cvtColor(img,_img,CV_GRAY2BGR);
//        line(_img,Point2d(0,c0l),cp,Scalar(0,255,0),1,LINE_AA);
//        line(_img,Point2d(1279,c1279r),cp,Scalar(0,255,0),1,LINE_AA);
        //cvtColor(img,s_img,CV_GRAY2BGR);
        //circle(s_img,cp,15,Scalar(0,255,0),2);
        //imwrite(logpd+to_string(_imgNum)+"l.png",s_img);
        return 0;

    }

    return -1;

}

int getXuvW::getxuv2(double& kl,double& kr,Mat& img,Point2d& cp,Point2d& center,Point2d& lwp,Point2d& rwp,double& c0l,double& c1279r)
{
//    center= Point2d(0,0);
//    Vec4i lLine{0,0,0,0};
//    Vec4i rLine{0,0,0,0};
//    double cx,cy,vxl,vyl,vxr,vyr,dl,dr;
    if(img.cols != _imgCols || img.rows != _imgRows)
    {
        _log << "wrong image size!!! " << _imgNum << endl;
        cout << "wrong image size!!! " << _imgNum << endl;
        _imgNum++;
        return -1;
    }

    Point2d vl,vln,vr,vrn;

    //vector<double> lru,rlu,lrv,rlv;
    //if(_save == 2 || _imgNum == 372) cvtColor(img,_img_, CV_GRAY2BGR);
    //vector<double> kv,cv;
    double cl,cr;
    cl= center.y-kl*center.x;
    cr= center.y-kr*center.x;

    vl.x= -1/sqrt(kl*kl+1);vln.y= vl.x;
    vl.y= kl*vl.x; vln.x= -vl.y;
    //rotHalfPi(vl.x,vl.y);
    vr.x= 1/sqrt(kr*kr+1); vrn.y= -vr.x;
    vr.y= kr*vr.x; vrn.x= vr.y;
    double leftCut1= 175*vl.x,rightCut1= 175*vr.x;
    double leftCut2= 421*vl.x,rightCut2= 421*vr.x;
    Point2d lc1,lc2,rc1,rc2;
    lc1.x= center.x+leftCut1;
    lc2.x= (center.x+leftCut2 > 22)? center.x+leftCut2 : 22;
    rc1.x= center.x+rightCut1;
    rc2.x= (center.x+rightCut2 < 1257)? center.x+rightCut2 : 1257;

    lc1.y= lc1.x*kl+cl;
    lc2.y= lc2.x*kl+cl;
    rc1.y= rc1.x*kr+cr;
    rc2.y= rc2.x*kr+cr;
    Point2d lpv= lc1-lc2;
    Point2d rpv= rc1-rc2;

    double lenthl= norm(lpv);
    double lenthr= norm(rpv);

    //medianBlur(img,img,5);
    Mat mask= Mat::zeros(img.rows,img.cols,CV_8UC1);
    Mat imgl,imgr;
    vector<Point> ptsl,ptsr;


    line(mask,lc1,lc2,Scalar(255),Width*4,LINE_4);
    findNonZero(mask,ptsl);
    Rect2i roil;
    roil.y= (ptsl.begin())->y;
    roil.height= (ptsl.end()-1)->y-roil.y+1;
    int x1= (ptsl.begin())->x,x2= (ptsl.begin())->x;
    for(auto &p:ptsl)
    {
        if(p.x < x1) x1=p.x;
        if(p.x > x2) x2=p.x;
    }
    roil.x= x1;
    roil.width= x2-x1+1;
    cout << roil << endl;
    img.copyTo(imgl,mask);
    threshold(imgl(roil),imgl(roil),15,255,THRESH_OTSU);
    ptsl.clear();
    findNonZero(imgl,ptsl);
    if(_save > 0) imwrite(logpd+to_string(_imgNum)+"l.png",imgl);

    mask= 0;
    line(mask,rc1,rc2,Scalar(255),Width*4,LINE_4);
    Rect2i roir;
    findNonZero(mask,ptsr);
    roir.y= (ptsr.begin())->y;
    roir.height= (ptsr.end()-1)->y-roir.y+1;
    x1= (ptsr.begin())->x;
    x2= (ptsr.begin())->x;
    for(auto &p:ptsr)
    {
        if(p.x < x1) x1=p.x;
        if(p.x > x2) x2=p.x;
    }
    roir.x= x1;
    roir.width= x2-x1+1;
    cout << roir << endl;
    img.copyTo(imgr,mask);
    threshold(imgr(roir),imgr(roir),15,255,THRESH_OTSU);
    ptsr.clear();
    findNonZero(imgr,ptsr);
    if(_save > 0) imwrite(logpd+to_string(_imgNum)+"r.png",imgr);

    Vec4d lineLeft,lineRight;
    if(ptsl.size() > 256/adjuster) fitLine(ptsl, lineLeft, DIST_L2, 0, 0.01,0.01);
    else return -4;
    if(ptsr.size() > 256/adjuster) fitLine(ptsr, lineRight, DIST_L2, 0, 0.01,0.01);
    else return -4;
    //if(SH == -1) fitLine(imgl, lineLeft, DIST_HUBER, 0, 0.01,0.01);
    //f(SH == 1) fitLine(imgr, lineRight, DIST_HUBER, 0, 0.01,0.01);
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

    c0l= cl;
    c1279r= cr+kr*1279/adjuster;

    //return 0;
    Mat simg;

    lc1.x= center.x+leftCut1;
    rc1.x= center.x+rightCut1;
    lc1.y= lc1.x*kl+cl;
    rc1.y= rc1.x*kr+cr;
    Point2d l_c= lc1-center;
    Point2d c_r= center-rc1;
    double lenthl_c= norm(l_c);
    double lenthc_r= norm(c_r);
    double sh= 0;
    Point2d dp;
//        double ht= 0;
//        int ct= 0;

//        roi= Rect2i(lc1.x,0,rc1.x-lc1.x,img.rows);
//        Mat rimg= s_img(roi);

    Rect2i roi= Rect2i(int(lc1.x+8),round(center.y-200),int(rc1.x-8)-int(lc1.x+8),200);
    Mat rimg= img(roi).clone();
    medianBlur(rimg,rimg,5);
    //for(auto &g:GV) cout << g << " "; cout << endl;
    int th= 15,count= 60;
    int nt= 0;
    if(0)
    {
        nt= getTh(rimg,th,count);
        //_log << "thresh: " << th << " count and nt: " << count << " " << nt << endl;
        threshold(rimg,rimg,th,255,THRESH_BINARY);
    }
    else
    {
        Mat mask15,rrimg;
        //threshold(rimg,mask15,th,255,THRESH_BINARY);
        //
        threshold(rimg,mask15,th,255,THRESH_OTSU);
        Mat mask51= 255-mask15;
        rimg.copyTo(rrimg,mask51);
        threshold(rrimg,mask51,th,255,THRESH_OTSU);
        rimg= mask15+mask51;
    }
    vector<Point> pts255;
    findNonZero(rimg,pts255); //cout << "pts255.size(): " << pts255.size() << endl;
    //rimg= Mat::zeros(img.rows,img.cols,CV_8UC1);
    if(_save > 0) imwrite(logpd+to_string(_imgNum)+"o.png",rimg);

    vector<int> shortLinel,shortLiner;
    vector<double> shortLineL,shortLineR;
//        for(int i= 0;i < s_img.rows;i++)
//        {
//            for(int j= 0;j < rc1.x; j++)
//        }
    int dl= Width,dh= 186;
    shortLinel.resize(dh,0);
    shortLiner.resize(dh,0);
    shortLineL.resize(dh,0);
    shortLineR.resize(dh,0);
    vector<double> shortLineL2,shortLineR2;
    shortLineL2.resize(dl*5,0);
    shortLineR2.resize(dl*5,0);
//        for (auto p:pts255)
//        {
//            dp.x= double(p.x+roi.x);
//            dp.y= double(p.y);
//            sh= l_c.cross(dp-center)/lenthl_c;
//            if(sh > dl && sh < dh)
//            {
//                shortLinel[int(sh-dl)]+= 1;
//                //rimg.at<uchar>(p.y,p.x+roi.x)= 255;
//            }

//            sh= c_r.cross(dp-rc1)/lenthc_r;
//            if(sh > dl && sh < dh)
//            {
//                shortLiner[int(sh-dl)]+= 1;
//                //rimg.at<uchar>(p.y,p.x+roi.x)= 255;
//            }
//        }
    vector<Point2d> pts25;
    double shl,shr;
    int dl_3= -3*dl;
    for (auto p:pts255)
    {
        dp.x= double(p.x+roi.x);
        dp.y= double(p.y+roi.y);
        shl= l_c.cross(dp-center)/lenthl_c*adjuster;
        shr= c_r.cross(dp-rc1)/lenthc_r*adjuster;
        if(shl > dl && shl < dh && shr > dl && shr < dh)
        {
            pts25.push_back(Point2d(dp));
            shortLinel[int(shl)]++;
            shortLiner[int(shr)]++;
            //rimg.at<uchar>(p.y,p.x+roi.x)= 255;
        }
        else if(shl > dl_3 && shr > dl_3)
        {
            if(shl <= dl) shortLineL2[int(dl-shl)]++;
            if(shr <= dl) shortLineR2[int(dl-shr)]++;
        }

    }
    for(auto &s:shortLineL2) _log << s << " ";
    _log << endl;
    for(auto &s:shortLineR2) _log << s << " ";
    _log << endl;
    int leftWidth= 0;
    int rightWidth= 0;
    double lmax= *(max_element(shortLineL2.begin(),shortLineL2.end()))/2;
    double rmax= *(max_element(shortLineR2.begin(),shortLineR2.end()))/2;
    _log << lmax << " lmax rmax " << rmax << endl;
    for(auto &s:shortLineL2)
    {
        if(s > lmax) leftWidth++;
    }
    for(auto &s:shortLineR2)
    {
        if(s > rmax) rightWidth++;
    }
    for(int i= dl;i < 2*dl;i++)
    {
        _log << shortLinel[i] << endl;
        if(shortLinel[i] <= lmax) break;
        else leftWidth++;
    }
    for(int i= dl;i < 2*dl;i++)
    {
        _log << shortLiner[i] << endl;
        if(shortLiner[i] <= rmax) break;
        else rightWidth++;
    }
    _log << leftWidth << " lwidth rwidth " << rightWidth << endl;
    _log << "pic" << _imgNum << ":\n";

    if(lmax < 50 || rmax < 50)
    {
        leftWidth= (lmax > rmax) ? leftWidth : rightWidth;
        rightWidth= leftWidth;
    }
    _log << leftWidth << " lwidth rwidth " << rightWidth << endl;
//        for(int i= 0;i < shortLinel.size();i++) _log << i << ">>>" << shortLinel[i] << " ";
//        _log << "\nlr\n";
//        for(int i= 0;i < shortLiner.size();i++) _log << i << ">>>" << shortLiner[i] << " ";

    //int breadth= 6;
    int cw= 0;
    double convolution= 0;
    for(int i= Width;i < dh-2*Width-1;i++)
    {
        cw= 0;
        convolution= 0;
        for(auto &g:GV) convolution+= g*shortLinel[i+(cw++)];
        shortLineL[i+Width]= (convolution);
    }
    for(int i= Width;i < dh-2*Width-1;i++)
    {
        cw= 0;
        convolution= 0;
        for(auto &g:GV) convolution+= g*shortLiner[i+(cw++)];
        shortLineR[i+Width]= (convolution);
    }
    int nc= 0;
    int ccl= dl;
    int ccr= dl;
    double m= 0;
    for(auto &it:shortLineL)
    {
        _log << "[" << nc << "]: " << it << " " << shortLinel[nc] << " ";
        //printf("[%d]: %9.3f %d",nc,it,shortLinel[nc]);
        if(it > m && nc > 2*Width)
        {
            m= it;
            ccl= nc;
        }
        nc++;
    }
    _log << endl;
    nc= 0;m= 0;
    for(auto &it:shortLineR) //cout << it << " ";
    {
        _log << "[" << nc << "]: " << it << " " << shortLiner[nc] << " ";
        //printf("[%d]: %9.3f %d",nc,it,shortLiner[nc]);
        if(it > m && nc > 2*Width)
        {
            m= it;
            ccr= nc;
        }
        nc++;
    }
    _log << endl;

    shortLinel.clear();
    shortLiner.clear();
    shortLineL.clear();
    shortLineR.clear();
    shortLinel.resize(dh,0);
    shortLiner.resize(dh,0);
    shortLineL.resize(dh,0);
    shortLineR.resize(dh,0);

    //img= 0;
    //Mat Rimg= img.clone();
    for (auto dp:pts25)
    {
        shl= l_c.cross(dp-center)/lenthl_c*adjuster;
        shr= c_r.cross(dp-rc1)/lenthc_r*adjuster;
        if(shl < dh && shr < ccr-Width)
        {
            shortLinel[int(shl)]+= 1;
            //img.at<uchar>(dp.y,dp.x)= 255;
        }
        if(shl < ccl-Width && shr < dh)
        {
            shortLiner[int(shr)]+= 1;
            //Rimg.at<uchar>(dp.y,dp.x)= 255;
        }
    }
    //imwrite(logpd+to_string(_imgNum)+"l.png",img);
    //imwrite(logpd+to_string(_imgNum)+"r.png",Rimg);
    //cout << endl << gg << endl;
    //cp= center;
    //for_each(shortLinel.begin(),shortLinel.end(),[](const int& i) {cout << i << " ";});
    //cout << endl;

    for(int i= Width;i < dh-2*Width-1;i++)
    {
        cw= 0;
        convolution= 0;
        for(auto &g:GV) convolution+= g*shortLinel[i+(cw++)];
        shortLineL[i+Width]= (convolution);
    }
    for(int i= Width;i < dh-2*Width-1;i++)
    {
        cw= 0;
        convolution= 0;
        for(auto &g:GV) convolution+= g*shortLiner[i+(cw++)];
        shortLineR[i+Width]= (convolution);
    }

    shortLineL2.clear();
    shortLineR2.clear();
    nc= 0;m= 0;
    for(auto &it:shortLineL)
    {
        //_log << "[" << nc << "]: " << it << " " << shortLinel[nc] << endl;
        printf("[%d]: %9.3f %d\n",nc,it,shortLinel[nc]);
        if(it > m && nc > 2*Width)
        {
            m= it;
            ccl= nc;
        }
        if(it > 2) shortLineL2.push_back(it);
        nc++;
    }
    int ssl= 3;
    if(shortLineL2.size() < 3) ssl= 0;
    //std::nth_element(shortLineL2.begin(),shortLineL2.begin()+shortLineL2.size()/2,shortLineL2.end());
    //double median = shortLineL2[shortLineL2.size()/2];
    auto max= max_element(shortLineL2.begin(),shortLineL2.end());double max_l= *max;
    double halfMax= (*max)/2;
    int lcount= 1;
    auto it= max;
    while (*(++it) > halfMax) lcount++;
    it= max;
    while (*(--it) > halfMax) lcount++;

    _log << "\nLmax: " << halfMax << " at " << ccl << "cc lcount: " << lcount << endl;

    //if(max < 2*median) return -3;



    cout << _imgNum << "p: " << endl;
    int ew= 0;
    getConfigEntry(_config, "ERASE_WELDED", ew);

    if(lcount > 2*leftWidth && ew) return -4;

    nc= 0;m= 0;
    for(auto &it:shortLineR)
    {
        //_log << "[" << nc << "]: " << it << " " << shortLiner[nc] << endl;
        printf("[%d]: %9.3f %d\n",nc,it,shortLiner[nc]);
        if(it > m && nc > 2*Width)
        {
            m= it;
            ccr= nc;
        }
        if(it > 2) shortLineR2.push_back(it);
        nc++;
    }
    cout << endl;
    int ssr= 3;
    if(shortLineR2.size() < 3) ssr= 0;
    //std::nth_element(shortLineR2.begin(),shortLineR2.begin()+shortLineR2.size()/2,shortLineR2.end());
    //median = shortLineR2[shortLineR2.size()/2];
    max= max_element(shortLineR2.begin(),shortLineR2.end());double max_r= *max;
    halfMax= (*max)/2;
    int rcount= 1;
    it= max;
    while (*(++it) > halfMax) rcount++;
    it= max;
    while (*(--it) > halfMax) rcount++;

    _log << "\nRmax: " << halfMax << " at " << ccr << "cc rcount: " << rcount << endl;

    if(rcount > 2*rightWidth && ew) return -4;

//    int ph= (ceil(center.y)+Width < _imgp.rows) ? ceil(center.y)+Width : _imgp.rows;
//    Rect2i roip= Rect2i(0,0,img.cols,ph);
//    simg= _imgp(roip).clone();


    if(max_l > 22 && ssl)
    {
        double clc= cl-ccl*sqrt(kl*kl+1);
        rc1.x= (cr-clc)/(kl-kr);
        rc1.y= rc1.x*kr +cr;
        //circle(simg,rc1,1,Scalar(0,0,255),2);
        //circle(simg,rc1,15,Scalar(255,0,0),1,LINE_AA);
        rwp= rc1;

    }
    else
    {
        mask= 0;
        line(mask,rc1,center,Scalar(255),Width,LINE_8);
        ptsr.clear();
        findNonZero(mask,ptsr);
        roir.y= (ptsr.begin())->y;
        roir.height= (ptsr.end()-1)->y-roir.y+1;
        x1= (ptsr.begin())->x;
        x2= (ptsr.begin())->x;
        for(auto &p:ptsr)
        {
            if(p.x < x1) x1=p.x;
            if(p.x > x2) x2=p.x;
        }
        roir.x= x1;
        roir.width= x2-x1+1;
        cout << roir << endl;
        Mat simgr;
        img.copyTo(simgr,mask);
        medianBlur(simgr(roir),simgr(roir),5);
        threshold(simgr(roir),simgr(roir),15,255,THRESH_OTSU);


        if(_save > 0) imwrite(logpd+to_string(_imgNum)+"ir.png",simgr);
        Moments mm= moments(simgr,true);
        if(mm.m00 > 50)
        {
            Point2d deltaw= (rc1-center)/norm(rc1-center);
            rc1.x-= (rc1.x-mm.m10/mm.m00)*2;
            rc1.y= rc1.x*kr +cr;
            rwp= rc1-Width*deltaw;
        }
    }

    if(max_r > 22 && ssr)
    {
        double crc= cr-ccr*sqrt(kr*kr+1);
        lc1.x= (crc-cl)/(kl-kr);
        lc1.y= lc1.x*kl +cl;
        //circle(simg,lc1,1,Scalar(0,0,255),2);
        //circle(simg,lc1,15,Scalar(255,0,0),1,LINE_AA);
        lwp= lc1;
    }
    else
    {
        mask= 0;
        line(mask,lc1,center,Scalar(255),Width,LINE_8);
        ptsl.clear();
        findNonZero(mask,ptsl);
        roil.y= (ptsl.begin())->y;
        roil.height= (ptsl.end()-1)->y-roil.y+1;
        int x1= (ptsl.begin())->x,x2= (ptsl.begin())->x;
        for(auto &p:ptsl)
        {
            if(p.x < x1) x1=p.x;
            if(p.x > x2) x2=p.x;
        }
        roil.x= x1;
        roil.width= x2-x1+1;
        cout << roil << endl;
        Mat simgl;
        img.copyTo(simgl,mask);
        medianBlur(simgl(roil),simgl(roil),5);
        threshold(simgl(roil),simgl(roil),15,255,THRESH_OTSU);
        if(_save > 0) imwrite(logpd+to_string(_imgNum)+"il.png",simgl);
        Moments mm= moments(simgl,true);
        if(mm.m00 > 50)
        {
            Point2d deltaw= (center-lc1)/norm(center-lc1);
            lc1.x+= (mm.m10/mm.m00-lc1.x)*2;
            lc1.y= lc1.x*kl +cl;
            lwp= lc1+Width*deltaw;
        }
    }

    return 0;

}
