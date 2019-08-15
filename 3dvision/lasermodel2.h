/*
 * rpcmodel.h
 *
 *  Created on: Mar 27, 2016
 *      Author: zhian
 */

#ifndef LASERMODEL2_H_
#define LASERMODEL2_H_

#include "point.h"
#include "array2d.h"
#include "grid.h"

namespace walgo
{
class lasermodel2
{
public:
	enum BASIS
	{
		C=0,
		U,
		V,
		NUM_BASIS
	};

	lasermodel2()
	{}

	virtual ~lasermodel2() {}

	void calibrate();

	virtual void imgToObj(float u, float v, float& z, float& x, float& y);
	virtual void objToImg(float x, float z, float& u, float& v) {}

	void setData(pairmap<float>& pm)
	{
		_cdata = &pm;
	}

protected:

	void buildAB(pairmap<float>* pm);

	void solvexz();
private:
	array2d<double> _amatrix;
	std::vector<double> _bvectorx;
	std::vector<double> _bvectorz;
	std::vector<double> _xrpc;
	std::vector<double> _zrpc;
	float _y;
	pairmap<float>* _cdata;

};

}

#endif /* RPCMODEL_H_ */
