/*
 * quaternion.cc
 *
 *  Created on: Apr 5, 2017
 *      Author: zhian
 */

#include "quaternion.h"
#include "math.h"
using namespace walgo;


walgo::Quaternion::Quaternion(double q[4])
{
	for ( int i = 0; i < 4; i++)
		_q[i] = q[i];
}

double walgo::Quaternion::dot(const Quaternion& p) const 
{
   double d = _q[0]*p._q[0]+_q[1]*p._q[1]+_q[2]*p._q[2]+_q[3]*p._q[3]; 
}

Quaternion walgo::Quaternion::times(const Quaternion& p) const
{
	double r[4];
	double p0 = p._q[0];
	double p1 = p._q[1];
	double p2 = p._q[2];
	double p3 = p._q[3];
	double q0 = _q[0];
	double q1 = _q[1];
	double q2 = _q[2];
	double q3 = _q[3];
	r[0] = p0*q0 - p1*q1 - p2*q2 - p3*q3;
	r[1] = p0*q1 + p1*q0 - p2*q3 + p3*q2;
	r[2] = p0*q2 + q0*p2 + p1*q3 - q1*p3;
	r[3] = p0*q3 - p1*q2 + q0*p3 + p2*q1;
	return Quaternion(r);
}

Quaternion walgo::Quaternion::add(const Quaternion& q) const
{
	double r[4];
	r[0]= q._q[0] + _q[0];
	r[1] = q._q[1] + _q[1];
	r[2] = q._q[2] + _q[2];
	r[3] = q._q[3] + _q[3];
	return Quaternion(r);
}

Quaternion walgo::Quaternion::sub(const Quaternion& q) const
{
	double r[4];
	r[0] = -q._q[0] + _q[0];
	r[1] = -q._q[1] + _q[1];
	r[2] = -q._q[2] + _q[2];
	r[3] = -q._q[3] + _q[3];
	return Quaternion(r);
}

Quaternion walgo::Quaternion::conj() const
{
	double q[4];
	q[0] = _q[0];
	q[1] = -_q[1];
	q[2] = -_q[2];
	q[3] = -_q[3];
	return Quaternion(q);
}

void walgo::Quaternion::normalize()
{
	double norm = 0;
	for ( int i = 0; i < 4; i++)
		norm += _q[i]*_q[i];
	if ( norm == 0 ) return;
	norm = sqrt(1.0/norm);
	for ( int i = 0; i< 4; i++)
		_q[i] *= norm;
}
