/*
 * searchdc.h
 *
 *  Created on: Mar 28, 2016
 *      Author: zhian
 */

#ifndef SEARCHDC_H_
#define SEARCHDC_H_
#include "model.h"

namespace walgo
{
float linearSearchDc(float u1, float v1, float u2, float v2,
					float& minz, float& maxz, int div, float& x, float& y,
					model* model1, model* model2);

float searchDc(float u1, float v1, float u2, float v2, float& x, float& y,
			   model* model1, model* model2);
}

#endif /* SEARCHDC_H_ */
