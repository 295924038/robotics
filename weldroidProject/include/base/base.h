#ifndef _BASE_H
#define _BASE_H
#include <string>
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include <chrono>

#include <boost/any.hpp>

#include <opencv2/opencv.hpp>

/*********************************************Begin TypeDef**************************************************/
std::chrono::milliseconds operator"" _ms( unsigned long long t ) ;

// 毫秒
typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::ratio<1, 1000>>> Millsecond ;

// 毫秒之差
typedef std::chrono::duration<double, std::ratio<1, 1000>> MillDiff ;

constexpr double precision = 0.000001 ;

bool equald( double x, double y ) ;

bool lessd( double x, double y ) ;

bool greaterd( double x, double y ) ;

bool lessequald( double x, double y ) ;

bool greaterequald( double x, double y ) ;


std::string getTime();

std::string getCurrentTime();

std::string getDate();

float stringToNum(const std::string& str);

#endif // _BASE_H
