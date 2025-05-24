
///////////////////////////////////////////////////////////////////////////////////////////////
// Shapes
///////////////////////////////////////////////////////////////////////////////////////////////

#include "magenta/geometry/defs.h"
#include "magenta/system/win32/module.h"
#include "magenta/types/general.h"
#include "magenta/types/string.h"

#include "device-context.h"
#include "graphic-objects.h"
#include "shapes.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// Ellipse
///////////////////////////////////////////////////////////////////////////////////////////////

Ellipse::Ellipse(const IConstRect &rect) : _rect(rect)
{
}

//----------------------------------------------------------------------------------------------

void Ellipse::draw(IDeviceContext &dc)
{
	::Ellipse(dc, _rect.left, _rect.top, _rect.right, _rect.bottom);
}

//----------------------------------------------------------------------------------------------

void Ellipse::fill(IDeviceContext &dc, const Brush &brush)
{
//	SaveBrush
//	SetBrash
//	::Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);
//	RestoreBrash
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Rectangle
///////////////////////////////////////////////////////////////////////////////////////////////

Rectangle::Rectangle(const IConstRect &rect) : _rect(rect) 
{
}

//----------------------------------------------------------------------------------------------

void Rectangle::draw(IDeviceContext &dc)
{
	::Rectangle(dc, _rect.left, _rect.top, _rect.right, _rect.bottom);
}

//----------------------------------------------------------------------------------------------

void Rectangle::fill(IDeviceContext &dc, const Brush &brush)
{
	::FillRect(dc, &_rect, brush);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
