#ifndef _POINT_H_
#define _POINT_H_
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <math.h>


namespace walgo
{

template <class T>
class point
{
public:
	point() { }
	virtual ~point() {}
	virtual T dist(point* p) = 0;
	virtual void scale(T scale) = 0;
};

template <class T>
class point2d : public point<T>
{
public:
	point2d(T u, T v)
	{
		_u = u;
		_v = v;
	}
	point2d() {}
	point2d(point2d& pt) { _u = pt._u; _v = pt._v; }
	virtual ~point2d() {}
	virtual T dist(point<T>* p);
	void scale(T scale) { _u = _u*scale; _v = _v*scale; }
	T _u;
	T _v;
};

template <class T>
class point3d  : public point<T>
{
public:
	point3d(T x,T y, T z)
	{
		_x = x;
		_y = y;
		_z = z;
	}
	point3d() {}
	virtual ~point3d() {}
	virtual T dist(point<T>* p);

	T dist(const point3d<T>& p)
	{
		T dx = _x-p._x;
		T dy = _y-p._y;
		T dz = _z-p._z;
		return sqrt(dx*dx+dy*dy+dz*dz);
	}
	void scale(T scale) { _x = _x*scale; _y = _y*scale; _z = _z*scale; }
	point3d operator+(const point3d& p) const
	{
		return point3d(_x+p._x, _y+p._y, _z+p._z);
	}
	point3d operator-(const point3d& p) const
	{
		return point3d(_x-p._x, _y-p._y, _z-p._z);
	}
	point3d times(T s) const
	{
		return point3d(_x*s, _y*s, _z*s);
	}
	T inner(const point3d& p) const
	{
		return p._x*_x+p._y*_y+p._z*_z;
	}
	point3d cross(const point3d& p) const
	{
		return point3d(_y*p._z-_z*p._y, _z*p._x-_x*p._z, _x*p._y-_y*p._x);
	}
	T norm()
	{
		return sqrt(_x*_x+_y*_y+_z*_z);
	}
	void normalize()
	{
		T n = T(1.0)/norm();
		_x = _x*n;
		_y = _y*n;
		_z = _z*n;
	}
	T _x;
	T _y;
	T _z;



};

template <class T>
std::ostream& operator <<(std::ostream& os, const point3d<T>& p)
{
	os << p._x << " "<< p._y << " " << p._z << std::endl;
	return os;
}

typedef point3d<float> point3df;
typedef point3d<double> point3dd;

// image object pair
template <class T>
class iopair
{
public:
	iopair(int id, T u, T v,
		 T x, T y, T z )
		: _id(id), _obj(x, y, z),
		  _img(u, v) {}
	void print() { std::cout << _img._u << " " << _img._v << " "
				<< _obj._x << " "<< _obj._y << " " << _obj._z; }
	int _id;
	point3d<T> _obj;
	point2d<T> _img;
};

template<class T>
class pairmap
{
public:
	void addPair(std::shared_ptr<iopair<T>> p)
	{
		_map[p->_id] = p;
	}

	void findCurve(point2d<T>& img, std::vector<int>& pairs, float distol);
	void findz(float z, std::vector<int>& pairs, float tol);
	std::map<int, std::shared_ptr<iopair<T>> > _map;
 };


/***********************************************************
 * Template implementation below
 ***********************************************************/
template <class T>
T point2d<T>::dist(point<T>* p)
{
	point2d<T>* p2d = static_cast<point2d<T>* >(p);
	T u1 = _u-p2d->_u;
	T v1 = _v-p2d->_v;
	return sqrt(u1*u1+v1*v1);
}

template <class T>
T point3d<T>::dist(point<T>* p)
{
	point3d<T>* p3d = static_cast<point3d<T>* >(p);
	T x1 = _x-p3d->_x;
	T y1 = _y-p3d->_y;
	T z1 = _z-p3d->_z;

	return sqrt(x1*x1+y1*y1+z1*z1);
}

template <class T>
void pairmap<T>::findCurve(point2d<T>& img, std::vector<int>& pairs, float distol)
{
	for ( auto& it : _map)
	{
		iopair<T>* ip = it.second;
		if (img.dist(&(ip->_img)) < distol)
		{
			std::cout << "found: ";
			it.second->print();
			std::cout << std::endl;
		}
	}
}

template <class T>
void pairmap<T>::findz(float z, std::vector<int>& pairs, float tol)
{
	for ( auto& it : _map)
	{
		std::shared_ptr<iopair<T>> ip = it.second;
		if ( fabs((ip->_obj._z)-z) < tol)
		{
			pairs.push_back(ip->_id);
		}
	}
	return;
}

}
#endif
