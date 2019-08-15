/*
 * testcircle.cc
 *
 *  Created on: Jul 2, 2017
 *      Author: zhian
 */
#include <opencv2/opencv.hpp>
#include <iostream>
#include <utils.h>
#include <contourseamdetector.h>
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

void stretchImage(const Mat& input, Mat& output, int min, int max)
{
	output= input.clone();
	float scale = 255.0/((float)max-(float)min);
	for ( int i = 1; i < input.rows; i++)
	{
		for (int j = 1; j < input.cols; j++)
		{

			float x  = (float) input.at<uchar>(i,j);
			float y =  (x-min)*scale;
			if ( y < 0) y = 0;
			if (y > 255) y = 255;
			unsigned iy = floor(y);
			output.at<uchar>(i,j) = 255-(uchar)y;
		}
	}

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
	ContourSeamDetector csd;

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
		csd.detectSeam(img, contours, seams, config);

		//img = img(roi);

		for ( size_t i = 0; i < contours.size(); i++) {
			size_t count = contours[i].size();
			if( count < 20 )
				continue;

			drawContours(img, contours,(int)i, Scalar(0,0,255), 1, 8);
		}
		for ( auto && it : seams )
			circle(img, it, 10, Scalar(0,255,0));
		imshow("contour", img);
		setMouseCallback( "contour", onMouseG, &img );
		waitKey();
	}


	return 0;
}



