#include "lasermodel.h"
#include <iostream>

using namespace std;
using namespace walgo;

lasermodel::lasermodel()
	: _cdata(NULL)
{
	_cameraSizeX = 1292;
	_cameraSizeY = 964;
	_centerX = 646;
	_centerY = 482;
}

void lasermodel::calibrate()
{
	// find out model for x = bz + c;
	float sumv = 0.0;
	float sumv2 = 0.0;
	float sumz = 0.0;
	float sumzv = 0.0;

	float sumu = 0.0;
	float sumu2 = 0.0;
	float sumx = 0.0;
	float sumxu = 0.0;
	int n = _cdata->_map.size();
	for ( auto && it : _cdata->_map) {
		float z= it.second->_obj._z;
		float x= it.second->_obj._x;
		float v = it.second->_img._v;
		float u = it.second->_img._u;
		_a = it.second->_obj._y;
		sumz += z;
		sumx += x;
		sumv += v;
		sumu += u;
		sumzv += z*v;
		sumv2 += v*v;
		sumxu += x*u;
		sumu2 += u*u;
	}
	_b = (n*sumzv - sumz*sumv)/(n*sumv2-sumv*sumv);
	_c = (sumz - _b*sumv)/n;
	_d = (n*sumxu - sumx*sumu)/(n*sumu2-sumu*sumu);
	_e = (sumx - _d*sumu)/n;
       cout << "_a = " << _a << endl;
       cout << "_b = " << _b << endl;
       cout << "_c = " << _c << endl;
       cout << "_d = " << _d << endl;
       cout << "_e = " << _e << endl;
}

void lasermodel::imgToObj(float u, float v, float& z, float& x, float& y)
{
	z = _b*v + _c;
	y = _a;
	x = _d*u + _e;
	return;
}

void lasermodel::objToImg(float x, float z, float& u, float& v)
{
	v = (z-_c)/_b;
	u = (x-_e)/_d;
	return;
}
