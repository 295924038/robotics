#ifndef _FITARC_H_
#define _FITARC_H_
#include "point.h"
#include "fitplane.h"
#include <vector>


namespace walgo
{
double myfunc(unsigned n, const double* x, double* grad, void* funcData);

class fitarc
{
public:
	void fitArc(const std::vector<point3dd>& points);
	const std::vector<point3dd>& getPoints() const { return _points; }
	const void getPlane(double& a, double& b, double& c) { a = _a; b = _b; c=_c; }
	point3dd pointAt(double t);
	point3dd getCenter() { return _center; }
	double getRadius() { return _radius; }

private:
	std::vector<point3dd> _points;
	double _a;
	double _b;
	double _c;
	point3dd _norm;
	point3dd _center;
	double _radius;
	point3dd _vstart;
	point3dd _vend;
	double _angle;
};
}
#endif
