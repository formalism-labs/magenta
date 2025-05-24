
///////////////////////////////////////////////////////////////////////////////////////////////
// Device Contexts
///////////////////////////////////////////////////////////////////////////////////////////////

#include "bitmap.h"
#include "shapes.h"
#include "device-context.h"

#include "magenta/ui/win32/defs.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////

void IDeviceContext::draw(IShape &shape)
{
	shape.draw(*this);
}

//----------------------------------------------------------------------------------------------

void IDeviceContext::fill(IShape &shape, const Brush &brush)
{
	shape.fill(*this, brush);
}

//----------------------------------------------------------------------------------------------

void IDeviceContext::bitblt(const IConstRect &srcRect, IDeviceContext &destCtx, 
	const IConstPoint &destPoint, DWORD op)
{
	BOOL rc;
	rc = ::BitBlt(handle(), srcRect.x(), srcRect.y(), srcRect.width(), srcRect.height(),
		destCtx, destPoint.x(), destPoint.y(), op);
	if (!rc)
		throw Error();
}

//----------------------------------------------------------------------------------------------

void IDeviceContext::copyDIB(const DIBitmap &dib, const IConstRect &dest, const IConstPoint &src)
{
	int rc = StretchDIBits(handle(), dest.x(), dest.y(), dest.width(), dest.height(),
		src.x(), src.y(), dest.width(), dest.height(),
		dib.data(), dib, DIB_RGB_COLORS, SRCCOPY);
	int rc1 = GetLastError();
}

///////////////////////////////////////////////////////////////////////////////////////////////

DrawSession::DrawSession(const IWindow &window) : WindowContext(window)
{
	_handle = GetDC(window);
}

//----------------------------------------------------------------------------------------------

DrawSession::~DrawSession()
{
	ReleaseDC(window, handle());
}

///////////////////////////////////////////////////////////////////////////////////////////////

PaintSession::PaintSession(const IWindow &window) : WindowContext(window)
{
	_handle = BeginPaint(window, &info);
}

//----------------------------------------------------------------------------------------------

PaintSession::~PaintSession()
{
	EndPaint(window, &info);
}

///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef MAGENTA_OPENGL

GLDrawSession::GLDrawSession(IDeviceContext &deviceContext) : device_context(&deviceContext)
{
	HDC hdc = *device_context;
	PIXELFORMATDESCRIPTOR pixed_format_descriptor = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,                    // 24-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		0,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // 32-bit z-buffer     
		0,                     // no stencil buffer 
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
    }; 
 
	int pixel_format = ChoosePixelFormat(hdc, &pixed_format_descriptor);
	SetPixelFormat(hdc, pixel_format, &pixed_format_descriptor);
 
	_handle = wglCreateContext(hdc);
	if (_handle != NULL && wglMakeCurrent(hdc, _handle))
		return;
	throw Error();
}

//----------------------------------------------------------------------------------------------

GLDrawSession::~GLDrawSession()
{
	wglMakeCurrent(NULL, NULL) ;
	wglDeleteContext(_handle);
}

//----------------------------------------------------------------------------------------------

void GLDrawSession::swapBuffers()
{
	ShowCursor(false);
	::SwapBuffers(*device_context);
	ShowCursor(true);
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta


