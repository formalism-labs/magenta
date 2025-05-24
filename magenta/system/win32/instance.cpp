
///////////////////////////////////////////////////////////////////////////////////////////////
// Instance
///////////////////////////////////////////////////////////////////////////////////////////////

#include "instance.h"

#include "magenta/files/defs.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// Instance
///////////////////////////////////////////////////////////////////////////////////////////////

File Instance::file() const
{
	char name[256];
	DWORD rc = GetModuleFileName(_handle, name, sizeof(name));
	if (!rc)
		THROW_EX(Exception);
	return Path(name);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Local Instance
///////////////////////////////////////////////////////////////////////////////////////////////

int LocalInstance::messageLoop()
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

DynamicLibraryInstance::DynamicLibraryInstance(const File &file, bool checkExt)
{
	string ext = file.basename().ext().tolower();
	if (checkExt && ext != ".dll" && ext != ".ocx" && ext != ".exe")
		THROW_EX(Exception)("unknown file extension");

	_handle = LoadLibrary(+file.path());
	if (_handle == NULL)
		THROW_EX(Exception)("cannot load library");
}

//---------------------------------------------------------------------------------------------

DynamicLibraryInstance::~DynamicLibraryInstance()
{
	FreeLibrary(_handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////

DynamicLibraryInstance::Function DynamicLibraryInstance::function(const string &name)
{
	return (Function) GetProcAddress(_handle, name.c_str());
}

//---------------------------------------------------------------------------------------------

DynamicLibraryInstance::Function DynamicLibraryInstance::operator[](const string &name)
{
	Function f = function(name);
	if (!f)
		THROW_EX(Exception)("cannot find function");
	return f;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
