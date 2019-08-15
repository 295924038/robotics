/*
 * lineseamdetector.cc
 *
 *  Created on: Aug 12, 2017
 *      Author: zhian
 */

#include "lineseamdetector.h"
#include <opencv2/features2d.hpp>
#include <map>
#include "utils.h"
#include "linemodel.h"
#include "detectors.h"
#include "bench.h"

using namespace cv;
using namespace std;
using namespace walgo;


bool LineSeamDetector::detectSeam(const cv::Mat& img,
					vector<Point>& points,
					map<string, int>& config)
{
	bench b;
	b.start();
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	b.stop();
	cout << "cvtColor used: " << b.duration() << endl;
	b.start();
	int gs = 9;
	getConfigEntry(config, "GAUSSIAN_SIZE", gs);

	GaussianBlur(gray, gray, Size(gs, gs), 2, 2 );
	//setMouseCallback( "blurred image", onMouseG, &gray );
	//imshow("blurred image", gray);
	//waitKey();
	b.stop();
	cout << " gaussian blur used: " << b.duration() << endl;
 	Mat bImg;;
 	b.start();
	int t = (int) THRESH;
	getConfigEntry(config, "LASER_DETECTOR_TYPE", t);
	cout << "laser detector type t = " << t << endl;
	detector* det = detector::getDetector((LASER_DETECTOR_TYPE)t);
	det->detect(gray, bImg, config);
	b.stop();
	cout << " detect laser used: " << b.duration() << endl;
	b.start();
	LineModel lm(bImg, 1);
	//lm.setUseOrigModel();
	int eps = 3;
	getConfigEntry(config, "LINE_MODEL_DISTANCE_THRESH",eps);
	lm.build(eps);
	points.clear();
	lm.findMaxGap(points);
	b.stop();
	cout << "gap detection used: " << b.duration() << endl;
	//delete det;
	if ( points.size()== 2)
		return true;
	else return false;
}




