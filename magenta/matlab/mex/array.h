
#ifndef _magenta_matlab_mex_array_
#define _magenta_matlab_mex_array_

#include "magenta/matlab/mex/general.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Array
///////////////////////////////////////////////////////////////////////////////////////////////

class CArray
{
public:
	int size() const { return handle() ? mxGetNumberOfElements(handle()) : 0; }

public:
	virtual const mxArray *handle() const = 0;
	operator const mxArray*() const { return handle(); }

	virtual void assign(const mxArray *array) = 0;

//	const mxArray *operator[](int d);
//	const mxArray *operator()(int d0, int d1);

	int rows() const { return mxGetM(handle()); }
	int cols() const { return mxGetN(handle()); }
};

//---------------------------------------------------------------------------------------------

class ConstArray : virtual public CArray
{
protected:
	const mxArray *_array;
	const mxArray *handle() const { return _array; }

public:
	ConstArray(const mxArray *array = 0)  : _array(array) {}
	ConstArray &operator=(const mxArray *array) { assign(array); return *this; }
	void assign(const mxArray *array) { _array = array; }

	int rows() const { return mxGetM(handle()); }
	int cols() const { return mxGetN(handle()); }
};

//---------------------------------------------------------------------------------------------

class Array : virtual public CArray
{
protected:
	mxArray *_array;
	const mxArray *handle() const { return _array; }

protected:
	Array() : _array(0) {}
	
public:
	Array(const mxArray *array) { assign(array); }
	Array(const Array &array) { assign(array); }

	Array &operator=(const mxArray *array) { assign(array); return *this; }
	void assign(const mxArray *array) 
	{
		_array = array ? mxDuplicateArray(array) : 0;
	}

	int dim(int i) const;

	operator mxArray*() { return _array; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Persistent Storage
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Persistent : public T
{
	void ctor() 
	{
		if (_array)
			mexMakeArrayPersistent(_array); 
	}

public:
	Persistent(int n) : T(n) { ctor(); }
	Persistent(int n, const T::ElementType *v) : T(n, v) { ctor(); }
	Persistent(const mxArray *array = 0) : T(array) { ctor(); }
	Persistent(const T &array) : T(array) { ctor(); }
	Persistent(const Persistent<T> &x) : T(x) { ctor(); }

	virtual ~Persistent()
	{
		if (_array)
			mxFree(_array);
	}

	T &operator=(const mxArray *array) { assign(array); return *this; }
	void assign(const mxArray *array)
	{
		if (_array)
			mxFree(_array);
		T::assign(array);
		ctor();
	}
	
	T::ElementType operator[](int i) { return T::operator[](i); }
	T::ElementType operator()(int i, int j) { return T::operator()(i, j); }

	const T::ElementType operator[](int i) const { return T::operator[](i); }
	const T::ElementType operator()(int i, int j) const { return T::operator()(i, j); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_array_
