/*
 * testcircle.cc
 *
 *  Created on: Jul 2, 2017
 *      Author: zhian
 */
#include <opencv2/opencv.hpp>
#include <iostream>
#include <utils.h>
#include <lineseamdetector.h>
#include "histimage.h"
using namespace cv;
using namespace std;
using namespace walgo;


static void onMouseC( int event, int x, int y, int f, void* data ){
 Mat* image = (Mat*) data;
 Vec3b pix=image->at<Vec3b>(y,x);
 int B=pix.val[0];
 int G=pix.val[1];
 int R=pix.val[2];
 cout<<R<<" "<<G<<" "<<B<<endl;

}

static void onMouseG( int event, int x, int y, int f, void* data ){
 Mat* image = (Mat*) data;
 uchar v =image->at<uchar>(y,x);
 cout << "at (" << x << "," << y << " )= "<< (int) v << endl;

}

int main(int argc, char** argv)
{
	map<string, int> config;
	if (!readConfig("../config.4", config)) return -1;
	cv::Mat input= imread("/home/weldroid/robotics/weldroidProject/src/UI/build-weldingRobot-Desktop_Qt_5_9_3_GCC_64bit-Debug/testLog/cameraShift/2019_03_01_09_44_21.jpg",0);
	std::vector<double> output;
	hist(input, output, config);
	for(auto o:output) cout << o;
	return 0;
}

int lmain(int argc, char** argv)
{
	map<string, int> config;
	if (!readConfig("config.2", config))
		cout <<"No trackconfig file " << endl;
	int x = config["ROI_X"];
	int y = config["ROI_Y"];
	int w = config["ROI_WIDTH"];
	int h = config["ROI_HEIGHT"];
	cout << "ROI: "<< x <<" "<< y << "  "<< w << "  "<< h << endl;
	Rect roi = Rect(x,y,w,h);
	LineSeamDetector lsd;

	for ( size_t i = 1; i < 560; i++) {
		string filename = string("/home/zhian/git-repos/testdata/Pics0820/picture/laser")+to_string(i)+string(".jpg");
		cout << " reading " << filename << endl;
		Mat img = imread(filename, IMREAD_COLOR);
		if(img.empty())
		{
			cout << "can not open " << filename << endl;
			return -1;
		}
		img = img(roi);


		vector<vector<Point>> contours;
		vector<Point> seams;
		lsd.detectSeam(img, seams, config);

		//img = img(roi);

		for ( auto && it : seams )
			circle(img, it, 10, Scalar(0,255,0));
		imshow("contour", img);
		setMouseCallback( "contour", onMouseG, &img );
		waitKey();
	}


	return 0;
}



