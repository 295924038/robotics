/*
 * tracker.cc
 *
 *  Created on: Nov 1, 2016
 *      Author: zhian
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include "bench.h"
#include "utils.h"
#include "pathdetector.h"

using namespace cv;
using namespace std;
using namespace walgo;

enum RobotTCP {CURRENT = 0, CAMERA, FLANGE, WELDING, VIRTUAL} ;
struct RobotPos
{
   double x, y, z, a, b, c ;
   RobotTCP tcp ;
};

//void UV2Point3D(const RobotPos &currPos, struct UV uv, RobotPos &posOut)
//{
//   Config config("etc/calibration.info") ;
//   UV tempUV = config.get<UV>("gridPointUV12");
//   uv.u -= tempUV.u ;
//   uv.v -= tempUV.v ;
//   float y, z;
//   laser_yz(uv.u, uv.v, y, z);
//   cout<< y<<" "<<z<<"\n";
//   double TCP[3] = {0, y, z};
//   point3d<double> P_Laser ;
//   laser2Base(TCP,  currPos, posOut);
//}

int main(int argc, char** argv)
{
//	if (argc < 4  || argc == 5)
//	{
//		cout << "usage: " << argv[0] << " <directory> lmin lmax" << endl;
//		cout << "or " << argv[0] << " <directory> lmin lmax smin smax" << endl;
//		cout << "where smin smax is range of images you want to display" << endl;
//		exit(0);
//	}
	Mat frame;
	string fname;

	string directory = argv[1];
	int nmin = atoi(argv[2]);
	int nmax = atoi(argv[3]);
	std::map<std::string, int> config;
    string ms = argv[4];
	myReadConfig("image.info",ms,config);
	PathDetector pd(nmax, PathDetector::D2POINT_2,config);
	int smin = nmin;
	int smax = nmax;
//	if ( argc == 6)
//	{
//		smin = atoi(argv[4]);
//		smax = atoi(argv[5]);
//		pd.setShowImage();
//		pd.setShowImageIndices(smin, smax);
//	}

	for (int l=nmin; l < nmax; l++)
	{
		bench b;
		fname = string(directory)+string("/")+to_string(l)+string(".jpg");
		//读取下个图形
                cout << "Reading " << fname << "..... "<< endl;
		frame = imread(fname, 0);
		if ( !(frame.data))
		{
	      cout << "can't read file " << fname << endl;
	      //exit(1);
	      continue;
	    }
		b.start();
		pd.addImage(frame, l);
		b.stop();
                //cout << "addImage used: " << b.duration() << endl;
	}
	vector<Point2f> epts;
	vector<Point> uLinePts, vLinePts;
	vector<vector<Point>> uSegments, vSegments;
	vector<int> endSteps;
	vector<float> uv_kc;

        if(argc == 5) uv_kc.clear();
//        else if(argc == 5) uv_kc.push_back(atoi(argv[4]));
//        else if(argc == 7)
//        {
//            uv_kc.push_back(atoi(argv[4]));
//            uv_kc.push_back(atoi(argv[5]));
//            uv_kc.push_back(atoi(argv[6]));
//        }
        else if(argc > 5)
        {
            uv_kc.clear();
            for(int i= 5;i < argc;i++)
            {
                cout << i-3 << " workpiece: " << argv[i] << endl;
                uv_kc.push_back(atoi(argv[i]));
            }
        }
        else
        {
            cout << "wrong argcv" << endl;
            exit(0);
        }
        clock_t s,e;
	pd.confirm(0);
        s= clock();
        pd.detect(epts, endSteps, uLinePts, vLinePts, uSegments, vSegments,uv_kc);
        e= clock();
        cout << "pdtime: " << e-s << endl;
    if (!uv_kc.empty())
        {
        //for(auto it : uv_kc )
        for(int i= 0;i < uv_kc.size();i++)    cout << uv_kc[i] << " ";
        cout << "$uv_kc$" <<  endl;
        }
    /*
    if (!uv_kc.empty() && !uSegments.empty())
        {
            int p0Start,p0End,p1Start,p1End;
            vector<int> L;
            int interNum= 666;
            for(auto it : uSegments )
            {
                for(auto io : it) L.push_back(io.x);
            }
            for(auto it : L) cout << it << " ";
            //cout << "$$uvSegments$$" << endl;
            sort(L.begin(),L.end());
            //for(auto it : L) cout << it << " ";
            //cout << "$$uvSegments$$" << endl;
            fstream fuv;
            fuv.open("fuv.txt",ios::out);
            p1Start= *(lower_bound(L.begin(),L.end(),interNum)); p1End= *(L.end()-1);
            p0Start= *(L.begin()); p0End= *(lower_bound(L.begin(),L.end(),interNum)-1);
            cout << "part0 start/end: " << p0Start << " , " << p0End << endl;
            cout << " UV: " << uv_kc[0]*p0Start+uv_kc[1] <<  " , " << uv_kc[2]*p0Start+uv_kc[3] << " , " << uv_kc[0]*p0End+uv_kc[1] << " , " << uv_kc[2]*p0End+uv_kc[3] << endl;
            cout << "part1 start/end: " << p1Start << " , " << p1End << endl;
            cout << " UV: " << uv_kc[0]*p1Start+uv_kc[1] <<  " , " << uv_kc[2]*p1Start+uv_kc[3] << " , " << uv_kc[0]*p1End+uv_kc[1] << " , " << uv_kc[2]*p1End+uv_kc[3] << endl;
            fuv << uv_kc[0]*p0Start+uv_kc[1] <<  " " << uv_kc[2]*p0Start+uv_kc[3] << " " << uv_kc[0]*p0End+uv_kc[1] << " " << uv_kc[2]*p0End+uv_kc[3] << " " << p0Start << " " << p0End << endl;
            fuv << uv_kc[0]*p1Start+uv_kc[1] <<  " " << uv_kc[2]*p1Start+uv_kc[3] << " " << uv_kc[0]*p1End+uv_kc[1] << " " << uv_kc[2]*p1End+uv_kc[3] << " " << p1Start << " " << p1End << endl;
            fuv.close();
        }
    //fstream uv_kc;

    if (!uv_kc.empty() && !uSegments.empty())
        {
            vector<int> L;
            int pse[8];
            int interNum[]= {225,777,1131};

            for(auto it : uSegments )
            {
                for(auto io : it) L.push_back(io.x);
            }
            for(auto it : L) cout << it << " ";
            sort(L.begin(),L.end());
            pse[0]= *L.begin(); pse[7]= *(L.end()-1);
            for(int i= 0; i < sizeof(interNum)/sizeof(interNum[0]); i++)
            {
                pse[2*i+1]= *(lower_bound(L.begin(),L.end(),interNum[i])-1);
                pse[2*i+2]= *(lower_bound(L.begin(),L.end(),interNum[i])) ;
            }
            fstream fuv;
            fuv.open("fuv.txt",ios::out);
            for ( int i = 0; i < sizeof(pse)/sizeof(pse[0])/2; i++)
            {
            cout << "part" << i << " start/end: " << pse[2*i] << " , " << pse[2*i+1] << endl;
            cout << " UV: " << uv_kc[0]*pse[2*i]+uv_kc[1] <<  " , " << uv_kc[2]*pse[2*i]+uv_kc[3] << " , " << uv_kc[0]*pse[2*i+1]+uv_kc[1] << " , " << uv_kc[2]*pse[2*i+1]+uv_kc[3] << endl;
            fuv << uv_kc[0]*pse[2*i]+uv_kc[1] <<  " " << uv_kc[2]*pse[2*i]+uv_kc[3] << " " << uv_kc[0]*pse[2*i+1]+uv_kc[1] << " " << uv_kc[2]*pse[2*i+1]+uv_kc[3]<< " " << pse[2*i] << " " << pse[2*i+1] << endl;
            }
            fuv.close();
        }*/
        fstream fuv;
        fuv.open("fuv.txt",ios::out);
        cout << " Seam start/end: ";

        for ( int i = 0; i < endSteps.size()/2; i++)
        {
            cout << endSteps[i*2] << "  " << endSteps[i*2+1] << "  ";

            fuv << endSteps[i*2] << " " << endSteps[i*2+1] << " " << epts[i*2].x << " " << epts[i*2].y << " " << epts[i*2+1].x << " " << epts[i*2+1].y << endl;
        }

	cout << endl;
        fuv.close();
//        cout<<"epts size :"<<epts.size()<<endl;
//        cout<<"endSteps size :"<<endSteps.size()<<endl;
//        cout << " Seam start/end: ";

//        Config _sys_config("etc/sys.info");
//        string strDate = getDate();
//        _logName = strDate + "_steadyTest.log";
//        ofstream log(_logName,ios::app);

//        if(epts.size() != 0 && endSteps.size() != 0){
//            for ( int i = 0; i < endSteps.size()/2; i++){
//                cout <<" pair :"<< i <<" ,"<< endSteps[i*2] << "  " << endSteps[i*2+1] << "  ";

//                UV inUV,outUV;
//                inUV.u = epts[i*2].x + _sys_config.get<int>("camera.roi_offset_x");
//                inUV.v = epts[i*2].y + _sys_config.get<int>("camera.roi_offset_y");
//                outUV.u = epts[i*2+1].x + _sys_config.get<int>("camera.roi_offset_x");
//                outUV.v = epts[i*2+1].y + _sys_config.get<int>("camera.roi_offset_y");
//                cout<<"inUV :"<<inUV.u <<" "<<inUV.v<<endl;
//                cout<<"outUV :"<<outUV.u <<" "<<outUV.v<<endl;

//                RobotPos inPos{0,0,0,0,0,0};
//                RobotPos outPos{0,0,0,0,0,0};

//                if(endSteps[i*2]>3 &&endSteps[i*2]+3 <= vec_pos.size()&&endSteps[i*2+1]>3&& endSteps[i*2+1]+3 <= vec_pos.size())
//                {
//                    for(int j = endSteps[i*2]-3; j<=endSteps[i*2]+3; j++)
//                    {
//                        inPos.x += vec_pos[j].x;
//                        inPos.y += vec_pos[j].y;
//                        inPos.z += vec_pos[j].z;
//                    }
//                    inPos.x =inPos.x/7;
//                    inPos.y =inPos.y/7;
//                    inPos.z =inPos.z/7;
//                    inPos.a = vec_pos[endSteps[i*2]].a;
//                    inPos.b = vec_pos[endSteps[i*2]].b;
//                    inPos.c = vec_pos[endSteps[i*2]].c;

//                    for(int j = endSteps[i*2+1]-3; j<=endSteps[i*2+1]+3; j++)
//                    {
//                        outPos.x += vec_pos[j].x;
//                        outPos.y += vec_pos[j].y;
//                        outPos.z += vec_pos[j].z;
//                    }
//                    outPos.x =outPos.x/7;
//                    outPos.y =outPos.y/7;
//                    outPos.z =outPos.z/7;
//                    outPos.a = vec_pos[endSteps[i*2]].a;
//                    outPos.b = vec_pos[endSteps[i*2]].b;
//                    outPos.c = vec_pos[endSteps[i*2]].c;

//                }else{
//                    inPos = vec_pos[endSteps[i*2]];
//                    outPos = vec_pos[endSteps[i*2+1]];
//                }


//                cout <<"inPos: "<< inPos <<"\n";
//                cout <<"outPos: "<< outPos <<"\n";

//                RobotPos beginPos,endPos;
//                UV2Point3D(inPos,inUV,beginPos);
//                UV2Point3D(outPos,outUV,endPos);

//                beginPos.tcp = RobotTCP::FLANGE;
//                endPos.tcp = RobotTCP::FLANGE;
//                cout <<"welding beginPos: "<< beginPos <<"\n";
//                cout <<"welding endPos: "<< endPos <<"\n";

//                log<<currTime<<" "<< seamName <<" "<<"part"<<i;
//                log<<" "<<beginPos.x<<" "<<beginPos.y<<" "<<beginPos.z
//                  <<" "<<beginPos.a<<" "<<beginPos.b<<" "<<beginPos.c;
//                log<<" "<<endPos.x<<" "<<endPos.y<<" "<<endPos.z
//                  <<" "<<endPos.a<<" "<<endPos.b<<" "<<endPos.c<<endl;
        for(auto i:uv_kc) cout << i << " ";
//        if (!uv_kc.empty())
//            {
//            cout << "uv_kc.size(): " << uv_kc.size() << endl;
//            for(auto it : uv_kc ) cout << it << " ";
//            //for(int i= 0;i < uv_kc.size();i++)    cout << uv_kc[i] << " ";
//            //cout << "$uv_kc$" <<  endl;
//            }
//        Mat ui= imread("d2mu2nd.png");
//        fstream build2ndUVImages;
//        build2ndUVImages.open("build2ndUVImages.txt",ios::in);
	return 0;
}



