#include "base/base.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

bool equald( double x, double y )
{
    return (x >= (y - precision)) && (x <= (y + precision)) ;
}

bool lessd( double x, double y )
{
   return !equald(x, y) && (x - y < 0 + precision) ;
}

bool greaterd( double x, double y )
{
   return !equald(x, y) && (x - y > 0 + precision) ;
}

bool lessequald( double x, double y )
{
   return x - y <= 0 + precision ;
}

bool greaterequald( double x, double y )
{
   return x - y >= 0 + precision ;
}

std::string getTime()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}

std::string getCurrentTime()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y_%m_%d_%H_%M_%S",localtime(&timep) );
    return tmp;
}

std::string getDate()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y_%m_%d",localtime(&timep) );
    return tmp;
}

float stringToNum(const std::string& str)
{
    std::istringstream iss(str);
    float num;
    iss>>num;
    return num;
}
