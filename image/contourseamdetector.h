#ifndef _CONTOURSEAMDETECTOR_H_
#define _CONTOURSEAMDETECTOR_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <map>

namespace walgo {
class ContourSeamDetector
{
public:
	virtual bool detectSeam(const cv::Mat image,
			std::vector<std::vector<cv::Point> >& contours,
			std::vector<cv::Point>& point,
			std::map<std::string, int>& params);
};
}

#endif
