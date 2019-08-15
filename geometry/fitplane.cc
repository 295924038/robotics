#include "fitplane.h"
#include <nlopt.h>
#include "Eigen/Dense"
using namespace std;
using namespace Eigen;
using namespace walgo;

double walgo::myfunc(unsigned n, const double* x, double* grad, void* funcData)
{
	fitplane* fit = (fitplane*) funcData;
	const vector<point3dd>& points= fit->getPoints();
	double a = x[0];
	double b = x[1];
	double c = x[2];
	double k = a*a + b*b+c*c;
	double sum = 0.0;
	for ( auto && it : points )
	{	
		double x = it._x;
		double y = it._y;
		double z = it._z;
		double e = (a*x+b*y+c*z - 1.0);
		sum += e*e;
	}
	sum = sum/k;
	return sum;	
}

void walgo::fitplane::fitPlane(const std::vector<point3dd>& points,
				  	double& a, double& b, double& c)
{
	_points = points;
	// use first three points to solve
	MatrixXd A(3,3);
	for ( int i = 0; i < 3; i++) 
	{
		A(i, 0) = _points[i]._x;
		A(i, 1) = _points[i]._y;
		A(i, 2) = _points[i]._z;
	}
	double param[3];
	VectorXd xb(3);
	xb(0) = 1;
	xb(1) = 1;
	xb(2) = 1;
	VectorXd s = A.fullPivLu().solve(xb);
	param[0] = s[0];
	param[1] = s[1];
	param[2] = s[2];
	double minf;
	nlopt_opt opt = nlopt_create(NLOPT_LN_NELDERMEAD, 3);
	nlopt_set_min_objective(opt, myfunc, (void*) this);
	
	nlopt_set_ftol_abs(opt, 1e-5);
	if ( nlopt_optimize(opt, param, &minf) < 0)
		cout << "nlopt failed " << endl;
	else
	{
		a = param[0];
		b = param[1];
		c = param[2];
		if ( minf > 10000.0)
			cout <<"minf too big " << a << "  " << b << "  " << c << endl;
	}
	nlopt_destroy(opt);
	return;
   	
}
