#ifndef _UPSAMPLE_H_
#define _UPSAMPLE_H_

#include "image.h"
#include "image3d.h"

namespace walgo
{

class upsample
{
public:
	upsample(int ratio);
	upsample() {}
	virtual ~upsample();
	/**
	 * for 2d image interpolation
	 */
	virtual void apply(image<float>& in, image<float>& out);

	/**
	 * for full 3d image interpolation
	 */
	virtual void apply(image3d<float>& in, image3d<float>& out);
private:
	float **_filter;
	int  _ratio;
	float* _iratio;
};
}
#endif
