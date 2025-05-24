
///////////////////////////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_geometry_types_
#define _magenta_geometry_types_

#include <xutility>
#include <windows.h>

#include <algorithm>

namespace magenta
{

//using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////
// Point
///////////////////////////////////////////////////////////////////////////////////////////////

class IConstPoint
{
public:
	virtual int x() const = 0;
	virtual int y() const = 0;
};

//----------------------------------------------------------------------------------------------

class IPoint : public IConstPoint
{
public:
	int x() const { return const_cast<IPoint *>(this)->x(); }
	int y() const { return const_cast<IPoint *>(this)->y(); }
	virtual int &x() = 0;
	virtual int &y() = 0;
};

//----------------------------------------------------------------------------------------------

class Point : public IPoint
{
	int _x, _y;

public:
	Point(int x, int y) : _x(x), _y(y) {}
	Point(const IConstPoint &point) : _x(point.x()), _y(point.y()) {}

	virtual int &x() { return _x; }
	virtual int &y() { return _y; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Position
///////////////////////////////////////////////////////////////////////////////////////////////

typedef IConstPoint IConstPosition;
typedef IPoint IPosition;
typedef Point Position;

///////////////////////////////////////////////////////////////////////////////////////////////
// Size
///////////////////////////////////////////////////////////////////////////////////////////////

class IConstSize
{
public:
	virtual int width() const = 0;
	virtual int height() const = 0;

	bool operator==(const IConstSize &size) const
	{
		return width() == size.width() && height() == size.height();
	}

	bool operator!=(const IConstSize &size) const
	{
		return !operator==(size);
	}
};

//----------------------------------------------------------------------------------------------

class ISize : public IConstSize
{
public:
	int width() const { return const_cast<ISize *>(this)->width(); }
	int height() const { return const_cast<ISize *>(this)->height(); }
	virtual int &width() = 0;
	virtual int &height() = 0;
};

//----------------------------------------------------------------------------------------------

class Size : public ISize
{
	int _width, _height;

public:
	Size(int width = 0, int height = 0) : _width(width), _height(height) {}
	Size(const IConstSize &size) : _width(size.width()), _height(size.height()) {}

	int &width() { return _width; }
	int &height() { return _height; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Rectangle
///////////////////////////////////////////////////////////////////////////////////////////////

class IConstRect
{
public:
	virtual int left() const = 0;
	virtual int right() const = 0;
	virtual int top() const = 0;
	virtual int bottom() const = 0;

	int x() const { return left(); }
	int y() const { return top(); }
	int width() const { return right() - left(); }
	int height() const { return bottom() - top(); }

	Position position() const { return Position(left(), top()); }
	Size size() const { return Size(width(), height()); }

	operator RECT() const { RECT r = { left(), top(), right(), bottom() }; return r; }
};

//----------------------------------------------------------------------------------------------

class IRect : public IConstRect
{
public:
	virtual int &left() = 0;
	virtual int &right() = 0;
	virtual int &top() = 0;
	virtual int &bottom() = 0;

	int left() const { return const_cast<IRect *>(this)->left(); }
	int right() const { return const_cast<IRect *>(this)->right(); }
	int top() const { return const_cast<IRect *>(this)->top(); }
	int bottom() const { return const_cast<IRect *>(this)->bottom(); }

	int &x() { return left(); }
	int &y() { return top(); }

	class Width
	{
	public:
		IRect &rect;
		Width(IRect &rect) : rect(rect) {}
		Width &operator=(int w) { rect.right() = rect.left() + w; return *this; }
		operator int() const { return ((const IConstRect &) rect).width(); }
	};

	Width width() { return *this; }

	class Height
	{
	public:
		IRect &rect;
		Height(IRect &rect) : rect(rect) {}
		Height &operator=(int h) { rect.bottom() = rect.top() + h; return *this; }
		operator int() const { return ((const IConstRect &) rect).height(); }
	};

	Height height() { return *this; }

	class Position
	{
	public:
		IRect &rect;
		Position(IRect &rect) : rect(rect) {}
		Position &operator=(const IConstPosition &pos) { rect.left() = pos.x(); rect.top() = pos.y(); return *this; }
		operator magenta::Position() const { return ((const IConstRect &) rect).position(); }
	};

	Position position() { return *this; }

	class Size
	{
	public:
		IRect &rect;
		Size(IRect &rect) : rect(rect) {}
		Size &operator=(const IConstSize &size) 
			{ rect.right() = rect.left() + size.width(); rect.bottom() = rect.top() + size.height(); return *this; }
		operator magenta::Size() const { return ((const IConstRect &) rect).size(); }
	};

	Size size() { return *this; }
};

//----------------------------------------------------------------------------------------------

class Rect : public IRect
{
	int _left, _right, _top, _bottom;

public:
	Rect(int left, int top, int right, int bottom) : 
		_left(left), _top(top), _right(right), _bottom(bottom) {}

	Rect(RECT r) : _left(r.left), _top(r.top), _right(r.right), _bottom(r.bottom) {}

	Rect(const IConstPosition &pos, const IConstSize &size)
	{
		_left = pos.x();
		_right = _left + size.width();
		_top = pos.y();
		_bottom = _top + size.height();
	}

	Rect(const IConstPoint &p1, const IConstPoint &p2)
	{
		_left = std::min(p1.x(), p2.x());
		_right = _left + abs(p1.x() - p2.x());
		_top = std::min(p1.y(), p2.y());
		_bottom = _top + abs(p1.y() - p2.y());
	}

	int &left() { return _left; }
	int &right() { return _right; }
	int &top() { return _top; }
	int &bottom() { return _bottom; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_geometry_types_
