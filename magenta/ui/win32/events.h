
///////////////////////////////////////////////////////////////////////////////////////////////
// Events
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_ui_win32_events_
#define _magenta_ui_win32_events_

#include "magenta/ui/win32/classes.h"
#include "magenta/ui/win32/general.h"

namespace magenta
{

namespace UI
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Event
{
	MSG message;

public:
	Event(UINT uMsg, WPARAM wParam, LPARAM lParam);

	UINT type() const { return message.message; }
	LPARAM lparam() const { return message.lParam; }
	WPARAM wparam() const { return message.wParam; }

	LRESULT result;
};

//----------------------------------------------------------------------------------------------

class WindowEvent
{
protected:
	Event &event;

public:
	WindowEvent(Event &event) : event(event) {}
	LRESULT &result() { return event.result; }
};

//----------------------------------------------------------------------------------------------

class CreateEvent : public WindowEvent
{
public:
	CreateEvent(Event &event) : WindowEvent(event) {}
};

//----------------------------------------------------------------------------------------------

class PaintEvent : public WindowEvent
{
public:
	PaintEvent(Event &event) : WindowEvent(event) {}
};

//----------------------------------------------------------------------------------------------

class MoveEvent : public WindowEvent
{
public:
	MoveEvent(Event &event) : WindowEvent(event) {}
};

//----------------------------------------------------------------------------------------------

class ResizeEvent : public WindowEvent
{
public:
	ResizeEvent(Event &event) : WindowEvent(event) {}
};

//----------------------------------------------------------------------------------------------

class MouseClickEvent : public WindowEvent
{
public:
	MouseClickEvent(Event &event) : WindowEvent(event) {}

	bool leftButton() const 
	{ 
		UINT t = event.type();
		return t == WM_LBUTTONDOWN || t == WM_LBUTTONUP || t == WM_LBUTTONDBLCLK;
	}

	bool middleButton() const 
	{ 
		UINT t = event.type();
		return t == WM_MBUTTONDOWN || t == WM_MBUTTONUP || t == WM_MBUTTONDBLCLK;
	}

	bool rightButton() const 
	{ 
		UINT t = event.type();
		return t == WM_RBUTTONDOWN || t == WM_RBUTTONUP || t == WM_RBUTTONDBLCLK;
	}

	bool buttonUp() const
	{
		UINT t = event.type();
		return t == WM_LBUTTONUP || t == WM_MBUTTONUP || t == WM_RBUTTONUP;
	}

	bool buttonDown() const
	{
		UINT t = event.type();
		return t == WM_LBUTTONDOWN || t == WM_MBUTTONDOWN || t == WM_RBUTTONDOWN;
	}

	bool doubleClick() const
	{
		UINT t = event.type();
		return t == WM_LBUTTONDBLCLK || t == WM_MBUTTONDBLCLK || t == WM_RBUTTONDBLCLK;
	}
};

//----------------------------------------------------------------------------------------------

class DestroyEvent : public WindowEvent
{
public:
	DestroyEvent(Event &event) : WindowEvent(event) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace UI
} // namespace magenta

#endif // _magenta_ui_win32_events_
