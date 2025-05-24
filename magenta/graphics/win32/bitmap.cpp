
///////////////////////////////////////////////////////////////////////////////////////////////
// Bitmaps
///////////////////////////////////////////////////////////////////////////////////////////////

#include "magenta/graphics/win32/defs.h"
#include "magenta/system/win32/module.h"
#include "magenta/types/defs.h"

#include "bitmap.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// Bitmap
///////////////////////////////////////////////////////////////////////////////////////////////

Bitmap::Bitmap(const Instance &instance, const String &name)
{
	handle = !name ? Handle(0) : LoadBitmap(instance, name);
}

//----------------------------------------------------------------------------------------------

Bitmap::Bitmap(const IWindow &window, const IConstSize &size)
{
	DrawSession wdc(window);
	handle = CreateCompatibleBitmap(wdc, size.width(), size.height());
}

///////////////////////////////////////////////////////////////////////////////////////////////

DIBitmap::DIBitmap(Byte *bitmap, const IConstSize &size, int depth, int compression, int bitmap_size)
{
	if (!bitmap_size)
		bitmap_size = (((size.width() * depth + 31) & ~31) >> 3) * size.height();
	_bitmap = bitmap;
	_info.bmiHeader.biSize = sizeof(_info) - sizeof(_info.bmiHeader.biSize);
	_info.bmiHeader.biWidth = size.width();
	_info.bmiHeader.biHeight = size.height();
	_info.bmiHeader.biBitCount = depth;
	_info.bmiHeader.biSizeImage = bitmap_size;
	_info.bmiHeader.biCompression = compression;
	_info.bmiHeader.biPlanes = 1;
	_info.bmiHeader.biXPelsPerMeter = 0;
	_info.bmiHeader.biYPelsPerMeter = 0;
	_info.bmiHeader.biClrImportant = 0;
	_info.bmiHeader.biClrUsed = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Icon
///////////////////////////////////////////////////////////////////////////////////////////////

Icon::Icon(LPCTSTR systemName)
{
	handle = LoadIcon(NULL, systemName);
}

//----------------------------------------------------------------------------------------------

Icon::Icon(const Instance &instance, const String &name)
{
	handle = !name ? Handle(0) : LoadIcon(instance, name);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Cursor
///////////////////////////////////////////////////////////////////////////////////////////////

Cursor::Cursor(LPCTSTR systemName)
{
	handle = LoadCursor(NULL, systemName);
}

//----------------------------------------------------------------------------------------------

Cursor::Cursor(const Instance &instance, const String &name)
{
	handle = !name ? Handle(0) : LoadCursor(instance, name);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
