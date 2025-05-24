
///////////////////////////////////////////////////////////////////////////////////////////////
// Event
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_event_
#define _magenta_system_event_

#include "magenta/system/classes.h"

#include "magenta/system/wait-object.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"

#include <string>

#ifndef _WINDOWS_
#include <windows.h>
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class Event : public WaitObject
{
private:
	HANDLE _handle;
public:
	HANDLE handle() const { return _handle; }
	operator HANDLE() const { return _handle; }

public:
	CLASS_EXCEPTION_DEF("magenta::Event");

protected:
	void ctor(const char *name, bool manualReset, bool state);

	Event(HANDLE handle) : _handle(handle) {}
	
public:
	Event(bool manualReset = false, bool state = false);
	Event(const string &name, bool manualReset = false, bool state = false);
	virtual ~Event();

public:
	void reset();
	void set();
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_event_
