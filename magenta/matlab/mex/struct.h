
#ifndef _magenta_matlab_mex_struct_
#define _magenta_matlab_mex_struct_

#include "magenta/matlab/mex/general.h"
#include "magenta/matlab/mex/array.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"

namespace magenta
{

namespace MEX
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

namespace _Struct_
{

class Type
{
	typedef Vector<string> FieldNames;
	FieldNames field_names;
	
public:
	Type(const ConstStruct &s);
	
	class FieldsIterator
	{
		Type &_type;
		int i;
	};
};

} // namespace _Struct_

///////////////////////////////////////////////////////////////////////////////////////////////

class CStruct
{
public:
	typedef _Struct_::Type Type;

};

///////////////////////////////////////////////////////////////////////////////////////////////

class ConstStruct : public CStruct
{
protected:
	const mxArray *_s;
	int _index;

public:
	ConstStruct(const mxArray *s, int index = 0);

	const mxArray *operator[](const char name[]) const;
	const mxArray *operator[](const string &name) const { return operator[](name.c_str()); }

	Type type() const;

	operator const mxArray *() const { return _s; }

	CLASS_EXCEPTION_DEF("magenta::ConstSturct");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Struct : public CStruct
{
protected:
	mxArray *_s;

public:
	Struct(const ConstStruct &s);
	Struct(const Struct &s);
	
	struct Field
	{
		const Struct &_s;
		int _index;

	public:
		Field(const Struct &s, const string &name);
		Field &operator=(const mxArray *v);
		operator const mxArray*();
	};
	
	Field operator[](const string &name) { return Field(*this, name); }
	
	CLASS_EXCEPTION_DEF("magenta::Sturct");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ConstStructArray : public ConstArray
{
public:
	ConstStructArray(const mxArray *array = 0) : ConstArray(array) {}

	ConstStructArray &operator=(const mxArray *array) { assign(array); return *this; }

	ConstStruct operator[](int i) const { return ConstStruct(_array, i); }

	typedef ConstStruct ElementType;
};

///////////////////////////////////////////////////////////////////////////////////////////////

class StructArray : public Array
{
public:
	StructArray(int n);
	StructArray(const mxArray *array = 0);

	StructArray &operator=(const mxArray *array) { assign(array); return *this; }

	Struct operator[](int i); // { return Struct(*this, i); }
	ConstStruct operator[](int i) const { return ConstStruct(*this, i); }

	typedef Struct ElementType;
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_struct_
