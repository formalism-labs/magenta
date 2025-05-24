
///////////////////////////////////////////////////////////////////////////////////////////////
// Job
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_job_
#define _magenta_system_job_

#include <mutex>

#include "magenta/system/classes.h"

#ifdef _WI32
#include "magenta/system/windows.h"
#endif

#include "magenta/text/defs.h"
#include "magenta/ipc/defs.h"
#include "magenta/types/defs.h"

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class Job
{
	HANDLE _job;
	PROCESS_INFORMATION _pi;

	ParentToChildPipe _in;
	ChildToParentPipe _out, _err;

	static std::mutex _mutex;
	static RefVector<Job> _jobs;
	static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);

public:
	Job(const text &cmd);
	~Job();

	DWORD run();
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_job_
