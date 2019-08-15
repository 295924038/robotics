/*
 * linemodel.cc
 *
 *  Created on: Jul 25, 2017
 *      Author: zhian
 */
#include <opencv2/opencv.hpp>
#include "bsplinemodel.h"
#include "Eigen/Dense"
using namespace Eigen;
using namespace cv;
using namespace std;
using namespace walgo;

BsplineModel::BsplineModel(const Mat& image, int ndiv, int numIter)
	:  AbsLineModel(image), _numIter(numIter)
{

	_xlow = 0;
	_xhigh = image.cols;
	_ylow = 0;
	_yhigh = image.rows;
	_nknot = ndiv+1;
	cout << "_nknot = " << _nknot << endl;
	_unit = 1280/ndiv;
	_epsFactor = 1.0;
	_k = 1.0/(double)_unit;
	_A.resize(_nknot, _nknot);
	_b.resize(_nknot);
	_b.setZero();
	_A.setZero();
	_a.resize(_nknot, 0);
	_useOrigModel = false;
	vector<Point> locations;   // output, locations of non-zero pixels
	cv::findNonZero(_image, locations);
	Vec4d line;
	fitLine(locations, line, DIST_L1, 0, 0.01,0.01);
	float vx = line[0];
	float vy = line[1];
	float x = line[2];
	float y = line[3];
	_k0 = vy/vx;
	_c0 = y - _k0*x;
	for ( int i = 0; i < _nknot; i++) {
		float x = i*_unit;
		_a[i] = _k0*x+_c0;
		cout << "_a["<< i << "]=" << _a[i] << endl;
	}
	cout << "intial k/c = " << _k0 << " " << _c0 << endl;
	//for ( auto && it : locations )
	//	build(it);
}

BsplineModel::BsplineModel(const Mat& image, const std::vector<Vec4i>& lines, int numIter, int ndiv)
	: AbsLineModel(image), _numIter(numIter)
{
	_useOrigModel = false;
	_xlow = 0;
	_xhigh = image.cols;
	_ylow = 0;
	_yhigh = image.rows;
	_nknot = ndiv+1;
	_epsFactor = 1.0;
	_unit = image.cols/ndiv;
	_a.resize(_nknot, 0);
	_k = 1.0/(double)_unit;
	_A.resize(_nknot, _nknot);
	_b.resize(_nknot);
	_b.setZero();
	_A.setZero();
	if ( lines.size() > 0)
	{
		_k0 = 0;
		_c0 = 0;
		for ( auto&& it : lines)
		{
			float x1 = it[0];
			float y1 = it[1];
			float x2 = it[2];
			float y2 = it[3];
		    float k =  (y2-y1)/(x2-x1);
			float c = y1 - k*x1;
			_k0 += k;
			_c0 += c;
		}
		_k0 = _k0/lines.size();
		_c0 = _c0/lines.size();
		for ( int i = 0; i < _nknot; i++) {
			float x = i*_unit;
			_a[i] = _k0*x+_c0;
			cout << "_a["<< i << "]=" << _a[i] << endl;
		}
		cout << "intial k/c = " << _k0 << " " << _c0 << endl;
	}

}



void BsplineModel::build(float eps)
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

float BsplineModel::model(float x) const
{
	int i = floor(x*_k);
	//cout << "original x = " << x << " linear model = " << _k0*x+_c0 << endl;
	x = (x - i*_unit);
	//cout << " x = "  << x << endl;
	if ( i < 0 ) i = 0;
	int j = i+1;
	double bi = _k*(_unit-x);
	double bj = _k*x;
	//cout << "i = " << i << " j = " << j << endl;
	//cout << " bi = "<< bi << " bj = " << bj << endl;

    return (_a[i]*bi+_a[j]*bj);
}

void BsplineModel::build(AbsLineModel& lm2, float eps1, float eps2)
{
	cout << "dual line build " << endl;
	cout << "start building, bound constraints: " << _xlow << "  " << _xhigh << "  "<< _ylow << "  "<< _yhigh << endl;
	cout << "numIter = " << _numIter << endl;
	float factor = _epsFactor;
	float eps;
	for ( int i = 0; i < _numIter; i++)
	{
		eps = eps1*pow(factor,_numIter-i);
		clearPts();
		vector<Point> locations;  // output, locations of non-zero pixels
		cv::findNonZero(_image, locations);
		cout << "total non-zero pixels: " << locations.size() << endl;
		for ( auto& it : locations)
		{
			if ( belongsTo(it, eps) && ( !(lm2.belongsTo(it, eps2))) ) {
				//cout << "found a point: " << it.x << "  " << it.y << endl;
				build(it);
			}
		}
		//eps *= factor;
		finalize();
		cout << " finish iter " << i << endl;
	}

}

bool BsplineModel::belongsTo(const Point& p, float eps)
{
	if ( p.x < _xlow || p.x > _xhigh || p.y < _ylow || p.y > _yhigh)
		return false;
	//cout << " for point " <<  p.x << " " << p.y << "  model is: " << model(p.x) << endl;
	if ( fabs(p.y-model(p.x)) < eps ) return true;
	return false;
}

void BsplineModel::intersect(const AbsLineModel& lm2, cv::Point& pt)
{
	float tol = 1e-3;

	float yleft = model(_xlow);
	float yleft2 = lm2.model(_xlow);
	float dyleft = yleft-yleft2;
	if ( fabs(dyleft) < tol ) {
		pt.x = _xlow;
		pt.y = yleft;
		return;
	}
	float yright = model(_xhigh);
	float yright2 = lm2.model(_xhigh);
	float dyright = yright-yright2;
	if ( fabs(dyright) < tol) {
		pt.x = _xhigh;
		pt.y = yright;
		return;
	}
	if ( dyleft*dyright > 0 ) {
		pt.x = -1;
		pt.y = -1;
		return;
	}
	float dy, y, y2;
	float x = (_xlow+_xhigh)*0.5;
	float xleft = _xlow;
	float xright = _xhigh;
	while ( true )
	{
		y = model(x);
		y2 = lm2.model(x);
		dy = y-lm2.model(x);
		if ( fabs(dy) < tol)
		{
			pt.x = x;
			pt.y = y;
			return;
		}
		if (dy*dyleft < 0)
		{
			dyright = dy;
			xright = x;
		}
		else
		{
			dyleft = dy;
			xleft = x;
		}
		x = (xleft+xright)*0.5;
	}
	return;
}


void BsplineModel::build(const Point& p)
{
	double x = p.x;
	double y = p.y;
	int i = floor(x/_unit);
	x = x-i*_unit;
	int j = i+1;
	double bi = _k*(_unit-x);
	double bj = _k*x;
	_A(i,j) += bi*bj;
	_A(i,i) += bi*bi;
	_A(j,j) += bj*bj;
	_A(j,i) += bi*bj;
	_b(i) += bi*y;
	_b(j) += bj*y;
	_points.push_back(p);
}

void BsplineModel::finalize()
{
	if ( _useOrigModel )  { cout << "use original model no finalize" << endl; return; }
	int N = _points.size();
	cout <<"There are " << N << " points in this model" << endl;
	VectorXd xd = _A.jacobiSvd(ComputeThinU | ComputeThinV).solve(_b);
	_a.resize(_nknot);
	for ( int i = 0; i < _nknot; i++)
	{
		_a[i] = xd[i];
	}

}



