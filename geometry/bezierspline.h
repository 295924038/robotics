/*
 * bezierspline.h
 *
 *  Created on: Aug 21, 2016
 *      Author: zhian
 */

#ifndef BEZIERSPLINE_H_
#define BEZIERSPLINE_H_

#include <vector>
#include "spline.h"
#include "bezier.h"
#include "Eigen/Dense"

namespace walgo
{
template <class T>
class bezierspline : public spline<T>
{
public:
	bezierspline(int order=3);

	virtual ~bezierspline() {}

	virtual std::vector<point3d<T> > getControlPts();

	virtual void reset(int segmentID) {}

	virtual void initKnots(const std::vector<point3d<T> >& knots);

	virtual void addKnot(const point3d<T>& knot);

	static void buildB(Eigen::VectorXd& bx, Eigen::VectorXd& by, Eigen::VectorXd& bz,
			const std::vector<point3d<T>>& knots, int n);
	static void buildA(Eigen::MatrixXd& A, int n);
protected:

	int _order;
};

template <class T>
bezierspline<T>::bezierspline(int order)
{
	_order = order;
}

using Eigen::MatrixXd;
template <class T>
void bezierspline<T>::buildA(MatrixXd& A, int n)
{
	for ( int i = 0; i < n+1; i++)
		for ( int j = 0; j < n+1; j++)
			A(i,j) = 0;
	A(0,0)= 2;
	A(0,1)= 1;
	for ( int i = 1; i < n; i++) {
		A(i,i-1) = 1;
		A(i,i) = 4;
		A(i,i+1) = 1;
	}
	A(n,n-1) = 1;
	A(n,n) = 2;
}

using Eigen::VectorXd;
template <class T>
void bezierspline<T>::buildB(VectorXd& bx, VectorXd& by, VectorXd& bz,
 		const std::vector<point3d<T>>& knots, int n)
{
	bx[0] = knots[1]._x - knots[0]._x;
	by[0] = knots[1]._y - knots[0]._y;
	bz[0] = knots[1]._z - knots[0]._z;
	for (int i = 1; i < n; i++)
	{
		bx[i] = knots[i+1]._x - knots[i-1]._x;
		by[i] = knots[i+1]._y - knots[i-1]._y;
		bz[i] = knots[i+1]._z - knots[i-1]._z;
	}
	bx[n] = knots[n]._x - knots[n-1]._x;
	by[n] = knots[n]._y - knots[n-1]._y;
	bz[n] = knots[n]._z - knots[n-1]._z;
}

template <class T>
vector<point3d<T> > bezierspline<T>::getControlPts()
{
	std::vector<point3d<T> > cts;
	int i = 0;
	for ( auto && it : this->_segments )
		for (auto && jt : it->getControlPts())
		{
			cts.push_back(jt);
			i++;
		}
	return cts;
	//cout <<"i = " << i << endl;
}

template <class T>
void bezierspline<T>::initKnots(const std::vector<point3d<T> >& knots)
{
	int n = knots.size()-1;
	MatrixXd A(n+1, n+1);
	buildA(A, n);

	VectorXd bx(n+1);
	VectorXd by(n+1);
	VectorXd bz(n+1);
	buildB(bx, by, bz, knots, n);
	VectorXd dx = A.fullPivLu().solve(bx);
	VectorXd dy = A.fullPivLu().solve(by);
	VectorXd dz = A.fullPivLu().solve(bz);
	for ( int i = 0; i < n; i++)
	{
		std::vector<point3d<T> > polyi;
		point3d<T> dp1(dx[i],dy[i],dz[i]);
		point3d<T> dp2(dx[i+1],dy[i+1],dz[i+1]);
		polyi.push_back(knots[i]);
		polyi.push_back(knots[i]+dp1);
		polyi.push_back(knots[i+1]-dp2);
		polyi.push_back(knots[i+1]);
		auto b = std::make_shared<bezier<T> >(polyi);
		this->_segments.push_back(b);
	}

}

template <class T>
void bezierspline<T>::addKnot(const point3d<T>& knot)
{
	// not implemented
	return;
}

}
#endif /* BEZIERSPLINE_H_ */
