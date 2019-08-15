/*
 * tracker.cc
 *
 *  Created on: Nov 1, 2016
 *      Author: zhian
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include "bench.h"
#include "utils.h"
#include "pathdetector.h"

using namespace cv;
using namespace std;
using namespace walgo;

int main(int argc, char** argv)
{
	if (argc < 4  || argc == 5)
	{
		cout << "usage: " << argv[0] << " <directory> lmin lmax" << endl;
		cout << "or " << argv[0] << " <directory> lmin lmax smin smax" << endl;
		cout << "where smin smax is range of images you want to display" << endl;
		exit(0);
	}
	Mat frame;
	string fname;

	string directory = argv[1];
	int nmin = atoi(argv[2]);
	int nmax = atoi(argv[3]);

	PathDetector pd(nmax, PathDetector::D2POINT_2);
	int smin = nmin;
	int smax = nmax;
	if ( argc == 6)
	{
		smin = atoi(argv[4]);
		smax = atoi(argv[5]);
		pd.setShowImage();
		pd.setShowImageIndices(smin, smax);
	}

	for (int l=nmin; l < nmax; l++)
	{
		bench b;
		fname = string(directory)+string("/")+to_string(l)+string(".jpg");
		//读取下个图形
		cout << "Reading " << fname << "..... "<< endl;
		frame = imread(fname, 0);
		if ( !(frame.data))
		{
	      cout << "can't read file " << fname << endl;
	      //exit(1);
	      continue;
	    }
		b.start();
		pd.addImage(frame, l);
		b.stop();
		cout << "addImage used: " << b.duration() << endl;
	}
	vector<Point2f> epts;
	vector<Point> uLinePts, vLinePts;
	vector<vector<Point>> uSegments, vSegments;
	vector<int> endSteps;
	std::vector<float> jjmm;
	pd.detect(epts, endSteps, uLinePts, vLinePts, uSegments, vSegments,jjmm);
	cout << " Seam start/end: ";
	for ( int i = 0; i < endSteps.size()/2; i = i+2)
			cout << endSteps[i*2] << "  " << endSteps[i*2+1] << "  ";
	cout << endl;
	return 0;

}



