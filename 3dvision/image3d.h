#ifndef _IMAGE3D_H_
#define _IMAGE3D_H_

#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>
#include "image.h"


/**
 * this class describes an array of images on a regular grid.
 * like a "deck of cards"
 */
namespace walgo
{
template <class T>
class image3d
{
public:
	image3d(int sizex, int sizey, int sizez);

	image3d() {_sizex = 0; _sizey = 0; }

	virtual ~image3d();

	/**
	 * return ith image
	 */
	image<T>& operator [](int i) { return *(_images[i]); }

	void addImage(image<T>* im) { _images.push_back(im); }

	/**
	 * get number of columns
	 */
	int getSizeX() const { return _sizex; }
	/**
	 *  get number of rows
	 */
	int getSizeY() const { return _sizey; }

	/**
	 * get number of images
	 */
	int getSizeZ() const { return _images.size(); }

	void getLineZ(int x, int y, std::vector<T>& line) {}

	bool saveToFile(std::string file);

	bool readFromFile(std::string file);
private:
		std::vector<image<T>* > _images;
		int _sizex;
		int _sizey;

};

template <class T>
image3d<T>::image3d(int sizex, int sizey, int sizez) :
	_sizex(sizex),
	_sizey(sizey)
{
	for ( int z = 0; z < sizez; z++)
	{
		image<T>* im = new image<T>(sizex, sizey);
		_images.push_back(im);
	}
	std::cout << "_images size: " << _images.size() << std::endl;
}

template <class T>
image3d<T>::~image3d()
{
	for ( int z = 0; z < _images.size(); z++)
	{
		delete _images[z];
	}
}

template <class T>
bool image3d<T>::saveToFile(std::string filename)
{
	FILE* fp = fopen(filename.c_str(), "wb");
	if ( fp == NULL) {
		std::cout << "unable to open file for write" << std::endl;
		return false;
	}
	fwrite(&_sizex, sizeof(int), 1, fp);
	fwrite(&_sizey, sizeof(int), 1, fp);
	int sizez = _images.size();
	fwrite(&sizez, sizeof(int), 1, fp);
	if ( sizez == 0)
	{
		fclose(fp);
		return true;
	}
	int size = (_images[0]->getSizeX())*(_images[0]->getSizeY());
	for ( int i = 0; i < sizez; i++)
	{
		fwrite(_images[i]->getData(), sizeof(T), size, fp);
	}
	fclose(fp);
	return true;
}

template <class T>
bool image3d<T>::readFromFile(std::string filename)
{
	FILE* fp = fopen(filename.c_str(), "rb");
	if ( fp == NULL) {
		std::cout << "unable to open file for read" << std::endl;
		return false;
	}
	size_t result;
	result = fread(&_sizex, sizeof(int), 1, fp);
	result = fread(&_sizey, sizeof(int), 1, fp);
	int sizez = _images.size();
	result = fread(&sizez, sizeof(int), 1, fp);
	if ( sizez == 0)
	{
		fclose(fp);
		return true;
	}
	int size = _sizex*_sizey;
	for ( int i = 0; i < sizez; i++)
	{
		image<T>* im = new image<T>(_sizex, _sizey);
		result = fread(im->getData(), sizeof(T), size, fp);
		_images.push_back(im);

	}
	fclose(fp);
	return true;
}
}
#endif
