
#ifndef _magenta_matlab_mex_function_
#define _magenta_matlab_mex_function_

#pragma warning(disable : 4250)

#include "magenta/matlab/mex/general.h"

#include "magenta/types/defs.h"

namespace magenta
{

namespace MEX
{


///////////////////////////////////////////////////////////////////////////////////////////////

struct InArgs
{
	Vector<const mxArray*> v;

	InArgs() {}

	InArgs &operator,(mxArray *x) { v << x; return *this; }
};

//---------------------------------------------------------------------------------------------

struct OutArgs
{
	Vector<mxArray**> v;

	OutArgs() {}

	OutArgs &operator,(mxArray *&x) { v << &x; return *this; }
};

//---------------------------------------------------------------------------------------------

#define MEX_IN	MEX::InArgs(), 
#define MEX_NO_IN MEX::InArgs()
#define MEX_OUT MEX::OutArgs(), 
#define MEX_NO_OUT MEX::OutArgs()

EXCEPTION_DEF(EvalException, "magenta::FunctionCall");

//---------------------------------------------------------------------------------------------

mxArray *call(const char *cmd, const InArgs &in = InArgs());
void call(const char *cmd, const InArgs &in, const OutArgs &out);
void call(const char *cmd, const OutArgs &out, const InArgs &in);
void exec(const char *cmd);

void eval(const magenta::String &cmd);

//---------------------------------------------------------------------------------------------

magenta::String lasterr();

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_function_
