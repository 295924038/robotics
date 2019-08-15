/*
 * lineanalysis.cc
 *
 *  Created on: Jun 24, 2016
 *      Author: zhian
 */

#include "seamdetector.h"
#include "lineanalysis.h"
#include "utils.h"
#include "linefeature.h"
#include "detectors.h"
#include "linemodel.h"
#include "bsplinemodel.h"
using namespace cv;
using namespace std;
using namespace walgo;

vector<SeamDetector*> SeamDetector::_obj(NUM_SEAM_DETECTORS);

static const int DEFAULT_BIN_SIZE = 10;

SeamDetector* SeamDetector::getSeamDetector(SEAM_DETECTOR_TYPE t)
{
	if ( _obj[t]== NULL)
	{
		if ( t == INTERSECTION )
			_obj[t] = new IntSeamDetector();
		else if ( t == LINEBREAK )
			_obj[t] = new LBSeamDetector();
		else if ( t == SKEL )
			_obj[t] = new SKSeamDetector();
		else
		{
			cout << "Unsuppored seam detector type" << endl;
			return NULL;
		}
	}
	return _obj[t];
}


bool IntSeamDetector::detectSeam(const cv::Mat image,
		const std::vector<cv::Vec4i>& lines,
		std::vector<cv::Point>& points,
		map<string, int>& config)
{
	int binsize = DEFAULT_BIN_SIZE;
	getConfigEntry(config, "ANGLE_BIN_SIZE", binsize);
	cout << "binsize = " << binsize << endl;
	int minGroupDist = config["MIN_GROUP_DISTANCE"];
	int numbins = (int) floor((360.0/(double)binsize)+0.5);
	cout << "numbins = " << numbins << endl;
	vector<double> weights(numbins);
	vector<vector<Vec4i> > hist(numbins);
	calcAngleHistogram(lines, hist, weights, binsize);
	vector<Vec4i> firstGroup;
	vector<Vec4i> secondGroup;
	getMax2Groups(hist, weights, numbins, firstGroup, secondGroup, minGroupDist);
	cv::Point point;
	groupIntersect(firstGroup, secondGroup, point);
	points.push_back(point);
	return true;

}

inline void swap(int& a, int& b)
{
	int tem = a;
	a = b;
	b = tem;
}

bool LBSeamDetector::detectSeam(const cv::Mat image,
		const std::vector<cv::Vec4i>& lines,
		std::vector<cv::Point>& points,
		map<string, int>& config)
{
	int binsize = config["ANGLE_BIN_SIZE"];
	int minGroupDist = config["MIN_GROUP_DISTANCE"];
	int numLMIter = 2;
	getConfigEntry(config, "LINE_MODEL_ITER", numLMIter);
	int numbins = (int) floor((360.0/(double)binsize)+0.5);
	vector<double> weights(numbins);
	vector<vector<Vec4i> > hist(numbins);
	calcAngleHistogram(lines, hist, weights, binsize);
	vector<Vec4i> firstGroup;
	vector<Vec4i> secondGroup;
	getMax2Groups(hist, weights, numbins, firstGroup, secondGroup, minGroupDist);
	int x1min, x1max, y1min, y1max;
	int x2min, x2max, y2min, y2max;
	getRanges(firstGroup, x1min, x1max, y1min, y1max);
	cout << "first group range: x,y = " << x1min << " " << x1max << " " << y1min << " " << y1max << endl;
	getRanges(secondGroup, x2min, x2max, y2min, y2max);
	cout << "second group range: x,y = " << x2min << " " << x2max << " " << y2min << " " << y2max << endl;

	int lineModelType = 0;
	getConfigEntry(config, "LINE_MODEL_TYPE", lineModelType);
	AbsLineModel* plm1 = NULL;
	AbsLineModel* plm2 = NULL;
	if ( lineModelType == 0)
	{
		plm1 = new LineModel(image, firstGroup, numLMIter);
		plm2 = new LineModel(image, secondGroup, numLMIter);
	}
	else if ( lineModelType == 1)   // first order b-spline
	{
		int ndiv = 1;
		getConfigEntry(config, "LINE_MODEL_BSPLINE_NDIV", ndiv);
		plm1 = new BsplineModel(image, firstGroup, numLMIter, ndiv);
		plm2 = new BsplineModel(image, secondGroup, numLMIter, ndiv);
	}
	int useHoughRanges = 0;
	AbsLineModel& lm1 = *plm1;
	AbsLineModel& lm2 = *plm2;
	getConfigEntry(config, "LINE_MODEL_USE_HOUGH_RANGES", useHoughRanges);
	if ( useHoughRanges )
	{
		if ( x1max > x2max) {
			cout << "first group on right: " << endl;
			lm1.setXLow(x2max);
			lm2.setXHigh(x1min);
		}
		else {
			cout << "first group on left" << endl;
			lm1.setXHigh(x2min);
			lm2.setXLow(x1max);
		}
	}
	int eps = 3;
	getConfigEntry(config, "LINE_MODEL_EPS", eps);
	int eps2 = 20;
	getConfigEntry(config, "LINE_MODEL_EPS2", eps2);

	int ifactor = 1;
	getConfigEntry(config, "LINE_MODEL_EPS_FACTOR", ifactor);
	if ( ifactor > 1)
	{
		lm1.setEpsFactor((float)ifactor);
		lm2.setEpsFactor((float)ifactor);
	}

	lm1.build(lm2, eps, eps2);
	lm2.build(lm1, eps, eps2);

	int rebuild = 0;
	int rebuildSize = 250;
	getConfigEntry(config, "LINE_MODEL_REBUILD", rebuild);
	getConfigEntry(config, "LINE_MODEL_REBUILD_SIZE", rebuildSize);
	cv::Point intPoint;
	bool intOn1, intOn2;
	if ( rebuild )
	{
		lm1.intersect(lm2, intPoint);
	}
	lm1.getRange(x1min, x1max, y1min, y1max);
	lm2.getRange(x2min, x2max, y2min, y2max);
	bool l1P = lm1.pSlope();
	bool l2P = lm2.pSlope();
	cout << "First Group: " << x1min << " " << y1min << " " << x1max << "  " << y1max << endl;
	//
	cout << "Second Group: " << x2min << " " << y2min << " " << x2max << "  " << y2max << endl;

	if ( x1max > x2max)
	{
		// 1 on right
		if ( rebuild )
		{
			lm1.setXHigh(intPoint.x+rebuildSize);
			lm2.setXLow(intPoint.x-rebuildSize);
		}
		cout << "swapping two groups" << endl;
		swap(x1min, x2min);
		swap(x1max, x2max);
		swap(y1min, y2min);
		swap(y1max, y2max);
		swap(l1P, l2P);
	}
	else
	{
		if ( rebuild )
		{
			lm1.setXLow(intPoint.x-rebuildSize);
			lm2.setXHigh(intPoint.x+rebuildSize);
		}
	}

	if ( rebuild )
	{
		lm1.setEpsFactor(2.0);
		lm2.setEpsFactor(2.0);
		lm1.build(lm2, eps*3, eps2);
		lm2.build(lm1, eps*3, eps2);
		lm1.getRange(x1min, x1max, y1min, y1max);
		lm2.getRange(x2min, x2max, y2min, y2max);
		bool l1P = lm1.pSlope();
		bool l2P = lm2.pSlope();
		cout << "After rebuild: First Group: " << x1min << " " << y1min << " " << x1max << "  " << y1max << endl;
		//
		cout << "After rebuild: Second Group: " << x2min << " " << y2min << " " << x2max << "  " << y2max << endl;

		if ( x1max > x2max)
		{
			// 1 on right
			cout << "swapping two groups" << endl;
			swap(x1min, x2min);
			swap(x1max, x2max);
			swap(y1min, y2min);
			swap(y1max, y2max);
			swap(l1P, l2P);
		}
	}

	cv::Point point1;
	point1.x = x1max;
	point1.y = l1P? y1max : y1min;
	points.push_back(point1);
	cv::Point point2;
	point2.x = x2min;
	point2.y = l2P? y2min : y2max;
	points.push_back(point2);

	cout << "line break found p1 =  " << point1 << "   p2 = " << point2 << endl;
	//
	cv::Point  point3;
	lm1.intersect(lm2, point3);

	points.push_back(point3);
 	delete plm1;
 	delete plm2;
	return true;

}

bool SKSeamDetector::detectSeam(const cv::Mat image,
		const std::vector<cv::Vec4i>& lines,
		std::vector<cv::Point>& points,
		map<string, int>& config)
{
    return true;

}


void walgo::findSeam(Mat& image, vector<Vec4i>& lines, vector<Point>& seam, map<string, int>& config)
{
	LASER_DETECTOR_TYPE t = (LASER_DETECTOR_TYPE) config["LASER_DETECTOR_TYPE"];
	detector* det = detector::getDetector(t);

	SEAM_DETECTOR_TYPE s = (SEAM_DETECTOR_TYPE) config["SEAM_DETECTOR_TYPE"];
	SeamDetector* seamdet = SeamDetector::getSeamDetector(s);
	Mat edges, maskRaw, mask;
	int doMediumBlur = 1;
	getConfigEntry(config, "SEAM_DETECTOR_MEDIUM_BLUR", doMediumBlur);
	Mat image1 = image;
	if ( doMediumBlur )
	{
		int mediumBlurSize = 5;
		getConfigEntry(config, "SEAM_DETECTOR_MEDIUM_BLUR_SIZE", mediumBlurSize);
		medianBlur(image, image1, mediumBlurSize);
	}
	det->detect(image1, maskRaw, config);
	int useLineFeature = 1;
	getConfigEntry(config, "USE_LINE_FEATURE", useLineFeature);
	if ( useLineFeature ) {
	    //获取线特征信息
	    LineFeature* linefeature = LineFeature::getLineFeature(HOUGH);
	    linefeature->extract(image1, maskRaw, lines, config);
	    //检测焊缝特征点
	    seamdet->detectSeam(maskRaw, lines, seam, config);
	}
	else {
		cout << " please use findD2Seam function " << endl;
	}
	return;
}





