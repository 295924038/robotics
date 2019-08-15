/*
 * linefeature.h
 *
 *  Created on: Jul 12, 2016
 *      Author: zhian
 */

#ifndef LINEFEATURE_H_
#define LINEFEATURE_H_

#include <opencv2/opencv.hpp>

namespace walgo
{
enum LINE_FEATURE_TYPE {
	HOUGH,
	SKELETON,
	NUM_LINE_FEATURES
};

class LineFeature
{
public:
	static LineFeature* getLineFeature(LINE_FEATURE_TYPE type);
	virtual ~LineFeature() {}
	virtual bool extract(const cv::Mat& image,
				         const cv::Mat& mask,
						 std::vector<cv::Vec4i>& lines,
						 std::map<std::string, int>& params)= 0;
	static std::vector<LineFeature*> _obj;
};

class HoughLineFeature : public  LineFeature
{
public:
	virtual bool extract(const cv::Mat& image,
			const cv::Mat& mask,
			std::vector<cv::Vec4i>& lines,
			std::map<std::string, int>& params);
};

class SkeletonLineFeature : public LineFeature
{
public:
	virtual bool extract(const cv::Mat& image,
			const cv::Mat& mask,
			std::vector<cv::Vec4i>& lines,
			std::map<std::string, int>& params);
};
}
#endif /* LINEFEATURE_H_ */
