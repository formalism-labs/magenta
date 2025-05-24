
#if defined(WIN32) && defined(MAGENTA_DLL)
#	ifdef IMPLEMENT_MAGENTA_MATLAB_SIMULINK_1
#		define _class class __declspec(dllexport)
#	else
#		define _class class __declspec(dllimport)
#	endif
#else
#	define _class class
#endif

namespace magenta
{
	_class SFunction;
}

#undef _class
