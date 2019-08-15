/*
 * bezier.h
 *
 *  Created on: Aug 14, 2016
 *      Author: zhian
 */

#ifndef BEZIER_H_
#define BEZIER_H_
#include <vector>
#include "curve.h"
#include "point.h"

namespace walgo
{

template <class T>
class bezier : public curve<T>
{
public:
	bezier(const std::vector<point3d<T> >& pts);
	virtual point3d<T> pointAt(T t);
	virtual point3d<T> getStartPt();
	virtual point3d<T> getEndPt();
	virtual T getStartParam() { return 0.0; }
	virtual T getEndParam()  { return 1.0; }
	virtual std::vector<point3d<T> > getControlPts() { return _contrPts; }
protected:
	virtual std::vector<point3d<T> > interp(const std::vector<point3d<T> >& temp, T t);
	std::vector<point3d<T> > _contrPts;
	int _order;
};

using std::vector;

template <class T>
bezier<T>::bezier(const vector<point3d<T> >& pts)
{
	_contrPts = pts;
	_order = pts.size() -1;
	//cout << "points are: " << endl;
	//for ( int i =0; i < pts.size(); i++)
	//	cout << pts[i] << "  " << endl;
}

template <class T>
point3d<T> bezier<T>::getStartPt()
{
	return _contrPts[0];
}

template <class T>
point3d<T> bezier<T>::getEndPt()
{
	return _contrPts[_order];
}

/**
 * Casteljau's algorithm,
 */
template <class T>
vector<point3d<T> > bezier<T>::interp(const vector<point3d<T> >& pts, T t)
{
	vector<point3d<T> > vals(pts.size()-1);
	for ( int i = 0; i < vals.size(); i++)
	{
		vals[i] = pts[i].times(1.0-t) + pts[i+1].times(t);
	}
	return vals;
}

template <class T>
point3d<T> bezier<T>::pointAt(T t)
{
	int n = _order;
	vector<point3d<T> > temp = _contrPts;
	vector<point3d<T> > temp2;
	while ( n )
	{
		temp2 = interp(temp, t);
		temp = temp2;
		n--;
	}
	//cout << "temp[0] = " << temp[0] << endl;
	return temp[0];
}

}
#endif /* BEZIER_H_ */
