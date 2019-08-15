/*
 * linemodel.h
 *
 *  Created on: Jul 25, 2017
 *      Author: zhian
 */

#ifndef _WALGO_LINEMODEL_H_
#define _WALGO_LINEMODEL_H_
#include <opencv2/opencv.hpp>
#include <vector>
#include "abslinemodel.h"

namespace walgo
{
class LineModel : public AbsLineModel
{
public:
	LineModel(const cv::Mat& mat, const std::vector<cv::Vec4i>& lines, int numIter);
	LineModel(const cv::Mat& mat, const cv::Vec4d& line, int numIter);
	LineModel(const cv::Mat& mat, int numIter);
	bool belongsTo(const cv::Point& p, float eps);
	void build(const cv::Point&  p);
	void build(float eps);
	void build(AbsLineModel& lm2, float eps, float eps2);
	void clearPts()
	{
		_points.clear();
		_sumx2 = 0.0;
		_sumx = 0.0;
		_sumxy = 0.0;
		_sumy = 0.0;
	}

	float model(float x) const { return (_k*x+_c); }
	void finalize();
	void setUseOrigModel() { _useOrigModel = true; }

	void intersect(const AbsLineModel& lm2, cv::Point& pt);
	bool pSlope() const { return _k > 0; }
    bool belongsToReal(const cv::Point& p, float eps);
    void setEpsFactor(float epsFactor) { _epsFactor = epsFactor; }
    float getK()  const { return _k; }
    float getC() const { return _c; }
private:
	float _sumx2;
	float _sumx;
	float _ns;
	float _sumxy;
	float _sumy;
	float _k;
	float _c;
	float _k1;
	int _numIter;

	float _epsFactor;
	bool _useOrigModel;

};

}

#endif /* LINEMODEL_H_ */
