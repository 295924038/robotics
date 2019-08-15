#ifndef _UTILS_H_
#define _UTILS_H_
#include "point.h"
#include "image3d.h"

namespace walgo
{
class grid;


bool readData(const char* fname, pairmap<float>& pm);

bool readDataDc(const char* fname, pairmap<float>& pm1, pairmap<float>& pm2);

bool readDataLaser(const char* fname, pairmap<float>& pm);

void createImages(pairmap<float>& pm, image3d<float>*& uimage, image3d<float>*& vimage,
			       grid& gr);
}
#endif
