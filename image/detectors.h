/*
 * detectors.h
 *
 *  Created on: Apr 10, 2016
 *      Author: zhian
 */

#ifndef DETECTORS_H_
#define DETECTORS_H_
#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include <memory>

namespace walgo {
enum LASER_DETECTOR_TYPE
{
	THRESH = 0,
	PEAK   = 1,
	LINE   = 2,
	PEAK1D = 3,
	NUM_DETECTORS
};

class detector
{
public:
	static detector* getDetector(LASER_DETECTOR_TYPE t);
	virtual ~detector() {}
	virtual bool detect(const cv::Mat& input, cv::Mat& output, std::map<std::string, int>& params) =  0;
	static std::vector<detector*> _obj;
};

// simple threshold function
class threshDetector : public detector
{
public:
	virtual bool detect(const cv::Mat& input, cv::Mat& output, std::map<std::string, int>& params);
};

// detect local peaks
class peakDetector : public detector
{
public:
	virtual bool detect(const cv::Mat& input, cv::Mat& output, std::map<std::string, int>& params);
};

// matched filter
class lineDetector : public detector
{
public:
	virtual bool detect(const cv::Mat& input, cv::Mat& output, std::map<std::string, int>& params);
protected:
	bool generateKernel(cv::Mat& kernel, int angle, int thickness);
};

class Peak1dDetector : public detector
{
public:
	virtual bool detect(const cv::Mat& input, cv::Mat& output, std::map<std::string, int>& params);
        private:
	virtual bool detectAVX(const cv::Mat& input, cv::Mat& output, std::map<std::string, int>& params);
};
bool detect0(cv::Mat& input,  std::vector<float>& line, int& count, std::map<std::string, int>& config);
}
#endif /* DETECTORS_H_ */
