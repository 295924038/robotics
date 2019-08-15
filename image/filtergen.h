#ifndef _FILTERGEN_H_
#define _FILTERGEN_H_

namespace walgo
{
const double c1 = 2.0/3.0;
const double c2 = 1.0/6.0;

/**
 * general cubic b_spline waveform
 */
double cubicBSpline(double x);

/**
 * 4 tab cubic b-spline filter on a regular grid.
 */

void cubicBSplineFilter(float* coeffs, float shift);

void genSGolay(int order, int length, int d, int offset, float* filter, float* grad);

}
#endif
