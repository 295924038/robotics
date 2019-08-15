/*
 * testcircle.cc
 *
 *  Created on: Jul 2, 2017
 *      Author: zhian
 */
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include <iostream>
using namespace cv;
using namespace std;


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

	if ( argc < 2 ) {
		cout << "usage: " << argv[0] << " <image file>" << endl;
		exit(1);
	}


	string filename(argv[1]);
	Mat img = imread(filename, 0);
	if(img.empty())
	{
		cout << "can not open " << filename << endl;
		return -1;
	}
	imshow(filename, img);
	setMouseCallback(filename, onMouseG, &img );
	waitKey();

	return 0;
}



