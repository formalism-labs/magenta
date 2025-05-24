
///////////////////////////////////////////////////////////////////////////////////////////////
// Event
///////////////////////////////////////////////////////////////////////////////////////////////

#include "event.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

void Event::ctor(const char *name, bool manualReset, bool state)
{
	_handle = CreateEvent(0, manualReset, state, name);
	if (_handle == NULL)
		THROW_OS(Exception)("event creation error");
}

//---------------------------------------------------------------------------------------------

Event::Event(bool manualReset, bool state)
{
	ctor(0, manualReset, state);
}

//---------------------------------------------------------------------------------------------

Event::Event(const string &name, bool manualReset, bool state)
{
	ctor(name.empty() ? 0 : name.c_str(), manualReset, state);
}

//---------------------------------------------------------------------------------------------

Event::~Event()
{
	CloseHandle(_handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Event::reset()
{
	BOOL rc = ResetEvent(_handle);
	if (!rc)
		THROW_OS(Exception)("event reset error");
}

//---------------------------------------------------------------------------------------------

void Event::set()
{
	BOOL rc = SetEvent(_handle);
	if (rc)
		THROW_OS(Exception)("event set error");
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
