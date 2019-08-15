/*
 * model.h
 *
 *  Created on: Mar 27, 2016
 *      Author: zhian
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "point.h"

namespace walgo
{
class model
{
public:
	virtual void calibrate()=0;
	virtual void imgToObj(float u, float v, float z, float& x, float& y)= 0;
	virtual void objToImg(float x, float y, float z, float& u, float& v)= 0;
	virtual double getMinZ() = 0;
	virtual double getMaxZ() = 0;
	virtual double getRange() = 0;
	virtual void setRange(double range) = 0;
};

}

#endif /* MODEL_H_ */
