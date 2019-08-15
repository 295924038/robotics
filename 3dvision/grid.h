#ifndef _GRID_H_
#define _GRID_H_

/**
 * this class convert between points and image pixels
 */

#include <string>
#include "point.h"

namespace walgo
{
class grid
{
public:
	grid(float xmin, float xmax, float xgridpitch,
		 float ymin, float ymax, float ygridpitch,
		 float zmin, float zmax, float zgridpitch) :
			 _xmin(xmin),_xmax(xmax), _ymin(ymin), _ymax(ymax), _zmin(zmin), _zmax(zmax),
			 _xgridpitch(xgridpitch), _ygridpitch(ygridpitch), _zgridpitch(zgridpitch)
	{
		_xgridsize = floor((xmax-xmin)/xgridpitch+1.5);
		_ygridsize = floor((ymax-ymin)/ygridpitch+1.5);
		_zgridsize = floor((zmax-zmin)/zgridpitch+1.5);
	}

	grid() {}

	void  grid2pt(int ix, int iy, int iz, point3d<float>& pt)
	{
		pt._x = ix*_xgridpitch + _xmin;
		pt._y = iy*_ygridpitch + _ymin;
		pt._z = iz*_zgridpitch + _zmin;
	}

	void grid2pt(float ix, float iy, point2d<float>& pt)
	{
		pt._u = ix*_xgridpitch+_xmin;
		pt._v = iy*_ygridpitch + _ymin;
	}
	void  pt2grid(float x, float y, float z, point3d<int>& grid)
	{
		grid._x = floor((x-_xmin)/_xgridpitch+0.5);
		grid._y = floor((y-_ymin)/_ygridpitch+0.5);
		grid._z = floor((z-_zmin)/_zgridpitch+0.5);
	}

	int getSizeX()
	{
		return _xgridsize;
	}

	int getSizeY()
	{
		return _ygridsize;
	}

	int getSizeZ()
	{
		return _zgridsize;
	}

	bool saveToFile(std::string fname );
	bool readFromFile(std::string fname );

	float getXGridPitch() { return _xgridpitch; }
	float getYGridPitch() { return _ygridpitch; }
	float getZGridPitch() { return _zgridpitch; }

	float getZMin() { return _zmin; }
	float getXMin() { return _xmin; }
	float getYMin() { return _ymin; }

private:
	float _xmin;
	float _xmax;
	float _ymin;
	float _ymax;
	float _zmin;
	float _zmax;
	float _xgridpitch;
	float _ygridpitch;
	float _zgridpitch;
	int _xgridsize;
	int _ygridsize;
	int _zgridsize;
};
}
#endif
