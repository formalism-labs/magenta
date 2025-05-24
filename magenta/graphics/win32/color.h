
#ifndef _magenta_graphics_win32_color_
#define _magenta_graphics_win32_color_

#include "magenta/graphics/win32/general.h"
#include "magenta/types/defs.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// Color
///////////////////////////////////////////////////////////////////////////////////////////////

class Color
{
	COLORREF color;

protected:
	Color(COLORREF color) : color(color) {}

public:
	Color(Byte blue, Byte green, Byte red);

	Byte blue() const;
	Byte green() const;
	Byte red() const;

	operator COLORREF() const { return color; }
};

//---------------------------------------------------------------------------------------------

class SystemColor : public Color
{
public:
	enum Code
	{
		window_background = COLOR_WINDOW
	};

	SystemColor(Code code);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_graphics_win32_color_
