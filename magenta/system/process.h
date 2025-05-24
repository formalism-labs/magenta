
///////////////////////////////////////////////////////////////////////////////////////////////
// Process
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_process_
#define _magenta_system_process_

#include "magenta/system/classes.h"

#include "magenta/lang/defs.h"
#include "magenta/types/defs.h"

#include <string>

#ifdef _WI32
#include "magenta/system/windows.h"
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////
// Process
///////////////////////////////////////////////////////////////////////////////////////////////

class Process
{
public:
	enum__(Attributes)
		NONE,
		Minimized = 1
	__enum(Attributes);

	class Id
	{
		friend Process;

		DWORD _id;

	protected:
		Id &operator=(DWORD id) { _id = id; return *this; }

	public:
		Id(DWORD id) : _id(id) {}
		operator DWORD() const { return _id; }
	};

private:
	HANDLE _handle;
	DWORD _id;
	Thread *main_thread;
	bool current_process;

public:
	
public:
	class Error {};

protected:
	Process(HANDLE handle, Id id = 0) : _handle(handle), _id(id), 
		main_thread(0), current_process(false) {}

public:
	Process();
	Process(const Id &id);
	Process(const string &command, Attributes attributes = Attributes::NONE);
	~Process();

	void start();
	void stop(unsigned int exitCode);
	DWORD run();

	operator HANDLE() const { return _handle; }

	void wait(long waitInterval = INFINITE);

	DWORD result() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_process_
