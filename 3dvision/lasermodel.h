/*
 * lasermodel.h
 *
 *  Created on: Mar 27, 2016
 *      Author: zhian
 */

#ifndef LASERMODEL_H_
#define LASERMODEL_H_

#include "point.h"
#include "array2d.h"
#include "grid.h"


class lasermodel
{
public:


	lasermodel();

	virtual ~lasermodel() {}

	void calibrate();

	virtual void imgToObj(float u, float v, float& z, float& x, float& y);
	virtual void objToImg(float x, float z, float& u, float& v);

	void setData(walgo::pairmap<float>& pm)
	{
		_cdata = &pm;
	}

protected:

private:

	int _cameraSizeX;
	int _cameraSizeY;
	int _centerX;
	int _centerY;
	// use z = _b*v + _c
	//     y = _a
	float _a;
	float _b;
	float _c;
	// x = _d*u+_e
	float _d;
	float _e;
	walgo::pairmap<float>* _cdata;

};



#endif /* LASERMODEL_H_ */
