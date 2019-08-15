#include "upsamplepoly.h"
#include "filtergen.h"
#include <iostream>

using namespace walgo;


upsamplepoly::upsamplepoly(int ratio )
{
	_ratio = ratio;
	_iratio = new float[ratio];
	float f = 1.0/(float)_ratio;
	for ( int i = 0; i < ratio; i++)
		_iratio[i] = i*f;
}

upsamplepoly::~upsamplepoly()
{
	delete [] _iratio;
}

const float c12 = 1.0/2.0;
const float c13 = 1.0/3.0;
const float c16 = 1.0/6.0;
void upsamplepoly::apply(image<float>& in, image<float>& out)
{
	int ystart = 1;  // erosion
	int yend = in.getSizeY() -2;
	int xstart = 1;
	int xend = in.getSizeX() -2;
	// need to check in, out image size ratio here:

	//
	// only cubic bspline for now,  in the future
	// will support sinc interpolator
	//
	int filterSize = 4;
	int offset = filterSize/2-1;
	float** temp = new float*[_ratio];
	for ( int k = 0; k < _ratio; k++)
		temp[k] = new float[in.getSizeX()];
	float poly[4];
	for ( int y = 0; y < in.getSizeY()-1; y++)
	{
		// handling top and bottom borders
		if (y < ystart || y >= yend )
		{
			for ( int k = 0; k < _ratio; k++)
			{
				for ( int x = 0; x < in.getSizeX(); x++)
				{
					temp[k][x] = _iratio[k]*in[y][x]+(1.0-_iratio[k])*in[y+1][x];
				}
			}
		}
		else
		{
			for ( int k = 0; k < _ratio; k++)
			{
				for ( int x = 0; x < in.getSizeX(); x++)
				{
					//temp[k][x] = 0;
					//for ( int m = 0; m < filterSize; m++)
					//	temp[k][x] += _filter[k][m]*in[y-offset+m][x];
					poly[0] = in[y][x];
					poly[1] = -c13*in[y-1][x]-c12*in[y][x]+in[y+1][x]-c16*in[y+2][x];
					poly[2] = c12*in[y-1][x]-in[y][x]+c12*in[y+1][x];
					poly[3] = -c16*in[y-1][x]+c12*in[y][x]-c12*in[y+1][x]+c16*in[y+2][x];
					temp[k][x] = calcPoly(poly, _iratio[k]);
				}
			}
		}
		int ym = y*_ratio;
		float tem;
		for ( int k = 0; k < _ratio; k++)
		{
			int ymk = ym+k;
			for ( int x = 0; x < in.getSizeX()-1; x++)
			{
				int xm = x*_ratio;
				for ( int n = 0; n < _ratio; n++)
				{
					int xmn = xm + n;
					if ( x < xstart || x >= yend)
						out[ymk][xmn] = _iratio[n]*temp[k][x]+(1.0-_iratio[n])*temp[k][x+1];
					else
					{
						//tem = 0;
						//for ( int m = 0; m < filterSize; m++)
						//	tem += _filter[n][m]*temp[k][x-offset+m];
						poly[0] = temp[k][x];
						poly[1] = -c13*temp[k][x-1]-c12*temp[k][x]+temp[k][x+1]-c16*temp[k][x+2];
						poly[2] = c12*temp[k][x-1]-temp[k][x]+c12*temp[k][x+1];
						poly[3] = -c16*temp[k][x-1]+c12*temp[k][x]-c12*temp[k][x+1]+c16*temp[k][x+2];
						tem = calcPoly(poly, _iratio[n]);
						out[ymk][xmn] = tem;
					}
				}
			}

		}
	}
	for ( int k = 0; k < _ratio; k++)
		delete [] temp[k];
	delete [] temp;
	return;
}

void upsamplepoly::apply(image3d<float>& in, image3d<float>& out)
{
	// check input for valid image sizes

	image3d<float> temp(in.getSizeX()*_ratio, in.getSizeY()*_ratio, in.getSizeZ());
	for ( int z = 0; z < in.getSizeZ(); z++)
	{
		// first do in plane interpolation
		apply(in[z], temp[z]);
	}
	int zstart = 1;
	int zend = in.getSizeZ() -2;
	// next interpolate between planes
	int filterSize = 4;
	int offset = filterSize/2 -1;
	float poly[4];
	for ( int z = 0; z < in.getSizeZ()-1; z++)
	{
		int zm = z*_ratio;
		for ( int k = 0; k < _ratio; k++)
		{
			for ( int y = 0; y < out.getSizeY(); y++)
			{
				for ( int x = 0; x < out.getSizeX(); x++)
				{
					int zmk = zm+k;
					if ( z < zstart || z >= zend)
						out[zmk][y][x] = temp[z][y][x]*_iratio[k] + temp[z+1][y][x]*(1.0-_iratio[k]);
					else
					{
						//float tem = 0;
						//for ( int m = 0; m <filterSize; m++)
						//	tem += _filter[k][m]*temp[z-offset+m][y][x];
						poly[0] = temp[z][y][x];
						poly[1] = -c13*temp[z-1][y][x]-c12*temp[z][y][x]+temp[z+1][y][x]-c16*temp[z+2][y][x];
						poly[2] = c12*temp[z-1][y][x]-temp[z][y][x]+c12*temp[z+1][y][x];
						poly[3] = -c16*temp[z-1][y][x]+c12*temp[z][y][x]-c12*temp[z+1][y][x]+c16*temp[z+2][y][x];

						out[zmk][y][x] = calcPoly(poly, _iratio[k]);
					}
				}
			}
		}
	}
	return;

}
