
#if defined(WIN32) && defined(MAGENTA_DLL)
#	ifdef IMPLEMENT_MAGENTA_GEOMETRY_1
#		define _class class __declspec(dllexport)
#	else
#		define _class class __declspec(dllimport)
#	endif
#else
#	define _class class
#endif

namespace magenta
{
	_class IConstPoint;
	_class IPoint;
	_class Point;
	
	_class IConstSize;
	_class ISize;
	_class Size;
	
	_class IConstRect;
	_class IRect;
	_class Rect;
}

#undef _class