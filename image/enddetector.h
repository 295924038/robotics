#ifndef _ENDDETECTOR_H_
#define _ENDDETECTOR_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include "lasermodel3.h"
#include "point.h"

namespace walgo
{
class EndDetector
{
public:
	enum END_LOCATION {
		LEFT,
		RIGHT,
		MIDDLE
	};
	virtual bool detect(const cv::Mat& image,
						cv::Point2f& endPoint,
						END_LOCATION loc,
					    std::map<std::string, int>& params);

	virtual bool detect(const cv::Mat& image,
							lasermodel3& laserModel,
							point3d<float>& endPoint,
							END_LOCATION loc,
						    std::map<std::string, int>& params);
};
}

#endif
