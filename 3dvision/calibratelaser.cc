/*
 * calibratelaser.cc
 *
 *  Created on: Jun 7, 2016
 *      Author: zhian
 */

#include "lasermodel2.h"
#include "3dutils.h"
#include <iostream>

using namespace std;
using namespace walgo;

int main()
{
	pairmap<float> pm;
	if (!readDataLaser("laser2.txt", pm) )
	{
		cout << "unable to read data" << endl;
	}

	lasermodel2 lmodel;
	lmodel.setData(pm);
	lmodel.calibrate();
	float x,y,z;
	float rmsz = 0.0;
	float rmsx = 0.0;
	for ( auto && it : pm._map)
	{
		float u =  it.second->_img._u;
		float v =  it.second->_img._v;
		float xc, yc, zc;
		lmodel.imgToObj(u, v, zc, xc, yc);
		float zerr = zc - it.second->_obj._z;
		float xerr = xc - it.second->_obj._x;
		rmsz += zerr*zerr;
		rmsx += xerr*xerr;
	}
	float n = pm._map.size();
	rmsz = rmsz/pm._map.size();
	rmsx = rmsx/pm._map.size();
	rmsz = sqrt(rmsz);
	rmsx = sqrt(rmsx);
	cout << "rms for z: " << rmsz << " rms for x: " << rmsx << endl;
}



