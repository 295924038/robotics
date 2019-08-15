#include <iostream>
#include <fstream>
#include "3dutils.h"
#include "grid.h"
#include "point.h"


using namespace std;
using namespace walgo;


bool walgo::readData(const char* fname, pairmap<float>& pm)
{
	float u,v,x,y,z;
	ifstream ifs(fname);
	if ( !ifs )
	{
		cout << "can't read file " << fname << endl;
		return false;
	}
	int i = 0;
	int j;
	while (ifs >> j && ifs >> u && ifs >> v && ifs >> x && ifs >> y && ifs >> z)
	{
		pm.addPair(make_shared<iopair<float>>(i,u,v,x,y,z));
		i++;
	}
	cout << "read " << i << " entries of data" << endl;
	return true;
}

bool walgo::readDataDc(const char* fname, pairmap<float>& pm1, pairmap<float>& pm2)
{
	float u1,v1, u2, v2, x,y,z;
	float dummy;
	ifstream ifs(fname);
	if ( !ifs )
	{
		cout << "can't read file " << fname << endl;
		return false;
	}
	int i = 0;
	int j;
	while ( ifs >> j && ifs >> dummy &&
			ifs >> dummy && ifs >> dummy && ifs >> dummy && ifs >> u2 &&
			ifs >> v2 && ifs >> u1 && ifs >> v1 && ifs >> dummy && ifs >> dummy &&
			ifs >> dummy && ifs >> x && ifs >> y && ifs >> z
			)
	{
		pm1.addPair(make_shared<iopair<float>>(i,u1,v1,x,y,z));
		pm2.addPair(make_shared<iopair<float>>(i,u2,v2,x,y,z));
		i++;
	}
	cout << "read " << i << " entries of data" << endl;
	return true;
}

bool walgo::readDataLaser(const char* fname, pairmap<float>& pm)
{
	float  u, v, x,y,z;
	float dummy;
	ifstream ifs(fname);
	if ( !ifs )
	{
		cout << "can't read file " << fname << endl;
		return false;
	}
	int i = 0;
	while ( ifs >> x && ifs >> y && ifs >> z && ifs >> u && ifs >> v )
	{
		//cout << "read: " << u << " " << v << " " << x << " " << y << " " << z << endl;
		float offset = ( -i%5 + 2 )*10;
	//	x += offset;
		pm.addPair(make_shared<iopair<float>>(i,u,v,x,y,z));
		i++;
	}
	cout << "read " << i << " entries of data" << endl;
	return true;
}


void walgo::createImages(pairmap<float>& pm,
		image3d<float>*& uimages,
		image3d<float>*& vimages,
		grid& gr)
{
	int sizex = gr.getSizeX();
	int sizey = gr.getSizeY();
	int sizez = gr.getSizeZ();
	cout << "sizes from grid: " << sizex << " " << sizey << " " << sizez << endl;
	uimages = new image3d<float>(sizex, sizey, sizez);
	vimages = new image3d<float>(sizex, sizey, sizez);

	for ( auto& it : pm._map)
	{
		iopair<float>& iop = *(it.second);
		point3d<int> gridlocation;
		gr.pt2grid(iop._obj._x, iop._obj._y, iop._obj._z, gridlocation);

		point3d<float> testp;
		gr.grid2pt(gridlocation._x, gridlocation._y, gridlocation._z, testp);
		float dist =  iop._obj.dist(&testp);
		if ( dist > 1e-5 )
		{
			cout << "ERROR: point conversion may be wrong: " << endl;
			cout << "Original location: " << iop._obj._x << " "<< iop._obj._y << " "<< iop._obj._z << endl;
			cout << "gridlocation: " << gridlocation._x << " " << gridlocation._y << " " << gridlocation._z << endl;
		}
		else
		{
			(*uimages)[gridlocation._z].setVal(gridlocation._y, gridlocation._x, iop._img._u);
			(*vimages)[gridlocation._z].setVal(gridlocation._y, gridlocation._x, iop._img._v);
		}
	}
	return;
}

