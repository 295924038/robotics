/*
 * rotation.cc
 *
 *  Created on: Apr 5, 2017
 *      Author: zhian
 */

#include "rotation.h"
#include "quaternion.h"
#include <iostream>
#include <math.h>
using namespace std;
using namespace walgo;

using Eigen::Matrix3d;
using Eigen::VectorXd;

walgo::Rotation::Rotation(double A, double B, double C,
			       bool isRad, EULERTYPE type,
				   bool intrinsic)
{
	_intrinsic = intrinsic;
	_A = toRad(A, isRad);
	_B = toRad(B, isRad);
	_C = toRad(C, isRad);
	_axesA = type/100;       // first axis
	_axesB = (type%100)/10;  // second axis
	_axesC = type%10;        // third axis
	//cout << " axis are: " << _axesA << " " <<  _axesB << " " << _axesC << endl;
	//cout << "angles are: " << _A << "  " << _B << "  " << _C << endl;
	calcQ();
	_genEuler = true;
	_genQ = true;
	_genMatrix = false;

}

walgo::Rotation::Rotation(const Quaternion& q)
{
	_rotQ = q;
	_genQ = true;
	_genMatrix = false;
	_genEuler = false;
}

walgo::Rotation::Rotation(double angle, double nx, double ny, double nz)
{
	double q[4];
	q[0] = cos(angle*0.5);
	double s = sin(angle*0.5);
	q[1] = s*nx;
	q[2] = s*ny;
	q[3] = s*nz;
	Quaternion nq(q);
	_rotQ = nq;
	_genQ = true;
	_genEuler = false;
	_genMatrix = false;
}

walgo::Rotation::Rotation(const Eigen::Matrix3d& m)
{
	_rotMatrix = m;
	_genMatrix = true;
	_genQ = false;
	_genEuler = false;
}

void walgo::Rotation::calcQ()
{
	double q[4];
	q[0] = cos(_A*0.5);
	memset(&q[1], 0, 3*sizeof(double));
	q[_axesA] = sin(_A*0.5);
	Quaternion qa(q);
	q[0] = cos(_B*0.5);
	memset(&q[1], 0, 3*sizeof(double));
	q[_axesB] = sin(_B*0.5);
	Quaternion qb(q);
	q[0] = cos(_C*0.5);
	memset(&q[1], 0, 3*sizeof(double));
	q[_axesC] = sin(_C*0.5);
	Quaternion qc(q);
	//cout << "norm of qa/qb/bc = " << qa.norm() << "  " << qb.norm() << "  " << qc.norm() << endl;
	_rotQ = _intrinsic? ((qa.times(qb)).times(qc)) : ((qc.times(qb)).times(qc));

}

Rotation walgo::Rotation::inverse()
{
	if ( _genQ )
		return Rotation(_rotQ.conj());
	else if ( _genEuler )
	{
		calcQ();
		return Rotation(_rotQ.conj());
	}
	else if ( _genMatrix )
	{
		return Rotation(_rotMatrix.transpose());
	}

}

Rotation walgo::Rotation::times(const Rotation& rot)
{
	if ( _genQ && rot._genQ) {
		return Rotation(_rotQ.times(rot._rotQ));
	}
	if ( _genMatrix && rot._genMatrix) {
		return Rotation(_rotMatrix*rot._rotMatrix);
	}
}

Matrix3d walgo::Rotation::getPrincipleMat(int axis, double angle)
{
	Matrix3d mat;
	if ( axis == X ) {
		mat << 1, 0, 0,
			 0, cos(angle), -sin(angle),
			 0, sin(angle),  cos(angle);
	}
	else if ( axis == Y ) {
		mat << cos(angle), 0, sin(angle),
				0, 1, 0,
				-sin(angle), 0, cos(angle);
	}
	else if ( axis == Z ) {
		mat << cos(angle), -sin(angle),  0,
			   sin(angle), cos(angle), 0,
			   0, 0, 1;
	}
	return mat;
}

int relsign(double a, double b)
{
	if ( a ==0 || b == 0 )  return 1;
	int i1 = signbit(a) ? -1 : 1;
	int i2 = signbit(b) ? -1 : 1;
	return i1*i2;
}

void walgo::Rotation::getEulerAngles(double& A, double& B, double& C,
			        bool deg, EULERTYPE type,
					bool intrinsic)
{
	/*
	if ( _genEuler )
	{
		A = _A;
		B = _B;
		C = _C;
		return;
	}
	*/
	if ( _genQ )
	{
		double q0 = _rotQ[0];
		double q1 = _rotQ[1];
		double q2 = _rotQ[2];
		double q3 = _rotQ[3];
		if ( type == zyx)
		{
			B = asin(2.0*(q0*q2-q3*q1));
			C = atan2(2.0*(q2*q3+q0*q1), 2.0*(q0*q0+q3*q3)-1.0);
			A = atan2(2.0*(q1*q2+q0*q3), 2.0*(q0*q0+q1*q1)-1.0);

		}
		else if ( type == zyz)
		{
			double e1 = q2*q3-q0*q1;
			double e2 = q0*q2+q1*q3;
			double f1 = q0*q1+q2*q3;
			double f2 = q0*q2-q1*q3;

			B = acos(2.0*(q0*q0+q3*q3)-1.0);
			A = atan2(e1, e2);
			C = atan2(f1, f2);

		}
		if ( !intrinsic )
		{
			double temp = A;
			A = C;
			C = temp;
		}
		_genEuler = true;
		_A = A;
		_B = B;
		_C = C;
	}

	A = toDeg(A, deg);
	B = toDeg(B, deg);
	C = toDeg(C, deg);
	return;

}

Matrix3d walgo::Rotation::getMatrix()
{
	if ( _genMatrix ) return _rotMatrix;
	if ( _genEuler ) {
		//cout << "Using euler angles to calculation matrix" << endl;
		Matrix3d ma = getPrincipleMat(_axesA, _A);
		Matrix3d mb = getPrincipleMat(_axesB, _B);
		Matrix3d mc = getPrincipleMat(_axesC, _C);
	    _rotMatrix = _intrinsic? ma*mb*mc : mc*mb*ma;
	    _genMatrix = true;
	    //return _rotMatrix;
	}
	if ( _genQ ) {
		//cout << "using Q to calculate matrix" << endl;
		double q0 = _rotQ[0];
		double q1 = _rotQ[1];
		double q2 = _rotQ[2];
		double q3 = _rotQ[3];
		Matrix3d m;
		// can be derived through axis angle formulation
		m(0,0) = 2.0*(q0*q0+q1*q1) - 1.0;
		m(0,1) = 2.0*(q1*q2-q0*q3);
		m(0,2) = 2.0*(q1*q3+q0*q2);
		m(1,0) = 2.0*(q1*q2+q0*q3);
		m(1,1) = 2.0*(q0*q0+q2*q2) -1.0;
		m(1,2) = 2.0*(q2*q3-q0*q1);
		m(2,0) = 2.0*(q1*q3-q0*q2);
		m(2,1) = 2.0*(q2*q3+q0*q1);
		m(2,2) = 2.0*(q0*q0+q3*q3)-1.0;
		_rotMatrix = m;
		_genMatrix = true;
		return _rotMatrix;
	}
	//cout << "No data available to calculate rotation matrix" << endl;
	return _rotMatrix;

}

// angle is now between 0 and 2*Pi
void walgo::Rotation::getAngleAxis(double& angle, double& nx, double& ny, double& nz)
{
	if ( _genQ )
	{
		//cout << "_rotQ[0] = " << _rotQ[0] << endl;
		if ( _rotQ[0] > 1.0 ) {
			angle = 0.0;
			nx = 0.0;
			ny = 0.0;
			nz = 1.0;
			return;
		}
		angle = std::acos(_rotQ[0])*2.0;
		double t = 1.0-_rotQ[0]*_rotQ[0];
		if ( t <= 1e-12 ) {
			// for zero degree rotation use z axis
			nx = 0.0;
			ny = 0.0;
			nz = 1.0;
			return;
		}
		double s = 1.0/sqrt(1.0-_rotQ[0]*_rotQ[0]);
		nx = _rotQ[1]*s;
		ny = _rotQ[2]*s;
		nz = _rotQ[3]*s;
		if ( angle > PI ) {
			angle = PI*2.0-angle;
			nx = -nx;
			ny = -ny;
			nz = -nz;
		}
	}
}

Quaternion Rotation::getQ()
{
	if ( _genQ )
		return _rotQ;

}

void walgo::Rotation::apply(const point3dd& vectorIn, point3dd& vectorOut)
{
	getMatrix();
	VectorXd vin(3);
	vin[0] = vectorIn._x;
	vin[1] = vectorIn._y;
	vin[2] = vectorIn._z;
	VectorXd vout = _rotMatrix*vin;
	vectorOut._x = vout[0];
	vectorOut._y = vout[1];
	vectorOut._z = vout[2];
}
