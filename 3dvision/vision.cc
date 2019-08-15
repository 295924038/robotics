
#include <iostream>
#include <fstream>
#include <string>
#include "point.h"
#include "3dutils.h"
#include "image3d.h"
#include "upsample.h"
#include "grid.h"
#include "search.h"

using namespace std;
using namespace walgo;



int main()
{

	int ratio = 50;
	image3d<float>* upuimage = new image3d<float>();
	image3d<float>* upvimage = new image3d<float>();
	grid gr;

	upuimage->readFromFile("uimage.dat");
	upvimage->readFromFile("vimage.dat");
	gr.readFromFile("grid.dat");
	//cout << "xmin = " << gr.getXMin() << " ymin = " << gr.getYMin() << endl;
	/***********************************************
	 * Test section
	 **********************************************/
	//point2d<float> pt1(845.801, 530.48);  // 545
	//point2d<float> pt2(741.791, 463.644);  // 568
	point2d<float> pt1(879.94, 634.312);   //334
	point2d<float> pt2(536.939, 638.054);   // 339
	float truez = -90.3391;
	//point2d<float> pt1(748.217, 695.328);
	//point2d<float> pt2(539.769, 491.263);
	float knownDist = sqrt(25.0)*10.0;
	cout << "knownDist = " << knownDist << endl;
	float accuz = 0.0;
	float scale = gr.getZGridPitch()/(float)ratio;
	float scalex = gr.getXGridPitch()/(float)ratio;
	point2d<float> objpt1, objpt2;
	int wherechanged = bisectionSearch(upuimage, upvimage, pt1, pt2, objpt1, objpt2,
					                gr, ratio, knownDist, accuz);
	if ( wherechanged ) {
		cout << "distance sign changed at " << wherechanged << endl;
		accuz = accuz*scale + gr.getZMin();
		cout << "Using linear interpolation z = " << accuz << endl;
		cout << "Error using linear interpolation = " << accuz-truez << endl;
		cout << "object location 1: " << objpt1._u*scalex+gr.getXMin() << "   " << objpt1._v*scalex + gr.getYMin() << endl;
		cout << "object location 2: " << objpt2._u*scalex+gr.getXMin() << "   " << objpt2._v*scalex + gr.getYMin() << endl;
	}
	else cout << "WARNING: sign didn't change" << endl;

	// clean up
	delete upuimage;
	delete upvimage;
}
