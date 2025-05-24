
#include "exceptions/defs.h"

#include "rvfc/Include/rvfc.h"
#include "rvfc/Include/rvfcLog.h"
#include "rvfc/Include/rvfcSys.h"

#include "sync/thread.h"

#if defined(__VXWORKS__)
#include <taskLib.h>
#elif defined(__linux__)
#include <signal.h>
#include <sys/syscall.h>
#endif

#include <stdio.h>

#define DISABLE_EXCEPTIONS

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

Thread::Thread() : _id(0)
#if defined(_WIN32)
	, _handle(0)
#endif
{}

//---------------------------------------------------------------------------------------------

Thread::Thread(Id id) : _id(id)
{
#if defined(__VXWORKS__)
	if (taskIdVerify(_id) == ERROR)
		THROW_OS(AccessErr)("taskIdVerify(%d) failed", _id);

#elif defined(_WIN32)
	_handle = OpenThread(THREAD_ALL_ACCESS, FALSE, _id);
	if (_handle == NULL)
		THROW_OS(AccessErr)("OpenThread(%d) failed", id);;

#elif defined(__linux__)
	if (pthread_kill(_id, 0) == ESRCH)
		THROW_EX(AccessErr)("pthread_kill(id) failed", id);;
#endif
}

//---------------------------------------------------------------------------------------------

Thread::~Thread()
{
#if defined(_WIN32)
	if (_handle != GetCurrentThread())
		CloseHandle(_handle);
#endif
}

//---------------------------------------------------------------------------------------------

Thread::Priority Thread::priority() const
{
	int pri;

#if defined(__VXWORKS__)
	if (taskPriorityGet(_id, &pri) == ERROR)
		THROW_EX(AccessErr)("cannot read priority");

#elif defined(_WIN32)
	pri = GetThreadPriority(_handle);
	if (pri == THREAD_PRIORITY_ERROR_RETURN)
		THROW_OS(AccessErr)("cannot read priority");

#elif defined(__linux__)
	int policy;
	struct sched_param sched;
	int rc = pthread_getschedparam(_id, &policy, &sched);
	pri = sched.sched_priority;
	if (rc)
		THROW_EX(AccessErr)("cannot read priority");
#endif

	return Priority(pri);
}

//---------------------------------------------------------------------------------------------

Thread::Priority Thread::setPriority(Priority pri)
{
	Priority oldpri = priority();

#if defined(__VXWORKS__)
	if (taskPrioritySet(_id, pri) == ERROR)
		THROW_OS(AccessErr)("cannot set priority");

#elif defined(_WIN32)
	if (!SetThreadPriority(_handle, pri))
		THROW_OS(AccessErr)("cannot set priority");

#elif defined(__linux__)
	struct sched_param sched;
	sched.sched_priority = pri;
	int rc = pthread_setschedparam(_id, SCHED_FIFO, &sched);
	if (rc)
		THROW_EX(AccessErr)("cannot set priority [err=%d]", rc);
#endif

	return oldpri;
}

//---------------------------------------------------------------------------------------------

void Thread::suspend()
{
#if defined(__VXWORKS__)
	if (taskSuspend(_id) == ERROR)
		THROW_OS(SuspendErr)("thread %d", _id);

#elif defined(_WIN32)
	if (SuspendThread(_handle) == -1)
		THROW_OS(SuspendErr)("thread %d", _id);

#elif defined(__linux__)
	THROW_EX(SuspendErr)("thread suspend/resume is not implemented on Linux");
#endif
}

//---------------------------------------------------------------------------------------------

void Thread::resume()
{
#if defined(__VXWORKS__)
	if (taskResume(_id) == ERROR)
		THROW_OS(SuspendErr)("thread %d", _id);

#elif defined(_WIN32)
	if (ResumeThread(_handle) == -1)
		THROW_OS(SuspendErr)("thread %d", _id);

#elif defined(__linux__)
	THROW_EX(ResumeErr)("thread suspend/resume is not implemented on Linux");
#endif
}

//---------------------------------------------------------------------------------------------

const Thread &Thread::join() const
{
#if defined(_WIN32)
	if (WaitForSingleObject(_handle, INFINITE) != WAIT_OBJECT_0)
		THROW_OS(JoinErr)("thread %d", _id);

#elif defined(__linux__)
	void *value;
	int rc = pthread_join(_id, &value);
	if (rc != 0)
		THROW_EX(JoinErr)("thread %d [rc=%d]", _id, rc);

#elif defined(__VXWORKS__)
	THROW_EX(Unimplemented)("Thread::join");
#endif

	
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////

NewThread::NewThread(const char *name, int priority, int stackSize, int options) : 
	_name(!*name ? 0 : name), _init_priority(priority), _init_options(options), 
	_stack_size(stackSize), _running(false)
{
	bool rc = _thread.create(const_cast<char*>(_name), _init_priority, _init_options, _stack_size, 
		(RV_FUNCPTR) _main, (int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (!rc)
		THROW_EX(CreateErr)("thread %s", name ? name : "[]");
	_id = _thread.element().rawId();
#ifdef _WIN32
	_handle = _thread.element().handle();
#endif
}

//---------------------------------------------------------------------------------------------

NewThread::~NewThread()
{
}

//---------------------------------------------------------------------------------------------

void NewThread::run()
{
	if (!_thread.activate())
		THROW_EX(ResumeErr)("starting thread failed");
}

//---------------------------------------------------------------------------------------------

int NewThread::_main(int *param)
{
#if !defined(RVFC_TARGET_OS_VXWORKS_5)
	int rc;
	NewThread *self;
	try
	{
		self = (NewThread *) param[0];
		self->_running = true;
		
#if defined(__linux__)
		self->_tid = (pid_t) syscall(SYS_gettid);
#endif

		rc = self->main();
	}
	catch (std::exception &x)
	{
		const char *name = self && self->_name ? self->_name : "";
		L0G(assert, "Thread %s: exception: %s\n", name, x.what());
		rc = -1;
	}
	catch (const char *x)
	{
		const char *name = self && self->_name ? self->_name : "";
		L0G(assert, "Thread %s: exception: %s\n", name, x);
		rc = -1;
	}
	catch (...)
	{
		const char *name = self && self->_name ? self->_name : "";
		L0G(assert, "Thread %s: exception\n", name);
		rc = -1;
	}

	self->_running = false;
	return rc;

#else // defined(RVFC_TARGET_OS_VXWORKS_5)
	NewThread *self = (NewThread *) param[0];
	self->_running = true;
	int rc = self->main();
	self->_running = false;
	return rc;
#endif
}

//---------------------------------------------------------------------------------------------

Thread::TID NewThread::tid() const
{
#if defined(__linux__)
	return _tid;
#else
	return id();
#endif
}

//---------------------------------------------------------------------------------------------

Thread::Priority NewThread::priority() const
{
	int n;
	if (!_thread.priority(&n))
		THROW_EX(AccessErr)("cannot read priority");

	return Priority(n);
}

//---------------------------------------------------------------------------------------------

Thread::Priority NewThread::setPriority(Priority pri)
{
	Priority oldpri = priority();
	if (!_thread.priority((int) pri))
		THROW_EX(AccessErr)("cannot set priority");

	return oldpri;
}

//---------------------------------------------------------------------------------------------

void NewThread::suspend()
{
	if (!_thread.suspend())
		THROW_EX(SuspendErr);
		
}

//---------------------------------------------------------------------------------------------

void NewThread::resume()
{
	if (!_thread.resume())
		THROW_EX(ResumeErr);
}

//---------------------------------------------------------------------------------------------

void NewThread::sleep(const Ticks &ticks)
{
	_thread.delay(ticks.value());
}

///////////////////////////////////////////////////////////////////////////////////////////////

ActiveThread::ActiveThread()
{
#if defined(__VXWORKS__)
	_id = taskIdSelf();

#elif defined(_WIN32)
	_id = GetCurrentThreadId();
	_handle = GetCurrentThread();

#elif defined(__linux__)
	_id = pthread_self();
#endif
}
	
//---------------------------------------------------------------------------------------------

void ActiveThread::sleep(const Ticks &ticks)
{
	taskDelay(ticks.value());
}

//---------------------------------------------------------------------------------------------

Thread::TID ActiveThread::tid() const
{
#if defined(__VXWORKS__) || defined(_WIN32)
	return id();

#elif defined(__linux__)
	return (pid_t) syscall(SYS_gettid);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // magenta
