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
#include "lasermodel3.h"

using namespace std;
using namespace Eigen;
using namespace walgo;

const double dampYIU = 0;
const double dampYIV = 0;
const double dampZIU = 0;
const double dampZIV = 0;

void lasermodel3::solveyz()
{
	MatrixXd Ay((int)NUM_BASIS, (int)NUM_BASIS);
	MatrixXd Az((int)NUM_BASIS, (int)NUM_BASIS);
	VectorXd by((int)NUM_BASIS);
	VectorXd bz((int)NUM_BASIS);
	for (int i = 0; i < NUM_BASIS; i++)
	{
		for (int j = 0; j < NUM_BASIS; j++)
		{
			Ay(i,j) = _amatrixy[i][j];
			Az(i,j) = _amatrixz[i][j];
		}
		by[i] = _bvectory[i];
		bz[i] = _bvectorz[i];
	}
	VectorXd vy = Ay.colPivHouseholderQr().solve(by);
	VectorXd vz = Az.colPivHouseholderQr().solve(bz);
	//cout << "The solution for u model is:\n" << xu << endl;
	for ( int i = 0; i < NUM_BASIS; i++)
	{
		_yrpc.push_back(vy(i));
		_zrpc.push_back(vz(i));
	}
	return;
}

void lasermodel3::calibrate()
{
	buildAB(_cdata);
	solveyz();
}

void lasermodel3::buildAB(pairmap<float>* pm)
{
	_amatrixy.resize(NUM_BASIS, NUM_BASIS);
	_amatrixy.setZero();
	_amatrixz.resize(NUM_BASIS, NUM_BASIS);
	_amatrixz.setZero();
	_bvectory.resize(NUM_BASIS,0);
	_bvectorz.resize(NUM_BASIS,0);
	vector<double> basis(NUM_BASIS, 0);
	int i = 0;
	for ( auto & it : pm->_map)
	{
	//	cout << "reading " << i << " th data" << endl;
		if ( i==0) _x = it.second->_obj._x;
		i++;
		double u = it.second->_img._u;
		double v = it.second->_img._v;
		double y = it.second->_obj._y;
		double z = it.second->_obj._z;
		//cout << u << " " << v << " " << x << " " << y << " "<< z << endl;
		basis[C] = 1;
		basis[U] = u;
		basis[V] = v;
		basis[IU] = -y*u;
		basis[IV] = -y*v;
		for ( int i = 0; i < NUM_BASIS; i++)
		{
			for ( int j = 0; j < NUM_BASIS; j++)
			{
				_amatrixy[i][j] += basis[i]*basis[j];
			}
		}
		basis[IU] = -z*u;
		basis[IV] =  -z*v;
		for ( int i = 0; i < NUM_BASIS; i++)
		{
			for ( int j = 0; j < NUM_BASIS; j++)
			{
				_amatrixz[i][j] += basis[i]*basis[j];
			}
		}
		_bvectory[C] += y;
		_bvectory[U] += y*u;
		_bvectory[V] += y*v;
		_bvectory[IU] += -y*y*u;
		_bvectory[IV] += -y*y*v;
		_bvectorz[C] += z;
		_bvectorz[U] += z*u;
		_bvectorz[V] += z*v;
		_bvectorz[IU] += -z*z*u;
		_bvectorz[IV] += -z*z*v;
	}
	cout << "accumulated " << i << " samples " << endl;
	_amatrixy[IU][IU] += dampYIU*i;
	_amatrixy[IV][IV] += dampYIV*i;
	_amatrixz[IU][IU] += dampZIU*i;
	_amatrixz[IV][IV] += dampZIV*i;

}


void lasermodel3::imgToObj(float u, float v, float& y, float& z, float& x)
{
	y = (_yrpc[C] + _yrpc[U]*u + _yrpc[V]*v)/(1.0+_yrpc[IU]*u+_yrpc[IV]*v);
	z = (_zrpc[C] + _zrpc[U]*u + _zrpc[V]*v)/(1.0+_zrpc[IU]*u+_zrpc[IV]*v);
	return;
}


