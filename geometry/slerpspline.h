/*
 * slerpspline.h
 *
 *  Created on: Oct 3, 2017
 *      Author: zhian
 */

#ifndef SLERPSPLINE_H_
#define SLERPSPLINE_H_

#include "slerp.h"
#include "spline.h"
#include <memory>

namespace walgo
{
template <class T>
class slerpspline : public spline<T>
{
public:
    slerpspline() {}
	virtual void initKnots(const std::vector<point3d<T> >& knots);

	virtual void addKnot(const point3d<T>& newpt) {}

	virtual void reset(int segmentID) {}

	virtual std::vector<point3d<T> > getControlPts()  { return {}; }
};

template <class T>
void slerpspline<T>::initKnots(const std::vector<point3d<T> >& knots)
{
	for ( int i = 0; i < knots.size()-1; i++)
	{
		auto s = std::make_shared<slerp<T> >(knots[i], knots[i+1]);
		this->_segments.push_back(s);
	}
}

}
#endif /* SLERPSPLINE_H_ */
