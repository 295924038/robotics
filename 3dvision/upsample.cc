#include "upsample.h"
#include "filtergen.h"
#include <iostream>

using namespace walgo;

walgo::upsample::upsample(int ratio )
{
	_ratio = ratio;
	_filter = new float*[ratio];
	_iratio = new float[ratio];
	for ( int i = 0; i < ratio; i++)
		_filter[i] = new float[4];
	float grid = 1.0/(float)ratio;
	float shift;
	float f = 1.0/(float)ratio;
	for ( int i = 0; i < ratio; i++)
	{
		shift = i*grid;
		cubicBSplineFilter(_filter[i], shift);
		_iratio[i] = i*f;
	}
}

walgo::upsample::~upsample()
{
	for ( int i = 0; i < _ratio; i++)
		delete []  _filter[i];
	delete [] _filter;
	delete [] _iratio;
}

void walgo::upsample::apply(image<float>& in, image<float>& out)
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

	for ( int y = 0; y < in.getSizeY()-1; y++)
	{
		// handling top and bottom borders
		if (y < ystart || y >= yend )
		{
			for ( int k = 0; k < _ratio; k++)
			{
				for ( int x = 0; x < in.getSizeX(); x++)
				{
					temp[k][x] = (1.0-_iratio[k])*in[y][x]+_iratio[k]*in[y+1][x];
				}
			}
		}
		else
		{
			for ( int k = 0; k < _ratio; k++)
			{
				for ( int x = 0; x < in.getSizeX(); x++)
				{
					temp[k][x] = 0;
					for ( int m = 0; m < filterSize; m++)
						temp[k][x] += _filter[k][m]*in[y-offset+m][x];

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
						out[ymk][xmn] = (1.0-_iratio[n])*temp[k][x]+_iratio[n]*temp[k][x+1];
					else
					{
						tem = 0;
						for ( int m = 0; m < filterSize; m++)
							tem += _filter[n][m]*temp[k][x-offset+m];
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

void walgo::upsample::apply(image3d<float>& in, image3d<float>& out)
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
						out[zmk][y][x] = temp[z][y][x]*(1.0-_iratio[k]) + temp[z+1][y][x]*_iratio[k];
					else
					{
						float tem = 0;
						for ( int m = 0; m <filterSize; m++)
							tem += _filter[k][m]*temp[z-offset+m][y][x];
						out[zmk][y][x] = tem;
					}
				}
			}
		}
	}
	return;

}
