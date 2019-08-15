/*
 * curve.h
 *
 *  Created on: Aug 14, 2016
 *      Author: zhian
 */

#ifndef CURVE_H_
#define CURVE_H_
#include <memory>
#include "point.h"

namespace walgo
{
template <class T>
class curve
{
public:
	virtual ~curve() {}
	/*
	 * calculates the curve point at parameter value t.
	 */
	virtual point3d<T> pointAt(T t) = 0;
	/*
	 * starting point3d
	 */
	virtual point3d<T> getStartPt() = 0;
	/*
	 * ending point3d
	 */
	virtual point3d<T> getEndPt() = 0;
	/*
	 * default is 0.0
	 */
	virtual T getStartParam() = 0;
	/*
	 * default is 1.0
	 */
	virtual T getEndParam() = 0;

	virtual std::vector<point3d<T> > getControlPts() = 0;

};
}

#endif /* CURVE_H_ */
