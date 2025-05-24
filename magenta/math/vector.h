
#ifndef _magenta_math_vector_
#define _magenta_math_vector_

#include <math.h>

#include "magenta/math/classes.h"
#include "magenta/types/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Vector
{
protected:
	typedef Vector<T> This;

	typedef magenta::Vector<T> STLVector;

	friend Matrix<T>;
	friend NumericMatrix<T>;

	T *v;
	int d;

	void check_dimensions(const This &v) const;

protected:
	Vector(T *w, int dim) : v(w), d(dim) {}

public:
	Vector();
	explicit Vector(int d);
	Vector(T a, T b);
	Vector(T a, T b, T c);
	Vector(const This &w, int prec);
	Vector(const This &w);
	Vector(const STLVector &w);
	virtual ~Vector();

	Vector &operator=(const This &v);

	int dim() const { return d; }

	bool operator==(const This& w) const;
	bool operator!=(const This& w)  const { return !(*this == w); }

	T coord(int i) const;
	T &coord(int i);

	T &operator[](int i) { return coord(i); }
	T operator[](int i) const { return coord(i); }

	T x() const { return coord(0); }
	T y() const { return coord(1); }
	T z() const { return coord(2); }

	T &x() { return coord(0); }
	T &y() { return coord(1); }
	T &z() { return coord(2); }

	operator magenta::Vector<T>(d, v); }

//	void print(ostream& O);
//	void print() { print(cout); }

//	void read(istream& I);
//	void read() { read(cin); }
  
//	friend ostream& operator<<(ostream& O, const This& v);
//	friend istream& operator>>(istream& I, This& v);
};

//---------------------------------------------------------------------------------------------

template <class T>
class RefVector : public Vector<T>
{
public:
	RefVector(T *w, int dim) : Vector<T>(w, dim) {}
	~RefVector() { v = 0; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#include "magenta/math/vector.hxx"

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_math_vector_
