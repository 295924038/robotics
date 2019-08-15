/*
 * lineanalysis.h
 *
 *  Created on: Jun 24, 2016
 *      Author: zhian
 */

#ifndef SEAMDETECTOR_H_
#define SEAMDETECTOR_H_

#include <opencv2/opencv.hpp>
#include "lineanalysis.h"
#include "pathdetector.h"

namespace walgo
{
enum SEAM_DETECTOR_TYPE
{
	INTERSECTION,
	LINEBREAK,
    SKEL,
	NUM_SEAM_DETECTORS
};


class SeamDetector
{
public:
	static SeamDetector* getSeamDetector(SEAM_DETECTOR_TYPE t);
	virtual ~SeamDetector() {}
	virtual bool detectSeam(const cv::Mat image,
				const std::vector<cv::Vec4i>& lines,
				std::vector<cv::Point>& points,
				std::map<std::string, int>& params) = 0;
	static std::vector<SeamDetector*> _obj;
};

class IntSeamDetector : public SeamDetector
{
	virtual bool detectSeam(const cv::Mat image,
				const std::vector<cv::Vec4i>& lines,
				std::vector<cv::Point>& point,
				std::map<std::string, int>& params);
};
/**
 * 用linemodel，hough变换的焊缝检测类
 */
class LBSeamDetector : public SeamDetector
{
	virtual bool detectSeam(const cv::Mat image,             // 二元化图形包括激光信息
			const std::vector<cv::Vec4i>& lines,             // 返回的霍夫变换线段信息
			std::vector<cv::Point>& point,                   // 焊缝点， 0： 左端点， 1： 右端点， 2： 交点
			std::map<std::string, int>& params);
};


class SKSeamDetector : public SeamDetector
{
	virtual bool detectSeam(const cv::Mat image,
			const std::vector<cv::Vec4i>& lines,
			std::vector<cv::Point>& point,
			std::map<std::string, int>& params);
};

/**
 * 寻找焊缝。
 *
 */
void findSeam(cv::Mat& image,                                   // 原图信息
		std::vector<cv::Vec4i>& lines,                              // 用LB检测时返回霍夫线段信息， 用topo检测时返回COG信息
		std::vector<cv::Point>& seam,                               // 用LB检测时返回两端点和交点信息， 用topo检测时返回所有不连续点信息
		std::map<std::string, int>& config);

bool getSeamByMatVec(vector<cv::Mat> mats,int XYZ,std::string & seamInfo);
}

#endif /* SEAMDETECTOR_H_ */
