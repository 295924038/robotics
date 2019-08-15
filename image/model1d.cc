/*
 * model1d.cc
 *
 *  Created on: Nov 4, 2018
 *      Author: zhian
 */
#include <math.h>
#include "model1d.h"
using namespace walgo;
using namespace std;

double Model1D::getRMS() const
{
	int ns = _imax-_imin+1;
	double sumd2 = 0;
	for (int i = _imin; i <= _imax; i++)
	{
		double d = _x[i] - model(_t[i]);
		sumd2 += d*d;
	}
	return sqrt(sumd2/ns);
}

