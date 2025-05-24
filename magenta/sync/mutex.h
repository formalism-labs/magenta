
#ifndef _magenta_sync_mutex_
#define _magenta_sync_mutex_

#include "magenta/time/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Mutex
{
	rvSemaphore _semaphore;

public:
	Mutex(const char *name = "", bool owner = false);

	void take();
	bool ask(); // timeout == 0
	bool ask(const Ticks &ticks);
	bool ask(const Milliseconds &time);
	void give();

#if defined(__linux__)
	pthread_mutex_t *raw();
#endif

private:
	bool tryAsk(int ticks);

	CLASS_EXCEPTION_DEF("magenta::Mutex");
	CLASS_SUB_EXCEPTION_DEF(CreateErr, "create error");
	CLASS_SUB_EXCEPTION_DEF(TakeErr, "take error");
	CLASS_SUB_EXCEPTION_DEF(GiveErr, "give error");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_sync_mutex_
