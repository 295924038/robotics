#include "fitarc.h"
#include "fitplane.h"
#include "point.h"
#include "rotation.h"
#include <nlopt.h>
#include "Eigen/Dense"
using namespace std;
using namespace Eigen;
using namespace walgo;

double arcobjfunc(unsigned n, const double* x, double* grad, void* funcData)
{
	fitarc* fit = (fitarc*) funcData;
	const vector<point3dd>& points= fit->getPoints();
	double xc = x[0];
	double yc = x[1];
	double zc = x[2];
	double r = x[3];
	double sum = 0;
	for ( auto && it : points )
	{	
		double x = it._x;
		double y = it._y;
		double z = it._z;
		double dx = x - xc;
		double dy = y - yc;
		double dz = z - zc;
		double dd = sqrt(dx*dx+dy*dy + dz*dz)-r;
		sum += dd*dd;
	}
	return sum;	
}

double myconstraint(unsigned n, const double* x, double* grad, void* funcData)
{
	fitarc* fit = (fitarc*) funcData;
	double a,b,c;
	fit->getPlane(a,b,c);
	return (a*x[0]+b*x[1]+c*x[2]-1.0);
}

void project(const point3dd& in, const point3dd& norm, point3dd& out)
{
	out = in-norm.times(norm.inner(in));
}

class veccompare
{
public:
	veccompare(const point3dd& center, const point3dd& norm, const vector<point3dd> points)
    {
	    _center = center;
	    _norm = norm;
	    point3dd veca, vecb;
	    project(points[0]-_center, _norm, veca);
	    project(points[points.size()-1]-_center, _norm, vecb);
	    double n = veca.cross(vecb).inner(_norm);
	    if ( n > 0 ) _inputOrder = false;
	    else if ( n < 0 ) _inputOrder = true;
	    if ( n==0) cout <<"*********** ERROR can't determine if input points are CW or ACW!" << endl;
    }
	bool operator() (const point3dd& a, const point3dd& b)
	{
		point3dd veca, vecb;
		project(a-_center, _norm, veca);
		project(b-_center, _norm, vecb);
		double n = veca.cross(vecb).inner(_norm);
		return _inputOrder? n < 0 : n > 0;
	}
	point3dd _center;
	point3dd _norm;
	bool _inputOrder;
};

void walgo::fitarc::fitArc(const std::vector<point3dd>& points)
{
	_points = points;
	fitplane fp;
	fp.fitPlane(points, _a, _b, _c);
	double param[4];
	point3dd sum(0,0,0);
	for ( auto && it : points)
		sum = sum+ it;
	double s = 1.0/(double)(points.size());
	sum.scale(s);
	param[0] = sum._x;
	param[1] = sum._y;
	param[2] = sum._z;
	param[3] = sum.dist(points[0]);

	double minf;
	nlopt_opt opt = nlopt_create(NLOPT_AUGLAG_EQ, 4);
	nlopt_opt subOpt = nlopt_create(NLOPT_LN_BOBYQA, 4);
	nlopt_set_ftol_abs(subOpt, 1e-6);
	nlopt_set_min_objective(opt, arcobjfunc, (void*) this);
	nlopt_add_equality_constraint(opt, myconstraint, (void*) this, 1e-5);
	nlopt_set_local_optimizer(opt,subOpt);
	
	nlopt_set_ftol_abs(opt, 1e-6);
	if ( nlopt_optimize(opt, param, &minf) < 0)
		cout << "nlopt failed " << endl;
	else
	{
		_center._x = param[0];
		_center._y = param[1];
		_center._z = param[2];
		_radius = param[3];
		point3dd norm(_a,_b,_c);
		norm.normalize();
		veccompare vc(_center, norm, points);
		std::vector<point3dd> vps = points;
		std::sort(vps.begin(), vps.end(), vc);
		point3dd vs,ve;
		project(vps[0]-_center, norm, vs);
		project(vps[vps.size()-1]-_center, norm, ve);
		vs.normalize();
		ve.normalize();
		_vstart = vs;
		_vend = ve;
		_angle = acos(_vend.inner(_vstart));
		_norm = norm;
	}

	nlopt_destroy(subOpt);
	nlopt_destroy(opt);
	return;
   	
}

point3dd walgo::fitarc::pointAt(double t)
{
	double angle = _angle*t;
	Rotation rot(angle, _norm._x, _norm._y, _norm._z);
	point3dd v;
	rot.apply(_vstart, v);
	return (_center+v.times(_radius));

}
