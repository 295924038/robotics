#include "search.h"
#include <iostream>
#include "point.h"
#include "image3d.h"
#include "grid.h"

using namespace std;
using namespace walgo;

void walgo::findNearest(image<float>& u, image<float>& v,
				  point2d<float>& pt1, point2d<float>& pt2,
				  point2d<float>& gridpt1, point2d<float>& gridpt2,
				  float& dist)
{
	int sizey = u.getSizeY();
	int sizex = u.getSizeX();
	float mindist1 = 10000000.0;
	float mindist2 = 10000000.0;
	int xmin1 = -1;
	int ymin1 = -1;
	int xmin2 = -1;
	int ymin2 = -1;
	for ( int y = 0; y < sizey; y++)
	{
		for ( int x = 0; x < sizex; x++)
		{
			float dist1 =  (u[y][x]-pt1._u)*(u[y][x]-pt1._u)+(v[y][x]-pt1._v)*(v[y][x]-pt1._v);
			float dist2 =  (u[y][x]-pt2._u)*(u[y][x]-pt2._u)+(v[y][x]-pt2._v)*(v[y][x]-pt2._v);
			if ( dist1 < mindist1)
			{
				mindist1 = dist1;
				ymin1 = y;
				xmin1 = x;
			}
			if ( dist2 < mindist2)
			{
				mindist2 = dist2;
				ymin2 = y;
				xmin2 = x;
			}

		}
	}
	// locate subpixel u/v values
	// first find u/v values at xmin/ymin point
	fineInterp(u, v, xmin1, ymin1, pt1, gridpt1);
	fineInterp(u, v, xmin2, ymin2, pt2, gridpt2);
/*
	gridpt1._u = xmin1;
	gridpt1._v = ymin1;
	gridpt2._u = xmin2;
	gridpt2._v = ymin2;
*/
	//cout <<" mindist1 = " << mindist1 << endl;
	//cout << "mindist2 = " << mindist2 << endl;
	dist = gridpt1.dist(&gridpt2);
	return;
}

void walgo::fineInterp(image<float>& u, image<float>& v, int xmin, int ymin,
		        point2d<float>& pt, point2d<float>& gridpt)
{
	if ( xmin == 0 || xmin == u.getSizeX()-1 ||
		 ymin == 0 || ymin == u.getSizeY()-1)
	{
		// avoid memory error
		gridpt._u = xmin;
		gridpt._v = ymin;
		return;
	}
	//cout << "**** integer: " << xmin << "  " << ymin << endl;
	float cu = u[ymin][xmin];
	float cv = v[ymin][xmin];
	float fx, fy;  // points at grid coordinates (object location)
	bool upolarity = (u[ymin][xmin+1] > u[ymin][xmin]);
	bool vpolarity = (v[ymin+1][xmin] > v[ymin][xmin]);
	if ( (pt._u >= cu && upolarity) ||
		 (pt._u < cu && !upolarity))
	{
		fx = xmin + (pt._u-cu)/(u[ymin][xmin+1]-u[ymin][xmin]);
	}
	else
	{
		fx = xmin - (pt._u-cu)/(u[ymin][xmin-1]-u[ymin][xmin]);
	}
	if ( (pt._v >= cv && vpolarity) ||
		 (pt._v < cv && !vpolarity))
	{
		fy = ymin + (pt._v-cv)/(v[ymin+1][xmin]-v[ymin][xmin]);
	}
	else
	{
		fy = ymin - (pt._v-cv)/(v[ymin-1][xmin]-v[ymin][xmin]);
	}
	gridpt._u = fx;
	gridpt._v = fy;
	//cout << "***** float: " << fx << "  " << fy << endl;
}

int walgo::linearSearch(image3d<float>* upuimage, image3d<float>* upvimage,
				 point2d<float>& pt1, point2d<float>& pt2,
				 point2d<float>& objpt1, point2d<float>& objpt2,
				 grid& gr, int ratio, float knownDist,
				 float& accuz)
{
	float dist;
	int sizez = upuimage->getSizeZ();
	float minDiff = 10000;
	bool sign = false;
	bool lastsign = false;
	bool signchanged = false;
	int wherechanged = 0;
	float lastdist = 0.0;
	float scale = gr.getXGridPitch()/(float)ratio;
	point2d<float> pt1_prev;
	point2d<float> pt2_prev;
	for ( int i = ratio; i < sizez-2*ratio; i++ )
	{
		point2d<float> gridpt1;
		point2d<float> gridpt2;
		findNearest((*upuimage)[i], (*upvimage)[i], pt1, pt2,  gridpt1, gridpt2, dist);
	    // cout << "SEARCH: at plane " << i << " found closest to (" << pt1._u << ", " << pt1._v << ") at ("
		//		<< gridpt1._u << ", "<< gridpt1._v << ")" << endl;
		//cout << "SEARCH: at plane " << i << " found closest to (" << pt2._u << ", " << pt2._v << ") at ("
		//				<< gridpt2._u << ", "<< gridpt2._v << ")" << endl;
		dist = dist*scale;
	   // cout << "at plane " << i << " dist = " << dist << endl;
		if ( i == ratio) lastsign = (dist < knownDist);
		sign = (dist < knownDist );
		if ( sign != lastsign )
		{
			signchanged = true;
			wherechanged = i;
			float f = (knownDist-lastdist)/(dist-lastdist);
			//cout << "f = " << f << endl;
			accuz = (float) (i-1) + f;
			objpt1._u = pt1_prev._u*(1.0-f)+gridpt1._u*f;
			objpt1._v = pt1_prev._v*(1.0-f)+gridpt1._v*f;
			objpt2._u = pt2_prev._u*(1.0-f)+gridpt2._u*f;
			objpt2._v = pt2_prev._v*(1.0-f)+gridpt2._v*f;
			break;
		}
		lastsign = sign;
		lastdist = dist;
		pt1_prev = gridpt1;
		pt2_prev = gridpt2;
	}
	return wherechanged;

}

int walgo::bisectionSearch(image3d<float>* upuimage, image3d<float>* upvimage,
				 point2d<float>& pt1, point2d<float>& pt2,
				 point2d<float>& objpt1, point2d<float>& objpt2,
				 grid& gr, int ratio, float knownDist,
				 float& accuz)
{

	float dist;
	int sizez = upuimage->getSizeZ();
	float minDiff = 10000;
	bool sign = false;
	bool lastsign = false;
	bool signchanged = false;
	int wherechanged = 0;
	float lastdist = 0.0;
	float scale = gr.getXGridPitch()/(float)ratio;
	int lefti = ratio;
	int righti = sizez-2*ratio;
	float leftdist, rightdist;
	point2d<float> leftpt1;
	point2d<float> leftpt2;
	point2d<float> rightpt1;
	point2d<float> rightpt2;
	point2d<float> gridpt1, gridpt2;
	findNearest((*upuimage)[lefti], (*upvimage)[lefti], pt1, pt2,  leftpt1, leftpt2, dist);
	leftdist = dist*scale;
	bool leftsign = (leftdist < knownDist);
	findNearest((*upuimage)[righti], (*upvimage)[righti], pt1, pt2, rightpt1, rightpt2, dist);
	rightdist = dist*scale;
	bool rightsign = (rightdist < knownDist);
	if ( rightsign == leftsign )
	{
		cout <<"WARNING:  no sign change.  Search terminated" << endl;
		return 0;
	}
	while ( abs(righti-lefti)>1 )
	{
		int midi = (lefti+righti)/2;
		findNearest((*upuimage)[midi], (*upvimage)[midi], pt1, pt2,  gridpt1, gridpt2, dist);
		dist = dist*scale;
		sign = (dist < knownDist);
		if ( sign == leftsign)
		{
			lefti = midi;
			leftdist = dist;
			leftpt1 = gridpt1;
			leftpt2 = gridpt2;
		}
		else
		{
			righti = midi;
			rightdist = dist;
			rightpt1 = gridpt1;
			rightpt2 = gridpt2;
		}
	}
	wherechanged = lefti;
	float f = (knownDist-leftdist)/(rightdist-leftdist);
	cout << "f = " << f << endl;
	accuz = (float) (lefti) + f;
	objpt1._u = leftpt1._u*(1.0-f)+rightpt1._u*f;
	objpt1._v = leftpt1._v*(1.0-f)+rightpt1._v*f;
	objpt2._u = leftpt2._u*(1.0-f)+rightpt2._u*f;
	objpt2._v = leftpt2._v*(1.0-f)+rightpt2._v*f;
	return wherechanged;

}
