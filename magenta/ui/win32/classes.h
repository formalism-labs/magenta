
#if defined(WIN32) && defined(MAGENTA_DLL)
#	ifdef IMPLEMENT_MAGENTA_UI_WIN32_3
#		define _class class __declspec(dllexport)
#	else
#		define _class class __declspec(dllimport)
#	endif
#else
#	define _class class
#endif

namespace magenta
{
namespace UI
{
	_class WindowClass;
	
	_class IWindow;
	_class Window;
	
	_class IFrameWindow;
	_class FrameWindow;
	
	_class MDIWindow;
	_class ChildWindow;
	
	_class IDialogWindow;
	_class DialogWindow;
	
	_class Event;
	_class WindowEvent;
	_class CreateEvent;
	_class PaintEvent;
	_class MoveEvent;
	_class ResizeEvent;
	_class MouseClickEvent;
	_class DestroyEvent;

	_class Menu;
	_class PopupMenu;
	_class MenuItem;
}
}

#undef _class
