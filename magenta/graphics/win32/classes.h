
#if defined(WIN32) && defined(MAGENTA_DLL)
#	ifdef IMPLEMENT_MAGENTA_GRAPHICS_WIN32_2
#		define _class class __declspec(dllexport)
#	else
#		define _class class __declspec(dllimport)
#	endif
#else
#	define _class class
#endif

namespace magenta
{
	_class Bitmap;
	_class DIBitmap;
	_class RGBDIBitmap;
	_class Icon;
	_class Cursor;
	
	_class Color;
	_class SystemColor;
	
	_class IDeviceContext;
	_class DeviceContext;
	_class WindowContext;
	_class DrawSession;
	_class PaintSession;
	_class PrintSession;
//	_class GLDrawSession;
	
	_class Brush;
	_class SolidBrush;
	_class PatternBrush;
	_class HatchBrush;

	_class IShape;
	_class Ellipse;
	_class Rectangle;
}

#undef _class
