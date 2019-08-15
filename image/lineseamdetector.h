#ifndef _LINESEAMDETECTOR_H_
#define _LINESEAMDETECTOR_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <map>

namespace walgo
{
class LineSeamDetector
{
public:
	virtual bool detectSeam(const cv::Mat& image,
					std::vector<cv::Point>& point,
					std::map<std::string, int>& params);
};
}

#endif
