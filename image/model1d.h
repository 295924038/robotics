/*
 * model1d.h
 *
 *  Created on: Dec 5, 2017
 *      Author: zhian
 */

#ifndef _MODEL1D_H_
#define _MODEL1D_H_
#include <assert.h>
#include <vector>
namespace walgo {
class Model1D
{
public:
	virtual ~Model1D() {}
	virtual void setData(const std::vector<double>& x, const std::vector<double>& t)
	{
		assert(x.size()==t.size());
		_x = x;
		_t = t;
	}
	virtual void updateData(double x, double t)
	{
		_x.erase(_x.begin());
		_t.erase(_t.begin());
		_x.push_back(x);
		_t.push_back(t);
	}
	virtual void setParams(const std::vector<double>& params) = 0;
	virtual double getStartT() const { return _validTMin; }
	virtual double getEndT() const { return _validTMax; }
	virtual void build() = 0;
	virtual double model(double ) const = 0;
	virtual double getRMS() const;

protected:
	std::vector<double> _x;
	std::vector<double> _t;
	double _validTMin;
	double _validTMax;
	int _imin;
	int _imax;

};
}

#endif /* MODEL1D_H_ */
