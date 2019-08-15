/*
 * abslinemodel.cc
 *
 *  Created on: Jan 18, 2018
 *      Author: zhian
 */
#include <opencv2/opencv.hpp>
#include "abslinemodel.h"
#include <algorithm>
using namespace std;
using namespace cv;
using namespace walgo;
struct {
	bool operator()(const Point& a, const Point& b) const
	{
		return a.x < b.x;
    }
} xless;

struct {
	bool operator()(const Point& a, const Point& b) const
	{
		return a.y < b.y;
    }
} yless;

void walgo::AbsLineModel::getRange(int& xmin, int& xmax, int& ymin, int& ymax)
{
	size_t s = _points.size();
	if ( s == 0 ) {
		cout << "LineModel has zero pixels" << endl;
		return;
	}
	std::sort(_points.begin(), _points.end(), xless);
	xmin = _points[0].x;
	xmax = _points[s-1].x;
	std::sort(_points.begin(), _points.end(), yless);
	ymin = _points[0].y;
	ymax = _points[s-1].y;

}

void walgo::AbsLineModel::getAllPoints(std::vector<cv::Point>& pts) const
{
	pts = _points;
}

void walgo::AbsLineModel::findMaxGap(std::vector<cv::Point>& pts)
{
	std::sort(_points.begin(), _points.end(), xless);
	int maxgap = 0;
	int maxi = 0;
	if ( _points.size() == 0 ) return;
	for ( size_t i = 0;i < _points.size()-1; i++ )
	{
		int d = _points[i+1].x - _points[i].x;
		if (d > maxgap ){
			maxi = i;
			maxgap = d;
		}
	}
	pts.push_back(_points[maxi]);
	pts.push_back(_points[maxi+1]);
}
