
#ifndef _magenta_math_numeric_vector_
#define _magenta_math_numeric_vector_

#include <math.h>

#include "magenta/math/classes.h"
#include "magenta/math/vector.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T = double>
class NumericVector : public Vector<T>
{
	friend NumericMatrix<T>;
	typedef NumericVector<T> This;

protected:
	NumericVector(T *w, int dim) : Vector<T>(w, dim) {}

public:
	NumericVector() {}
	explicit NumericVector(int d) : Vector<T>(d) {}
	NumericVector(T a, T b) : Vector<T>(a, b) {}
	NumericVector(T a, T b, T c) : Vector<T>(a, b, c) {}
	NumericVector(const Vector<T> &w, int prec) : Vector<T>(w, prec) {}
	NumericVector(const Vector<T> &w) : Vector<T>(w) {}
	virtual ~NumericVector() {}

	T sqr_length() const;

	T length() const;
	This norm() const { return *this/length(); }
	T angle(const This &w) const; 
	This rotate90() const;
	This rotate(double a) const;

	This &operator+=(const This &);
	This &operator-=(const This &);
  
	This operator+(const This &v1) const;
	This operator-(const This &v1) const;

	T operator*(const This& v1) const;
	This operator*(T r) const;

	This operator-() const;
	This operator/(T) const;
  
	friend This operator*(T f, const This &v) { return v *f; }

//	friend int compare_by_angle(const This &v1, const This &v2);
};

//---------------------------------------------------------------------------------------------

template <class T>
class RefNumericVector : public NumericVector<T>
{
public:
	RefNumericVector(T *w, int dim) : NumericVector<T>(w, dim) {}
	~RefNumericVector() { v = 0; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#include "magenta/math/numeric-vector.hxx"

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_math_numeric_vector_
