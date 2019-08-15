/*
 * spline.h
 *
 *  Created on: Aug 21, 2016
 *      Author: zhian
 */

#ifndef SPLINE_H_
#define SPLINE_H_

#include "curve.h"
#include <memory>

namespace walgo
{
template <class T>
using curvePtr = std::shared_ptr<curve<T> >;

template <class T>
class spline : public curve <T>
{
public:
	virtual ~spline() {}

	virtual point3d<T> pointAt(T t)
	{
		int i = floor(t);
		T localt = t - (float) i;
		curvePtr<T>& segment = _segments[i];

		return segment->pointAt(localt);
	}

	virtual point3d<T> getStartPt()
	{
		return _segments[0]->getStartPt();
	}
	virtual point3d<T> getEndPt()
	{
		size_t size = _segments.size();
		curvePtr<T>& lastcurve = _segments[size-1];
		return lastcurve->getEndPt();
	}

	virtual T getStartParam()
	{
		return 0.0;
	}

	virtual T getEndParam()
	{
		return (float)(_segments.size()+1);
	}

	virtual void addKnot(const point3d<T>& newpt) = 0;

	virtual void initKnots(const std::vector<point3d<T> >& knots) = 0;

	// making the <segmentID>'th segment the first one
	virtual void reset(int segmentID) = 0;

protected:
	std::vector<curvePtr<T> >  _segments;
};

}

#endif /* SPLINE_H_ */
