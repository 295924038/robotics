/*
 * model3d.h
 *
 *  Created on: Nov 4, 2018
 *      Author: zhian
 */

#ifndef MODEL3D_H_
#define MODEL3D_H_
#include <assert.h>
#include <vector>
#include <deque>
#include "point.h"
namespace walgo {
enum XYZ{xlonger= 0,ylonger= 1,zlonger};

class Model3D
{
public:
	virtual ~Model3D() {}
	virtual void setData(const std::vector<double>& x,
				         const std::vector<double>& y,
                         const std::vector<double>& z)
	{
        _lenth= x.size();
        assert(x.size()==y.size()&&
               z.size()==y.size() && _lenth > 2);
        _x.clear();
        _y.clear();
        _z.clear();
        for(int i= 0;i < _lenth;i++)
        {
            _x.push_back(x[i]);
            _y.push_back(y[i]);
            _z.push_back(z[i]);
        }
	}

    virtual void updateData(double x, double y, double z)
	{
        _x.push_back(x);_x.pop_front();
        _y.push_back(y);_y.pop_front();
        _z.push_back(z);_z.pop_front();
	}
	virtual void setParams(const std::vector<double>& params) = 0;
	virtual double getStartT() const { return _validTMin; }
	virtual double getEndT() const { return _validTMax; }
	virtual void build() = 0;
	virtual point3d<double> model(double ) const = 0;
	virtual double getRMS() const = 0;

protected:
    std::deque<double> _x;
    std::deque<double> _y;
    std::deque<double> _z;
    std::deque<double> _t;
	double _validTMin;
	double _validTMax;
	int _imin;
	int _imax;
    unsigned int _lenth;
    XYZ _xyz;
};
}





#endif /* MODEL3D_H_ */
