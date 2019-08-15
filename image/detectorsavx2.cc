/*
 * detectors.cc
 *
 *  Created on: Apr 10, 2016
 *      Author: zhian
 */
#include <immintrin.h>
#include "detectors.h"
#include "utils.h"
using namespace cv;
using namespace std;
using namespace walgo;

bool Peak1dDetector::detectAVX(const Mat& input, Mat& output, map<string, int>& config)
{
	int th = 15;
	getConfigEntry(config, "PEAK1D_THRESH", th);
	output=input.clone();
	output.setTo(0);
	vector<uchar> maxv(input.cols);
	std::fill(maxv.begin(), maxv.end(), 0);
	const unsigned c = input.cols;
	unsigned l = c & 31U;
	__m256i vin;
	__m256i vmax;
	__m256i v;
	__m256i vth = _mm256_set1_epi8((char) th);
	for ( int i = 0; i < input.rows; i++)
	{
		const unsigned char* irp = input.ptr<uchar>(i);
		size_t j = 0;
		for (j = 0; j < c; j = j+32)
		{
			//if (input.at<uchar>(i,j) > maxv[j])
			//	maxv[j] = input.at<uchar>(i,j);
			vin = _mm256_loadu_si256((__m256i*)&(irp[j]));
			vmax = _mm256_loadu_si256((__m256i*)&(maxv[j]));
			v = _mm256_max_epu8(vin, vmax);
			_mm256_storeu_si256((__m256i*)&(maxv[j]), v);
		}
		if ( l > 0 ) {
			for (j = j-32; j < c; j++  )
			{
				if (input.at<uchar>(i,j) > maxv[j])
					maxv[j] = input.at<uchar>(i,j);
			}
		}
	}
	__m256i v1;
	for ( int i = 0; i < input.rows; i++)
	{
		const unsigned char* irp = input.ptr<uchar>(i);
		unsigned char* orp = output.ptr<uchar>(i);
		size_t j = 0;
		for (int j = 0; j < c; j = j + 32)
		{
			//if (input.at<uchar>(i,j) >= maxv[j] && maxv[j] >= th)
			//	output.at<uchar>(i,j) = 255;
			vin = _mm256_loadu_si256((__m256i*)&(irp[j]));
			vmax = _mm256_loadu_si256((__m256i*)&(maxv[j]));
			v = _mm256_cmpeq_epi8(vin, vmax);
			v1 = _mm256_cmpeq_epi8(_mm256_max_epu8(vin, vth), vin);
			v = _mm256_and_si256(v,v1);
			_mm256_storeu_si256((__m256i*) &(orp[j]), v);
		}
		if ( l > 0 ) {
			for (j = j-32; j < c; j++  )
			{
				if (input.at<uchar>(i,j) > maxv[j])
					maxv[j] = input.at<uchar>(i,j);
			}
		}
	}
	return true;
}





