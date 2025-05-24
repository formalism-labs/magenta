
#ifndef _magenta_sync_thread_
#define _magenta_sync_thread_

#include "magenta/exceptions/defs.h"
#include "magenta/time/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#define DEFAULT_THREAD_PRIORITY			THREAD_PRIORITY_NORMAL
#define DEFAULT_THREAD_STACK_SIZE		0 /* default */
#define DEFAULT_THREAD_CREATE_OPTIONS	0
#elif defined(__VXWORKS__)
#define DEFAULT_THREAD_PRIORITY			100
#define DEFAULT_THREAD_STACK_SIZE		20000
#define DEFAULT_THREAD_CREATE_OPTIONS	0
#elif defined(__linux__)
#define DEFAULT_THREAD_PRIORITY			0
#define DEFAULT_THREAD_STACK_SIZE		20000
#define DEFAULT_THREAD_CREATE_OPTIONS	0
#endif

//---------------------------------------------------------------------------------------------

class Thread
{
public:
#if defined(__VXWORKS__)
	typedef int Id;
	typedef int TID;
#elif defined(_WIN32)
	typedef DWORD Id;
	typedef DWORD TID;
#elif defined(__linux__)
	typedef pthread_t Id;
	typedef pid_t TID;
#endif

protected:
	Thread();

	Id _id;
#if defined(_WIN32)
	HANDLE _handle;
#endif

public:
	Thread(Id id);
	virtual ~Thread();

	Id id() const { return _id; }
	virtual TID tid() const = 0;

	typedef int Priority;

	virtual Priority priority() const;
	virtual Priority setPriority(Priority priority);
	
	virtual void suspend();
	virtual void resume();

public:
	const Thread &join() const;
//	const Thread &join(const Milliseconds &timeout) const;
	const Thread &operator<<(const Thread &thread) const { return join(); thread.join(); }

	CLASS_EXCEPTION_DEF("magenta::Thread");
	CLASS_SUB_EXCEPTION_DEF(AccessErr, "cannot access thread");
	CLASS_SUB_EXCEPTION_DEF(SuspendErr, "cannot suspend thread");
	CLASS_SUB_EXCEPTION_DEF(ResumeErr, "cannot resume thread");
	CLASS_SUB_EXCEPTION_DEF(JoinErr, "cannot join thread");
};

//---------------------------------------------------------------------------------------------

class ActiveThread : public Thread
{
public:
	ActiveThread();
	
	static void sleep(const Ticks &ticks);

	virtual TID tid() const;

	CLASS_EXCEPTION_DEF("magenta::ActiveThread");
	CLASS_SUB_EXCEPTION_DEF(SleepErr, "create error");
};

//---------------------------------------------------------------------------------------------

inline void sleep(const Milliseconds &interval)
{
	ActiveThread::sleep(interval);
}

//---------------------------------------------------------------------------------------------

class NewThread : public Thread
{
protected:
	rvThread _thread;
	const char *_name;
	int _init_priority, _init_options, _stack_size;
	bool _running;
#if defined(__linux__)
	TID _tid;
#endif

	virtual int main() { return 0; }
	
	static int _main(int *param);

public:
	NewThread(const char *name = "", int priority = DEFAULT_THREAD_PRIORITY, int stackSize = DEFAULT_THREAD_STACK_SIZE, 
		int options = DEFAULT_THREAD_CREATE_OPTIONS);

	virtual ~NewThread();

	TID tid() const;

	void run();

	Priority priority() const;
	Priority setPriority(Priority priority);

	void suspend();
	void resume();

	bool isRunning() const { return _running; }

	void sleep(const Ticks &ticks);

	CLASS_EXCEPTION_DEF("magenta::NewThread");
	CLASS_SUB_EXCEPTION_DEF(CreateErr, "create error");
	CLASS_SUB_EXCEPTION_DEF(SleepErr, "create error");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_sync_thread_
