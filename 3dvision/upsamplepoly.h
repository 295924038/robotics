#ifndef _UPSAMPLEPOLY_H_
#define _UPSAMPLEPOLY_H_

#include "image.h"
#include "image3d.h"

namespace walgo
{

class upsamplepoly
{
public:
	upsamplepoly(int ratio);
	virtual ~upsamplepoly();
	/**
	 * for 2d image interpolation
	 */
	void apply(walgo::image<float>& in, walgo::image<float>& out);

	/**
	 * for full 3d image interpolation
	 */
	void apply(walgo::image3d<float>& in, walgo::image3d<float>& out);
private:
	inline const float calcPoly(float* poly, float x)
	{
		return poly[0]+x*(poly[1]+x*(poly[2]+x*poly[3]));
	}
	int  _ratio;
	float* _iratio;
};
}

#endif
