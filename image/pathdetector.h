#ifndef _PATHDETECTOR_H_
#define _PATHDETECTOR_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include "d2seamdetector.h"
#include "abslinemodel.h"

namespace walgo
{

class PathDetector
{
public:
	enum D2PointType
	{
		D2POINT_1,
		D2POINT_2,
		D2POINT_MID
	};
	enum CullMethod
	{
		USE_HOUGH,
		USE_STD,
	};

	PathDetector(int lmax, D2PointType ptType, std::string configFile=std::string("config.4"));

	PathDetector(int lmax, D2PointType ptType, const std::map<std::string, int>& config);

	virtual bool addImage(const cv::Mat& image, int l, int isub=0);

        bool confirm(int m);

	virtual bool detect(std::vector<cv::Point2f>& endPoints,        // 每个子缝端点坐标
						std::vector<int>& endSteps,                 // 每个子缝端点时间
						std::vector<cv::Point>& uLinePts,
						std::vector<cv::Point>& vLinePts,
						std::vector<std::vector<cv::Point>>& uSegments,
						std::vector<std::vector<cv::Point>>& vSegments
						);
        virtual bool detect(std::vector<cv::Point2f>& endPoints,
                                         vector<int>& endSteps,
                                         std::vector<cv::Point>& uLinePts,
                                         std::vector<cv::Point>& vLinePts,
                                         std::vector<std::vector<cv::Point>>& uSegments,
                                         std::vector<std::vector<cv::Point>>& vSegments,
                                         std::vector<float>& jjmm);

	virtual void setShowImageIndices(int  smin, int smax)  { _smin = smin; _smax = smax; }
	virtual void setShowImage(int showImage = 1) { _showImage = 1; }
        vector<int> subParts;
        int weldPoits;
        int pics;
        const std::string pd_num= "v2019.03.06pd_release!";
        std::string pd;
        cv::Mat maskRaw;

protected:
	virtual void init();

	virtual bool detect(cv::Mat& uimg,
                            std::vector<float>& uvec,
                            walgo::AbsLineModel*& model,
                            std::vector<cv::Point>& linePoints,
                            std::vector<std::vector<cv::Point>>& lineSegments,
                            std::vector<cv::Point2f>& endPoints);

	virtual bool build2ndUVImages(float u0, float v0, float u1, float v1, int lmin, int lmax);

        virtual bool eraseSpargePts(vector<int>& L,AbsLineModel* umodel,AbsLineModel* vmodel);
        virtual bool eraseSpargePts(vector<int>& L);
        virtual bool eraseSpargePts(int eraseSparge,vector<int>& L,AbsLineModel* umodel,AbsLineModel* vmodel);
        void findWeldPoints(const int i,AbsLineModel* umodel,AbsLineModel* vmodel,vector<int>& endSteps);

private:
	std::map<std::string, int> _config;
	int _lmax;
	cv::Rect2d _roi;
	cv::Mat _d2mu, _d2mv, _d2mw;
	cv::Mat _d2mu2nd, _d2mv2nd, _d2mw2nd;
	std::vector<float> _uvec;
	std::vector<float> _vvec;
	std::vector<float> _wvec;
	D2PointType _d2PointType;
	std::vector<std::vector<D2> > _d2vs;
	std::vector<std::vector<float> > _cogs;
	int _nmin, _nmax;
	int _smin, _smax;  // showimage lower/upper indices
	int _showImage;
	int _cullMethod;
	std::vector<int> _subMap;
	std::vector<int> _subLMin;
	std::vector<int> _subLMax;
	int _subMin, _subMax;
	float _minSignal, _maxSignal;
	int _saveImage;
        float _minS;
        float _maxS;
        int _minmaxS;
        vector<int> _blackpics;
        int _weldbool;
        fstream _log;

};

}
#endif
