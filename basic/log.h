/*
 * log.h
 *
 *  Created on: Aug 25, 2016
 *      Author: zhian
 */

#ifndef LOG_H_
#define LOG_H_
#include <iostream>

namespace walgo
{
enum LogLevel
{
	LOG_NOLOG,
	LOG_INFO,
	LOG_DEBUG
};

class log
{
public:


private:
	LogLevel _level;
};

}

#endif /* LOG_H_ */
