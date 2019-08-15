/*
 * linemodel.cc
 *
 *  Created on: Jul 25, 2017
 *      Author: zhian
 */
#include <opencv2/opencv.hpp>
#include "linemodel.h"
#include "utils.h"
using namespace cv;
using namespace std;
using namespace walgo;

walgo::LineModel::LineModel(const Mat& image, const std::vector<Vec4i>& lines, int numIter)
	: AbsLineModel(image), _ns(0), _sumx(0), _sumy(0), _sumx2(0), _sumxy(0), _numIter(numIter)
{
	_useOrigModel = false;
	_k = 0;
	_c = 0;
	_k1 = 1.0;
	_xlow = 0;
	_xhigh = image.cols;
	_ylow = 0;
	_yhigh = image.rows;
	_epsFactor = 1.0;
	if ( lines.size() > 0)
	{
		for ( auto&& it : lines)
		{
			float x1 = it[0];
			float y1 = it[1];
			float x2 = it[2];
			float y2 = it[3];
			float k = (y2-y1)/(x2-x1);
			float c = y1 - k*x1;
			_k += k;
			_c += c;
		}
		_k = _k/lines.size();
		_c = _c/lines.size();
		_k1 = 1.0/sqrt(1.0+_k*_k);
		cout << "_k1 = " << _k1 << endl;
	}

}

walgo::LineModel::LineModel(const Mat& image, const Vec4d& line, int numIter)
	: AbsLineModel(image), _ns(0), _sumx(0), _sumy(0), _sumx2(0), _sumxy(0), _numIter(numIter)
{
	_k = 0;
	_c = 0;
	_k1 = 1.0;
	_xlow = 0;
	_xhigh = image.cols;
	_ylow = 0;
	_yhigh = image.rows;
	_useOrigModel = false;
	_epsFactor = 1.0;
	float vx = line[0];
	float vy = line[1];
	float x = line[2];
	float y = line[3];
	_k = vy/vx;
	_c = y - _k*x;
	_k1 = 1.0/sqrt(1.0+_k*_k);
	cout << "_k1 = " << _k1 << endl;
}

walgo::LineModel::LineModel(const Mat& image, int numIter)
	:AbsLineModel(image), _ns(0), _sumx(0), _sumy(0), _sumx2(0), _sumxy(0), _numIter(numIter)
{
	_k = 0;
	_c = 0;
	_k1 = 1.0;
	_xlow = 0;
	_xhigh = image.cols;
	_ylow = 0;
	_yhigh = image.rows;
	_useOrigModel = false;
	_epsFactor = 1.0;
	vector<Point> locations;   // output, locations of non-zero pixels
	cv::findNonZero(_image, locations);
	Vec4d line;
	fitLine(locations, line, DIST_HUBER, 0, 0.01,0.01);
	float vx = line[0];
	float vy = line[1];
	float x = line[2];
	float y = line[3];
	_k = vy/vx;
	_c = y - _k*x;
	_k1 = 1.0/sqrt(1.0+_k*_k);
	cout << "initial k/c = " << _k << " " << _c << endl;
	cout << "_k1 = " << _k1 << endl;
}

void walgo::LineModel::build(float eps)
{
	cout << "start building, bound constraints: " << _xlow << "  " << _xhigh << "  "<< _ylow << "  "<< _yhigh << endl;
	for ( int i = 0; i < _numIter; i++) {
		clearPts();
		vector<Point> locations;   // output, locations of non-zero pixels
		cv::findNonZero(_image, locations);
		for ( auto& it : locations)
		{
			if ( belongsTo(it, eps) )
				build(it);
		}
		finalize();
	}

}

void walgo::LineModel::build(AbsLineModel& lm2, float eps1, float eps2)
{
	cout << "dual line build " << endl;
	cout << "start building, bound constraints: " << _xlow << "  " << _xhigh << "  "<< _ylow << "  "<< _yhigh << endl;
	cout << "initial model: _k = " << _k << " _c = " << _c << endl;
	cout << "numIter = " << _numIter << endl;
	float factor = _epsFactor;
	float eps;
	for ( int i = 0; i < _numIter; i++)
	{
		eps = eps1*pow(factor,_numIter-1-i);
		clearPts();
		vector<Point> locations;  // output, locations of non-zero pixels
		cv::findNonZero(_image, locations);
		cout << "total non-zero pixels: " << locations.size() << endl;
		for ( auto& it : locations)
		{
			if ( belongsTo(it, eps) && ( !(lm2.belongsTo(it, eps2))) )
				build(it);
		}
		//eps *= factor;
		finalize();
		cout << " finish iter " << i << endl;
	}

}

bool walgo::LineModel::belongsTo(const Point& p, float eps)
{
	if ( p.x < _xlow || p.x > _xhigh || p.y < _ylow || p.y > _yhigh)
		return false;
	if ( fabs(_k*p.x+_c-p.y) < eps ) return true;
	return false;
}

bool walgo::LineModel::belongsToReal(const Point& p, float eps)
{
	//if ( !(belongsTo(p, eps))) return false;
	bool retVal = false;
	float xp = p.x;
	float yp = p.y;
	for ( auto && it : _points)
	{
		float dx = xp - it.x;
		float dy = yp - it.y;
		float d = sqrt(dx*dx+dy*dy);
		if ( d <= eps )
		{
			retVal = true;
			break;
		}
	}
	return retVal;
}

void walgo::LineModel::intersect(const AbsLineModel& lm2, cv::Point& pt)
{
	float dk = _k - lm2.getK();
	float dc = lm2.getC() - _c;
	float x = dc/dk;
	float y = _k*x + _c;
	pt.x = x;
	pt.y = y;
	return;
}


void walgo::LineModel::build(const Point& p)
{
	float x = p.x;
	float y = p.y;
	_sumx += x;
	_sumy += y;
	_sumxy += x*y;
	_sumx2 += x*x;
	_points.push_back(p);

}

void walgo::LineModel::finalize()
{
	if ( _useOrigModel )  { cout << "use original model no finalize" << endl; return; }
	int N = _points.size();
	cout <<"There are " << N << " points in this model" << endl;
	float b = _sumx*_sumy - N*_sumxy;
	float a = _sumx*_sumx - N*_sumx2;
	_k = b/a;
	_c = (_sumy - _sumx*_k)/N;
	_k1 = 1.0/sqrt(1.0+_k*_k);

	cout << "final line model: _k = " << _k << " _c = " << _c << endl;
}



