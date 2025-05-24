
///////////////////////////////////////////////////////////////////////////////////////////////
// Module
///////////////////////////////////////////////////////////////////////////////////////////////

#include "module.h"

#include "magenta/files/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Instance
///////////////////////////////////////////////////////////////////////////////////////////////

File Module::file() const
{
	char name[256];
	DWORD rc = GetModuleFileName(_handle, name, sizeof(name));
	if (!rc)
		THROW_EX(Exception);
	return File(text(name));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Local Instance
///////////////////////////////////////////////////////////////////////////////////////////////

uintmax_t Instance::messageLoop()
{
	MSG msg;
	while (GetMessage(&msg, (HWND) NULL, 0, 0))
	{
		TranslateMessage(&msg);
		try
		{
			DispatchMessage(&msg);
		}
		catch (...)  {}
	}  
	return msg.wParam; 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// DynamicLibraryInstance
///////////////////////////////////////////////////////////////////////////////////////////////

DynamicLibrary::DynamicLibrary(const File &file, bool checkExt)
{
	text ext = file.path().basename().ext().tolower();
	if (checkExt && ext != ".dll" && ext != ".ocx" && ext != ".exe")
		THROW_EX(Exception)("unknown file extension");

	_handle = LoadLibrary(+file.path());
	if (_handle == NULL)
		THROW_EX(Exception)("cannot load library");
}

//---------------------------------------------------------------------------------------------

DynamicLibrary::~DynamicLibrary()
{
	FreeLibrary(_handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////

DynamicLibrary::Function DynamicLibrary::function(const string &name)
{
	return (Function) GetProcAddress(_handle, name.c_str());
}

//---------------------------------------------------------------------------------------------

DynamicLibrary::Function DynamicLibrary::operator[](const string &name)
{
	Function f = function(name);
	if (!f)
		THROW_EX(Exception)("cannot find function");
	return f;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
