#ifndef _IMAGE_H_
#define _IMAGE_H_
#include "array2d.h"

namespace  walgo
{
template <class T>
class image : public array2d<T>
{
public:
	image(int sx, int sy)
	: array2d<T>(sx,sy)
	{}
	virtual ~image() {}


protected:
};
}
#endif
