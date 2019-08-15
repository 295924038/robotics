/*
 * testcircle.cc
 *
 *  Created on: Jul 2, 2017
 *      Author: zhian
 */
#include <opencv2/opencv.hpp>
#include <iostream>
#include <utils.h>
#include <enddetector.h>
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
	if (!readConfig("config.2", config))
		cout <<"No trackconfig file " << endl;
	int x = config["ROI_X"];
	int y = config["ROI_Y"];
	int w = config["ROI_WIDTH"];
	int h = config["ROI_HEIGHT"];
	cout << "ROI: "<< x <<" "<< y << "  "<< w << "  "<< h << endl;
	Rect roi = Rect(x,y,w,h);


	EndDetector edd;

	string filename = string("/home/zhian/Downloads/1015/scan0.jpg");
	cout << " reading " << filename << endl;
	Mat img = imread(filename, IMREAD_COLOR);
	if(img.empty())
	{
		cout << "can not open " << filename << endl;
		return -1;
	}
	img = img(roi);
	Point2f endPoint;
	EndDetector::END_LOCATION loc = EndDetector::MIDDLE;
	edd.detect(img,
				endPoint,
				loc,
				config);

	//img = img(roi);

	circle(img, endPoint, 10, Scalar(0,255,0));
	imshow("end", img);
	setMouseCallback( "end", onMouseG, &img );
	waitKey();


	return 0;
}



