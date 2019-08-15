/*
 * utils.h
 *
 *  Created on: Apr 10, 2016
 *      Author: zhian
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <map>
#include <string>

namespace walgo
{
bool readConfig(const std::string& fname, std::map<std::string, int>& config);

bool getConfigEntry(const std::map<std::string, int>& config, const char* key, int& val);

bool getConfigEntry(const std::map<std::string, int>& config, std::string& key, int& val);

bool myReadConfig(const std::string& fname, const std::string& motorTypeSeamNo, std::map<std::string,int>& config);

}
#endif /* UTILS_H_ */
