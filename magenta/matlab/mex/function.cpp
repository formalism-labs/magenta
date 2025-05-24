
#include "function.h"

#include "matlab-5/extern/mex.h"

#include "string.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

static void call(const char *cmd, int in_n, const mxArray *in_p[], int out_n, mxArray *out_p[])
{
	mexSetTrapFlag(1);
	int rc = mexCallMATLAB(out_n, out_p, in_n, (mxArray **) in_p, cmd);
	if (rc)
		THROW_EX(EvalException)("error calling function '%s': %s", cmd, lasterr().c_str());
}

//---------------------------------------------------------------------------------------------

mxArray *call(const char *cmd, const InArgs &in)
{
	mxArray *result;
	int in_n = in.v.size();
	const mxArray **in_p = in_n ? (const mxArray **) &in.v[0] : 0;
	call(cmd, in_n, in_p, 1, &result);
//	int rc = mexCallMATLAB(1, &result, in_n, in_p, cmd);
//	if (rc)
//		THROW_EX(EvalException)("error calling function %s: %s", cmd, lasterr().c_str());
	return result;
}

//---------------------------------------------------------------------------------------------

void call(const char *cmd, const InArgs &in, const OutArgs &out)
{
	int in_n = in.v.size();
	const mxArray **in_p = in_n ? (const mxArray **) &in.v[0] : 0;
	int out_n = out.v.size();
	mxArray **out_v = out_n ? new mxArray*[out_n] : 0;
	try
	{
		call(cmd, in_n, in_p, out_n, out_v);
	}
	catch (EvalException)
	{
		delete[] out_v;
		throw;
	}

/*	int rc = mexCallMATLAB(out_n, out_v, in_n, in_p, cmd);
	if (rc)
	{
		delete[] out_v;
		THROW_EX(EvalException)("error calling function %s", cmd);
	}
*/	
	for (int i = 0; i < out_n; ++i)
		*out.v[i] = out_v[i];
	delete[] out_v;
}

//---------------------------------------------------------------------------------------------

void call(const char *cmd, const OutArgs &out, const OutArgs &in)
{
	call(cmd, in, out);
}

//---------------------------------------------------------------------------------------------

void exec(const char *cmd)
{
	call(cmd, MEX_NO_IN, MEX_NO_OUT);
}

//---------------------------------------------------------------------------------------------

void eval(const magenta::String &cmd)
{
	const char *p = cmd.c_str();
	int rc = mexEvalString(p);
	if (rc)
		THROW_EX(EvalException)("error during evaluation of command '%s': %s", p, lasterr().c_str());
}

//---------------------------------------------------------------------------------------------

magenta::String lasterr()
{
	return MEX::ConstString(call("lasterr"));
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta
