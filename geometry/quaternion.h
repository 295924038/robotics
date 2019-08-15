/*
 * quarternion.h
 *
 *  Created on: Apr 1, 2017
 *      Author: zhian
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

#include <math.h>

namespace walgo
{
class Quaternion
{
public:
	Quaternion(double q[4]);

	Quaternion() {}

	Quaternion times(const Quaternion& q) const;
   
        double dot(const Quaternion& q) const;

	Quaternion add(const Quaternion& q) const;

	Quaternion sub(const Quaternion& q) const;

	double norm()
	{
		return sqrt(_q[0]*_q[0]+_q[1]*_q[1]+_q[2]*_q[2]+_q[3]*_q[3]);
	}

	void normalize();

	Quaternion conj() const;

	double operator[](int i) { return _q[i]; }

private:
	double _q[4];

};

}

#endif /* QUARTERNION_H_ */
