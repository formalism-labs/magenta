
#include "exceptions/defs.h"
#include "time/defs.h"
#include "sync/common.h"
#include "sync/mutex.h"         

#if defined(__linux__)
namespace rvfc { pthread_mutex_t *rvSemaphore_getMutex(rvSemaphore &sem); }
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

Mutex::Mutex(const char *name, bool owner)
{
	int rc = _semaphore.create(RV_SEM_MUTEX, (INT32) (RV_SEM_Q_PRIORITY | RV_SEM_INVERSION_SAFE), 
		owner ? 1 : 0,1, const_cast<char *>(name));
	if (!rc)
		THROW_EX(CreateErr);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Mutex::take()
{
	if (!_semaphore.take(WAIT_FOREVER))
		THROW_EX(TakeErr);
}

//---------------------------------------------------------------------------------------------

bool Mutex::ask()
{
	return _semaphore.take(NO_WAIT);
}

//---------------------------------------------------------------------------------------------

bool Mutex::ask(const Ticks &ticks)
{
	return _semaphore.take(ticks.value());
}

//---------------------------------------------------------------------------------------------

bool Mutex::ask(const Milliseconds &time)
{
	return _semaphore.take(Ticks(time).value());
}

//---------------------------------------------------------------------------------------------

void Mutex::give()
{
	if (!_semaphore.give())
		THROW_EX(GiveErr);
}

//---------------------------------------------------------------------------------------------

#if defined(__linux__)

pthread_mutex_t *Mutex::raw()
{
	return rvfc::rvSemaphore_getMutex(_semaphore);
}

#endif // defined(__linux__)

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
