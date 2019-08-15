/*
 * testspline.cc
 *
 *  Created on: Aug 22, 2016
 *      Author: zhian
 */
#include <math.h>
#include <vector>
#include "point.h"
#include "bezierspline.h"

using namespace std;
using namespace walgo;

int main()
{
	float x,y, z;
	vector<point3df> knots;
	float scale = 0.2;
	float s1 = 1.0/3.0;
	for ( int i = 0; i < 3; i++)
	{
		x = cos(i*s1*scale);
		y = sin(i*s1*scale);
		z = 0.0;
		point3df p(x,y,z);
		cout << "adding initial: " << p << endl;
		knots.push_back(p);
	}
	for ( int i = 1; i < 8; i++)
	{
		x = cos(i*1.0*scale);
		y = sin(i*1.0*scale);
		z = 0.0;
		point3df p(x,y,z);
		cout <<"adding: " << p << endl;
		knots.push_back(p);
	}
	bezierspline<float> bs(3);
	bs.initKnots(knots);

	point3df p1 = bs.pointAt(4.5);
	x = p1._x;
	y = p1._y;
	cout << "x = " << x << " y = " << y << endl;
	cout << "r = " << sqrt(x*x+y*y) << endl;

	p1 = bs.pointAt(4.75);
	x = p1._x;
	y = p1._y;
	cout << "x = " << x << " y = " << y << endl;
	cout << "r = " << sqrt(x*x+y*y) << endl;

	p1 = bs.pointAt(4.3);
	x = p1._x;
	y = p1._y;
	cout << "x = " << x << " y = " << y << endl;
	cout << "r = " << sqrt(x*x+y*y) << endl;

	p1 = bs.pointAt(4.2);
	x = p1._x;
	y = p1._y;
	cout << "x = " << x << " y = " << y << endl;
	cout << "r = " << sqrt(x*x+y*y) << endl;

}

