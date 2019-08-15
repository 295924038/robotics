/*
 * utils.cc
 *
 *  Created on: Apr 10, 2016
 *      Author: zhian
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include "utils.h"
#include <map>
#include <string>

using namespace std;
using namespace walgo;

bool walgo::readConfig(const std::string& fname, std::map<string, int>& config)
{
	ifstream ifs(fname);
	if ( !ifs )
	{
		cout << "can't read configuration file " << fname << endl;
		return false;
	}
	int i = 0;
	string name;
	int val;
	string line;
	while ( std::getline(ifs, line) )
	{
               //		cout << line << endl;
		if (line[0] == '#') continue;

		istringstream sl(line);
		sl >> name >> val;

		line.clear();
		config[name] = val;
	      //	cout << "config entry for "<< name << "is " << val << endl;
		i++;
	}
	cout << "Read " << i << " configuration entries " << endl;
	return true;
}

bool walgo::myReadConfig(const std::string& fname, const std::string& motorTypeSeamNo, std::map<std::string,int>& config)
{
    ifstream ifs(fname);
    if ( !ifs )
    {
        cout << "can't read configuration file ====" << fname << endl;
        return false;
    }

    int value;
    bool record = false;
    string key;
    string line;
    string comTitle= "[common]";
    string seamTitle = "[" + motorTypeSeamNo + "]";

    cout<<comTitle.c_str()<<" "<<seamTitle.c_str()<<endl;

    while( std::getline(ifs, line) )
    {
        if (line[0] == '#') continue ;

        if(comTitle.compare(line.substr(0,8)) == 0 || seamTitle.compare(line.substr(0,10)) == 0)
        {
            record = true;
            continue;
        }
        if(record)
        {
            if(line[0] == '[')
            {
                record = false;
            }
            else
            {
                istringstream sl(line);
                sl >> key >> value;
                config[key] = value;
                line.clear();
            }
        }
    }

    return true;
}


bool walgo::getConfigEntry(const map<string, int>& config, string& key, int& val)
{
	map<string, int>::const_iterator it = config.find(key);
	if (it == config.end())
	{
//		cout << "Can't find parameter for key: " << key << endl;
		return false;
	}
	else
	{
		val = (*it).second;
		return true;
	}
	return true;
}

bool walgo::getConfigEntry(const map<string, int>& config, const char* key, int& val)
{
	string s(key);
	return getConfigEntry(config, s, val);
}
