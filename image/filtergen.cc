#include <math.h>
#include "filtergen.h"
#include <iostream>

using namespace std;
using namespace walgo;

double walgo::cubicBSpline(double x)
{
	double retVal = 0.0;
	if ( x < -2.0 ) return 0;
	if ( x >= -2.0 && x <= -1.0 ) {
		double y = 2.0+x;
		retVal = c2*y*y*y;
		return retVal;
	}
	else if ( x >= -1.0 && x <= 1.0) {
		double y = fabs(x);
		retVal = c1 + 0.5*y*y*y-y*y;
		return retVal;
	}
	else if ( x >= 1.0 && x <= 2.0 ) {
		double y = 2.0-x;
		retVal = c2*y*y*y;
		return retVal;
	}
	return 0.0;
}

void walgo::cubicBSplineFilter(float* coefs, float shift )
{
	// coeffs has to have size 4.
	if ( shift >= 0 ) {
		// for positive shift in (0,1), interpolate using
		// -1,0,1,2
		coefs[0] = cubicBSpline(shift+1.0);
		coefs[1] = cubicBSpline(shift);
		coefs[2] = cubicBSpline(shift-1.0);
		coefs[3] = cubicBSpline(shift-2.0);
	}
	else {
		// for negative shift in (-1,0), interpolate using
		// -2, -1, 0, 1
		coefs[0] = cubicBSpline(shift+2.0);
		coefs[1] = cubicBSpline(shift+1.0);
		coefs[2] = cubicBSpline(shift);
		coefs[3] = cubicBSpline(shift-1.0);
	}
	return;
}

static float sg57[7][7] = {{0.9989177,  0.0064935,  -0.0162338,   0.0216450,  -0.0162338,   0.0064935,  -0.0010823},
		   {0.0064935,   0.9610390,   0.0974026,  -0.1298701,   0.0974026,  -0.0389610,   0.0064935},
		  {-0.0162338,   0.0974026,   0.7564935,   0.3246753,  -0.2435065,   0.0974026,  -0.0162338},
		   {0.0216450,  -0.1298701,   0.3246753,   0.5670996,   0.3246753,  -0.1298701,   0.0216450},
		  {-0.0162338,   0.0974026,  -0.2435065,   0.3246753,   0.7564935,   0.0974026,  -0.0162338},
		   {0.0064935,  -0.0389610,   0.0974026,  -0.1298701,   0.0974026,   0.9610390,   0.0064935},
		  {-0.0010823,   0.0064935,  -0.0162338,   0.0216450,  -0.0162338,   0.0064935,   0.9989177}};
static float sg571[7][7] = {
		  {-2.0735e+00,   3.7409e+00,  -1.8523e+00, -8.6364e-01,   1.8977e+00,  -1.0591e+00,   2.0985e-01},
		  {-2.6667e-01,  -6.8333e-01,   1.0000e+00,   3.3333e-01,  -6.6667e-01,   3.5000e-01,  -6.6667e-02},
		  {7.8030e-02,  -6.6818e-01,   8.7121e-02,   4.3939e-01,   1.7045e-01,  -1.3485e-01,   2.8030e-02},
		  {-1.6667e-02,   1.5000e-01,  -7.5000e-01,  -4.6829e-16,   7.5000e-01,  -1.5000e-01,   1.6667e-02},
		  {-2.8030e-02,   1.3485e-01,  -1.7045e-01,  -4.3939e-01,  -8.7121e-02,   6.6818e-01,  -7.8030e-02},
		  {6.6667e-02,  -3.5000e-01,   6.6667e-01,  -3.3333e-01,  -1.0000e+00,   6.8333e-01,   2.6667e-01},
		  {-2.0985e-01,   1.0591e+00,  -1.8977e+00,   8.6364e-01,   1.8523e+00,  -3.7409e+00,   2.0735e+00}};

static float sg37[7] = {-0.095238,   0.142857,   0.285714,   0.333333,   0.285714,   0.142857,  -0.095238};

static float sg371[7] = {8.7302e-02,  -2.6587e-01,  -2.3016e-01,   7.6008e-17,   2.3016e-01,   2.6587e-01,  -8.7302e-02};

static float sg391[9] = {7.2391e-02,  -1.1953e-01,  -1.6246e-01,  -1.0606e-01,  -7.5391e-19,   1.0606e-01,   1.6246e-01,   1.1953e-01,  -7.2391e-02};

static float sg3151[15] = {3.8671e-02,  -1.2333e-02,  -4.2346e-02,  -5.4867e-02,  -5.3395e-02,  -4.1427e-02,  -2.2463e-02,   5.8100e-18,   2.2463e-02,   4.1427e-02,   5.3395e-02,   5.4867e-02,   4.2346e-02,   1.2333e-02,  -3.8671e-02};

static float sg5151[15] = {-3.1099e-02,   6.7405e-02,   2.5891e-02,  -5.1800e-02,  -1.0574e-01,  -1.1113e-01,  -6.9511e-02,  -2.6227e-17,   6.9511e-02,   1.1113e-01,   1.0574e-01,   5.1800e-02,  -2.5891e-02,  -6.7405e-02,   3.1099e-02};

void walgo::genSGolay(int order, int length, int d, int offset, float* filter, float* grad)
{
	/*
	if ( order != 5 && length != 7 )  {
		cout << "error unsupported order and size for sgolay" << endl;
		exit(1);
	}
	*/
	if ( order == 5 && length == 7)
	{
		int row = 3+offset;
		for ( int i = 0; i < length; i++)
			filter[i] = sg57[row][i];

		for ( int i = 0; i < length; i++)
			grad[i] = sg571[row][i];
	}
	if ( order == 3 && length == 7 )
	{
		for ( int i = 0; i < length; i++)
		{
			filter[i] = sg37[i];
			grad[i] = sg371[i];
		}
	}
	if ( order == 3 && length == 9)
	{
		for ( int i = 0; i < length; i++)
			grad[i] = sg391[i];
	}
	if ( order == 3 && length == 15)
	{
		for ( int i = 0; i < length; i++)
			grad[i] = sg3151[i];
	}
	if ( order == 5 && length == 15)
	{
		for ( int i = 0; i < length; i++)
			grad[i] = sg5151[i];
	}
}
