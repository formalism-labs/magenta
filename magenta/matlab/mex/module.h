
#ifndef _magenta_matlab_mex_module_
#define _magenta_matlab_mex_module_

#include "magenta/matlab/mex/general.h"

#include "magenta/exceptions/defs.h"

#include "matlab-5/extern/mex.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Module
{
	friend void ::mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

	static Module *_module;

protected:
	static void atExit();

//public:
	Module &module();

public:
	Module();
	virtual ~Module() {}

private:
	const mxArray **_in;
	mxArray **_out;
	int n_in, n_out;
public:
	int in() const { return n_in; }
	const mxArray *in(int k) { return _in[k]; }
	int out() const { return n_in; }
	mxArray *&out(int k) { return _out[k]; }

public:
	void main(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	virtual void main() {}
};

//---------------------------------------------------------------------------------------------

#define MEX_MODULE(M) \
\
extern "C" __declspec(dllexport) \
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) \
{ \
	using namespace magenta::MEX; \
	try \
	{ \
		if (!Module::_module) \
			Module::_module = new M; \
		Module &m = *Module::_module; \
		m.main(nlhs, plhs, nrhs, prhs); \
	} \
	catch (const char *x) \
	{ \
		mexErrMsgTxt(x); \
	} \
	catch (magenta::Exception &x) \
	{ \
		mexErrMsgTxt(x.report().c_str()); \
	} \
}

//---------------------------------------------------------------------------------------------

#define MEX_FUNCTION \
\
class Function : public MEX::Module \
{ \
public: \
	void main(); \
}; \
\
MEX_MODULE(Function); \
\
void Function::main

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_module_
