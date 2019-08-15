/*
 * fitpoly.h
 *
 *  Created on: Nov 6, 2017
 *      Author: zhian
 */

#ifndef FITPOLY_H_
#define FITPOLY_H_

#include <vector>

namespace walgo
{
template <typename T>
class poly
{
public:
	poly(const std::vector<T>& c);
	poly() {}
	void init(const std::vector<T>& c);
	void init(const std::vector<T>& c, T scale, T offset);
	T val(T x) const;
	const std::vector<T>& getCoeffs() const { return _c; }
	void getScaleOffset(T& scale, T& offset) { scale = _scale; offset = _offset; }
private:
	std::vector<T> _c;
	T _scale;
	T _offset;
	int _n;
};
}

template <typename T>
walgo::poly<T>::poly(const std::vector<T>& c)
{
	init(c);
}

template <typename T>
void walgo::poly<T>::init(const std::vector<T>& c)
{
	_c = c;
	_n = c.size() -1;
	_scale = 1.0;
	_offset = 0.0;
}

template <typename T>
void walgo::poly<T>::init(const std::vector<T>& c, T scale, T offset)
{
	_c = c;
	_n = c.size() -1;
	_scale = scale;
	_offset = offset;
}

template <typename T>
T walgo::poly<T>::val(T x) const
{
	T b = _c[0];
	T realx = (x-_offset)*_scale;
	for ( int i = 1; i < _n+1; i++)
	{
		b = _c[i]+b*realx;
	}
	return b;
}

namespace walgo
{
template <typename T>
class fitpoly
{
public:
        fitpoly(const std::vector<T>& xdata,
			const std::vector<T>& ydata,
			int order);

	const walgo::poly<T>& getPoly() const { return _poly; }
private:
	int _n;
	walgo::poly<T> _poly;
};

}

#endif /* FITPOLY_H_ */
