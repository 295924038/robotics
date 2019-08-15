/*
 * rpcmodel.cc
 *
 *  Created on: Mar 27, 2016
 *      Author: zhian
 */
#include <vector>
#include <limits>
#include <iostream>
#include <math.h>
#include "Eigen/Dense"
#include "point.h"
#include "lasermodel2.h"

using namespace std;
using namespace Eigen;
using namespace walgo;


void lasermodel2::solvexz()
{
	MatrixXd A((int)NUM_BASIS, (int)NUM_BASIS);
	VectorXd bx((int)NUM_BASIS);
	VectorXd bz((int)NUM_BASIS);
	for (int i = 0; i < NUM_BASIS; i++)
	{
		for (int j = 0; j < NUM_BASIS; j++)
		{
			A(i,j) = _amatrix[i][j];
		}
		bx[i] = _bvectorx[i];
		bz[i] = _bvectorz[i];
	}
	VectorXd vx = A.colPivHouseholderQr().solve(bx);
	VectorXd vz = A.colPivHouseholderQr().solve(bz);
	//cout << "The solution for u model is:\n" << xu << endl;
	for ( int i = 0; i < NUM_BASIS; i++)
	{
		_xrpc.push_back(vx(i));
		_zrpc.push_back(vz(i));
	}
	return;
}

void lasermodel2::calibrate()
{
	buildAB(_cdata);
	solvexz();
}

void lasermodel2::buildAB(pairmap<float>* pm)
{
	_amatrix.resize(NUM_BASIS, NUM_BASIS);
	_amatrix.setZero();
	_bvectorx.resize(NUM_BASIS,0);
	_bvectorz.resize(NUM_BASIS,0);
	vector<double> basis(NUM_BASIS, 0);
	int i = 0;
	for ( auto & it : pm->_map)
	{
	//	cout << "reading " << i << " th data" << endl;
		if ( i==0) _y = it.second->_obj._y;
		i++;
		double u = it.second->_img._u;
		double v = it.second->_img._v;
		double x = it.second->_obj._x;
		double z = it.second->_obj._z;
		//cout << u << " " << v << " " << x << " " << y << " "<< z << endl;
		basis[C] = 1;
		basis[U] = u;
		basis[V] = v;

		for ( int i = 0; i < NUM_BASIS; i++)
		{
			for ( int j = 0; j < NUM_BASIS; j++)
			{
				_amatrix[i][j] += basis[i]*basis[j];
			}
		}
		_bvectorx[C] += x;
		_bvectorx[U] += x*u;
		_bvectorx[V] += x*v;
		_bvectorz[C] += z;
		_bvectorz[U] += z*u;
		_bvectorz[V] += z*v;
	}

}


void lasermodel2::imgToObj(float u, float v, float& z, float& x, float& y)
{
	x = _xrpc[C] + _xrpc[U]*u + _xrpc[V]*v;
	z = _zrpc[C] + _zrpc[U]*u + _zrpc[V]*v;
	return;

}


