#include "fitplane.h"
#include <iostream>

using namespace std;
using namespace walgo;

int main()
{
	point3dd p1(0.1,0.3, 0.6);
	point3dd p2(0.25, 0.5, 0.25);
	point3dd p3(0.4, 0.2, 0.4);
	point3dd p4(0.8, 0.1, 0.1);
	point3dd p5(0.6, 0.3, 0.1);
	vector<point3dd> vp;
	vp.push_back(p1);
	vp.push_back(p2);
	vp.push_back(p3);
	vp.push_back(p4);
	vp.push_back(p5);
	fitplane fp;
	double a,b,c;
	fp.fitPlane(vp, a, b, c);
	cout << a << "  " << b << "  " << c << endl;
	
}
