/*
 * linefeature.cc
 *
 *  Created on: Jul 12, 2016
 *      Author: zhian
 */

#include <map>
#include "linefeature.h"
#include "bench.h"
#include "utils.h"

using namespace std;
using namespace cv;
using namespace walgo;

static const int DEFAULT_EDGE_LOW_THRESHOLD = 60;
static const int DEFAULT_EDGE_HIGH_THRESHOLD = 230;

vector<LineFeature*> LineFeature::_obj(NUM_LINE_FEATURES);

LineFeature* LineFeature::getLineFeature(LINE_FEATURE_TYPE t)
{
	if ( _obj[t]== NULL)
	{
		if ( t == HOUGH )
			_obj[t] = new HoughLineFeature();
		else if ( t == SKELETON)
			_obj[t]= new SkeletonLineFeature();
	}
	return _obj[t];
}

bool HoughLineFeature::extract(const Mat& image,
			const Mat& maskRaw,
			vector<Vec4i>& lines,
			map<string, int>& config)
{
	Mat edges, mask;
    int doEdgeDetect = 1;
    int houghThresh = 50;
    int houghMinLineLength = 40;
    int houghMaxLineGap = 10;
    getConfigEntry(config, "DO_EDGE_DETECTION", doEdgeDetect);
    getConfigEntry(config, "HOUGH_THRESH", houghThresh);
    getConfigEntry(config, "HOUGH_MINLINELENGTH", houghMinLineLength);
    getConfigEntry(config, "HOUGH_MAXLINEGAP", houghMaxLineGap);
	int low = DEFAULT_EDGE_LOW_THRESHOLD;
	int high = DEFAULT_EDGE_HIGH_THRESHOLD;
	bench b;
        if ( doEdgeDetect ) 
        {
            getConfigEntry(config, "EDGE_LOW_THRESHOLD", low);
            getConfigEntry(config, "EDGE_HIGH_THRESHOLD", high);
	    b.start();
	    Canny(image, edges, low, high, 3);
	    b.stop();
	    Mat edges_orig = edges.clone();
	    cout << "Edge detection used: " << b.duration() << endl;
	    int dilation_size = 2;
	    int dilation_type = MORPH_RECT;
	    b.start();
	    Mat element = getStructuringElement( dilation_type,
			Size( 2*dilation_size + 1, 2*dilation_size+1 ),
			Point( dilation_size, dilation_size ) );
            dilate(maskRaw,mask,element);
	    b.stop();
	    cout << "Mask Dilation: " << b.duration() << endl;
	    b.start();
	    bitwise_and(edges, mask, edges);
	    b.stop();
	    cout << "Masking used: " << b.duration() << endl;
        }
	b.start();
        if ( doEdgeDetect)
        	HoughLinesP(edges, lines, 1, CV_PI/180,
        				houghThresh, houghMinLineLength, houghMaxLineGap );
        else
        	HoughLinesP(maskRaw, lines, 1, CV_PI/180,
        				houghThresh, houghMinLineLength, houghMaxLineGap);
	b.stop();
	cout << "Hough transform used: " << b.duration() << endl;
	cout << "Number of lines detected: " << lines.size() << endl;
}


bool SkeletonLineFeature::extract(const Mat& image,
			const Mat& maskRaw,
			vector<Vec4i>& lines,
			map<string, int>& config)
{
	Mat skel(image.size(), CV_8UC1);
	Mat temp(image.size(), CV_8UC1);
 	int dilation_size = 1;
	int dilation_type = MORPH_RECT;
	int dilation_size2 = 2;
	int dilation_size3 = 3;
	int dilation_size4 = 4;
	Mat element = getStructuringElement( dilation_type,
			Size( 2*dilation_size + 1, 2*dilation_size+1 ),
			Point( dilation_size, dilation_size ) );

	Mat element2 = getStructuringElement( dilation_type,
				Size( 2*dilation_size2 + 1, 2*dilation_size2+1 ),
				Point( dilation_size2, dilation_size2 ) );
	Mat element3 = getStructuringElement( dilation_type,
				Size( 2*dilation_size3 + 1, 2*dilation_size3+1 ),
				Point( dilation_size3, dilation_size3 ) );
	Mat element4 = getStructuringElement( dilation_type,
				Size( 2*dilation_size4 + 1, 2*dilation_size4+1 ),
				Point( dilation_size4, dilation_size4 ) );
	cv::morphologyEx(maskRaw,maskRaw, cv::MORPH_CLOSE, element2);
	cv::morphologyEx(maskRaw, maskRaw, cv::MORPH_OPEN, element2);
	//cv::dilate(maskRaw, maskRaw, element);
	//cv::dilate(maskRaw, maskRaw, element);
	bench b;
	bool done;
	//for ( int i = 0; i < 5; i++)
	do
	{
	  cv::morphologyEx(maskRaw, temp, cv::MORPH_OPEN, element);
	  cv::bitwise_not(temp, temp);
	  cv::bitwise_and(maskRaw, temp, temp);
	  cv::bitwise_or(skel, temp, skel);
	  cv::erode(maskRaw, maskRaw, element);

	  double max;
	  cv::minMaxLoc(maskRaw, 0, &max);
	  done = (max == 0);
	} while (!done);


	//morphologyEx(skel, skel, cv::MORPH_OPEN, element);
	skel.copyTo(maskRaw);
	b.start();
	HoughLinesP(skel, lines, 1, CV_PI/180, 10, 10, 10 );
	b.stop();
	cout << "Hough transform used: " << b.duration() << endl;
	cout << "Number of lines detected: " << lines.size() << endl;
}
