/*
 * grid.cc
 *
 *  Created on: Mar 7, 2016
 *      Author: zhian
 */

#include "grid.h"
#include <stdio.h>
#include <string.h>
using namespace walgo;
using namespace std;
bool grid::saveToFile(string fname)
{
	FILE* fp = fopen(fname.c_str(), "wb");
	if ( fp == NULL) {
		std::cout << "unable to open file " << fname << " for write" << std::endl;
		return false;
	}
	size_t osize = sizeof(grid);
	char* buf = new char[osize];
	memcpy(buf, (void*) this, osize);
	fwrite(buf, sizeof(char), osize, fp);
	fclose(fp);
	delete [] buf;
	return true;
}

bool grid::readFromFile(string fname)
{
	FILE* fp = fopen(fname.c_str(), "rb");
	if ( fp == NULL) {
		std::cout << "unable to open file " << fname << " for read" << std::endl;
		return false;
	}
	size_t osize = sizeof(grid);
	char* buf = new char[osize];
    size_t result = fread(buf, sizeof(char), osize, fp);
	if ( result != osize) {
		std::cout << "error reading grid file: " << fname << endl;
		fclose(fp);
		return false;
	}
	memcpy(this, buf, osize);
	fclose(fp);
	delete [] buf;
	return true;
}
