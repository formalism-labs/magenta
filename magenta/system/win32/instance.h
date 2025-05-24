
///////////////////////////////////////////////////////////////////////////////////////////////
// Instance
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_win32_instance_
#define _magenta_system_win32_instance_

#include "magenta/files/classes.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"

#include <string>

#ifndef _WINDOWS_
#include <windows.h>
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////
// Instance
///////////////////////////////////////////////////////////////////////////////////////////////

class Instance
{
protected:
	HINSTANCE _handle;

	Instance() : _handle(0) {}

public:
	Instance(HINSTANCE handle) : _handle(handle) {}

	operator HINSTANCE() const { return _handle; }

	File file() const;

	CLASS_EXCEPTION_DEF("magenta::Instance");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class LocalInstance : public Instance
{
public:
	LocalInstance(HINSTANCE handle = 0, LPSTR lpCmdLine = 0, int nCmdShow = 0) :
		Instance(handle) {}

	int messageLoop();
};

///////////////////////////////////////////////////////////////////////////////////////////////

class DynamicLibraryInstance : public Instance
{
public:
	DynamicLibraryInstance(const File &file, bool checkExt = true);
	~DynamicLibraryInstance();

	typedef int (__stdcall *Function)(...);

	Function function(const string &name);
	Function operator[](const string &name);

	CLASS_EXCEPTION_DEF("magenta::DynamicLibraryInstance");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_win32_instance_
