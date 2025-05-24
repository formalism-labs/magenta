
///////////////////////////////////////////////////////////////////////////////////////////////
// Mutex
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_mutex_
#define _magenta_system_mutex_

#include "magenta/system/classes.h"

#include "magenta/system/wait-object.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"

#include <string>

#ifndef _WI32
#include "magenta/system/windows.h"
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class Mutex : public WaitObject
{
private:
	HANDLE _handle;
public:
	HANDLE handle() const { return _handle; }
	operator HANDLE() const { return _handle; }

public:
	CLASS_EXCEPTION_DEF("magenta::Mutex");

protected:
	Mutex(HANDLE handle) : _handle(handle) {}
public:
	Mutex(const string &name = "", bool initialOwner = false);
	virtual ~Mutex();

public:
	void release();
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_mutex_
