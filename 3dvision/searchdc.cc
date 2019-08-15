/*
 * searchdc.cc
 *
 *  Created on: Mar 28, 2016
 *      Author: zhian
 */

#include <math.h>
#include "searchdc.h"
#include <iostream>

using namespace std;
using namespace walgo;


float walgo::linearSearchDc(float u1, float v1, float u2, float v2,
			   float& minz, float& maxz, int div, float& x, float& y,
		       model* model1, model* model2)
{
	//cout << "search between " << minz <<" and " << maxz << endl;
	double mindist = 10000000;
	double dz = (maxz-minz)/div;
	float result = -1;
	float x1,y1,x2,y2;
	int mini = -1;
	for ( int i = 0; i <= div; i++)
	{
		float z = minz + i*dz;
		model1->imgToObj(u1,v1,z,x1,y1);
		model2->imgToObj(u2,v2,z,x2,y2);
		float dx = x2-x1;
		float dy = y2-y1;
		float dist = sqrt(dx*dx + dy*dy);
		//cout << "dist at z = " << z << " is "<< dist << endl;
		if ( dist < mindist )
		{
			mindist = dist;
			mini = i;
			x = x1;
			y = y1;
			result = z;
		}
	}
	if ( mini == 0) minz = result;
	else minz = result -dz;
	if ( mini == div) maxz = result;
	else maxz = result + dz;
	return result;
}

float walgo::searchDc(float u1, float v1, float u2, float v2, float& x, float& y,
			   model* model1, model* model2)
{
	float minz = model1->getMinZ();
	float maxz = model1->getMaxZ();
	float range = model1->getRange();
	minz = minz - range;
	maxz = maxz + range;
	float ztol = 0.1;
	int div = 10;
	float result = 0.0;
	while ( maxz-minz > ztol)
		result= linearSearchDc(u1,v1,u2,v2,minz,maxz, div, x, y, model1, model2);

	return result;
}
