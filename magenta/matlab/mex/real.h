
#ifndef _magenta_matlab_mex_real_
#define _magenta_matlab_mex_real_

#include "magenta/matlab/mex/general.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Real
{
	double _n;

public:
	Real(double n) : _n(n) {}
	Real(double *n) : _n(*n) {}
	Real(const mxArray *array)
	{
		_n = mxGetScalar(array);
	}

//	operator double&() { return _n; }
	operator const double() const { return _n; }

	Real &operator=(double n)
	{
		_n = n;
		return *this;
	}

	operator mxArray*() const
	{
#ifdef MAT6SIM4
		return mxCreateScalarDouble(_n);
#else
		mxArray *p = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(p) = _n;
		return p;
#endif
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_real_
