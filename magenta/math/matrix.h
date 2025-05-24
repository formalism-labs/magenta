
#ifndef _magenta_math_matrix_
#define _magenta_math_matrix_

#include <iostream>
#include <math.h>

#include "magenta/math/classes.h"
#include "magenta/math/vector.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class CConstMatrix
{
	typedef CConstMatrix<T> This;

protected:
//	typedef Vector<T> Vector;

	int d1;
	int d2;

	void check_dimensions(const This&) const; 
    
public:
	int dim1() const {  return d1; }
	int dim2() const {  return d2; }

	virtual T elem(int i, int j) const = 0;

	Vector<T> row(int i) const;
	Vector<T> operator[](int i) const { return row(i); }

	Vector<T> col(int i) const;

	operator Vector<T>() const; 

	T operator()(int,int) const;

	void transposed() const;

	bool operator==(const This &x) const;
	bool operator!=(const This &x)  const { return !(*this == x); }

	void read(std::istream &in);
	void write(std::ostream &out) const;
	void print(std::ostream &out = std::cout) const;

	friend std::ostream &operator<<(std::ostream &out, const CConstMatrix &M);
};

//---------------------------------------------------------------------------------------------

template <class T>
std::ostream &operator<<(std::ostream &out, const CConstMatrix<T> &M)
{
	M.write(out);
	return out;
}

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class CMatrix : public CConstMatrix<T>
{
	typedef CMatrix<T> This;
	typedef CConstMatrix<T> Super;

//	void flip_rows(int,int);
    
public:
	virtual T &elem(int i, int j) = 0;
	virtual T elem(int i, int j) const = 0;
	virtual void setElem(int i, int j, T x) = 0;

	T &operator()(int i, int j);
	T operator()(int i,int j) const { return Super::operator()(i, j); }

	friend std::istream &operator>>(std::istream &in, CMatrix &M);

	void transpose() const;
};

//---------------------------------------------------------------------------------------------

template <class T>
std::istream &operator>>(std::istream &in, CMatrix<T> &M)
{
	M.read(in);
	return in;
}

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Matrix : virtual public CMatrix<T>
{
	typedef Matrix<T> This;
//	typedef Vector<T> Vector;
	typedef RefVector<T> RefVector;

protected:
	T *v;

	void ctor(int d1, int d2, const T *w = 0);

public:
	Matrix(int n=0, int m=0);
	Matrix(int n, int m, T D);
	Matrix(int n, int m, const T *D);
	Matrix(const Matrix &m);
	Matrix(const Vector<T> &v);
	virtual ~Matrix();

	This& operator=(const This&);

	T elem(int i, int j) const { return v[i * d2 + j]; }
	T &elem(int i, int j) { return v[i * d2 + j]; }
	void setElem(int i, int j, T x) { elem(i, j) = x; }

	RefVector row(int i);
	RefVector &operator[](int i) { return row(i); }

	This trans() const { This M(d1, d2); trans(M); return M; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#include "magenta/math/matrix.hxx"

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_math_matrix_
