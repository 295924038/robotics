/*
 * polymodel3d.h
 *
 *  Created on: Nov 4, 2018
 *      Author: zhian
 */

#ifndef SEPMODEL3D_H_
#define SEPMODEL3D_H_

#include "model3d.h"
#include "polymodel1d.h"
#include<cmath>

using namespace std;

namespace walgo {
class SepModel3D : virtual public Model3D
{
public:
	SepModel3D();
	virtual ~SepModel3D() {}
	virtual void setParams(const std::vector<double>& params);
        virtual void build();
        virtual void init();
	virtual point3d<double> model(double t) const;
        virtual double getRMS() const;
        virtual double getMinT(double t0, double & ti);
        virtual double sepMiss(double t1);
        virtual void reBuild(point3d<double> & p0,double t);
        virtual void setRealSE(int s,int e);
        virtual void printTvec(std::vector<int>& tvec);
    double next;
    double curT;
    vector<vector<int>> cutVec;
    vector<double> rmslimit;
private:
        int initCounts;  /// 原始点数
        int start_t;     /// real start t
        int end_t;     /// real end t
        vector<int> lastTvec;
	PolyModel1D  _modelX;
	PolyModel1D  _modelY;
	PolyModel1D  _modelZ;
};
}

#endif /* SEPMODEL3D_H_ */
