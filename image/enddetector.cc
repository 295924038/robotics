/*
 * lineseamdetector.cc
 *
 *  Created on: Aug 12, 2017
 *      Author: zhian
 */

#include "enddetector.h"
#include <opencv2/features2d.hpp>
#include <map>
#include "utils.h"
#include "linemodel.h"
#include "detectors.h"
#include "bench.h"
#include "linefeature.h"
#include "lineanalysis.h"

using namespace cv;
using namespace std;
using namespace walgo;

bool EndDetector::detect(const cv::Mat& img,
					cv::Point2f& endPoint,
					EndDetector::END_LOCATION loc,
					std::map<std::string, int>& config)
{
	bench b;
	b.start();
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	b.stop();
	cout << "cvtColor used: " << b.duration() << endl;
 	Mat bImg;;
 	b.start();
	int t = (int) THRESH;
	getConfigEntry(config, "LASER_DETECTOR_TYPE", t);
	cout << "laser detector type t = " << t << endl;
	detector* det = detector::getDetector((LASER_DETECTOR_TYPE)t);
	det->detect(gray, bImg, config);
	b.stop();
	cout << " detect laser used: " << b.duration() << endl;
	LineFeature* linefeature = LineFeature::getLineFeature(HOUGH);
	vector<Vec4i> lines;
	linefeature->extract(gray, bImg, lines, config);
	int angle = 0;
	getConfigEntry(config, "END_LINE_ANGLE", angle);
	cout << "angle = " << angle << endl;
	int angleRange = 15;
	getConfigEntry(config, "END_LINE_ANGLE_RANGE", angleRange);
	vector<Vec4i>  selectedLines;
	selectByAngle(lines, selectedLines, (double)angle, (double)angleRange);

	b.start();
	LineModel lm(bImg, selectedLines, 2);
	//lm.setUseOrigModel();
	int eps = 3;
	getConfigEntry(config, "LINE_MODEL_DISTANCE_THRESH",eps);
	lm.build(eps);
	int xmin, xmax, ymin, ymax;
	lm.getRange(xmin, xmax, ymin, ymax);
	b.stop();
	cout << "end detection used: " << b.duration() << endl;
	float u,v;
	if (loc == LEFT )  {
		u = xmin;
		v = lm.model(u);
	}
	else if ( loc == RIGHT) {
		u = xmax;
		v = lm.model(u);
	}
	else if ( loc == MIDDLE) {
		u = (xmin+xmax)*0.5;
		v = lm.model(u);
	}
	endPoint.x = u;
	endPoint.y = v;
	return true;
}


bool EndDetector::detect(const cv::Mat& img,
					lasermodel3& laserModel,
					point3d<float>& endPoint,
					EndDetector::END_LOCATION loc,
					std::map<std::string, int>& config)
{
	cv::Point2f p2d;
	detect(img, p2d, loc, config);
	float x,y,z;
	laserModel.imgToObj(p2d.x, p2d.y, z,x,y );
	endPoint._x = x;
	endPoint._y = y;
	endPoint._z = z;
	return true;

}
