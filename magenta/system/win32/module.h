
///////////////////////////////////////////////////////////////////////////////////////////////
// Instance
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_win32_instance_
#define _magenta_system_win32_instance_

#include "magenta/files/classes.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"

#include <string>

#ifdef _WIN32
#include "magenta/system/windows.h" 
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////
// Instance
///////////////////////////////////////////////////////////////////////////////////////////////

class Module
{
protected:
	HMODULE _handle;

	Module() : _handle(0) {}

public:
	explicit Module(HMODULE handle) : _handle(handle) {}

	operator HMODULE() const { return _handle; }

	File file() const;

	CLASS_EXCEPTION_DEF("magenta::Module");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CurrentModule : public Module
{
public:
	CurrentModule() : Module(NULL) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Instance : public Module
{
public:
	Instance(HINSTANCE handle, LPSTR lpCmdLine, int nCmdShow) :
		Module(HMODULE(handle)) {}

	uintmax_t messageLoop();

	operator HINSTANCE() const { return HINSTANCE(_handle); }

	CLASS_EXCEPTION_DEF("magenta::Module");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class DynamicLibrary : public Module
{
public:
	DynamicLibrary(const File &file, bool checkExt = true);
	~DynamicLibrary();

	typedef int (__stdcall *Function)(...);

	Function function(const string &name);
	Function operator[](const string &name);

	CLASS_EXCEPTION_DEF("magenta::DynamicLibrary");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_win32_instance_
