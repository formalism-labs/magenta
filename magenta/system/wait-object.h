
///////////////////////////////////////////////////////////////////////////////////////////////
// Wait Objects
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_wait_object_
#define _magenta_system_wait_object_

#include "magenta/system/classes.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"
#include "magenta/time/defs.h"

#include <string>

#ifdef _WIN32
#include "magenta/system/windows.h"
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class WaitObject
{
public:
	CLASS_EXCEPTION_DEF("magenta::WaitObject");

public:
//	WaitObject();
//	virtual ~WaitObject();

	virtual HANDLE handle() const = 0;
	operator HANDLE() const { return handle(); }

protected:
	bool _wait(DWORD ms);

public:
	bool wait();
	bool wait(Milliseconds interval);

	virtual void onReady() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class WaitObjects
{
public:
	WaitObjects(int n, WaitObject *objects[]);

	void waitAll();
	WaitObject *waitAny();
};

///////////////////////////////////////////////////////////////////////////////////////////////

class WaitFor
{
	int _n;
	Vector<HANDLE> _handles;
	Vector<int> _indices;

public:
	WaitFor(HANDLE *objs, int num);
	
	int wait(bool forAll = FALSE, DWORD timeout = INFINITE);

	bool done() const { return !_n; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_wait_object_
