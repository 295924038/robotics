#include <opencv2/opencv.hpp>
#include <string>
#include "seamdetector.h"
#include "linefeature.h"
#include "detectors.h"
#include "utils.h"

using namespace cv;
using namespace std;
using namespace walgo;

int main()
{
	map<string, int> config;
        if (!readConfig("config", config))
                cout <<"No trackconfig file " << endl;
	
	string fname = "/home/zhian/git-repos/robotics/image/image_0531/20170531165904.625.jpg";
	Mat image = imread(fname, 0);
	
	cout << "Size of " << fname << "  " << image.size() << endl;
	SEAM_DETECTOR_TYPE sdt = LINEBREAK;
	SeamDetector* seamdet = SeamDetector::getSeamDetector(sdt);
	LASER_DETECTOR_TYPE t = (LASER_DETECTOR_TYPE) config["LASER_DETECTOR_TYPE"];
    detector* det = detector::getDetector(t);
    LineFeature* linefeature = LineFeature::getLineFeature(SKELETON);
	Mat maskRaw;
	vector<Vec4i> lines;
	det->detect(image, maskRaw, config);
    linefeature->extract(image, maskRaw, lines, config);
    vector<Point> seam;
    seamdet->detectSeam(image, lines, seam, config);
	for ( int i = 0; i < seam.size(); i++)
        cout << "Detected Seam at: " << seam[i].x << " " << seam[i].y << endl;
    Mat cimage;
    //cvtColor(maskRaw, cimage, CV_GRAY2BGR);
    cvtColor(image, cimage, CV_GRAY2BGR);
    for( size_t i = 0; i < lines.size(); i++ )
   	{
      	Vec4i l = lines[i];
        line( cimage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 1, CV_AA);
    }
   
   	  for ( int i = 0; i < seam.size(); i++)
         circle(cimage, seam[i], 10, Scalar(255,255,255));
      
      namedWindow("Image", WINDOW_AUTOSIZE );
      imshow("Image", cimage);
    	waitKey(0);
	
	
	
	
}
