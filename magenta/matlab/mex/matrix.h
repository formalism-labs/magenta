
#ifndef _magenta_matlab_mex_matrix_
#define _magenta_matlab_mex_matrix_

#include "magenta/matlab/mex/general.h"
#include "magenta/matlab/mex/vector.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

class CRealMatrix : virtual public CArray
{
public:
	virtual const double *elements() const = 0;

public:
	double operator()(int i, int j) const;

	typedef double ElementType;
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ConstRealMatrix : public ConstRealVector, public CRealMatrix
{
public:
	const double *elements() const { return ConstRealVector::elements(); }

public:
	ConstRealMatrix(const mxArray *array = 0) : ConstRealVector(array) {}
	
	ConstRealMatrix &operator=(const mxArray *array) { assign(array); return *this; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class RealMatrix : public RealVector, public CRealMatrix
{
public:
	double *elements() { return RealVector::elements(); }

protected:
	void copy(const double *v);

public:
	RealMatrix(int rows, int cols, const double *v = 0);
	RealMatrix(const mxArray *array = 0);

	RealMatrix &operator=(const mxArray *array) { assign(array); return *this; }
	void assign(const mxArray *array);

	double &operator()(int i, int j);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_matrix_
