
#ifndef _magenta_matlab_mex_vector_
#define _magenta_matlab_mex_vector_

#include "magenta/matlab/mex/general.h"

#include "magenta/matlab/mex/array.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

class CRealVector : virtual public CArray
{
public:
	virtual const double *elements() const = 0;
	
public:
	double operator[](int i) const { return elements()[i]; }

	typedef double ElementType;
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ConstRealVector : public ConstArray, public CRealVector
{
protected:
	const double *_elements;
public:
	const double *elements() const { return _elements; }
	
public:
	ConstRealVector(const mxArray *array = 0);

	ConstRealVector &operator=(const mxArray *array) { assign(array); return *this; }
	void assign(const mxArray *array);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class RealVector : public Array, public CRealVector
{
protected:
	double *_elements;
public:
	const double *elements() const { return _elements; }
	double *elements() { return _elements; }

public:
	RealVector(int n, const double *v = 0);
	RealVector(const mxArray *array = 0);

	RealVector &operator=(const mxArray *array) { assign(array); return *this; }
	void assign(const mxArray *array);

	double &operator[](int i) { return _elements[i]; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_vector_
