
///////////////////////////////////////////////////////////////////////////////////////////////
// Process
///////////////////////////////////////////////////////////////////////////////////////////////

#include "process.h"
#include "thread.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

Process::Process()
{
	_handle = GetCurrentProcess();
	_id = GetCurrentProcessId();
	main_thread = 0;
	current_process = true;
}

//---------------------------------------------------------------------------------------------

Process::Process(const Id &id)
{
	_id = id;
	_handle = OpenProcess(PROCESS_ALL_ACCESS, true, _id);
	if (!_handle)
		throw Error();
	main_thread = 0;
	current_process = false;
}

//---------------------------------------------------------------------------------------------

Process::Process(const string &command, Attributes attributes)
{
	STARTUPINFO startup_info;
	PROCESS_INFORMATION proc_info;
	

	startup_info.cb = sizeof(STARTUPINFO);
	startup_info.lpDesktop = 0;
	startup_info.lpTitle = 0;
	startup_info.dwFlags = 0;
	startup_info.wShowWindow = SW_SHOWNORMAL;
	startup_info.lpReserved = 0;
	startup_info.cbReserved2 = 0;
	startup_info.lpReserved2 = 0;

	if (attributes & Attributes::Minimized)
	{
		startup_info.dwFlags |= STARTF_USESHOWWINDOW;
		startup_info.wShowWindow = SW_SHOWMINIMIZED;
	}

	char _command[1024];
	strncpy(_command, command.c_str(), sizeof(_command));
	BOOL rc = CreateProcess(0, _command, 
		0, 0, false, CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, 0, 0, 
		&startup_info, &proc_info);
	if (!rc)
		throw Error();

	_handle = proc_info.hProcess;
	_id = proc_info.dwProcessId;
	main_thread = new Thread(proc_info.hThread, proc_info.dwThreadId);
	current_process = false;
}

//---------------------------------------------------------------------------------------------

Process::~Process()
{
	delete main_thread; 
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Process::start()
{
	if (!main_thread)
		return;
	main_thread->start();
}

//---------------------------------------------------------------------------------------------

void Process::stop(unsigned int exitCode)
{
	if (current_process)
		ExitProcess(exitCode);
	else
		TerminateProcess(_handle, exitCode);
}

//---------------------------------------------------------------------------------------------

DWORD Process::run()
{
	start();
	wait();
	return result();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Process::wait(long WaitInterval)
{
	WaitForSingleObject(_handle, WaitInterval);
}

//---------------------------------------------------------------------------------------------

DWORD Process::result() const
{
	DWORD n;
	GetExitCodeProcess(_handle, &n);
	return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
