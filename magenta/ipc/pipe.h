
///////////////////////////////////////////////////////////////////////////////////////////////
// Pipe
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_ipc_pipe_
#define _magenta_ipc_pipe_

#include <stdexcept>

#ifdef _WIN32
#include "magenta/system/windows.h"
#include "magenta/security/win32/security-attributes.h"
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Pipe
{
	typedef Pipe This;

protected:
	static SecurityAttributes sa;

	HANDLE rd, wr;
//	HANDLE thread;
	const char *tag;
	char *buffer;
	int buffer_size;

public:
	Pipe(const char *tag = "");
	virtual ~Pipe();

	virtual HANDLE handle() const = 0;

	HANDLE otherSideHandle() const { return const_cast<Pipe*>(this)->otherSideHandle(); }
	virtual HANDLE &otherSideHandle() = 0;

	virtual HANDLE sourceHandle() const = 0;
	virtual HANDLE destHandle() const = 0;
	
	void closeOtherSideHandle();

//	void listen()
//	{
//		closeOtherSideHandle();
//		ResumeThread(thread);
//	}

//	void collect()
//	{
//		pump();
//	}

protected:
//	static int _main(This *self)
//	{
//		return self->main();
//	}
//	
//	virtual int main()
//	{
//		for (;;)
//			if (! pump(true))
//				break;
//
//		return 0;
//	}
	
public:
	bool pump(bool wait = false);
};

//---------------------------------------------------------------------------------------------

class ParentToChildPipe : public Pipe
{
	HANDLE std_rd;

public:
	ParentToChildPipe(HANDLE stdRd, const char *tag = "");
	
	HANDLE handle() const { return wr; }
	HANDLE &otherSideHandle() { return rd; }

	HANDLE sourceHandle() const { return std_rd; }
	HANDLE destHandle() const { return wr; }
};

//---------------------------------------------------------------------------------------------

class ChildToParentPipe : public Pipe
{
	HANDLE std_wr;

public:
	ChildToParentPipe(HANDLE stdWr, const char *tag = "");

	HANDLE handle() const { return rd; }
	HANDLE &otherSideHandle() { return wr; }
	
	HANDLE sourceHandle() const { return rd; }
	HANDLE destHandle() const { return std_wr; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_ipc_pipe_
