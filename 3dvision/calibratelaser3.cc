/*
 * calibratelaser.cc
 *
 *  Created on: Jun 7, 2016
 *      Author: zhian
 */

#include "lasermodel3.h"
#include "3dutils.h"
#include <iostream>

using namespace std;
using namespace walgo;

int main()
{
	pairmap<float> pm;
	if (!readDataLaser("laser4_s.txt", pm) )
	{
		cout << "unable to read data" << endl;
	}

	lasermodel3 lmodel;
	lmodel.setData(pm);
	lmodel.calibrate();
	float x,y,z;
	float rmsz = 0.0;
	float rmsy = 0.0;
	for ( auto && it : pm._map)
	{
		float u =  it.second->_img._u;
		float v =  it.second->_img._v;
		float xc, yc, zc;
		lmodel.imgToObj(u, v, zc, xc, yc);
		float zerr = zc - it.second->_obj._z;
		float yerr = yc - it.second->_obj._y;
		rmsz += zerr*zerr;
		rmsy += yerr*yerr;
	}
	float n = pm._map.size();
	cout << "number of samples = " << n << endl;
	rmsz = rmsz/pm._map.size();
	rmsy = rmsy/pm._map.size();
	rmsz = sqrt(rmsz);
	rmsy = sqrt(rmsy);
	cout << "rms for z: " << rmsz << " rms for y: " << rmsy << endl;

	pairmap<float> pmv;
	if (!readDataLaser("laser4_l.txt", pmv) )
	{
		cout << "unable to read data" << endl;
	}
	rmsz = 0;
	rmsy = 0;
        double maxerrz = 0;
        double maxerry = 0;
        
	for ( auto && it : pmv._map)
	{
		float u =  it.second->_img._u;
		float v =  it.second->_img._v;
		float xc, yc, zc;
		lmodel.imgToObj(u, v, zc, xc, yc);
		float zerr = zc - it.second->_obj._z;
		float yerr = yc - it.second->_obj._y;
		rmsz += zerr*zerr;
		rmsy += yerr*yerr;
                if ( maxerrz < zerr ) maxerrz = zerr;
                if ( maxerry < yerr ) maxerry = yerr;
	}
	n = pmv._map.size();
	cout << "number of validation samples = " << n << endl;
	rmsz = rmsz/pmv._map.size();
	rmsy = rmsy/pmv._map.size();
	rmsz = sqrt(rmsz);
	rmsy = sqrt(rmsy);
	cout << "rms for z: " << rmsz << " rms for y: " << rmsy << endl;
        cout << "max error for z: " << maxerrz << "  maxerror for y: " << maxerry << endl;

}



