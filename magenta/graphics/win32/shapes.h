
#ifndef _magenta_graphics_win32_shapes_
#define _magenta_graphics_win32_shapes_

#include "magenta/geometry/classes.h"

#include "magenta/graphics/win32/general.h"
#include "magenta/graphics/win32/classes.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// Shapes
///////////////////////////////////////////////////////////////////////////////////////////////

class IShape
{
public:
	virtual void draw(IDeviceContext &dc) = 0;
	virtual void fill(IDeviceContext &dc, const Brush &brush) = 0;
};

//---------------------------------------------------------------------------------------------

class Ellipse : public IShape
{
	RECT _rect;

public:
	Ellipse(const IConstRect &rect);
	void draw(IDeviceContext &dc);
	virtual void fill(IDeviceContext &dc, const Brush &brush);
};

//---------------------------------------------------------------------------------------------

class Rectangle : public IShape
{
	RECT _rect;

public:
	Rectangle(const IConstRect &rect);
	void draw(IDeviceContext &dc);
	void fill(IDeviceContext &dc, const Brush &brush);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_graphics_win32_shapes_
