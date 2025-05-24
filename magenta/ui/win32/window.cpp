
///////////////////////////////////////////////////////////////////////////////////////////////
// Window
///////////////////////////////////////////////////////////////////////////////////////////////

#include "window.h"

#include "events.h"
#include "menu.h"

#include "magenta/graphics/win32/defs.h"

namespace magenta
{

namespace UI
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Window Class
///////////////////////////////////////////////////////////////////////////////////////////////

Brush IWindowClass::i_background() const 
{ 
	return SolidBrush(SystemColor(SystemColor::window_background)); 
}

///////////////////////////////////////////////////////////////////////////////////////////////

Brush RWindowClass::i_background() const 
{ 
	return self().i_background();
}

///////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowClass::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int winptr_offset = GetClassLong(hWnd, GCL_CBWNDEXTRA) - sizeof(Window *);
	Window *window = reinterpret_cast<Window *>(GetWindowLong(hWnd, winptr_offset));
	if (!window)
		if (uMsg == WM_CREATE || uMsg == WM_NCCREATE)
		{
			UNALIGNED CreateData *create_data = (UNALIGNED CreateData *) (((LPCREATESTRUCT) lParam)->lpCreateParams); 
			window = create_data->window;
			window->_handle = hWnd;
			SetWindowLong(hWnd, winptr_offset, LONG(window));
		}
		else
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

	Event event(uMsg, wParam, lParam);
	return window->dispatch(event);
}

//----------------------------------------------------------------------------------------------

void WindowClass::create()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = i_class_style();
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = i_extra_class_storage();
	wc.cbWndExtra = i_extra_win_storage() + sizeof(Window *);
	wc.hInstance = _instance;

	wc.hIcon = *(initial_data.icon = new Icon(_instance, i_icon_name()));
	if (!wc.hIcon)
		wc.hIcon = Icon(IDI_WINLOGO);
	wc.hIconSm = *(initial_data.small_icon = new Icon(_instance, i_small_icon_name()));
	wc.hCursor = *(initial_data.cursor = new Cursor(_instance, i_cursor_name()));
	if (!wc.hCursor)
		wc.hCursor = Cursor(IDC_ARROW);
//	new Brush(i_background());
	wc.hbrBackground = *(initial_data.background = new Brush(i_background()));
	String menu_name = i_class_menu_name();
	wc.lpszMenuName = menu_name;
	wc.lpszClassName = _name;
	if (!RegisterClassEx(&wc))
		THROW_EX(Exception);
}

//----------------------------------------------------------------------------------------------

WindowClass::~WindowClass()
{
	UnregisterClass(name(), _instance);

	delete initial_data.icon;
	delete initial_data.small_icon;
	delete initial_data.cursor;
	delete initial_data.background;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// IWindow
///////////////////////////////////////////////////////////////////////////////////////////////

void IWindow::show()
{
	ShowWindow(handle(), SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Rect IWindow::clientRect() const
{
	RECT rect;
	GetClientRect(*this, &rect);
	return rect;
}

//----------------------------------------------------------------------------------------------

void IWindow::move(const IConstPosition &position)
{
	reshape(&position, 0);
}

//----------------------------------------------------------------------------------------------

void IWindow::resize(const IConstSize &size)
{
	reshape(0, &size);
}

//----------------------------------------------------------------------------------------------

void IWindow::reshape(const IConstPosition *position, const IConstSize *size)
{
	Position pos_(0, 0);
	if (position)
		pos_ = *position;
	Size size_(0, 0);
	if (size)
		size_ = *size;
	int mode = SWP_NOZORDER | SWP_NOACTIVATE;
	!!size && (mode |= SWP_NOMOVE);
	!!position && (mode |= SWP_NOSIZE);
	SetWindowPos(handle(), 0, pos_.x(), pos_.y(), size_.width(), size_.height(), mode);
}

//----------------------------------------------------------------------------------------------

Size IWindow::size() const
{
	RECT r;
	BOOL rc = GetWindowRect(handle(), &r);
	return Size(r.right - r.left, r.bottom - r.top);
}

///////////////////////////////////////////////////////////////////////////////////////////////

LRESULT IWindow::dispatch(Event &event)
{
	bool processed = false;
	switch (event.type())
	{
	case WM_CREATE: processed = onEvent(CreateEvent(event)); break;
	case WM_PAINT: processed = onEvent(PaintEvent(event)); break;
	case WM_DESTROY: processed = onEvent(DestroyEvent(event)); break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		processed = onEvent(MouseClickEvent(event));
		break;

	case WM_SIZE:
		processed = onEvent(ResizeEvent(event));
		break;

	case WM_MOVE:
		processed = onEvent(MoveEvent(event));
		break;

	default:
		break;
	}

	if (processed)
		return event.result;
	else
		return DefWindowProc(handle(), event.type(), event.wparam(), event.lparam());
}

//----------------------------------------------------------------------------------------------

bool IWindow::onEvent(PaintEvent &event)
{
	PaintSession paint_session(*this);
	event.result() = 0;
	return true;
}

//----------------------------------------------------------------------------------------------

bool IWindow::onEvent(ResizeEvent &event)
{
	event.result() = 0;
	return true;
}

//----------------------------------------------------------------------------------------------

bool IWindow::onEvent(MoveEvent &event)
{
	event.result() = 0;
	return true;
}

//----------------------------------------------------------------------------------------------

bool IWindow::onEvent(MouseClickEvent &event)
{
	event.result() = 0;
	return true;
}

//----------------------------------------------------------------------------------------------

bool IWindow::onEvent(CreateEvent &event)
{
	event.result() = 0; 
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Window
///////////////////////////////////////////////////////////////////////////////////////////////

DEFINE_WINDOW_CLASS(Window);

//----------------------------------------------------------------------------------------------

void Window::create(const Instance &instance, const IWindow *owner_window)
{
	if (!window_class())
	{
		WindowClass *_class = new WindowClass(instance);
		_class->create();
		window_class() = _class;
	}

	Position position = i_position();
	Size size = i_size();
	String class_name = window_class()->name();
	String title = i_title();
	CreateData create_data(this);

	HMENU menu = !!i_menu_name() ? Menu(instance, i_menu_name()) : HMENU(0);
	_handle = CreateWindowEx(i_ex_style(), class_name, title, i_style(),
		position.x(), position.y(), size.width(), size.height(),
		owner_window ? *owner_window : HWND(0), menu, instance, &create_data);

	if (!_handle)
		THROW_EX(CreateError);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Frame Window
///////////////////////////////////////////////////////////////////////////////////////////////

Size IFrameWindow::clientSize() const
{
	RECT r;
	BOOL rc = GetClientRect(handle(), &r);
	return Size(r.right - r.left, r.bottom - r.top);
}

//----------------------------------------------------------------------------------------------

Rect IFrameWindow::clientRect() const
{
	RECT r;
	BOOL rc = GetClientRect(handle(), &r);
	return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////

DEFINE_WINDOW_CLASS(FrameWindow);

//----------------------------------------------------------------------------------------------

FrameWindow::FrameWindow() : initial_menu(0)
{
	delete initial_menu;
}

//----------------------------------------------------------------------------------------------

FrameWindow::~FrameWindow()
{
	delete initial_menu;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace UI

} // namespace magenta
