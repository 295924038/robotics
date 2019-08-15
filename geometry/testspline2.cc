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
#include <iomanip>
#include "point.h"
#include "bezierspline.h"
#include "slerpspline.h"
#include "interppath.h"

using namespace std;
using namespace walgo;

bool readData(const char* fname, vector<point3dd>& pos, vector<point3dd>& euler)
{
        float x,y,z, a,b,c;
        ifstream ifs(fname);
        if ( !ifs )
        {
                cout << "can't read file " << fname << endl;
                return false;
        }

        while (ifs >> x && ifs >> y && ifs >> z && ifs >> a && ifs >> b && ifs >> c)
        {
             point3dd  p(x,y,z);
             pos.push_back(p);
             point3dd e(a,b,c);
             euler.push_back(e);
        }
        cout << "read " << pos.size() << " entries of data" << endl;
        return true;
}


int main()
{
	vector<point3dd> pos;
	vector<point3dd> euler;
	readData("0To1", pos, euler);

	vector<point3dd> dpos;
	vector<point3dd> deuler;

	int interval = 100;
	int acc = 2;

	vector<point3dd> newp;
	vector<point3dd> newe;
	walgo::interpPath(pos, euler, newp, newe, interval, acc);
	ofstream ofs1("out1.dat");
	ofs1 << std::setprecision(10);
	for ( int i = 0; i < newp.size(); i++) {
		point3dd p = newp[i];
		point3dd e = newe[i];
		ofs1 << p._x << " " << p._y << " " << p._z << " " << e._x << " " << e._y << " " << e._z << endl;
	}

}
