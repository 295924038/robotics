/*
 * histimage.h
 *
 *  Created on: Jan 25, 2019
 *      Author: Hu XP
 */

#ifndef HISTIMAGE_H_
#define HISTIMAGE_H_
#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include <memory>
#include "utils.h"

namespace walgo {

bool hist(const cv::Mat& input,  std::vector<double>& output, std::map<std::string, int>& config,int ct= 0,bool doMediumBlur= false);
}
#endif /* HISTIMAGE_H_ */
