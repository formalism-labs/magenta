
#include "module.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

Module *Module::_module = 0;

//---------------------------------------------------------------------------------------------

Module::Module()
{
	::mexAtExit(atExit);
}

//---------------------------------------------------------------------------------------------

void Module::atExit()
{
	delete _module;
	_module = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Module::main(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) 
{
	n_in = nrhs;
	_in = prhs;
	n_out = nlhs;
	_out = plhs;
	main();
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta
