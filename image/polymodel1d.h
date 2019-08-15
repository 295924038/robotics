/*
 * polymodel1d.h
 *
 *  Created on: Nov 4, 2018
 *      Author: zhian
 */

#ifndef POLYMODEL1D_H_
#define POLYMODEL1D_H_


#include "fitpoly.h"
#include "model1d.h"
#include <deque>
#include <memory>

namespace walgo {
template <class T>
using FitPolyPtr =  std::shared_ptr<fitpoly<T> >;

class PolyModel1D : virtual public Model1D
{
public:
	PolyModel1D();
	virtual ~PolyModel1D() {}
	virtual void setParams(const std::vector<double>& params);
    virtual void setData(const std::deque<double>& x, const std::deque<double>& t);
	virtual void build();
	virtual double model(double t) const;
private:
	int _order;
	FitPolyPtr<double>  _fitPoly;
};
}

#endif /* POLYMODEL1D_H_ */
