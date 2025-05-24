
#include "exceptions/defs.h"
#include "time/defs.h"
#include "sync/common.h"
#include "sync/event.h"
                              
#include "rvfc/Include/rvfcTime.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

Event::Event(const char *name, bool raised)
{
#if defined(_WIN32)
	_handle = CreateEvent(NULL, FALSE, raised ? TRUE : FALSE, (LPCTSTR) name);
	if (!_handle)
		THROW_EX(CreateErr);

#elif defined(__VXWORKS__)
	_id = semBCreate(SEM_Q_PRIORITY, raised ? SEM_FULL : SEM_EMPTY);
	if (_id == NULL)
		THROW_EX(CreateErr);

#elif defined(__linux__)
	_flag = false;

	pthread_condattr_t attr;
	pthread_condattr_init(&attr);

	if (pthread_cond_init(&_cond, &attr))
		THROW_EX(CreateErr);
	
	if (raised)
		raise();
#endif
}

//---------------------------------------------------------------------------------------------

Event::~Event()
{
#if defined(_WIN32)
	if (_handle)
		CloseHandle(_handle);

#elif defined(__VXWORKS__)
	if (_id != NULL)
		semDelete(_id);

#elif defined(__linux__)
	pthread_cond_destroy(&_cond);
#endif
}

//---------------------------------------------------------------------------------------------

bool Event::raise()
{
#if defined(_WIN32)
	return !!SetEvent(_handle);

#elif defined(__VXWORKS__)
	return semGive(_id) == OK;

#elif defined(__linux__)
	Take<Mutex> lock(_mutex);
	_flag = true;
	return !pthread_cond_signal(&_cond);
#endif
}

//---------------------------------------------------------------------------------------------

bool Event::broadcast()
{
#if defined(_WIN32)
	RV_NOT_IMPLEMENTED_ON_WIN32;
	return false;

#elif defined(__VXWORKS__)
	return semFlush(_id) == OK;

#elif defined(__linux__)
	Take<Mutex> lock(_mutex);
	_flag = true;
	return !pthread_cond_broadcast(&_cond);
#endif
}

//---------------------------------------------------------------------------------------------

bool Event::reset()
{
#if defined(_WIN32)
	return !!ResetEvent(_handle);

#elif defined(__VXWORKS__)
	RV_NOT_IMPLEMENTED_ON_WIN32;
	return false;

#elif defined(__linux__)
	Take<Mutex> lock(_mutex);
	_flag = false;
#endif
}

//---------------------------------------------------------------------------------------------

bool Event::wait()
{
#if defined(_WIN32)
	return WaitForSingleObject(_handle, INFINITE) == WAIT_OBJECT_0;

#elif defined(__VXWORKS__)
	return semTake(_id, WAIT_FOREVER) == OK;

#elif defined(__linux__)
	Take<Mutex> lock(_mutex);
	while (!_flag)
	{
		int rc = pthread_cond_wait(&_cond, _mutex.raw());
		if (rc)
			return false;
	}
	_flag = false;
	return true;
#endif
}

//---------------------------------------------------------------------------------------------

bool Event::on()
{
#if defined(_WIN32)
	return WaitForSingleObject(_handle, 0) == WAIT_OBJECT_0;

#elif defined(__VXWORKS__)
	return semTake(_id, NO_WAIT) == OK;

#elif defined(__linux__)
	if (!_mutex.ask())
		return false;

	struct timespec t;
	::clock_gettime(CLOCK_MONOTONIC, &t);

	while (!_flag)
	{
		int rc = pthread_cond_timedwait(&_cond, _mutex.raw(), &t);
		if (rc) // no difference between ETIMEDOUT and errors
			return false;
	}
	_flag = false;
	_mutex.give();

	return true;
#endif
}

//---------------------------------------------------------------------------------------------

bool Event::wait(const rvTicks &timeout)
{
#if defined(_WIN32)
	return WaitForSingleObject(_handle, timeout.milliseconds()) == WAIT_OBJECT_0;

#elif defined(__VXWORKS__)
	return semTake(_id, timeout.ticks()) == OK;

#elif defined(__linux__)
	Take<Mutex> lock(_mutex);

	struct timespec t = rvSystemTime(timeout).timespec();

	while (!_flag)
	{
		int rc = pthread_cond_timedwait(&_cond, _mutex.raw(), &t);
		if (rc) // no difference between ETIMEDOUT and errors
			return false;
	}
	_flag = false;
	return true;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
