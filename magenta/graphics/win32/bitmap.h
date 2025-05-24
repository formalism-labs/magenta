
#ifndef _magenta_graphics_win32_bitmap_
#define _magenta_graphics_win32_bitmap_

#include "magenta/graphics/win32/graphic-objects.h"

#include "magenta/geometry/defs.h"
#include "magenta/system/defs.h"
#include "magenta/system/win32/classes.h"
#include "magenta/types/defs.h"
#include "magenta/ui/win32/classes.h"

namespace magenta
{

using namespace UI;

///////////////////////////////////////////////////////////////////////////////////////////////
// Bitmap
///////////////////////////////////////////////////////////////////////////////////////////////

class Bitmap : public GraphicObject<HBITMAP>
{
public:
	Bitmap(const Instance &instance, const String &name);
	Bitmap(const IWindow &window, const IConstSize &size);
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Device Independant Bitmap
///////////////////////////////////////////////////////////////////////////////////////////////

class DIBitmap
{
	BITMAPINFO _info;
	Byte *_bitmap;

public:
	DIBitmap(Byte *bitmap, const IConstSize &size, int depth, int compression = BI_RGB, 
		int bitmap_size = 0);

	operator const BITMAPINFO*() const { return &_info; }
	operator const BITMAPINFO&() const { return _info; }

	operator const BITMAPINFOHEADER*() const { return &_info.bmiHeader; }
	operator const BITMAPINFOHEADER&() const { return _info.bmiHeader; }
	
	Byte *data() { return _bitmap; }
	const Byte *data() const { return _bitmap; }

	Size size() const { return Size(_info.bmiHeader.biWidth, _info.bmiHeader.biHeight); }
};

//---------------------------------------------------------------------------------------------

class RGBDIBitmap : public DIBitmap
{
public:
	RGBDIBitmap(Byte *bitmap, const IConstSize &size, int depth) :
		DIBitmap(bitmap, size, depth, BI_RGB, 0) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Icon
///////////////////////////////////////////////////////////////////////////////////////////////

class Icon : public GraphicObject<HICON>
{
public:
	Icon(LPCTSTR systemName);
	Icon(const Instance &instance, const String &name);
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Cursor
///////////////////////////////////////////////////////////////////////////////////////////////

class Cursor : public GraphicObject<HCURSOR>
{
public:
	Cursor(LPCTSTR systemName);
	Cursor(const Instance &instance, const String &name);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_graphics_win32_shapes_
