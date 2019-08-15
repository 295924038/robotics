/*
 * slerp.h
 *
 *  Created on: Oct 3, 2017
 *      Author: zhian
 */

#ifndef SLERP_H_
#define SLERP_H_

#include "rotation.h"
#include "point.h"
#include "curve.h"

namespace walgo
{
template <class T>
class slerp : public curve<T>
{
public:
	slerp(const point3d<T>& p0, const point3d<T>& p1);
	virtual point3d<T> pointAt(T t);
	virtual point3d<T> getStartPt()  { return _p0; }
	virtual point3d<T> getEndPt()  { return _p1; }
	virtual T getStartParam() { return 0.0; }
	virtual T getEndParam()  { return 1.0; }
	 virtual std::vector<point3d<T> > getControlPts() { return {}; }
private:
	point3d<T> _p0;
	point3d<T> _p1;
	Rotation _r0;
	Rotation _r1;
	double _angle;
	double _nx;
	double _ny;
	double _nz;
};

template <class T>
slerp<T>::slerp(const point3d<T>& p0, const point3d<T>& p1) :
	_p0(p0),_p1(p1)
{
	Rotation::EULERTYPE t = static_cast<Rotation::EULERTYPE>(321);
    Rotation r0(p0._x, p0._y, p0._z, false, t, true);
    Rotation r1(p1._x, p1._y, p1._z, false, t, true);
	_r0 = r0;
	_r1 = r1;
	Rotation r01 = r1.times(r0.inverse());
	r01.getAngleAxis(_angle, _nx,_ny,_nz);
}

template <class T>
point3d<T> slerp<T>::pointAt(T t)
{
	 Rotation r(t*_angle, _nx,_ny,_nz);
	 Rotation rt = r.times(_r0);
	 double a,b,c;
	 rt.getEulerAngles(a,b,c,true, Rotation::zyx, true);
	 return point3d<T>(a,b,c);
}

}

#endif /* SLERP_H_ */
