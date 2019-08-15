#ifndef _FITPLANE_H_
#define _FITPLANE_H_
#include "point.h"
#include <vector>

namespace walgo
{
double myfunc(unsigned n, const double* x, double* grad, void* funcData);

class fitplane
{
public:
	void fitPlane(const std::vector<point3dd>& points,
				  double& a, double& b, double& c);
	const std::vector<point3dd>& getPoints() const { return _points; }
private:
	std::vector<point3dd> _points;
};
}
#endif
