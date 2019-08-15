/*
 * rotation.h
 *
 *  Created on: Apr 1, 2017
 *      Author: zhian
 */

#ifndef ROTATION_H_
#define ROTATION_H_

#include <Eigen/Dense>
#include "defines.h"
#include "quaternion.h"
#include "point.h"

/**
 * Rotation class
 */
namespace walgo
{
class Rotation
{
public:
	static double toRad(double angle, bool isRad)
	{
		return (isRad? angle : angle*PI/180.0);
	}

	static double toDeg(double angle, bool toDeg)
	{
		return (toDeg? angle*180.0/PI : angle);
	}

        enum AXIS
        {
                X = 1,
                Y = 2,
                Z = 3
        };

        /**
         * type of euler  angles, proper and non-proper
         */
        enum EULERTYPE
        {
                zxz = 313,
                zyx = 321,
                zyz = 323,
        };
	Rotation() {}
	/**
	 * create from euler angles
	 * intrinsic means that axes are local
	 */
	Rotation(double A, double B, double C,
			 bool isRad, EULERTYPE type,
			 bool intrinsic);
	Rotation(double angle, double nx, double ny, double nz);

	Rotation(const Quaternion& q);

	Rotation(const Eigen::Matrix3d& m);

	Rotation inverse();

	void getEulerAngles(double& A, double& B, double& C,
				        bool useDeg, EULERTYPE type,
						bool intrinsic);
	void getEulerAngles(double& A, double& B, double& C)
	{
		getEulerAngles(A, B, C, false, _type, _intrinsic);
	}

	Eigen::Matrix3d getPrincipleMat(int axis, double angle);

	Rotation times(const Rotation& rot);

	Quaternion getQ();

	Eigen::Matrix3d getMatrix();

	void getAngleAxis(double& angle, double& nx, double& ny, double& nz);

	EULERTYPE getType() const { return _type; }

	void apply(const point3dd& vectorIn, point3dd& vectorOut);

protected:
	void calcQ();

private:
	double _A;
	double _B;
	double _C;
	int _axesA;
	int _axesB;
	int _axesC;
	Quaternion _rotQ;
	bool _intrinsic;
	EULERTYPE _type;

	Eigen::Matrix3d _rotMatrix;

	bool _genEuler;
	bool _genMatrix;
	bool _genQ;
};


}
#endif /* ROTATION_H_ */
