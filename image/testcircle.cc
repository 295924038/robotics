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
	/*
	if ( argc < 2 ) {
		cout << "usage: " << argv[0] << " <image file>" << endl;
		exit(1);
	}
    */
	for ( int n = 1975; n <= 1978; n++)  {
		string filename = string("image_0702/IMG_")+std::to_string(n)+string(".JPG");
		Mat img = imread(filename, IMREAD_COLOR);
		if(img.empty())
		{
			cout << "can not open " << filename << endl;
			return -1;
		}

		Mat temp, gray;
		cvtColor(img, temp, COLOR_BGR2GRAY);
		stretchImage(temp, gray, 20, 70);
		GaussianBlur( gray, gray, Size(9, 9), 2, 2 );
		/*
		Ptr<Feature2D> blobsDetector = SimpleBlobDetector::create();
		vector<KeyPoint> keypoints;
		blobsDetector->detect(gray, keypoints);
		Mat drawImage = img.clone();
		for (size_t i = 0; i < keypoints.size(); ++i)
			circle(drawImage, keypoints[i].pt, 4, Scalar(255, 0, 0), -1);
		imshow("detected blogs", drawImage);
		waitKey();
        */

		imshow("inverted image", gray);
		setMouseCallback( "inverted image", onMouseG, &gray );
		waitKey();
		//medianBlur(gray, gray, 5);
		Mat bImg;
		threshold(gray, bImg, 200, 255, 0);
		imshow("binary image", bImg);
		waitKey();
		vector<vector<Point> > contours;
	    findContours(bImg, contours, RETR_LIST, CHAIN_APPROX_NONE);
	    for(size_t i = 0; i < contours.size(); i++)
	    {
	        size_t count = contours[i].size();
	        if( count < 6 )
	            continue;
	        Mat pointsf;
	        Mat(contours[i]).convertTo(pointsf, CV_32F);
	        RotatedRect box = fitEllipse(pointsf);
	        //drawContours(img, contours, (int)i, Scalar::all(255), 1, 8);
	        ellipse(img, box, Scalar(255,0,0), 1, LINE_AA);
	        Point2f vtx[4];
	        box.points(vtx);
	        for( int j = 0; j < 4; j++ )
	            line(img, vtx[j], vtx[(j+1)%4], Scalar(255,0,0), 1, LINE_AA);
	        cout << "eclipse detected " << i << endl;
	    }
	    imshow("fit eclipse", img);
	    waitKey();
		int minDist = 200;
		int edge = 100;
		int center = 20;
		vector<Vec3f> circles;
		HoughCircles(gray, circles, HOUGH_GRADIENT, 1,
				minDist, edge, center, 120, 180);
		for( size_t i = 0; i < circles.size(); i++ )
		{
			Vec3i c = circles[i];
			circle( img, Point(c[0], c[1]), c[2], Scalar(0,0,255), 2, LINE_AA);
			circle( img, Point(c[0], c[1]), 2, Scalar(0,255,0), 2, LINE_AA);
		}
		imshow("hough detected circles", img);
		setMouseCallback( "hough detected circles", onMouseC, &img );
		waitKey();
	}
	return 0;
}



