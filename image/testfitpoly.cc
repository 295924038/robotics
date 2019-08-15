/*
 * testfitpoly.cc
 *
 *  Created on: Nov 6, 2017
 *      Author: zhian
 */

#include "fitpoly.h"
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>

using namespace std;
using namespace walgo;

int main()
{
	vector<double> x;
	vector<double> y;
	ifstream ifs("test.dat");
	double v;
	int i = 1;
	while( ifs >> v)
	{
		x.push_back(i);
		y.push_back(v);
		cout << " added i, v" << i << "  "<< v << endl;
		i++;
	}
	fitpoly<double> fp(x, y, 5);
	const vector<double>& cnew = (fp.getPoly()).getCoeffs();

	for ( auto && it : cnew ) cout << it << endl;
	const poly<double>& p = fp.getPoly();
	for ( int i = 1; i < x.size()+1; i++)  {
		cout << p.val(i) << endl;
	}
}
