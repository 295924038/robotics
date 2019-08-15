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
        cp= center;
        if(wpOpt == 0)
        {
            //cp= center;
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

            int wyc= int(center.y)-Width*2;
            if(int(center.x)-320 < 0 || int(center.x)+320 > 1280) return -6;
            Rect2i leftRoi= Rect2i(int(center.x)-320,wyc,320,Width*6);
            int th= 15,count= 30;
            int nt= 0;
            nt= getTh(dstl(leftRoi),th,count);
            cout << "th,nt: " << th << " " << nt << endl;
            if(th < 25) th= 15;
            threshold(dstl(leftRoi),dstl(leftRoi),th,255,THRESH_BINARY);
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
            th= 15;count= 30;
            nt= getTh(dstr(rightRoi),th,count);
            cout << "th,nt: " << th << " " << nt << endl;
            if(th < 25) th= 15;
            threshold(dstr(rightRoi),dstr(rightRoi),th,255,THRESH_BINARY);
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
            //return 0;
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
            //return 0;
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

        img= 0;
        Mat Rimg= img.clone();
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
        int ss= 3;
        if(shortLineL2.size() < 3) ss= 0;
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
        if(shortLineR2.size() < 3) ss= 0;
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

        if(max_l > meanl/2 && ss)
        {
            double clc= cl-ccl*sqrt(kl*kl+1);
            rc1.x= (cr-clc)/(kl-kr);
            rc1.y= rc1.x*kr +cr;
            circle(simg,rc1,1,Scalar(0,0,255),2);
            circle(simg,rc1,15,Scalar(255,0,0),1,LINE_AA);
            rwp= rc1;

        }

        if(max_r > meanr/2 && ss)
        {
            double crc= cr-ccr*sqrt(kr*kr+1);
            lc1.x= (crc-cl)/(kl-kr);
            lc1.y= lc1.x*kl +cl;
            circle(simg,lc1,1,Scalar(0,0,255),2);
            circle(simg,lc1,15,Scalar(255,0,0),1,LINE_AA);
            lwp= lc1;
        }
//        if(_endORstart || 1) _gapLen[_imgNum]= meanl+meanr;
	cp= lwp+rwp-center;
        if(_save > 0) imwrite(logpd+to_string(_imgNum)+"csimg.png",simg);
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

int getXuvW::getxuv2(vector<Vec4i>& selectedLinesL,vector<Vec4i>& selectedLinesR,Mat& img,Point2d& cp,Point2d& center,double& c0l,double& c1279r)
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
        vl.x= -1/sqrt(kl*kl+1);
        vl.y= kl*vl.x;
        //rotHalfPi(vl.x,vl.y);
        vr.x= 1/sqrt(kr*kr+1);
        vr.y= kr*vr.x;
        double leftCut1= 175*vl.x,rightCut1= 175*vr.x;
        double leftCut2= 386*vl.x,rightCut2= 386*vr.x;
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
        Mat rimg,simg,mask;//= Mat::zeros(img.rows,img.cols,CV_8UC1);
        //cvtColor(img,simg,CV_GRAY2BGR);
        //simg= img.clone();

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
        if(left.size() > 200) fitLine(left, lineLeft, DIST_L2, 0, 0.01,0.01);
        else return -4;
        if(right.size() > 200) fitLine(right, lineRight, DIST_L2, 0, 0.01,0.01);
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

        //cvtColor(img,_img,CV_GRAY2BGR);
//        line(_img,Point2d(0,cl),center,Scalar(0,255,0),1,LINE_AA);
//        line(_img,Point2d(1279,cr+kr*1279),center,Scalar(0,255,0),1,LINE_AA);
//        circle(_img,cp,15,Scalar(0,255,0),2);
//        imwrite(logpd+to_string(_imgNum)+"sl.png",_img);

        vl.x= -1/sqrt(kl*kl+1);
        vl.y= kl*vl.x;
        //rotHalfPi(vl.x,vl.y);
        vr.x= 1/sqrt(kr*kr+1);
        vr.y= kr*vr.x;

        leftCut1= 320*vl.x,rightCut1= 320*vr.x;
        lc1.x= center.x+leftCut1;
        rc1.x= center.x+rightCut1;
        lc1.y= lc1.x*kl+cl; //if(abs(lc1.y-(center.y+300*vl.y)) > 0.000001) cout << "delta ly" << lc1.y-(center.y+300*vl.y) << endl;
        rc1.y= rc1.x*kr+cr; //if(abs(rc1.y-(center.y+300*vr.y)) > 0.000001) cout << "delta ry" << rc1.y-(center.y+300*vr.y) << endl;
        Point2d l_c= lc1-center;
        Point2d c_r= center-rc1;
        double lenthl_c= norm(l_c);
        double lenthc_r= norm(c_r);
        double sh= 0;
//        double ht= 0;
//        int ct= 0;
        Mat dstl,dstr;
        //Point center(src.cols/2,src.rows/2); //旋转中心

        double scale = 1.0;  //缩放系数

        cout << center << endl;
        roi= Rect2i(0,0,img.cols,ceil(center.y)+4*Width);
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
        double angle = atan(kl)*180/CV_PI;  cout << "anglel: " << angle << endl;
        Mat rotMat = getRotationMatrix2D(center,angle,scale);
        warpAffine(simg,dstl,rotMat,simg.size(),INTER_CUBIC);
        angle= atan(kr)*180/CV_PI;  cout << "angler: " << angle << endl;
        rotMat= getRotationMatrix2D(center,angle,scale);
        warpAffine(simg,dstr,rotMat,simg.size(),INTER_CUBIC);
        //imwrite(logpd+to_string(_imgNum)+"dstl.png",dstl);
        medianBlur(dstl,dstl,5);
        medianBlur(dstr,dstr,5);

        int wyc= int(center.y)-Width*2;
        if(int(center.x)-320 < 0 || int(center.x)+320 > 1280) return -6;
        Rect2i leftRoi= Rect2i(int(center.x)-320,wyc,320,Width*6);
        threshold(dstl(leftRoi),dstl(leftRoi),15,255,THRESH_OTSU);
        //kernel= Mat(7,9,CV_8UC1,elementArray79);
        //erode(dstl(leftRoi),dstl(leftRoi),kernel);
        imwrite(logpd+to_string(_imgNum)+"dstl.png",dstl);
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
        for(int i= 0;i < Width*2;i++)
        {
            if(maxv.at<float>(0,i) > max)
            {
                tBound= i;
                break;
            }
        }
        for(int i= tBound;i < Width*6;i++)
        {
            if(maxv.at<float>(0,i) < max)
            {
                bBound= i-1;
                break;
            }
        }
        cout << tBound << " bound " << bBound << endl;
        //return 0;
        vector<int> tEnd,bEnd;
        int tBound_9= (tBound-9 > 0) ? tBound-9 : 0;
        cout << tBound_9 << endl;
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

        std::nth_element(tEnd.begin(),tEnd.begin()+tEnd.size()/2,tEnd.end());
        int medianl = tEnd[tEnd.size()/2];
        double meanl= 0;
        int counts= 0;
        for(auto &i:tEnd)
        {
            cout << i << " ";
            if(abs(i-medianl) < 4*Width)
            {
                meanl+= i;
                counts++;
            }
        }
        meanl/= counts;
        cout << tEnd.size() << " lend\n";

        Rect2i rightRoi= Rect2i(int(center.x),wyc,320,Width*6);
        threshold(dstr(rightRoi),dstr(rightRoi),15,255,THRESH_OTSU);
        imwrite(logpd+to_string(_imgNum)+"dstr.png",dstr);
        maxv= 0;
        cc= 0;
        dstr(rightRoi).convertTo(cc,CV_32FC1);
        reduce(cc,maxv,1,1);
        max, min;
        min_loc, max_loc;
        cv::minMaxLoc(maxv, &min, &max, &min_loc, &max_loc);
        cout << maxv << " " << max << " " << max_loc;
        max*= 0.39;

        //line(dstr,center,Point2d(center.x+333,center.y),Scalar(0),1,LINE_AA);
        tBound= 0;bBound= Width*6;
        for(int i= 0;i < Width*2;i++)
        {
            if(maxv.at<float>(0,i) > max)
            {
                tBound= i;
                break;
            }
        }
        for(int i= tBound;i < Width*6;i++)
        {
            if(maxv.at<float>(0,i) < max)
            {
                bBound= i-1;
                break;
            }
        }
        cout << tBound << " bound " << bBound << endl;
        //return 0;
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

        std::nth_element(tEnd.begin(),tEnd.begin()+tEnd.size()/2,tEnd.end());
        int medianr = tEnd[tEnd.size()/2];
        double meanr= 0;
        counts= 0;
        for(auto &i:tEnd)
        {
            cout << i << " ";
            if(abs(i-medianr) < 4*Width)
            {
                meanr+= i;
                counts++;
            }
        }
        meanr/= counts;
        cout << tEnd.size() << " rend\n";

        double lcount= meanl;
        leftCut1= lcount*vl.x;//,rightCut1= rcount*vr.x;
        lc1.x= center.x+leftCut1;
        lc1.y= lc1.x*kl+cl;


        double rcount= meanr;
        rightCut1= rcount*vr.x;
        rc1.x= center.x+rightCut1;
        rc1.y= rc1.x*kr+cr;

        cp= lc1+rc1-center;
        cp= center;
        if(_save > 0)
        {
        cvtColor(simg,simg,CV_GRAY2BGR);
        circle(simg,lc1,1,Scalar(0,0,255),2);
        circle(simg,lc1,15,Scalar(0,255,0),2,LINE_AA);
        circle(simg,rc1,1,Scalar(0,0,255),2);
        circle(simg,rc1,15,Scalar(0,255,0),2,LINE_AA);
        circle(simg,cp,15,Scalar(0,255,0),2,LINE_AA);
        imwrite(logpd+to_string(_imgNum)+"csimg.png",simg);
        }

        return -1;

        mask= 0;
        img= 0;
        line(mask,center,rc1,Scalar(11),25,LINE_AA);
        //imwrite(logpd+to_string(_imgNum)+"mask.png",mask);
        img= 0;
        simg.copyTo(img,mask);
        //imwrite(logpd+to_string(_imgNum)+"mi.png",img);

        roi= Rect2i(0,0,center.x,img.rows);
        threshold(img(roi),img(roi),15,255,THRESH_OTSU);
        roi= Rect2i(center.x,0,img.cols-center.x,img.rows);
        threshold(img(roi),img(roi),15,255,THRESH_OTSU);

        pts255.clear();
        findNonZero(img,pts255);
        double shl,shr,chl,chr;
        vector<int> leftLineS,rightLineS,leftLineC,rightLineC;
        leftLineS.resize(Width*4,0);
        rightLineS.resize(Width*4,0);
        leftLineC.resize(200,0);
        rightLineC.resize(200,0);
        //_log << vl.ddot(p-center)
        //Mat leftImg= Mat::zeros(img.rows,img.cols,CV_8UC1),rightImg= Mat::zeros(img.rows,img.cols,CV_8UC1);
        int wb= -2*Width,wt= 2*Width;
        for (auto &p:pts255)
        {
            dp.x= double(p.x);
            dp.y= double(p.y);
            shl= l_c.cross(dp-center)/lenthl_c;//*adjuster;
            shr= c_r.cross(dp-rc1)/lenthc_r;//*adjuster;
            if(shl > wb && shl < wt) leftLineS[int(shl-wb)]++;
            if(shr > wb && shr < wt) rightLineS[int(shr-wb)]++;
//            chl= vl.ddot(dp-center);
//            chr= vr.ddot(dp-center);
//            if(chl > 0  && chl < 200)
//            {
//                //leftLineC[int(chl)]++;
//                leftImg.at<uchar>(p)= 255;
//            }
//            if(chr > 0 && chr < 200)
//            {
//                //rightLineC[int(chr)]++;
//                rightImg.at<uchar>(p.y,p.x)= 255;
//            }
        }
        _log << to_string(_imgNum)+"leftLineS: " << endl;
        for(auto &it:leftLineS) _log << it << " ";
        _log << "\nrightLineS: " << endl;
        for(auto &it:rightLineS)  _log << it << " ";
        _log << "\nleftLineC: " << endl;
//        for(auto &it:leftLineC) _log << it << "\n";
//        _log << "\nrightLineC: " << endl;
//        for(auto &it:rightLineC) _log << it << "\n";
        //imwrite(logpd+to_string(_imgNum)+"line.png",img);
        //imwrite(logpd+to_string(_imgNum)+"left.png",leftImg);
        //imwrite(logpd+to_string(_imgNum)+"right.png",rightImg);

        double lmax= ((*max_element(leftLineS.begin(),leftLineS.end()))/3.0);
        int lb= 0,lt= 4*Width;
        bool overturn= false;
        for(int i= 0;i  < leftLineS.size();i++)
        {
            if(!overturn && leftLineS[i] > lmax)
            {
                lb= i-wt;
                overturn= true;
            }
            if(overturn && leftLineS[i] < lmax)
            {
                lt= i-wt;
                break;
            }
        }
        double rmax= ((*max_element(rightLineS.begin(),rightLineS.end()))/3.0);
        int rb= 0,rt= 4*Width;
        overturn= false;
        for(int i= 0;i  < rightLineS.size();i++)
        {
            if(!overturn && rightLineS[i] > rmax)
            {
                rb= i-wt;
                overturn= true;
            }
            if(overturn && rightLineS[i] < rmax)
            {
                rt= i-wt;
                break;
            }
        }

        //imwrite(logpd+to_string(_imgNum)+"simg.png",simg);
        vector<double> leftLineG,rightLineG;
        leftLineG.resize(200,0);
        rightLineG.resize(200,0);
        uchar* pptr= simg.data;
        cout << "pic"+to_string(_imgNum)+":\n";
        cout << lb << "lb lt" << lt << endl;
        cout << rb << "lb lt" << rt << endl;
        for(int i= 0;i < simg.rows;i++)
        {
            for(int j=0;j < simg.cols;j++)
            {
                shl= l_c.cross(Point2d(j,i)-center)/lenthl_c;
                chl= vl.ddot(Point2d(j,i)-center);
                //Point2d(Point2d(i,j)-center);
                if(shl < lt*.0 && shl > lb && chl < 200 && chl >0)
                {
                    leftLineC[int(chl)]++;
                    leftLineG[int(chl)]+= *(pptr);
                    //cout << int(*(pptr)) << " ";
                }
                shr= c_r.cross(Point2d(j,i)-rc1)/lenthc_r;
                chr= vr.ddot(Point2d(j,i)-center);
                if(shr < rt*.0 && shr > rb && chr < 200 && chr >0)
                {
                    rightLineC[int(chr)]++;
                    rightLineG[int(chr)]+= *(pptr);
                    //cout << int(*(pptr)) << " ";
                }
                pptr++;
            }
        }
        for(int i=0;i < 200;i++)
        {
            if(leftLineC[i] > 0)
            {
                leftLineG[i]/= leftLineC[i]; //cout << leftLineG[i] << endl;
            }
        }
        cout << "LineG\n";
        for(int i=0;i < 200;i++)
        {
            if(rightLineC[i] > 0)
            {
                rightLineG[i]/= rightLineC[i];//cout << rightLineG[i] << endl;
            }
        }
        vector<double> leftLineGc,rightLineGc;
        for(auto &it:leftLineG) leftLineGc.push_back(it);
        for(auto &it:rightLineG) rightLineGc.push_back(it);
        int cw= 0;
        double convolution= 0;
        for(int i= 0;i < 200-2*Width-1;i++)
        {
            cw= 0;
            convolution= 0;
            for(auto &g:GV) convolution+= g*leftLineG[i+(cw++)];
            leftLineGc[i+Width]= (convolution);
        }
        for(int i= 0;i < 200-2*Width-1;i++)
        {
            cw= 0;
            convolution= 0;
            for(auto &g:GV) convolution+= g*rightLineG[i+(cw++)];
            rightLineGc[i+Width]= (convolution);
        }
        auto lmin= *min_element(leftLineGc.begin(),leftLineGc.end());
        lmax= *max_element(leftLineGc.begin(),leftLineGc.end());
        auto rmin= *min_element(rightLineGc.begin(),rightLineGc.end());
        rmax= *max_element(rightLineGc.begin(),rightLineGc.end());

        //for();
        leftLineG.clear();
        leftLineG.resize(200,0);
        rightLineG.clear();
        rightLineG.resize(200,0);

        auto lminp= leftLineGc.begin()+10;
        auto rminp= rightLineGc.begin()+10;
         lcount= 10;
         rcount= 10;
        while(++lminp != leftLineGc.end()-Width)
        {
            lcount++;
            if(*lminp > 4*lmin && *lminp-*(lminp-Width) > 2*lmin)
            {
                leftCut1= lcount*vl.x;//,rightCut1= rcount*vr.x;
                lc1.x= center.x+leftCut1;
                lc1.y= lc1.x*kl+cl;
                if(lc1.y+12 > simg.rows-1) continue;
                int x1= ceil(lc1.x),x2= floor(lc1.x),y= round(lc1.y),countg= 0;
                for(int i= 7;i < 12;i++) countg+= simg.at<uchar>(y+i,x1)+simg.at<uchar>(y+i,x2);
                if(countg/10.0 > 2*lmin) continue;
                else break;
            }
        }
        while(++rminp != rightLineGc.end()-Width)
        {
            rcount++;
            if(*rminp > 4*rmin && *rminp-*(rminp-Width) > 2*lmin)
            {
                rightCut1= rcount*vr.x;//,rightCut1= rcount*vr.x;
                rc1.x= center.x+rightCut1;
                rc1.y= rc1.x*kr+cr;
                if(rc1.y+12 > simg.rows-1) continue;
                int x1= ceil(rc1.x),x2= floor(rc1.x),y= round(rc1.y),countg= 0;
                for(int i= 7;i < 12;i++) countg+= simg.at<uchar>(y+i,x1)+simg.at<uchar>(y+i,x2);
                if(countg/10.0 > 2*lmin) continue;
                else break;
            }
        }

        if(rminp == rightLineGc.end()-Width || lminp == leftLineGc.end()-Width) return -9;



//        for(int i= Width;i < 200-2*Width;i++) leftLineG[i]= (leftLineGc[i-Width/2]-leftLineGc[i+Width/2]);
//        auto lminp= min_element(leftLineG.begin(),leftLineG.end());
//        double lcount= 0;
//        while(lminp-- != leftLineG.begin()) lcount++;

//        for(int i= Width;i < 200-2*Width;i++) rightLineG[i]= (rightLineGc[i-Width/2]-rightLineGc[i+Width/2]);
//        auto rminp= min_element(rightLineG.begin(),rightLineG.end());
//        double rcount= 0;
//        while(rminp-- != rightLineG.begin()) rcount++;

        lcount+= (rt-rb)/2.0;
        leftCut1= lcount*vl.x;//,rightCut1= rcount*vr.x;
        lc1.x= center.x+leftCut1;
        lc1.y= lc1.x*kl+cl;


        rcount+= (lt-lb)/2.0;
        rightCut1= rcount*vr.x;
        rc1.x= center.x+rightCut1;
        rc1.y= rc1.x*kr+cr;

        cp= lc1+rc1-center;
        if(_save > 0 || 1)
        {
        cvtColor(simg,simg,CV_GRAY2BGR);
        circle(simg,lc1,1,Scalar(0,0,255),2);
        circle(simg,lc1,15,Scalar(0,255,0),2,LINE_AA);
        circle(simg,rc1,1,Scalar(0,0,255),2);
        circle(simg,rc1,15,Scalar(0,255,0),2,LINE_AA);
        circle(simg,cp,15,Scalar(0,255,0),2,LINE_AA);
        imwrite(logpd+to_string(_imgNum)+"csimg.png",simg);
        }



        return 0;

    }

    return -1;

}
