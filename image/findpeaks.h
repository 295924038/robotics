#ifndef FINDPEAKS_H
#define FINDPEAKS_H

#include<vector>
#include<map>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void findPeaks(const Mat Matdata, int minpeakdistance, int minpeakheight, std::map <int, double> &mapPeaks_max, vector<pair<int, double>>&Vector_map_Peaks_max);

void findPeaks(double* num,int count,vector<int>& peaks);

#endif // FINDPEAKS_H
