
///////////////////////////////////////////////////////////////////////////////////////////////
// Grahpic Objects
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_graphics_win32_graphic_objects_
#define _magenta_graphics_win32_graphic_objects_

#include "magenta/graphics/win32/classes.h"
#include "magenta/ui/win32/classes.h"

#include "magenta/graphics/win32/general.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Graphic Object
///////////////////////////////////////////////////////////////////////////////////////////////

template <class _Handle>
class GraphicObject
{
public:
	typedef _Handle Handle;

protected:
	Handle handle;

public:
	GraphicObject() : handle(0) {}
	GraphicObject(Handle handle) : handle(handle) {}
	operator Handle() const { return handle; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Brush
///////////////////////////////////////////////////////////////////////////////////////////////

class Brush : public GraphicObject<HBRUSH>
{
public:
	Brush(Handle handle = 0) : GraphicObject<Handle>(handle) {}
	Brush(const Brush &b) : GraphicObject<Handle>(Handle(b)) {}
};

//---------------------------------------------------------------------------------------------

class SolidBrush : public Brush
{
public:
	SolidBrush(Color color);
};

//---------------------------------------------------------------------------------------------

class PatternBrush : public Brush
{
public:
	PatternBrush(const Bitmap &bitman);
};

//---------------------------------------------------------------------------------------------

class HatchBrush : public Brush
{
public:
	enum Style
	{
		horizontal = HS_HORIZONTAL,
		vertical = HS_VERTICAL,
		cross = HS_CROSS,
		diagonal_lt2rb = HS_BDIAGONAL,
		diagonal_lb2rt = HS_FDIAGONAL,
		diagonal_cross = HS_DIAGCROSS
	};
 
	HatchBrush(Style style, Color color);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_graphics_win32_graphic_objects_
