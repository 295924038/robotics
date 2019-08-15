/*
 * smooth.cc
 *
 *  Created on: Oct 6, 2016
 *      Author: zhian
 */
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include "array2d.h"
#include "filtergen.h"

using namespace std;
using namespace walgo;

void readData(string fname, array2d<float>& data)
{
	ifstream dfile(fname);
	if ( !dfile )
	{
		cout << "unable to open " << fname << endl;
	}
	data.resize(22, 146);
	for ( int i = 0; i < 146; i++)
		for ( int j = 0; j < 22; j++ )
		dfile >> data[i][j];
	return;
}

const double pi = 3.14159265;

int main()
{
	array2d<float> data;
	readData("data.csv", data);
	int length = 15;
	int order = 5;
	float* filter = new float[length];
	float* dfilter = new float[length];
	genSGolay(order,length,1,0, filter, dfilter);

	double dx,dy,dz;

	double angleb;
	double anglec;
	int n = 0;
	double sumb, sumc;
	double sumb2, sumc2;
	sumb = 0;
	sumc = 0;
	sumb2 = 0;
	sumc2 = 0;
	int e = (length-1)/2;
	for ( int i = e; i < 126-e; i++)
	{
		n++;
		dx = 0;
		dy = 0;
		dz = 0;
		for ( int l = 0; l < length; l++)
		{
			dx = dx + dfilter[l]*data[i-e+l][13];
			dy = dy + dfilter[l]*data[i-e+l][14];
			dz = dz + dfilter[l]*data[i-e+l][15];
		}
		angleb = atan2(dy, dx)*180/pi;
		anglec = atan2(-dz, sqrt(dx*dx+dy*dy))*180/pi;
		sumb += angleb;
		sumc += anglec;
		sumb2 += angleb*angleb;
		sumc2 += anglec*anglec;
		cout <<   angleb << "   " << anglec << endl;
	}
	double aveb = sumb/n;
	double avec = sumc/n;
	double rmsb = sqrt(sumb2/n-aveb*aveb);
	double rmsc = sqrt(sumc2/n-avec*avec);
	cout << "n = " << n << " rmsb = " << rmsb << "  rmsc = " << rmsc << endl;
	delete [] filter;
	delete [] dfilter;
}


