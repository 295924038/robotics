#ifndef _ARRAY2D_H_
#define _ARRAY2D_H_
#include <string.h>

namespace walgo
{
template <class T>
class array2d
{
public:
	array2d(int sx, int sy);
	array2d();
	virtual ~array2d();

	/**
	 * perform deep copy for copy constructor
	 */
	array2d(const array2d& im);
	/**
	 * overload asignment
	 */
	array2d& operator=(const array2d& im);

	/**
	 * return ith row
	 */
	T* operator [](int i) { return _ptrs[i]; }

	/**
	 * return contiguous data
	 */
	T* getData() { return _data; }

	/**
	 * get number of columns
	 */
	virtual int getSizeX() const { return _sizex; }
	/**
	 *  get number of rows
	 */
	virtual int getSizeY() const { return _sizey; }

	virtual void setVal(int y, int x, T val) { _ptrs[y][x] = val; }

	virtual void resize(int sizex, int sizey);

	virtual void setZero();
protected:
	void init(int sx, int sy);
	void destroy();
	void copy(const array2d& im);
	T* _data;
	T** _ptrs;
	int _size;
	int _sizex;
	int _sizey;
};

template<class T>
void array2d<T>::destroy()
{
	if (_ptrs )
	{
		delete [] _ptrs;
		_ptrs = 0;
	}
	if ( _data )
	{
		delete [] _data;
		_data = 0;
	}
}

template<class T>
void array2d<T>::init(int sx,int sy)
{
	_sizex = sx;
	_sizey = sy;
	_size = sx*sy;
	_data = new T[_size];
	_ptrs = new T*[_sizey];
	for ( int i = 0; i < _sizey; i++)
		_ptrs[i] = &_data[_sizex*i];
}

template <class T>
array2d<T>::array2d(int sx, int sy)
{
	init(sx,sy);
}

template <class T>
array2d<T>::array2d() :
	_data(NULL),
	_ptrs(NULL),
	_size(0),
	_sizex(0),
	_sizey(0)
{
}

template<class T>
void array2d<T>::resize(int sx, int sy)
{
	destroy();
	init(sx,sy);
}

template <class T>
array2d<T>::array2d(const array2d& im)
{
	copy(im);
}

template <class T>
void array2d<T>::copy(const array2d& im)
{
	_sizex = im._sizex;
	_sizey = im._sizey;
	_size = im._size;
	if ( _data )  destroy();
	_data = new T[_size];
	memcpy(_data, im._data, _size*sizeof(T));
	_ptrs = new T*[_sizey];
	for ( int i = 0; i < _sizey; i++)
		_ptrs[i] = &_data[_sizex*i];

}

template <class T>
array2d<T>::~array2d()
{
	destroy();
}

template <class T>
void array2d<T>::setZero()
{
	if ( _data )
	{
		memset(_data, 0, sizeof(T)*_size);
	}
}


template <class T>
array2d<T>& array2d<T>::operator=(const array2d<T>& im)
{
	if ( this == &im )
		return (*this);
	copy(im);
	return (*this);
}
}
#endif
