#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "image.h"
#include "point.h"
#include "image3d.h"
#include "grid.h"

namespace walgo
{
void findNearest(image<float>& u,
				 image<float>& v,
				 point2d<float>& pt1,
				 point2d<float>& pt2,
				 point2d<float>& gridpt1,
				 point2d<float>& gridpt2,
				 float& dist);

void fineInterp(image<float>& u, image<float>& v, int xmin, int ymin,
		        point2d<float>& pt, point2d<float>& gridpt);

int linearSearch(image3d<float>* upuimage, image3d<float>* upvimage,
				  point2d<float>& pt1, point2d<float>& pt2,
				  point2d<float>& objpt1, point2d<float>& objpt2,
				  grid& gr, int ratio, float knownDist,
				  float& accuz);
int bisectionSearch(image3d<float>* upuimage, image3d<float>* upvimage,
		            point2d<float>& pt1, point2d<float>& pt2,
		            point2d<float>& objpt1, point2d<float>& objpt2,
				    grid& gr, int ratio, float knownDist,
				    float& accuz);
}
#endif
