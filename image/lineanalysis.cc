/*
 * lineanalysis.cc
 *
 *  Created on: Apr 11, 2016
 *      Author: zhian
 */

#include <lineanalysis.h>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace walgo;

const int EPS_X = 3;
const int EPS_Y = 3;
const float factor = 180.0/CV_PI;
const int ahistbin = 10;

bool walgo::calcAngleHistogram(const vector<Vec4i>& lines,
			vector<vector<Vec4i> >& hist,
			vector<double>& weights,
			int binsize)
{
	double hbin = binsize*0.5;
	double ibin = 1.0/binsize;
	int numbins = (int)(360*ibin+0.5);
	for ( size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		//cout << "(x1,y1) = " << l[0] << " " << l[1] << "   (x2,y2) = "
	        //				<< l[2] <<" " << l[3] << endl;
		double dy = (l[3]-l[1]);
		double dx = (l[2]-l[0]);
		double length = sqrt(dx*dx+dy*dy);
		double angle = factor*atan2(dy,dx);
	        //	cout << "angle = " << angle << endl;
		int bin = ((int) ((angle+180)*ibin+0.5))%numbins;
		hist[bin].push_back(l);
		weights[bin] += length;
	}
	cout << "line angle histogram: ";
	for ( int i = 0; i < numbins; i++)
	{
		cout << hist[i].size() <<"  ";
	}
	cout << endl;
	return true;
}

bool walgo::selectByAngle(const vector<Vec4i>& lines,
			       vector<Vec4i>& selectedLines,
				   double angle,
				   double angleRange)
{
	for ( size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		double dy = (l[3]-l[1]);
		double dx = (l[2]-l[0]);
		double length = sqrt(dx*dx+dy*dy);
		double angle1 = factor*atan2(dy,dx);
		if (fabs(angle1-angle) < angleRange ) selectedLines.push_back(l);
	}
}

bool walgo::selectMaxAngles(const vector<vector<Vec4i> >& hist,
		             vector<double>& weights,
			         int numbins,
			         vector<Vec4i>& selectedLines,
					 int angleRangeInBins)
{
	selectedLines.clear();
	// find first group of lines
	unsigned maxsize = 0;
	int imax = -1;
	for ( int i = 0; i < numbins;  i++)
	{
		if ( hist[i].size() > maxsize )
		{
			maxsize = hist[i].size();
			imax = i;
		}
	}
	if ( imax == -1)
	{
		cout << "no max group in selectMaxAngles " << endl;
		return false;
	}
	else
		cout << "max group of lines at: " << imax << endl;
	int ilow = imax-angleRangeInBins;
	if ( ilow < 0) ilow = 0;
	int ihigh = imax+angleRangeInBins;
	if ( ihigh >= numbins) ihigh = numbins-1;
	for ( int j = ilow; j <= ihigh; j++)
	{
		for ( auto && it : hist[j])
			selectedLines.push_back(it);
	}

	return true;
}

bool walgo::getMax2Groups(const vector<vector<Vec4i> >& hist,
			vector<double>& weights,
			int numbins,
			vector<Vec4i>& group1,
			vector<Vec4i>& group2,
			int minGroupDist)
{
	// find first group of lines
	unsigned maxsize = 0;
	int imax = -1;
	for ( int i = 0; i < numbins;  i++)
	{
		if ( hist[i].size() > maxsize )
		{
			maxsize = hist[i].size();
			imax = i;
		}
	}
	if ( imax == -1)
	{
		cout << "no max group in detectSeam " << endl;
		return false;
	}
	else
		cout << "first group of lines at: " << imax << endl;
	// find second group of lines
	group1 = hist[imax];
	int jmax = -1;
	maxsize = 0;
	for ( int i = 0; i< numbins; i++)
	{
		if ( (std::abs(i-imax)>minGroupDist) && (hist[i].size() > maxsize) )
		{
			maxsize = hist[i].size();
			jmax = i;
		}
	}
	if ( jmax == -1)
	{
		cout << "no max group 2 found "<< endl;
		return false;
	}
	else
		cout << "second group of lines at " << jmax << endl;
	group2 = hist[jmax];
	return true;
}

bool walgo::groupIntersect(std::vector<cv::Vec4i>& group1,
				    std::vector<cv::Vec4i>& group2,
				    cv::Point& point)
{
	// needs to improve
	float sumx = 0.0;
	float sumy = 0.0;
	int n = 0;
	for (size_t i = 0; i < group1.size(); i++)
	{
		for ( size_t j = 0; j < group2.size(); j++)
		{
			Vec4i l1 = group1[i];
			Vec4i l2 = group2[j];
			float x1 = l1[0];
			float y1 = l1[1];
			float x2 = l1[2];
			float y2 = l1[3];
			float u1 = l2[0];
			float v1 = l2[1];
			float u2 = l2[2];
			float v2 = l2[3];
			float x,y;
			float k1,c1,k2,c2;
			if ( (l1[0] != l1[2]) && (l2[0] != l2[2]))
			{
				k1 = (y2-y1)/(x2-x1);
				c1 = y1-k1*x1;
				k2 = (v2-v1)/(u2-u1);
				c2 = v1 -k2*u1;
				x = (c2-c1)/(k1-k2);
				y = k1*x+c1;
			}
			else
			{
				// for exact 90 degree lines
				cout << "TO BE DONE" << endl;
			}
			//cout <<"x,y = " << x << " "<< y << endl;
			sumx += x;
			sumy += y;
			n++;
		}

	}
	cout << "n = " << n << " sumx,y = "<< sumx << "  "<< sumy << endl;
	point.x = (int) (sumx/n+0.5);
	point.y = (int) (sumy/n+0.5);
	//cout << "point "<< point.x << "  "<< point.y << endl;
	return true;
}

bool walgo::getRanges(const vector<Vec4i>& group, int& xmin, int& xmax,
			int& ymin, int& ymax)
{
	xmin = 100000;
	xmax = 0;
	ymin = 100000;
	ymax = 0;
	for ( auto && it: group)
	{
		Vec4i l = it;
		int x1 = l[0];
		int y1 = l[1];
		int x2 = l[2];
		int y2 = l[3];
		if ( x1 > xmax ) xmax = x1;
		if ( x1 < xmin ) xmin = x1;
		if ( x2 > xmax ) xmax = x2;
		if ( x2 < xmin ) xmin = x2;
		if ( y1 > ymax ) ymax = y1;
		if ( y1 < ymin ) ymin = y1;
		if ( y2 > ymax ) ymax = y2;
		if ( y2 < ymin ) ymin = y2;
	}
}


