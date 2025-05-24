
#ifndef _magenta_matlab_mex_string_
#define _magenta_matlab_mex_string_

#include "magenta/matlab/mex/general.h"

#include "magenta/exceptions/defs.h"
#include <string>

namespace magenta
{

namespace MEX
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class ConstString
{
	const mxArray *_array;
	string _string;

public:
	ConstString(const mxArray *array);

	const string &operator->() const { return _string; }
	const string &operator*() const { return _string; }

	operator string() const { return _string; }

	CLASS_EXCEPTION_DEF("magenta::ConstString");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class String
{
	mxArray *mx_string;

public:
	String() : mx_string(0) {}
	String(const char *s);
	String(const string &s);

	operator mxArray*() { return mx_string; }
	operator string() const { return ConstString(mx_string); }

	CLASS_EXCEPTION_DEF("magenta::String");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_string_
