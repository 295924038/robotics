#ifndef _FITELLIPSE_H_
#define _FITELLIPSE_H_
#include "point.h"
#include <vector>


namespace walgo
{
double ellipseobjfunc(unsigned n, const double* x, double* grad, void* funcData);

class fitellipse
{
public:
	double fitEllipse(const std::vector<point3dd>& points, double elimit=1.3);
	const std::vector<point3dd>& getPoints() const { return _points; }
	point3dd pointAt(double t);
	point3dd getCenter() { return _center; }
	point3dd getF1() { return _f1; }
	point3dd getF2() { return _f2; }
	double getElimit() { return _elimit; }
private:
	std::vector<point3dd> _points;
	double _a;    // major axis
	double _b;    // minor axis
	double _c;     // distance between focal points
	point3dd _center;
	point3dd _f1;
	point3dd _f2;    // focal points
	double _angleS;
	double _angleE;
	double _angleM;
	double _elimit;
};
}
#endif
