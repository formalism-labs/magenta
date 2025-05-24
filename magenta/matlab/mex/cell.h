
#ifndef _magenta_matlab_mex_cell_
#define _magenta_matlab_mex_cell_

#include "magenta/matlab/mex/general.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

class ConstCellArray : public ConstArray
{
public:
	ConstCellArray(const mxArray *array = 0) : ConstArray(array) {}

	ConstCellArray &operator=(const mxArray *array) { assign(array); return *this; }

	const mxArray *operator[](int i) const { return mxGetCell(_array, i); }

	typedef mxArray *ElementType;
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CellArray : public Array
{
protected:
	struct Cell
	{
		mxArray *_v;
		int _i;

		Cell(mxArray *array, int i) : _v(array), _i(i) {}
		Cell &operator=(mxArray *x)
		{
			mxSetCell(_v, _i, x);
			return *this;
		}

		operator mxArray*() { return mxGetCell(_v, _i); }
		operator const mxArray*() const { return mxGetCell(_v, _i); }
	};

public:
	CellArray(int n)  : Array(mxCreateCellMatrix(n, 1)) {}
	CellArray(const mxArray *array = 0) { assign(array); }
	CellArray(const CellArray &array) { assign(array); }

	CellArray &operator=(const mxArray *array) { assign(array); return *this; }

	Cell operator[](int i) { return Cell(*this, i); }

	typedef Cell ElementType;
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_cell_
