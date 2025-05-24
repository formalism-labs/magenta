
#include "bitmap-viewer.h"

#include "magenta/ui/win32/defs.h"
#include "magenta/graphics/win32/device-context.h"

namespace magenta
{



///////////////////////////////////////////////////////////////////////////////////////////////

DEFINE_WINDOW_CLASS(BitmapViewerWindow);

//---------------------------------------------------------------------------------------------

bool BitmapViewerWindow::onEvent(magenta::CreateEvent &event)
{
	Size s1 = size(), s2 = clientRect().size();
	int w = s1.width() - s2.width(), h = s1.height() - s2.height();
	resize(Size(_bitmap.size().width() + w, _bitmap.size().height() + h));
	event.result() = 0;
	return true;
}

//---------------------------------------------------------------------------------------------

bool BitmapViewerWindow::onEvent(PaintEvent &event)
{
	PaintSession canvas(*this);
	canvas.copyDIB(_bitmap, Rect(0, 0, _bitmap.size().width(), _bitmap.size().height()));
	event.result() = 0;
	return true;
}

//---------------------------------------------------------------------------------------------

bool BitmapViewerWindow::onEvent(MouseClickEvent &event) 
{ 
	return true; 
}

//---------------------------------------------------------------------------------------------

bool BitmapViewerWindow::onEvent(DestroyEvent &event) 
{ 
	PostQuitMessage(0); 
	return true; 
}

///////////////////////////////////////////////////////////////////////////////////////////////

BitmapViewer::BitmapViewer(Instance &instance, DIBitmap &bitmap)
{
	_win = new BitmapViewerWindow(bitmap);
	_win->create(instance);
	_win->show();
}

//---------------------------------------------------------------------------------------------

BitmapViewer::~BitmapViewer()
{
	delete _win;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

