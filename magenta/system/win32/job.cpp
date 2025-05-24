
#include "job.h"

namespace magenta
{

/////////////////////////////////////////////////////////////////////////////////////////////// 

std::mutex Job::_mutex;
RefVector<Job> Job::_jobs;

//---------------------------------------------------------------------------------------------

Job::Job(const text &cmd) :
	_in(GetStdHandle(STD_INPUT_HANDLE)),
	_out(GetStdHandle(STD_OUTPUT_HANDLE)),
	_err(GetStdHandle(STD_ERROR_HANDLE))
{
	_job = CreateJobObject(NULL, NULL);
	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput  = _in.otherSideHandle();
	si.hStdOutput = _out.otherSideHandle();
	si.hStdError  = _err.otherSideHandle();

	ZeroMemory(&_pi, sizeof(_pi));

	BOOL rc = CreateProcess(NULL, (LPSTR) +cmd, NULL, NULL, TRUE, 
		CREATE_SUSPENDED, NULL, NULL, &si, &_pi);
	if (!rc)
		throw std::runtime_error("cannot start job");
	
	AssignProcessToJobObject(_job, _pi.hProcess);

	_in.closeOtherSideHandle();
	_out.closeOtherSideHandle();
	_err.closeOtherSideHandle();
	
	std::lock_guard<std::mutex> guard(_mutex);
	_jobs << this;
}

//---------------------------------------------------------------------------------------------

Job::~Job()
{
	std::lock_guard<std::mutex> guard(_mutex);
	foreach(j, _jobs.begin())
		if (*j == this)
		{
			j.remove();
			break;
		}

	CloseHandle(_pi.hProcess);
	CloseHandle(_pi.hThread);
	CloseHandle(_job);
}

//---------------------------------------------------------------------------------------------

DWORD Job::run()
{
	ResumeThread(_pi.hThread);

	for (;;)
	{
		bool b = false;
		b = _in.pump() || b;
		b = _out.pump() || b;
		b = _err.pump() || b;
		DWORD rc = WaitForSingleObject(_pi.hProcess, b ? 0 : 10);
		if (rc == WAIT_OBJECT_0)
			break;
	}

	while (_out.pump())
		;
	while (_err.pump())
		;

	DWORD exit_code;
	GetExitCodeProcess(_pi.hProcess, &exit_code);
	return exit_code;
}

//---------------------------------------------------------------------------------------------

BOOL WINAPI Job::ConsoleCtrlHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		SetConsoleCtrlHandler(ConsoleCtrlHandler, FALSE);
		{
			std::lock_guard<std::mutex> guard(_mutex);
			foreach(j, _jobs.begin())
				TerminateJobObject(*j, 100);
		}
		return TRUE;

	default:
		break;
	}
	
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
