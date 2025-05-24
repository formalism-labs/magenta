
#ifndef _magenta_sync_event_
#define _magenta_sync_event_

#include "rvfc/Include/rvfcSemaphore.h"

#include "magenta/sync/mutex.h"
#include "magenta/time/time-units.h"

#if defined(_WIN32)
#include <windows.h>

#elif defined(__linux__)
#include <pthread.h>
#include <semaphore.h>
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Event
{
#if defined(__linux__)
	Mutex _mutex;
	pthread_cond_t _cond;
	bool _flag;

#elif defined(_WIN32)
	HANDLE _handle;

#elif defined(__VXWORKS__)
	SEM_ID _id;
#endif

public:
	Event(const char *name = 0, bool raised = false);
	~Event();

	bool raise();
	bool broadcast();
	bool reset();

	bool wait();
	bool wait(const rvTicks &timeout);

	bool on(); // immediate
	bool off() { return !on(); }

#if defined(__linux__)
	bool flag() const { return _flag; }
#endif

	CLASS_EXCEPTION_DEF("magenta::Event");
	CLASS_SUB_EXCEPTION_DEF(CreateErr, "create error");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_sync_event_
