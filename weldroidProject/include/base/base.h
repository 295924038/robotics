#ifndef _BASE_H
#define _BASE_H
#include <string>

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
