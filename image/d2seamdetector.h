/*
 * d2seamdetector.h
 *
 *  Created on: Apr 13, 2018
 *      Author: zhian
 */

#ifndef D2SEAMDETECTOR_H_
#define D2SEAMDETECTOR_H_
#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>
using namespace std;

namespace walgo
{

enum TOPO_TYPE
{
	LINE_END = 0,  // line end with a gap
	LINE_JUMP,     // line end followed immediately with a jump
	LINE_BEND,     // line continuous but derivative jumps
};

class TopoPoint
{
public:
	TopoPoint(TOPO_TYPE type, cv::Point2f& point, int gap, bool onLeft, float signal, bool polarity) :
		_type(type),
		_point(point),
		_gap(gap),
		_onLeft(onLeft),
		_signal(signal),
		_polarity(polarity)    // false is min signal,  true is max signal
{}
	TOPO_TYPE     _type;
	cv::Point2f   _point;
	int           _gap;
	bool          _onLeft;
	float		  _signal;
	bool          _polarity;
};

class D2
{
public:
	D2(int i, float d2) :
		_i(i),
		_d2(d2) {}
	int _i;
	float _d2;
};

struct {
		bool operator() (const D2& a, const D2& b) const
		{
			return a._d2 < b._d2;
		}
} d2less;

/**
 * 寻找焊缝。
 *
 */
void findD2Seam(const cv::Mat& image,                                            // 原图信息
                std::vector<float>& smoothCog,                                 // smoothed Cog
                std::vector<D2>& d2v,                                       // d2 vector
                std::vector<cv::Vec4i>& lines,                              // 用LB检测时返回霍夫线段信息， 用topo检测时返回COG信息
                std::vector<TopoPoint>& seam,                               // 用LB检测时返回两端点和交点信息， 用topo检测时返回所有不连续点信息
                std::map<std::string, int>& config,
                int p= 9999);

//bool findD2Seam(cv::Mat& image, int thresh, std::vector<TopoPoint>& seam, bool rl= true);

bool findD2Seam(cv::Mat& image,int thresh,std::vector<TopoPoint>& seam,int l= 150,int r= 150);
}

#endif /* D2SEAMDETECTOR_H_ */
