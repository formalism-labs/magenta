
///////////////////////////////////////////////////////////////////////////////////////////////
// Thread
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_thread_
#define _magenta_system_thread_

#include "magenta/system/classes.h"

#include "magenta/system/wait-object.h"

#include "magenta/types/defs.h"

#ifndef _WINDOWS_
#include <windows.h>
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Thread
///////////////////////////////////////////////////////////////////////////////////////////////

class Thread : public WaitObject
{
protected:
	HANDLE _handle;
	DWORD _id;

public:
	DWORD id() const { return _id;}
	HANDLE handle() const { return _handle;}

private:
	static unsigned long _stdcall _main(void *thread);

protected:
	virtual int main() { return 0; }

public:
	Thread();
	Thread(HANDLE handle, DWORD id = 0) : _handle(handle), _id(id) {}
	~Thread();

	void start();
	void stop(int exitCode);

	void suspend();
	void resume();

	bool isAlive() const;

private:
	bool termination_req;

public:
	void requestTermination();
	bool isRequestedTermination() const { return termination_req; }

public:
	class Error {};
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_thread_
