/*
 * intersect.h
 *
 *  Created on: Apr 11, 2016
 *      Author: zhian
 */

#ifndef LINEANALYSIS_H_
#define LINEANALYSIS_H_
#include <opencv2/opencv.hpp>
#include <vector>

using std::vector;
using cv::Point;
using cv::Vec4i;

namespace walgo
{
bool calcAngleHistogram(const vector<Vec4i>& lines,
			vector<vector<Vec4i> >& hist,
			vector<double>& weights,
			int binsize);

bool selectByAngle(const vector<Vec4i>& lines,
			       vector<Vec4i>& selectedLines,
				   double angle,
				   double angleRange);

bool getMax2Groups(const vector<vector<Vec4i> >& hist,
			vector<double>& weights,
			int numbins,
			vector<Vec4i>& group1,
			vector<Vec4i>& group2,
			int minGroupDist);

bool selectMaxAngles(const vector<vector<Vec4i> >& hist,
		             vector<double>& weights,
			         int numbins,
			         vector<Vec4i>& selectedLines,
					 int angleRangeInBins);

bool getRanges(const vector<Vec4i>& group, int& xmin, int& xmax,
			int& ymin, int& ymax);

bool groupIntersect(std::vector<cv::Vec4i>& group1,
				    std::vector<cv::Vec4i>& group2,
				    cv::Point& point);
}
#endif /* LINEANALYSIS_H_ */
