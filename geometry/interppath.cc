/*
 * testspline2.cc
 *
 *  Created on: Oct 3, 2017
 *      Author: zhian
 */
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "point.h"
#include "bezierspline.h"
#include "slerpspline.h"
#include "interppath.h"

using namespace std;
using namespace walgo;

void walgo::interpPathScale(const vector<point3dd>& pos,
			    	const vector<point3dd>& euler,
					vector<point3dd>& newpos,
					vector<point3dd>& neweuler,
					int intervalIn,
					int intervalOut,
					int acc)
{

	vector<point3dd> dpos;
	vector<point3dd> deuler;
	int m = pos.size();
	cout << "sampling original data at intervalIn: " << intervalIn << endl;
	int ii = 0;
	for ( ; ii < m; ii = ii + intervalIn)
	{
		dpos.push_back(pos[ii]);
		deuler.push_back(euler[ii]);
	}
	if ( ii-intervalIn < m-1 ) {
		dpos.push_back(pos[m-1]);
		deuler.push_back(euler[m-1]);
	}
	bezierspline<double> bs(3);
	bs.initKnots(dpos);
	slerpspline<double> sls;
	sls.initKnots(deuler);
	int n = dpos.size();
	cout <<"interpolate from 0 to " << n-1<< endl;
	int nn = 2*acc*intervalOut-1;
	double dinterval = 1.0/(double)intervalOut;
	double a = dinterval/nn;
        double t = 0;
	for ( int i = 0; i < nn; i++)
	{
		t = i*(i+1)*0.5*a;
		newpos.push_back(bs.pointAt(t));
		neweuler.push_back(sls.pointAt(t));
	}
        cout << "last t after acc = " << t << endl;
	double epsilon = 1e-8;
        
	cout << "first t = " << acc << endl;
	for ( t = acc; t < (n-1-acc)+epsilon; t = t+dinterval) {
		newpos.push_back( bs.pointAt(t));
		neweuler.push_back(sls.pointAt(t));
		//ofs << p._x << " " << p._y << " " << p._z << " " << e._x << " " << e._y << " " << e._z << endl;
	}
	cout << "last t = " << t-dinterval << endl;
	for ( int i = nn-1; i > 0; i--)
	{
		t = n-1-i*(i+1)*0.5*a;
		if ( i == nn-1 ) cout << "first t = " << t << endl;
		if ( t > n-1 ) t = t-epsilon;
		newpos.push_back(bs.pointAt(t));
		neweuler.push_back(sls.pointAt(t));
	}
	cout << "last t = " << t << endl;
	return;

}

void walgo::interpPath(const vector<point3dd>& pos,
			    const vector<point3dd>& euler,
				vector<point3dd>& newpos,
				vector<point3dd>& neweuler,
				int interval,
				int acc)
{
	interpPathScale(pos, euler, newpos, neweuler, interval, interval, acc);
}
