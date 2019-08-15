#include "fitarc.h"
#include "fitellipse.h"
#include <iostream>

using namespace std;
using namespace walgo;


void print(const point3dd& p)
{
	cout << p._x << "  " << p._y << "  " << p._z << endl;
}

int main()
{
	point3dd p1(0, 0, 0);
	point3dd p2(10.6352, -23.3927, 0);
	point3dd p3(42.6252, -34.7179, 0);
	point3dd p4(76.4093, -29.9024, 0);
	point3dd p5(104.47,0,0);
	vector<point3dd> vp;
	vp.push_back(p1);
	vp.push_back(p2);
	vp.push_back(p3);
	vp.push_back(p4);
	vp.push_back(p5);
	fitarc fa;
	fa.fitArc(vp);
	double a,b,c;
	fa.getPlane(a,b,c);
	cout << a << "  " << b << "  " << c << endl;
	cout << " radius = "  << fa.getRadius() << endl;
	point3dd center = fa.getCenter();
	cout <<" center = " << center._x << " " << center._y << " " << center._z << endl;
	print(fa.pointAt(0));
	print(fa.pointAt(0.25));
	print(fa.pointAt(0.5));
	print(fa.pointAt(0.75));
	print(fa.pointAt(1.0));
	point3dd pp1(2.27374e-13, 5.68434e-14, 0);
	point3dd pp2(13.3474, -8.33022, 0);
	point3dd pp3(22.6686, -14.2734, 0);
	point3dd pp4(44.8525, -21.9699, 0);
	point3dd pp5(77.9493, -20.5339, 0);
	point3dd pp6(96.6039, -10.5105, 0);
	point3dd pp7(105.547, 0, 0);
	vector<point3dd> vp1;
	vp1.push_back(pp1);
	vp1.push_back(pp2);
	vp1.push_back(pp3);
	vp1.push_back(pp4);
	vp1.push_back(pp5);
	vp1.push_back(pp6);
	vp1.push_back(pp7);
	cout << "fitting ellipse" << endl;
	fitellipse fe;
	double rms = fe.fitEllipse(vp1, 1.4);
	cout << "*** fit rms = " << rms << endl;
	center = fe.getCenter();
	cout <<" center = " << center._x << " " << center._y << " " << center._z << endl;
	cout << "f1 = " << endl;
	point3dd f1 = fe.getF1();
	print(f1);
	cout << "f2 = " << endl;
	point3dd f2 = fe.getF2();
	print(f2);
	print(fe.pointAt(0));
	print(fe.pointAt(0.25));
	print(fe.pointAt(0.5));
	print(fe.pointAt(0.75));
	print(fe.pointAt(1.0));


}
