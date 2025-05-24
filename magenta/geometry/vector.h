
#ifndef _magenta_geometry_vector_
#define _magenta_geometry_vector_

#include <math.h>
#include <memory.h>

namespace magenta
{

//using namespace std;

////////////////////////////////////////////////////////////////////////////////

template <int N = 3, class T = float>
class _V
{
protected:
	T v[N];

public:
	_V() {}

	_V(T c)
	{
		for (int i = 0; i < N; ++i)
			v[i] = c;
	}

	_V(T w[])
	{
		for (int i = 0; i < N; ++i)
			v[i] = w[i];
	}

	template <class T1>
	_V(const _V<N, T1> &w)
	{
		*this = w;
	}

	const _V &operator=(const _V &w)
	{
		for (int i = 0; i < N; ++i)
			v[i] = w[i];
		return w;
	}
	
	int dims() const { return N; }
	T &operator[](int i) { return v[i]; }
	T operator[](int i) const { return v[i]; }
	operator T*() { return v; }

	_V operator*(T t) const
	{ 
		_V v = *this;
		for (int i = 0; i < N; ++i)
			v[i] *= t;
		return v;
	}

	_V operator+(const _V &v) const
	{
		_V w = *this;
		for (int i = 0; i < N; ++i)
			w[i] += v[i];
		return w;
	}
};

//----------------------------------------------------------------------------------------------

template <class T = float>
class _V2 : public _V<2, T>
{
public:
	_V2() {}
	_V2(T x, T y) { v[0] = x; v[1] = y; }
	_V2(const _V<2, T> &v) { (_V<2, T> &)*this = v; }

	T x() const { return v[0]; }
	T y() const { return v[1]; }
	
	T &x() { return v[0]; }
	T &y() { return v[1]; }
};
typedef _V2<> V2;

//----------------------------------------------------------------------------------------------

template <class T = float>
class _V3 : public _V<3, T>
{
public:
	_V3() {}
	_V3(T x, T y, T z) { v[0] = x; v[1] = y; v[2] = z; }
	_V3(const _V<3, T> &v) { (_V<3, T> &)*this = v; }

	T x() const { return v[0]; }
	T y() const { return v[1]; }
	T z() const { return v[2]; }

	T &x() { return v[0]; }
	T &y() { return v[1]; }
	T &z() { return v[2]; }
};
typedef _V3<> V3;

//----------------------------------------------------------------------------------------------

template <class T = float>
class _V4 : public _V<4, T>
{
public:
	_V4() {}
	_V4(T x, T y, T z, T w) { v[0] = x; v[1] = y; v[2] = z; v[3] = w; }
	_V4(const _V<4, T> &v) { (_V<4, T> &)*this = v; }

	T x() const { return v[0]; }
	T y() const { return v[1]; }
	T z() const { return v[2]; }
	T w() const { return v[3]; }

	T &x() { return v[0]; }
	T &y() { return v[1]; }
	T &z() { return v[2]; }
	T &w() { return v[3]; }
};
typedef _V4<> V4;

////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_geometry_vector_
