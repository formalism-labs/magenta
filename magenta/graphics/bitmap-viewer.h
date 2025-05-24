
#ifndef _magenta_graphics_bitmap_viewer_
#define _magenta_graphics_bitmap_viewer_

#include "magenta/geometry/defs.h"
#include "magenta/graphics/win32/bitmap.h"
#include "magenta/ui/win32/window.h"

namespace magenta
{



///////////////////////////////////////////////////////////////////////////////////////////////

class BitmapViewerWindow : public FrameWindow
{
	DECLARE_WINDOW_CLASS(BitmapViewer);

	DIBitmap &_bitmap;

public:
	BitmapViewerWindow(DIBitmap &bitmap) : _bitmap(bitmap) {}

	String i_title() const { return "Bitmap"; }
	Size i_size() const { return Size(_bitmap.size().width(), _bitmap.size().height()); }

	bool onEvent(magenta::CreateEvent &event);
	bool onEvent(PaintEvent &event);
	bool onEvent(MouseClickEvent &event);
	bool onEvent(DestroyEvent &event);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class BitmapViewer
{
	BitmapViewerWindow *_win;

public:
	BitmapViewer(Instance &instance, DIBitmap &bitmap);
	~BitmapViewer();
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_graphics_bitmap_viewer_
