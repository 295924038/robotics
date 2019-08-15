/*
 * fitpoly.cc
 *
 *  Created on: Nov 6, 2017
 *      Author: zhian
 */

#include "fitpoly.h"
#include "Eigen/Dense"
using namespace  std;
using namespace Eigen;

namespace walgo 
{
template <>
walgo::fitpoly<double>::fitpoly(const std::vector<double>& xdata,
				const std::vector<double>& ydata,
				int order)
{
	_n = order;
	double min = 1e20;
	double max = -1e20;
	for ( auto&& it : xdata) {
		if (it > max ) max = it;
		if ( it < min ) min = it;
	}
	double scale = 1.0/(max-min);
	double offset = min;
	vector<double> xdata1(xdata.size());
	for ( int i = 0; i <  xdata.size(); i++)
		xdata1[i] = (xdata[i]-offset)*scale;
	MatrixXd A(_n+1, _n+1);
	VectorXd b(_n+1);
	A.setZero(_n+1, _n+1);
	b.setZero(_n+1);
	for ( int l = 0; l < xdata1.size(); l++)
	{
		for ( int i = 0; i < _n+1; i++)
		{
			for ( int j = 0; j <= i; j++)
			{
				A(i, j) += pow(xdata1[l], i+j);
			}
			b[i] += pow(xdata1[l], i)*ydata[l];
		}
	}
	for (int i = 0; i < _n+1; i++)
	{
		for ( int j = i+1; j < _n+1; j++) {
			A(i,j) = A(j,i);
		}
	}
	VectorXd s = A.fullPivLu().solve(b);
	vector<double> c(_n+1);
	for ( int i = 0; i < _n+1; i++)
	{
		c[i] = s[_n-i];
	}
	_poly.init(c, scale, offset);
}
}
