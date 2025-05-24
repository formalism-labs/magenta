
#ifndef _magenta_math_numeric_matrix_
#define _magenta_math_numeric_matrix_

#ifdef _WIN32
#pragma warning (disable : 4250)
#endif

#include <math.h>

#include <hash_map>

#include "magenta/math/classes.h"
#include "magenta/math/matrix.h"
#include "magenta/math/numeric-vector.h"

#if defined(__linux__)
typedef long long int int64_t;
#elif defined(__DECCXX)
typedef long int64_t;
#elif defined(WIN32)
typedef __int64 int64_t;
#endif

#if defined(__DECCXX) || (_MSC_VER >= 1300)
#define PLATFORM_64_BIT
#endif

#ifndef PLATFORM_64_BIT
namespace std
{

template<> struct hash<int64_t> {
  size_t operator()(int64_t __x) const { return size_t(__x) ; }
};

}
#endif

namespace magenta
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T = double>
class CNumericMatrix : virtual public CMatrix<T>
{
public:
	typedef NumericVector<T> Vector;

protected:
	typedef CNumericMatrix<T> This;

	T **triang(const This&, int&) const;
    
public:
	Vector row(int i) const;
	Vector col(int i) const;

	T det() const;

	void add(const This& M1, This &result) const;
	void subtract(const This& M1, This &result) const;
	void negate(This &result) const;
	void multiply(const This& M1, This &result) const;
	void multiply(T x, This &result) const;

	This &operator-=(const This&);
	This &operator+=(const This&);

protected:
	void inv(This &result)   const;
	void solve(const This&M, This &result) const;

public:
	Vector solve(const Vector& b) const 
	{ 
		Matrix<T> result(d1);
		solve(This(b), result);
		return Vector(result); 
	}
};

#define DEFINE_CommonMatrixInterface \
	This operator+(const This& M1) const { This M(d1, d2); add(M1, M); return M;	} \
	This operator-(const This& M1) const { This M(d1, d2); subtract(M1, M); return M;	}\
	This operator-() const { This M(d1, d2); CNumericMatrix<T>::negate(M); return M;	} \
	This operator*(const This& M1) const { This M(d1, d2); multiply(M1, M); return M;	} \
	This operator*(T x) const { This M(d1, d2); multiply(x, M); return M;	} \
\
	Vector operator*(const Vector& vec) const { return Vector(*this * This(vec)); } \
\
	This inv() const \
	{ \
			This result(d1, d2); \
			inv(*this, result); \
			return result; \
	} \
\
	This solve(const This&M) const \
	{ \
		This result(d1, M.d2); \
		solve(*M, result); \
		return result; \
	}

//---------------------------------------------------------------------------------------------

template <class T = double>
class IdentityMatrix : public CConstMatrix<T>
{
public:
	IdentityMatrix(int n) { d1 = d2 = n; }

	virtual T elem(int i, int j) const { return i == j ? 1 : 0; }
};

//---------------------------------------------------------------------------------------------

template <class T = double>
class NumericMatrix : public Matrix<T>, public CNumericMatrix<T>
{
	typedef NumericMatrix<T> This;

protected:
	typedef NumericVector<T> Vector;

public:
	typedef RefNumericVector<T> RefVector;

public:
	NumericMatrix(int n = 0, int m = 0) : Matrix<T>(n, m) {}
	NumericMatrix(int n, int m, const T *D) : Matrix<T>(n, m, D) {}
	NumericMatrix(int n, int m, T D) : Matrix<T>(n, m, D) {}
	NumericMatrix(const CConstMatrix<T> &m) : Matrix<T>(m) {}
	NumericMatrix(const Vector &v) : Matrix<T>(v) {}
	virtual ~NumericMatrix() {}

	T elem(int i, int j) const { return Matrix<T>::elem(i, j); }
	T &elem(int i, int j) { return Matrix<T>::elem(i, j); }
	void setElem(int i, int j, T x) { Matrix<T>::setElem(i, j, x); }

	This &operator=(const This&);

	RefVector row(int i) const;
	RefVector operator[](int i) const { return row(i); }

	void set(T value);
	void setIdentity();

	DEFINE_CommonMatrixInterface;
};

//---------------------------------------------------------------------------------------------

template <class T = double>
class SparseNumericMatrix : public CNumericMatrix<T>
{
	typedef SparseNumericMatrix<T> This;

	typedef std::hash_map<int64_t, T>  Table;
	Table _table;

protected:
	typedef NumericVector<T> Vector;

public:
	SparseNumericMatrix(int n = 0, int m = 0);
	SparseNumericMatrix(int n, int m, int hashSize);
	SparseNumericMatrix(int n, int m, T* D);
	SparseNumericMatrix(const CConstMatrix<T> &m);
	SparseNumericMatrix(const NumericVector<T> &v);
	virtual ~SparseNumericMatrix() {}

	This &operator=(const CConstMatrix<T>&M);

	T elem(int i, int j) const;
	T &elem(int i, int j);

	void setElem(int i, int j, T val)
	{
		if (val != 0)
			elem(i, j) = val;
	}

	DEFINE_CommonMatrixInterface;
};

///////////////////////////////////////////////////////////////////////////////////////////////

#undef DEFINE_CommonMatrixInterface

} // namespace magenta

#include "magenta/math/numeric-matrix.hxx"

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_math_numeric_matrix_
