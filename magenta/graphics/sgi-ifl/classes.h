
#if defined(WIN32) && defined(MAGENTA_DLL)
#	ifdef IMPLEMENT_MAGENTA_GRAPHICS_SGI_IFL_1
#		define _class class __declspec(dllexport)
#	else
#		define _class class __declspec(dllimport)
#	endif
#else
#	define _class class
#endif

namespace magenta
{
	_class Image;
}

#undef _class
