
#ifdef MAGENTA_DLL
#	ifdef IMPLEMENT_MAGENTA_GRAPHICS_2
#		define _class class __declspec(dllexport)
#	else
#		define _class class __declspec(dllimport)
#	endif
#else
#	define _class class
#endif

namespace magenta
{
	_class BitmapViewerWindow;
	_class BitmapViewer;
}

#undef _class