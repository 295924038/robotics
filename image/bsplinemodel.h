/*
 * linemodel.h
 *
 *  Created on: Jul 25, 2017
 *      Author: zhian
 */

#ifndef BSLINEMODEL_H_
#define BSLINEMODEL_H_
#include <opencv2/opencv.hpp>
#include <vector>
#include <Eigen/Dense>
#include "abslinemodel.h"

namespace walgo {
class BsplineModel   :  public AbsLineModel
{
public:
	BsplineModel(const cv::Mat& mat, int ndiv, int numIter);
	BsplineModel(const cv::Mat& mat, const std::vector<cv::Vec4i>& lines, int numIter, int ndiv=4);
	bool belongsTo(const cv::Point& p, float eps);
	void build(const cv::Point&  p);
	void build(float eps);
	void build(AbsLineModel& lm2, float eps, float eps2);
	void clearPts()
	{
		_points.clear();
		_A.setZero();
		_b.setZero();
	}

	float model(float x) const;
	void finalize();
	void setUseOrigModel() { _useOrigModel = true; }

	void intersect(const AbsLineModel& lm2, cv::Point& pt);
	bool belongsToReal(const cv::Point& p, float eps) { return true; }
    bool pSlope() const { return _k0 > 0; }
    float getK() const { return _k0; }
    float getC() const { return _c0; }
    void setEpsFactor(float epsFactor) { _epsFactor = epsFactor; }
private:
    int _nknot;
    int _unit;
    double _k;
	int _numIter;

	Eigen::MatrixXd _A;
	Eigen::VectorXd _b;
	std::vector<double> _a;
	bool _useOrigModel;
	float _epsFactor;

	float _k0;
	float _c0;

};

}

#endif /* BSPLINEMODEL_H_ */
