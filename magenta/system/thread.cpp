
///////////////////////////////////////////////////////////////////////////////////////////////
// Thread
///////////////////////////////////////////////////////////////////////////////////////////////

#include "thread.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

Thread::Thread()
{
	termination_req = false;
	_handle = CreateThread(0, 0, _main, (void *) this, CREATE_SUSPENDED, &_id);
	if (_handle == NULL)
		throw Error();
}

//---------------------------------------------------------------------------------------------

Thread::~Thread()
{
	CloseHandle(_handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Thread::start()
{
	resume();
}

//---------------------------------------------------------------------------------------------

void Thread::stop(int exitCode)
{
	if (!isAlive())
		return;
	BOOL rc = TerminateThread(_handle, exitCode);
	if (!rc)
		throw Error();
}

//---------------------------------------------------------------------------------------------

void Thread::suspend()
{
	DWORD count = SuspendThread(_handle);
	if (count == -1)
		throw Error();
}

//---------------------------------------------------------------------------------------------

void Thread::resume()
{
	DWORD count = ResumeThread(_handle);
	if (count == -1)
		throw Error();
}

//---------------------------------------------------------------------------------------------

bool Thread::isAlive() const
{
	DWORD exitCode;
	if (!GetExitCodeThread(_handle, &exitCode))
		return false;

	return exitCode == STILL_ACTIVE;
}

///////////////////////////////////////////////////////////////////////////////////////////////

unsigned long _stdcall Thread::_main(void *thread)
{
	try
	{
		ExitThread(((Thread *) thread)->main());
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
