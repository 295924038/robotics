/*
 * abslinemodel.h
 *
 *  Created on: Dec 5, 2017
 *      Author: zhian
 */

#ifndef _ABSLINEMODEL_H_
#define _ABSLINEMODEL_H_

#include <opencv2/opencv.hpp>
#include <vector>

namespace walgo {
class AbsLineModel
{
public:
	AbsLineModel(const cv::Mat& image) { _image = image; }
	virtual ~AbsLineModel() {}
	virtual void build(AbsLineModel& lm2, float eps, float eps2) = 0;
	virtual void build(float eps) = 0;
	virtual float model(float ) const = 0;
	virtual void getRange(int& xmin, int& xmax,
				  int& ymin, int& ymax);
	virtual bool belongsTo(const cv::Point& p, float eps) = 0;
	virtual void setXLow(float xlow) { _xlow = xlow; }
	virtual void setXHigh(float xhigh) { _xhigh = xhigh; }
	virtual void setYLow(float ylow) { _ylow = ylow; }
	virtual void setYHigh(float yhigh)  { _yhigh = yhigh; }
	virtual void findMaxGap(std::vector<cv::Point>& pts);
	virtual void intersect(const AbsLineModel& lm2, cv::Point& pt) = 0;
	virtual bool pSlope() const = 0;
    virtual bool belongsToReal(const cv::Point& p, float eps) = 0;
    virtual float getK() const = 0;
    virtual float getC() const = 0;
    virtual void setEpsFactor(float epsFactor) = 0;
    virtual void getAllPoints(std::vector<cv::Point>& pts) const;
protected:
	std::vector<cv::Point> _points;
	cv::Mat _image;
	float _xlow;
	float _xhigh;
	float _ylow;
	float _yhigh;
};
}


#endif /* ABSLINEMODEL_H_ */
