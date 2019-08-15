/*
 * rpcmodel.h
 *
 *  Created on: Mar 27, 2016
 *      Author: zhian
 */

#ifndef LASERMODEL3_H_
#define LASERMODEL3_H_

#include "point.h"
#include "array2d.h"
#include "grid.h"

namespace walgo
{
class lasermodel3
{
public:
	enum BASIS
	{
		C=0,
		U,
		V,
		NUM_UPPER,
		IU = NUM_UPPER,
		IV,
		NUM_BASIS
	};

	lasermodel3()
	{}

	virtual ~lasermodel3() {}

	void calibrate();

        virtual void imgToObj(float u, float v, float& y, float& z, float& x);
        virtual void objToImg(float y, float z, float& u, float& v) {}

	void setData(pairmap<float>& pm)
	{
		_cdata = &pm;
	}

protected:

	void buildAB(pairmap<float>* pm);

	void solveyz();
private:
	array2d<double> _amatrixy;
	array2d<double> _amatrixz;
	std::vector<double> _bvectory;
	std::vector<double> _bvectorz;
	std::vector<double> _yrpc;
	std::vector<double> _zrpc;
	float _x;
	pairmap<float>* _cdata;

};
}


#endif /* RPCMODEL_H_ */
