
///////////////////////////////////////////////////////////////////////////////////////////////
// Device Contexts
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_graphics_win32_device_context_
#define _magenta_graphics_win32_device_context_

//#include "oogl/engine.h"

#include "magenta/geometry/defs.h"
#include "magenta/types/defs.h"
#include "magenta/ui/win32/classes.h"

#include "magenta/graphics/win32/general.h"
#include "magenta/graphics/win32/classes.h"

namespace magenta
{

using namespace UI;

///////////////////////////////////////////////////////////////////////////////////////////////

class IDeviceContext
{
protected:
	typedef HDC Handle;

public:
	virtual Handle handle() const = 0;

public:
	operator Handle() const { return handle(); }
	void draw(IShape &shape);
	void fill(IShape &shape, const Brush &brush);

	void bitblt(const IConstRect &srcRect, IDeviceContext &destCtx, 
		const IConstPoint &destPoint, DWORD op);

	void IDeviceContext::copyDIB(const DIBitmap &dib, const IConstRect &dest, 
		const IConstPoint &source = Point(0, 0));

	class Error {};
};

//---------------------------------------------------------------------------------------------

class DeviceContext : public IDeviceContext
{
protected:
	HDC _handle;

public:
	Handle handle() const { return _handle; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class WindowContext : public DeviceContext
{
protected:
	const IWindow &window;

public:
	WindowContext(const IWindow &window) : window(window) {}
};

//---------------------------------------------------------------------------------------------

class DrawSession : public WindowContext
{
public:
	DrawSession(const IWindow &window);
	~DrawSession();
};

//---------------------------------------------------------------------------------------------

class PaintSession : public WindowContext
{
	PAINTSTRUCT info;

public:
	PaintSession(const IWindow &window);
	~PaintSession();
};

//---------------------------------------------------------------------------------------------

class PrintSession : public DeviceContext
{
public:
	PrintSession(const IString &driver_name, const IString &device_name);
	~PrintSession();
};

//---------------------------------------------------------------------------------------------

#ifdef MAGENTA_OPENGL

class GLDrawSession : public oogl_4::IDisplayHost
{
	IDeviceContext *device_context;
	HGLRC _handle;

public:
	GLDrawSession(IDeviceContext &deviceContext);
	~GLDrawSession();

	void select(IDeviceContext &context)
	{
		device_context = &context;
		wglMakeCurrent(context, _handle);
	}

	operator HGLRC() const { return _handle; }

	void swapBuffers();
	void updateDisplay() { swapBuffers(); }

	class Error : public IDeviceContext::Error {};
};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif
