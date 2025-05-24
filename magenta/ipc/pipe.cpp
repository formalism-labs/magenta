
///////////////////////////////////////////////////////////////////////////////////////////////
// Pipe
///////////////////////////////////////////////////////////////////////////////////////////////

#include "pipe.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

SecurityAttributes Pipe::sa;

//---------------------------------------------------------------------------------------------

Pipe::Pipe(const char *tag) : rd(INVALID_HANDLE_VALUE), wr(INVALID_HANDLE_VALUE), 
	tag(tag), buffer(0), buffer_size(0)
{
//		thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _main, (LPVOID) this, CREATE_SUSPENDED, NULL);
}

//---------------------------------------------------------------------------------------------

Pipe::~Pipe()
{
	if (rd != INVALID_HANDLE_VALUE)
		CloseHandle(rd);
	if (wr != INVALID_HANDLE_VALUE)
		CloseHandle(wr);
	if (buffer)
		delete[] buffer;
//		CloseHandle(thread);
}

//---------------------------------------------------------------------------------------------

void Pipe::closeOtherSideHandle()
{
	CloseHandle(otherSideHandle());
	otherSideHandle() = INVALID_HANDLE_VALUE;
}

//---------------------------------------------------------------------------------------------

bool Pipe::pump(bool wait)
{
	HANDLE source = sourceHandle();

	DWORD avail = 0;
	if (! PeekNamedPipe(source, NULL, 0, NULL, &avail, NULL) || !avail && !wait)
		return false;

	char local_buf[1024], *p;
	int p_size;
	if (avail > sizeof(local_buf))
	{
		if (avail > (DWORD) buffer_size)
		{
			if (buffer_size)
				delete[] buffer;
			buffer = new char[avail];
			buffer_size = avail;
		}
		p = buffer;
		p_size = buffer_size;
	}
	else
	{
		p = local_buf;
		p_size = sizeof(local_buf);
	}

	DWORD n;
	if (! ReadFile(source, p, p_size, &n, NULL) || !n)
	{
		if (GetLastError() == ERROR_BROKEN_PIPE)
			return false;
	}

	HANDLE dest = destHandle();
	DWORD written;
	do
	{
		if (!WriteFile(dest, p, n, &written, NULL) || written == 0)
			break;
		n -= written;
		p += written;
	}
	while (n > 0);
	
	FlushFileBuffers(dest);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ParentToChildPipe::ParentToChildPipe(HANDLE stdRd, const char *tag) : Pipe(tag), std_rd(stdRd)
{
	HANDLE proc = GetCurrentProcess();
	HANDLE tmp;
	if (! CreatePipe(&rd, &tmp, sa, 0))
		throw std::runtime_error("cannot create pipe");
	if (! DuplicateHandle(proc, tmp, proc, &wr, 0, FALSE, DUPLICATE_SAME_ACCESS))
		throw std::runtime_error("cannot dup handle");
	CloseHandle(tmp);
}

///////////////////////////////////////////////////////////////////////////////////////////////

ChildToParentPipe::ChildToParentPipe(HANDLE stdWr, const char *tag) : Pipe(tag), std_wr(stdWr)
{
	HANDLE proc = GetCurrentProcess();
	HANDLE tmp;
	if (! CreatePipe(&tmp, &wr, sa, 0))
		throw std::runtime_error("cannot create pipe");
	if (! DuplicateHandle(proc, tmp, proc, &rd, 0, FALSE, DUPLICATE_SAME_ACCESS))
		throw std::runtime_error("cannot dup handle");
	CloseHandle(tmp);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
