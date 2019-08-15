/*
 * contourseamdetector.cc
 *
 *  Created on: Aug 12, 2017
 *      Author: zhian
 */

#include "contourseamdetector.h"
#include <opencv2/features2d.hpp>
#include <map>
#include "utils.h"

using namespace walgo;
using namespace cv;
using namespace std;

double getCurvature(vector<Point>& pts)
{


}

void minmax(vector<Point> pts, int& minx, int& maxx, int& miny, int& maxy,
							   int& iminx, int& imaxx, int& iminy, int& imaxy)
{
	minx = 1279;
	maxx = 0;
	miny = 1023;
	maxy = 0;
	for ( int i = 0; i < pts.size(); i++) {

		if (pts[i].x < minx)  {
			minx = pts[i].x;
			iminx = i;
		}
		if (pts[i].x > maxx)  {
			maxx = pts[i].x;
			imaxx = i;
		}
		if (pts[i].y < miny)  {
			miny = pts[i].y;
			iminy = i;
		}
		if (pts[i].y > maxy)  {
			maxy = pts[i].y;
			imaxy = i;
		}
	}
	return;
}

bool ContourSeamDetector::detectSeam(const cv::Mat img,
			vector<vector<Point> >& contours,
			vector<Point>& points,
			map<string, int>& config)
{
	int x0 = 0;
	getConfigEntry(config, "ROI_X", x0);
	int y0 = 0;
	getConfigEntry(config, "ROI_Y", y0);
	int w = 1280;
	getConfigEntry(config, "ROI_WIDTH", w);
	int h = 1024;
	getConfigEntry(config, "ROI_HEIGHT", h);
	int tailOnLeft = 1;
	getConfigEntry(config, "TAIL_ON_LEFT", tailOnLeft);
	int tailOnRight = 0;
	getConfigEntry(config, "TAIL_ON_RIGHT", tailOnRight);

	int border = 10;
	getConfigEntry(config, "CONTOUR_LIMIT", border);
	int borderw = (border/100.0)*w;
	int borderh = (border/100.0)*h;
	int xlower = 0+borderw;
	int ylower = 0+borderh;
	int xupper = w-1-borderw;
	int yupper = h-1-borderh;
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);

	int gs = 9;
	getConfigEntry(config, "GAUSSIAN_SIZE", gs);

	GaussianBlur(gray, gray, Size(gs, gs), 2, 2 );
	//setMouseCallback( "blurred image", onMouseG, &gray );
	//imshow("blurred image", gray);
	//waitKey();
	Mat bImg;

	threshold(gray, bImg, 20, 255, 0);
	//imshow("binary image", bImg);
	//waitKey();
   /*
	Mat dist;
    distanceTransform(bImg, dist, CV_DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1., NORM_MINMAX);
    imshow("Distance Transform Image", dist);

    waitKey();
	vector<vector<Point> > contours;
	*/
	vector<vector<Point> > simpleContours;

	findContours(bImg, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	//simpleContours.resize(contours.size());
	vector<int> minx;
	vector<int> miny;
	vector<int> maxx;
	vector<int> maxy;
	vector<int> iminx;
	vector<int> iminy;
	vector<int> imaxx;
	vector<int> imaxy;

	int n = 0;
	Vec4d tailLeft;
	Vec4d tailRight;
	double xminright, xmaxleft;
	Vec4d leftline, rightline;
	// estimate of minimum tail length
	int s = 50;
	getConfigEntry(config, "MIN_TAIL_SIZE", s);
	int threshContour = 100;
	getConfigEntry(config, "THRESH_CONTOUR_POINTS", threshContour);
	bool haveRight = false;
	bool haveLeft = false;
	for(size_t i = 0; i < contours.size(); i++)
	{
		size_t count = contours[i].size();
		if( count < threshContour )
			continue;
		//approxPolyDP(Mat(contours[i]), simpleContours[i], 3, true);
		n++;
		minx.resize(n);
		maxx.resize(n);
		miny.resize(n);
		maxy.resize(n);
		iminx.resize(n);
		imaxx.resize(n);
		iminy.resize(n);
		imaxy.resize(n);
		Vec4d l;
		fitLine(contours[i], l, DIST_L2, 0, 0.01,0.01);
		double vx = l[0];
		double vy = l[1];
		double x = l[2];
		double y = l[3];
		cout << "vx,vy,x,y = " << vx << "  " << vy << "  " << x  << "  " << y << endl;

		minmax(contours[i], minx[n-1],maxx[n-1], miny[n-1], maxy[n-1],
				iminx[n-1], imaxx[n-1], iminy[n-1], imaxy[n-1]);
		cout << "minx=" << minx[n-1] << " maxx="<< maxx[n-1]<<" miny="<< miny[n-1] << " maxy="<< maxy[n-1] << endl;

		if ( minx[n-1] <= xlower) {
			cout << "found left line " << minx[n-1] << endl;
			haveLeft = true;
			// left
			leftline = l;
			int j = imaxx[n-1];
			points.push_back(contours[i][j]);
			xmaxleft = maxx[n-1];
			if ( tailOnLeft ) {
				int npts = contours[i].size();

				int start = j-s;
				int end = j+s;
				if ( start < 0) start += npts;
				if ( end >= npts ) end -= npts;

				vector<Point> arr(2*s+1);
				for ( size_t k = 0; k < 2*s+1; k++) {
					arr[k] = contours[i][(start+k)%npts];
				}

				fitLine(arr, tailLeft,  DIST_L2, 0, 0.01,0.01);
				cout << "fitted tail = " << tailLeft[0]<< " "<< tailLeft[1] << " " << tailLeft[2] << " " << tailLeft[3] << endl;
			}
		}
		else if ( maxx[n-1] >= xupper) {
			cout << "found right line " << maxx[n-1] << endl;
			// right
			haveRight = true;
			rightline = l;
			points.push_back(contours[i][iminx[n-1]]);
			xminright = minx[n-1];
			int j = iminx[n-1];
			if ( tailOnRight) {
				int npts = contours[i].size();

				int start = j-s;
				int end = j+s;
				if ( start < 0) start += npts;
				if ( end >= npts ) end -= npts;

				vector<Point> arr(2*s+1);
				for ( size_t k = 0; k < 2*s+1; k++) {
					arr[k] = contours[i][(start+k)%npts];
				}

				fitLine(arr, tailRight,  DIST_L2, 0, 0.01,0.01);
				cout << "fitted tail = " << tailRight[0]<< " "<< tailRight[1] << " " << tailRight[2] << " " << tailRight[3] << endl;
			}
		}
	}
	if ( minx.size() != 2) {
		cout << " line may have been merged, failed to find seam" << endl;

		return false;
	}
	else {
		if ( haveRight && haveLeft && tailOnLeft )
		{
			double vx = rightline[0];
			double vy = rightline[1];
			double x0 = rightline[2];
			double y0 = rightline[3];
			double k = vy/vx;
			double yright = y0 + k*(xminright-x0);
			double xt = (tailLeft[0]*yright - tailLeft[3]*tailLeft[0]+tailLeft[1]*tailLeft[2])/tailLeft[1];
			points.push_back(Point(xt, yright));
		}
		if (haveLeft && haveRight && tailOnRight)
		{
			double vx = leftline[0];
			double vy = leftline[1];
			double x0 = leftline[2];
			double y0 = leftline[3];
			double k = vy/vx;
			double yleft = y0 + k*(xmaxleft-x0);
			double xt = (tailRight[0]*yleft - tailRight[3]*tailRight[0]+tailRight[1]*tailRight[2])/tailRight[1];
			points.push_back(Point(xt, yleft));
		}
	}
	cout <<"end contour finding" << endl;
	return true;
}




