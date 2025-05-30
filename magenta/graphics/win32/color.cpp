
///////////////////////////////////////////////////////////////////////////////////////////////
// Color
///////////////////////////////////////////////////////////////////////////////////////////////

#include "color.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// Color
///////////////////////////////////////////////////////////////////////////////////////////////

Color::Color(Byte blue, Byte green, Byte red)
{
	color = RGB(blue, green, red);
}

Byte Color::blue() const { return GetBValue(color); }
Byte Color::green() const { return GetGValue(color); }
Byte Color::red() const { return GetRValue(color); }

//----------------------------------------------------------------------------------------------

SystemColor::SystemColor(Code code) : Color(GetSysColor(code)) {}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
