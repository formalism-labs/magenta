
#include <memory.h>

#include "matrix.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////
// CRealMatrix
///////////////////////////////////////////////////////////////////////////////////////////////

double CRealMatrix::operator()(int i, int j) const
{
	int k = i + j * rows();
	if (k >= size())
		throw "index out of range";
	return elements()[k];
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ConstRealMatrix
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// RealMatrix
///////////////////////////////////////////////////////////////////////////////////////////////

RealMatrix::RealMatrix(int rows, int cols, const double *v)
{
	_array = mxCreateDoubleMatrix(rows, cols, mxREAL);
	_elements = mxGetPr(_array);
	if (v)
		copy(v);
}

//---------------------------------------------------------------------------------------------

RealMatrix::RealMatrix(const mxArray *array)
{
	assign(array);
}

//---------------------------------------------------------------------------------------------

void RealMatrix::assign(const mxArray *array)
{
	if (!array)
	{
		_array = 0;
		_elements = 0;
		return;
	}

	int rows = mxGetM(array);
	int cols = mxGetN(array);
	_array = mxCreateDoubleMatrix(rows, cols, mxREAL);
	_elements = mxGetPr(_array);
	memcpy(_elements, mxGetPr(array), size() * mxGetElementSize(_array));
}

//---------------------------------------------------------------------------------------------

void RealMatrix::copy(const double *v)
{
	double *e = elements();
	int r = rows(), c = cols();
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			e[j * r + i] = v[i * c + j];
}

///////////////////////////////////////////////////////////////////////////////////////////////

double &RealMatrix::operator()(int i, int j)
{
	int k = i + j * rows();
	if (k >= size())
		throw "index out of range";
	return _elements[k];
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta
