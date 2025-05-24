
///////////////////////////////////////////////////////////////////////////////////////////////
// Window
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_ui_win32_window_
#define _magenta_ui_win32_window_

#include "magenta/exceptions/defs.h"
#include "magenta/geometry/defs.h"
#include "magenta/graphics/win32/classes.h"
#include "magenta/system/defs.h"
#include "magenta/system/win32/defs.h"
#include "magenta/types/defs.h"

#include "magenta/ui/win32/classes.h"

namespace magenta
{

namespace UI
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Window Class
///////////////////////////////////////////////////////////////////////////////////////////////

class IWindowClass
{
	friend Window;

public:
	typedef UINT ClassStyle;

protected:
	struct InitialData
	{
		Icon *icon, *small_icon;
		Cursor *cursor;
		Brush *background;
	};

	struct CreateData : public CREATESTRUCT
	{
		CreateData(Window *w) : window(w)
		{
			lpCreateParams = &cbExtra;
			cbExtra = sizeof(*this) - ((char *) &window - (char *) &cbExtra);
		}

		SHORT cbExtra;
		Window *window;
	};

public:
	virtual ClassStyle i_class_style() const { return CS_HREDRAW | CS_VREDRAW; }
	virtual String i_icon_name() const { return ""; }
	virtual String i_small_icon_name() const { return ""; }
	virtual String i_cursor_name() const { return ""; }
	virtual Brush i_background() const;
	virtual String i_class_menu_name() const { return ""; }
	virtual int i_extra_win_storage() const { return 0; }
	virtual int i_extra_class_storage() const { return 0; }

	virtual IWindowClass *&window_class() { THROW_EX(Unimplemented); }

	virtual const String &name() const = 0;

	CLASS_EXCEPTION_DEF("WindowClass error");
};

//----------------------------------------------------------------------------------------------

class RWindowClass : public IWindowClass
{
protected:
	IWindowClass &self() { return *window_class(); }
	const IWindowClass &self() const 
		{ return *((RWindowClass *)this)->window_class(); }

public:
	ClassStyle i_class_style() const { return self().i_class_style(); }
	String i_icon_name() const { return self().i_icon_name(); }
	String i_small_icon_name() const { return self().i_small_icon_name(); }
	String i_cursor_name() const { return self().i_cursor_name(); }
	Brush i_background() const;
	String i_class_menu_name() const { return self().i_class_menu_name(); }
	int i_extra_win_storage() const { return self().i_extra_win_storage(); }
	int i_extra_class_storage() const { return self().i_extra_class_storage(); }

	virtual const String &name() const { return self().name(); }
};

//----------------------------------------------------------------------------------------------

class WindowClass : public IWindowClass
{
protected:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 

	InitialData initial_data;
	magenta::Instance _instance;
	String _name;

public:
    const String &name() const { return _name; }

public:
	WindowClass(const magenta::Instance &instance, 
		const IString &name = String(Uuid())) : _instance(instance), _name(name) {}
	~WindowClass();

	void create();
};

#define DEFINE_WINDOW_CLASS(name) \
	IWindowClass *name::_class = 0

#define DECLARE_WINDOW_CLASS(name) \
	static IWindowClass *_class; \
	virtual IWindowClass *&window_class() { return _class; }

///////////////////////////////////////////////////////////////////////////////////////////////
// Window
///////////////////////////////////////////////////////////////////////////////////////////////

class IWindow
{
public:
	typedef HWND Handle;

protected:
	virtual Handle handle() const = 0;

public:
	operator Handle() const { return handle(); }

	typedef DWORD Style;

	virtual LRESULT dispatch(magenta::UI::Event &event);

	virtual bool onEvent(CreateEvent &event);
	virtual bool onEvent(PaintEvent &event);
	virtual bool onEvent(ResizeEvent &event);
	virtual bool onEvent(MoveEvent &event);
	virtual bool onEvent(MouseClickEvent &event);
	virtual bool onEvent(DestroyEvent &event) { return false; }

public:
	void show();
	Rect clientRect() const;

	void move(const IConstPosition &position);
	void resize(const IConstSize &size);
	void reshape(const IConstPosition *position, const IConstSize *size);

	Size size() const;

	CLASS_EXCEPTION_DEF("IWindow error");
};

//----------------------------------------------------------------------------------------------

class Window : virtual public IWindow, public RWindowClass
{
	friend WindowClass;

	typedef WindowClass Class;

	DECLARE_WINDOW_CLASS(Window);

protected:
	virtual String i_title() const { return ""; }
	virtual Size i_size() const { return Size(CW_USEDEFAULT, CW_USEDEFAULT); }
	virtual Position i_position() const { return Position(CW_USEDEFAULT, CW_USEDEFAULT); }
	virtual Style i_style() const { return 0L; }
	virtual Style i_ex_style() const { return 0L; }
	virtual String i_menu_name() const { return ""; }

	Handle _handle;
	Handle handle() const { return _handle; }

public:
	Window() {}
	virtual void create(const Instance &instance, const IWindow *owner_window = 0);

	CLASS_EXCEPTION_DEF("Window error");
	CLASS_SUB_EXCEPTION_DEF(CreateError, "Window create error");
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Frame Window
///////////////////////////////////////////////////////////////////////////////////////////////

class IFrameWindow : virtual public IWindow
{
public:
	Size clientSize() const;
	Rect clientRect() const;
};

//----------------------------------------------------------------------------------------------

class FrameWindow : public Window, public IFrameWindow
{
	DECLARE_WINDOW_CLASS(FrameWindow);

protected:
	Handle handle() const { return Window::handle(); }

	Menu *initial_menu;

	Style i_style() const { return WS_OVERLAPPEDWINDOW; }
	Menu *i_menu();

public:
	FrameWindow();
	~FrameWindow();
};

///////////////////////////////////////////////////////////////////////////////////////////////

class MDIWindow : public FrameWindow
{
	int children_count;

public:
	MDIWindow(const Instance &instance);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ChildWindow : public FrameWindow
{
public:
	ChildWindow(const MDIWindow &parent);
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Dialog Window
///////////////////////////////////////////////////////////////////////////////////////////////

class IDialogWindow : virtual public IWindow
{
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
};

//----------------------------------------------------------------------------------------------

class DialogWindow : public Window, public IDialogWindow
{
	Handle handle() const { return Window::handle(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace UI
} // namespace magenta

#endif // _magenta_ui_win32_window_
