/*
 * rpcmodel.h
 *
 *  Created on: Mar 27, 2016
 *      Author: zhian
 */

#ifndef RPCMODEL_H_
#define RPCMODEL_H_

#include "model.h"
#include "point.h"
#include "array2d.h"
#include "grid.h"

namespace walgo
{
double myfunc(unsigned n, const double* x, double* grad, void* funcData);

class rpcmodel : public model
{
public:
	enum BASIS
	{
		C=0,
		X,
		Y,
		Z,
		XZ,
		YZ,
		XY,
		XX,
		YY,
		ZZ,
		NUM_UPPER,
		UX = NUM_UPPER,
		UY,
		UZ,
		UXZ,
		UYZ,
		UXY,
		UXX,
		UYY,
		UZZ,
		NUM_BASIS
	};

	rpcmodel(double range) :
		_range(range),
		_doGradient(false)
	{}

	virtual ~rpcmodel() {}

	void calibrate();

	virtual void imgToObj(float u, float v, float z, float& x, float& y);
	virtual void objToImg(float x, float y, float z, float& u, float& v);

	inline void setData(pairmap<float>& pm, grid& gr)
	{
		_cdata = &pm;
		_grid = &gr;
	}

	inline void setZ(double z) { _z = z; }
	inline double getZ() { return _z;}
	inline void setTarget(double u, double v)
	{
		_u = u; _v = v;
	}
	inline void getTarget(double& u, double& v)
	{
		u = _u; v = _v;
	}

	inline void getGradient(double& dxu, double& dxv, double& dyu, double& dyv)
	{
		dxu = _dxu;
		dxv = _dxv;
		dyu = _dyu;
		dyv = _dyv;
	}
	double getMinZ() { return _minz; }
	double getMaxZ() { return _maxz; }

	double getRange() { return _range; }
	void setRange(double range) { _range = range; }

protected:

	void buildAB(pairmap<float>* pm);
	void solveu();
	void solvev();
	void findBestGuess(float u, float v, float z, float& x, float& y);
private:
	array2d<double> _amatrixu;
	array2d<double> _amatrixv;
	std::vector<double> _bvectoru;
	std::vector<double> _bvectorv;
	std::vector<double> _urpc;
	std::vector<double> _vrpc;
	pairmap<float>* _cdata;
	grid* _grid;
	double _minx, _maxx;
	double _miny, _maxy;
	double _minz, _maxz;
	double  _dx,_dy,_dz;
	double _z;
	double _u, _v;
	double _range;
	double _doGradient;
	double _dxu, _dxv, _dyu, _dyv;
	inline double scalex(double x) { return (x-_minx)*_dx; }
	inline double scaley(double y) { return (y-_miny)*_dy; }
	inline double scalez(double z) {return (z-_minz)*_dz; }

};

}

#endif /* RPCMODEL_H_ */
