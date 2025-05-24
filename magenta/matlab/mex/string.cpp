
#include "string.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

ConstString::ConstString(const mxArray *array) : _array(array)
{
	int n = 0;
	if (array)
		n = mxGetM(array) * mxGetN(array) * sizeof(mxChar);
	if (!n)
	{
		_string.assign("");
		return;
	}
	
	char *p = new char[n];
	if (!mxGetString(array, p, n))
	{
		_string.assign(p);
		delete[] p;
	}
	else
	{
		delete[] p;
		throw "cannot convert string";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

String::String(const char *s)
{
	mx_string = mxCreateString(s);
}

//---------------------------------------------------------------------------------------------

String::String(const string &s)
{
	mx_string = mxCreateString(s.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta
