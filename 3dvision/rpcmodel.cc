/*
 * rpcmodel.cc
 *
 *  Created on: Mar 27, 2016
 *      Author: zhian
 */
#include <vector>
#include <limits>
#include <iostream>
#include <math.h>
#include <nlopt.h>
#include "Eigen/Dense"
#include "point.h"
#include "rpcmodel.h"

using namespace std;
using namespace Eigen;
using namespace walgo;

double walgo::myfunc(unsigned n, const double* x, double* grad, void* funcData)
{
	rpcmodel* model = (rpcmodel*) funcData;
	double z = model->getZ();
	double u0, v0;
	model->getTarget(u0,v0);
	float u, v;
	model->objToImg(x[0], x[1], z, u, v);
	double du = u-u0;
	double dv = v-v0;
	if ( grad )
	{
		double dxu, dxv, dyu, dyv;
		model->getGradient(dxu, dxv, dyu, dyv);
		grad[0] = 2.0*(du*dxu+dv*dxv);
		grad[1] = 2.0*(du*dyu+dv*dyv);

		//cout << "***** gridients: " << grad[0] << "   "<< grad[1]<< endl;
	}
	return du*du+dv*dv;
}


void walgo::rpcmodel::solveu()
{
	MatrixXd Au((int)NUM_BASIS, (int)NUM_BASIS);
	VectorXd bu((int)NUM_BASIS);
	for (int i = 0; i < NUM_BASIS; i++)
	{
		for (int j = 0; j < NUM_BASIS; j++)
		{
			Au(i,j) = _amatrixu[i][j];
		}
		bu[i] = _bvectoru[i];
	}
	VectorXd xu = Au.colPivHouseholderQr().solve(bu);
	//cout << "The solution for u model is:\n" << xu << endl;
	for ( int i = 0; i < NUM_BASIS; i++)
		_urpc.push_back(xu(i));
}

void walgo::rpcmodel::solvev()
{
	MatrixXd Av((int)NUM_BASIS, (int)NUM_BASIS);
	VectorXd bv((int)NUM_BASIS);
	for (int i = 0; i < NUM_BASIS; i++)
	{
		for (int j = 0; j < NUM_BASIS; j++)
		{
			Av(i,j) = _amatrixv[i][j];
		}
		bv[i] = _bvectorv[i];
	}
	VectorXd xv = Av.fullPivLu().solve(bv);
	for ( int i = 0; i < NUM_BASIS; i++)
		_vrpc.push_back(xv(i));
}

void walgo::rpcmodel::calibrate()
{
	buildAB(_cdata);
	solveu();
	solvev();
}

void walgo::rpcmodel::buildAB(pairmap<float>* pm)
{
	_amatrixu.resize(NUM_BASIS, NUM_BASIS);
	_amatrixv.resize(NUM_BASIS, NUM_BASIS);
	_amatrixu.setZero();
	_amatrixv.setZero();
	_bvectoru.resize(NUM_BASIS,0);
	_bvectorv.resize(NUM_BASIS,0);
	vector<double> basis(NUM_BASIS, 0);
	int i = 0;
	_minx = 1000000;
	_maxx = -1000000;
	_miny = 1000000;
	_maxy = -1000000;
	_minz = 1000000;
	_maxz = -1000000;
	for ( auto & it : pm->_map)
	{
		double x = it.second->_obj._x;
		double y = it.second->_obj._y;
		double z = it.second->_obj._z;
		if ( x < _minx) _minx = x;
		if ( x > _maxx) _maxx = x;
		if ( y < _miny) _miny = y;
		if ( y > _maxy) _maxy = y;
		if ( z < _minz) _minz = z;
		if ( z > _maxz) _maxz = z;
	}
	_dx = 1.0/(_maxx - _minx);
	_dy = 1.0/(_maxy - _miny);
	_dz = 1.0/(_maxz - _minz);
	for ( auto & it : pm->_map)
	{
	//	cout << "reading " << i << " th data" << endl;
		i++;
		double u = it.second->_img._u;
		double v = it.second->_img._v;
		double x = scalex(it.second->_obj._x);
		double y = scaley(it.second->_obj._y);
		double z = scalez(it.second->_obj._z);
		//cout << u << " " << v << " " << x << " " << y << " "<< z << endl;
		double ux = u*x;
		double uy = u*y;
		double uz = u*z;
		double xx = x*x;
		double yy = y*y;
		double zz = z*z;
		double xz = x*z;
		double yz = y*z;
		double xy = x*y;
		double uu = u*u;
		basis[C] = 1;
		basis[X] = x;
		basis[Y] = y;
		basis[Z] = z;
		basis[UX] = -ux;
		basis[UY] = -uy;
		basis[UZ] = -uz;
		basis[XZ] = xz;
		basis[XY] = xy;
		basis[YZ] = yz;
		basis[XX] = xx;
		basis[YY] = yy;
		basis[ZZ] = zz;
		basis[UXZ] = -u*xz;
		basis[UXY] = -u*xy;
		basis[UYZ] = -u*yz;
		basis[UXX] = -u*xx;
		basis[UYY] = -u*yy;
		basis[UZZ] = -u*zz;
		_bvectoru[C] += u;
		_bvectoru[X] += ux;
		_bvectoru[Y] += uy;
		_bvectoru[Z] += uz;
		_bvectoru[UX] += -uu*x;
		_bvectoru[UY] += -uu*y;
		_bvectoru[UZ] += -uu*z;
		_bvectoru[XZ] += u*xz;
		_bvectoru[XY] += u*xy;
		_bvectoru[YZ] += u*yz;
		_bvectoru[XX] += u*xx;
		_bvectoru[YY] += u*yy;
		_bvectoru[ZZ] += u*zz;
		_bvectoru[UXZ] += -uu*xz;
		_bvectoru[UYZ] += -uu*yz;
		_bvectoru[UXY] += -uu*xy;
		_bvectoru[UXX] += -uu*xx;
		_bvectoru[UYY] += -uu*yy;
		_bvectoru[UZZ] += -uu*zz;
		for ( int i = 0; i < NUM_BASIS; i++)
		{
			for ( int j = 0; j < NUM_BASIS; j++)
			{
				_amatrixu[i][j] += basis[i]*basis[j];
			}
		}

		double vv = v*v;
		double vx = v*x;
		double vy = v*y;
		double vz = v*z;

		basis[UX] = -vx;
		basis[UY] = -vy;
		basis[UZ] = -vz;
		basis[UXZ] = -v*xz;
		basis[UXY] = -v*xy;
		basis[UYZ] = -v*yz;
		basis[UXX] = -v*xx;
		basis[UYY] = -v*yy;
		basis[UZZ] = -v*zz;
		_bvectorv[C] += v;
		_bvectorv[X] += vx;
		_bvectorv[Y] += vy;
		_bvectorv[Z] += vz;
		_bvectorv[UX] += -vv*x;
		_bvectorv[UY] += -vv*y;
		_bvectorv[UZ] += -vv*z;
		_bvectorv[XZ] += v*xz;
		_bvectorv[XY] += v*xy;
		_bvectorv[YZ] += v*yz;
		_bvectorv[XX] += v*xx;
		_bvectorv[YY] += v*yy;
		_bvectorv[ZZ] += v*zz;
		_bvectorv[UXZ] += -vv*xz;
		_bvectorv[UYZ] += -vv*yz;
		_bvectorv[UXY] += -vv*xy;
		_bvectorv[UXX] += -vv*xx;
		_bvectorv[UYY] += -vv*yy;
		_bvectorv[UZZ] += -vv*zz;
		for ( int i = 0; i < NUM_BASIS; i++)
		{
			for ( int j = 0; j < NUM_BASIS; j++)
			{
				_amatrixv[i][j] += basis[i]*basis[j];
			}
		}
	}

}

void walgo::rpcmodel::findBestGuess(float u, float v, float z, float& x, float& y)
{
	float mindist = 1000000;
	float minz = 0;
	for ( int nz = 0; nz < _grid->getSizeZ(); nz++)
	{
		float zz = _grid->getZMin()+ nz*_grid->getZGridPitch();
		float dz;
		if ( (dz = fabs(zz-z)) < mindist)
		{
			mindist = dz;
			minz = zz;
		}
	}
	//cout << "input z is: " << z << endl;
	//cout <<"closest z is: " << minz << endl;
	vector<int> pairs;
	mindist = 1000000;
	_cdata->findz(minz, pairs, 0.01);
	point2d<float> img(u,v);
	for ( auto & i : pairs)
	{
		float dist;
		if ((dist =  img.dist(&(_cdata->_map[i]->_img))) < mindist)
		{
			x = _cdata->_map[i]->_obj._x;
			y = _cdata->_map[i]->_obj._y;
			mindist = dist;
		}
	}
	return;
}

void walgo::rpcmodel::imgToObj(float u, float v, float z, float& x, float& y)
{
	setZ(z);
	setTarget(u,v);
	findBestGuess(u, v, z, x,y);
	//cout <<"best guess for (z, u,v,) = (" << z << " "<< u << " "<< v << ") = " << x << " " << y << endl;
	double param[2];
	param[0] = x;
	param[1] = y;
	//param[0] = 0.5*(_minx+_maxx);
	//param[1] = 0.5*(_miny+_maxy);
	_doGradient = false;
	nlopt_opt opt = nlopt_create(NLOPT_LN_NELDERMEAD, 2);
	nlopt_set_min_objective(opt, myfunc, (void*) this);
	double tol[2];
	tol[0] = 1e-5;
	tol[1] = 1e-5;
	//nlopt_set_xtol_abs(opt, tol);
	nlopt_set_ftol_abs(opt, 1e-5);
	double lb[2];
	lb[0] = _minx - _range;
	lb[1] = _miny - _range;
	double ub[2];
	ub[0] = _maxx + _range;
	ub[1] = _maxy + _range;
	nlopt_set_lower_bounds(opt, lb);
	nlopt_set_upper_bounds(opt, ub);
	double minf;
	//cout << "initial objective func: " << myfunc(2, param, NULL, (void*)this) << endl;
	if ( nlopt_optimize(opt, param, &minf) < 0)
		cout << "nlopt failed " << endl;
	else
	{
		x = param[0];
		y = param[1];
		//cout << "minf = "<< minf << endl;
		//cout << "final solution: x,y = " << x << "  "<< y << endl;
		if ( minf > 10000.0)
			cout <<"minf too big " << u << "  " << v << "  " << z << endl;
	}

	return;

}

void walgo::rpcmodel::objToImg(float x, float y, float z, float& u, float& v)
{
	x = scalex(x);
	y = scaley(y);
	z = scalez(z);
	vector<double> basis(NUM_BASIS, 0);
	double xx = x*x;
	double yy = y*y;
	double zz = z*z;
	double xz = x*z;
	double yz = y*z;
	double xy = x*y;
	basis[C] = 1;
	basis[X] = x;
	basis[Y] = y;
	basis[Z] = z;
	basis[UX] = x;
	basis[UY] = y;
	basis[UZ] = z;
	basis[XZ] = xz;
	basis[XY] = xy;
	basis[YZ] = yz;
	basis[XX] = xx;
	basis[YY] = yy;
	basis[ZZ] = zz;
	basis[UXZ] = xz;
	basis[UXY] = xy;
	basis[UYZ] = yz;
	basis[UXX] = xx;
	basis[UYY] = yy;
	basis[UZZ] = zz;
	double upperu = 0;
	for ( int i = 0; i < NUM_UPPER; i++)
	{
		upperu += _urpc[i]*basis[i];
	}
	double loweru = 1.0;
	for ( int i = NUM_UPPER; i < NUM_BASIS; i++)
	{
		loweru += _urpc[i]*basis[i];
	}
	u = upperu/loweru;


	double upperv = 0;
	for ( int i = 0; i < NUM_UPPER; i++)
	{
		upperv += _vrpc[i]*basis[i];
	}
	double lowerv = 1.0;
	for ( int i = NUM_UPPER; i < NUM_BASIS; i++)
	{
		lowerv += _vrpc[i]*basis[i];
	}
	v = upperv/lowerv;
	if ( _doGradient )
	{
		vector<double> basisx(NUM_BASIS,0);
		vector<double> basisy(NUM_BASIS,0);
		basisx[C] = 0;
		basisx[X] = 1;
		basisx[Y] = 0;
		basisx[Z] = 0;
		basisx[UX] = 1;
		basisx[UY] = 0;
		basisx[UZ] = 0;
		basisx[XZ] = z;
		basisx[XY] = y;
		basisx[YZ] = 0;
		basisx[XX] = 2.0*x;
		basisx[YY] = 0;
		basisx[ZZ] = 0;
		basisx[UXZ] = z;
		basisx[UXY] = y;
		basisx[UYZ] = 0;
		basisx[UXX] = 2.0*x;
		basisx[UYY] = 0;
		basisx[UZZ] = 0;
		double upperxu = 0.0;
		double upperxv = 0.0;
		for ( int i = 0; i < NUM_UPPER; i++)
		{
			upperxu += _urpc[i]*basisx[i];
			upperxv += _vrpc[i]*basisx[i];
		}
		double lowerxu = 0.0;
		double lowerxv = 0.0;
		for ( int i = NUM_UPPER; i < NUM_BASIS; i++)
		{
			lowerxu += _urpc[i]*basisx[i];
			lowerxv += _vrpc[i]*basisx[i];
		}
		_dxu = (upperxu*loweru-lowerxu*upperu)/(loweru*loweru);
		_dxv = (upperxv*lowerv-lowerxv*upperv)/(lowerv*lowerv);
		basisy[C] = 0;
		basisy[X] = 0;
		basisy[Y] = 1;
		basisy[Z] = 0;
		basisy[UX] = 0;
		basisy[UY] = 1;
		basisy[UZ] = 0;
		basisy[XZ] = 0;
		basisy[XY] = x;
		basisy[YZ] = z;
		basisy[XX] = 0;
		basisy[YY] = 2.0*y;
		basisy[ZZ] = 0;
		basisy[UXZ] = 0;
		basisy[UXY] = x;
		basisy[UYZ] = z;
		basisy[UXX] = 0;
		basisy[UYY] = 2.0*y;
		basisy[UZZ] = 0;
		double upperyu = 0.0;
		double upperyv = 0.0;
		for ( int i = 0; i < NUM_UPPER; i++)
		{
			upperyu += _urpc[i]*basisy[i];
			upperyv += _vrpc[i]*basisy[i];
		}
		double loweryu = 0.0;
		double loweryv = 0.0;
		for ( int i = NUM_UPPER; i < NUM_BASIS; i++)
		{
			loweryu += _urpc[i]*basisy[i];
			loweryv += _vrpc[i]*basisy[i];
		}
		_dyu = (upperyu*loweru-loweryu*upperu)/(loweru*loweru);
		_dyv = (upperyv*lowerv-loweryv*upperv)/(lowerv*lowerv);
	}
	return;
}

