/*
 * polymodel1d.cc
 *
 *  Created on: Nov 4, 2018
 *      Author: zhian
 */

#include <math.h>
#include <vector>
#include "polymodel1d.h"
#include "fitpoly.h"

using namespace std;
using namespace walgo;

PolyModel1D::PolyModel1D()
{
	_validTMin=0;
	_validTMax=0;
}

void PolyModel1D::setParams(const std::vector<double>& params)
{
	_order = round(params[0]);
}

void PolyModel1D::setData(const std::deque<double>& x, const std::deque<double>& t)
{
    assert(x.size()==t.size());
    _x.clear();
    _t.clear();
    int S= x.size();
    for(auto it:x) _x.push_back(it);
    for(auto it:t) _t.push_back(it);
}

void PolyModel1D::build()
{
	_fitPoly = make_shared<fitpoly<double>>(_t,_x, _order);
	if ( _t.size() > 0)
	{
		_imin = 0;
		_validTMin = _t[_imin];
		_imax = _t.size()-1;
		_validTMax = _t[_imax];
	}
}

double PolyModel1D::model(double T) const
{
	return _fitPoly->getPoly().val(T);
}
