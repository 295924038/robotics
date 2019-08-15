#include "fitellipse.h"
#include "point.h"
#include <nlopt.h>
#include "fitarc.h"
using namespace std;
using namespace walgo;

namespace walgo
{

double ellipseobjfunc(unsigned n, const double* x, double* grad, void* funcData)
{
	fitellipse* fit = (fitellipse*) funcData;
	const vector<point3dd>& points= fit->getPoints();
	double xc1 = x[0];
	double yc1 = x[1];
	double xc2 = x[2];
	double yc2 = x[3];
	double a = x[4];
	double sum = 0;
	for ( auto && it : points )
	{	
		double x = it._x;
		double y = it._y;
		double dx1 = x - xc1;
		double dy1 = y - yc1;
		double dx2 = x - xc2;
		double dy2 = y - yc2;
		double dd1 = sqrt(dx1*dx1+dy1*dy1);
		double dd2 = sqrt(dx2*dx2+dy2*dy2);
		double dd = dd1+dd2-2.0*a;
		if ( grad )
		{
			grad[0] -= 2.0*dd*dx1/dd1;
			grad[1] -= 2.0*dd*dy1/dd1;
			grad[2] -= 2.0*dd*dx2/dd2;
			grad[3] -= 2.0*dd*dy2/dd2;
			grad[4] -= 4.0*dd;
		}
		sum += dd*dd;
	}
	return sum;	
}

double ellipseconstraint(unsigned n, const double* x, double* grad, void* funcData)
{
	fitellipse* fit = (fitellipse*) funcData;
	const vector<point3dd>& points= fit->getPoints();
	double xc1 = x[0];
	double yc1 = x[1];
	double xc2 = x[2];
	double yc2 = x[3];
	double a = x[4];
	double dcx = xc1-xc2;
	double dcy = yc1-yc2;
	double c =sqrt(dcx*dcx+dcy*dcy)*0.5;
	double b = sqrt(a*a-c*c);
	double elimit = 1.0/(fit->getElimit());
	return (a*elimit - b);

}
}

double walgo::fitellipse::fitEllipse(const std::vector<point3dd>& points, double elimit)
{
	_points = points;
	_elimit = elimit;
	double param[5];

	point3dd sum(0,0,0);
	for ( auto && it : points)
		sum = sum+ it;
	double s = 1.0/(double)(points.size());
	sum.scale(s);
	param[0] = sum._x;
	param[1] = sum._y;
	param[2] = sum._x;
	param[3] = sum._y;
	param[4] = sum.dist(points[0]);

	double minf;

	nlopt_opt opt = nlopt_create(NLOPT_AUGLAG, 5);
	nlopt_opt subOpt = nlopt_create(NLOPT_LN_BOBYQA, 5);
	nlopt_set_ftol_abs(subOpt, 1e-5);
	nlopt_set_min_objective(opt, ellipseobjfunc, (void*) this);
	nlopt_add_inequality_constraint(opt, ellipseconstraint, (void*) this, 1e-5);
	nlopt_set_local_optimizer(opt,subOpt);

	if ( nlopt_optimize(opt, param, &minf) < 0) {
		cout << "nlopt failed " << endl;
		nlopt_destroy(opt);
		return -1;
	}
	else
	{
		cout << "minf = " << minf << endl;
		_f1._x = param[0];
		_f1._y = param[1];
		_f1._z = 0;
		_f2._x = param[2];
		_f2._y = param[3];
		_f2._z = 0;
		_a = param[4];
		_c = _f1.dist(_f2)*0.5;
		_b = sqrt(_a*_a - _c*_c);
		cout << "ellipse a/b/c " << _a << "  "<< _b << "  "<< _c << endl;
		_center = _f1+_f2;
		_center.scale(0.5);
		point3dd p1 = points[0]-_center;
		_angleS = atan2(p1._y, p1._x);
		int ps = points.size();
		point3dd p2 = points[ps-1]-_center;
		_angleE = atan2(p2._y, p2._x);
		_angleM = atan2(-_f2._y+_f1._y, -_f2._x+_f1._x);
		//if ( _angleE < _angleS) _angleE = _angleE+atan(1.0)*8.0;
		cout << "_angleS = " <<  _angleS << endl;
		cout << "_angleE = " << _angleE << endl;
		cout << "_angleM = " << _angleM << endl;
		nlopt_destroy(opt);
		return sqrt(minf/points.size());
	}


	return -1;
   	
}

point3dd walgo::fitellipse::pointAt(double t)
{
	double angle = _angleS+t*(_angleE-_angleS);
	double angleM = angle-_angleM;
	double cs = _b*cos(angleM);
	double ss = _a*sin(angleM);
	double r = _a*_b/sqrt(cs*cs+ss*ss);
	point3dd p;
	p._x = r*cos(angle);
	p._y = r*sin(angle);
	p._z = 0;
	return (_center+p);
}
