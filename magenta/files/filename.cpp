
#include "filename.h"
#include "directory.h"

#include "exceptions/defs.h"
#include "types/defs.h"

namespace magenta
{

using std::vector;

///////////////////////////////////////////////////////////////////////////////////////////////
// Filename
///////////////////////////////////////////////////////////////////////////////////////////////

void Filename::assign(const Filename &filename)
{
	_name = filename;
//	refresh();
}

//---------------------------------------------------------------------------------------------

Path Filename::fullPath() const
{
#ifdef _WIN32
	char path[MAX_PATH];
	LPTSTR file_part;
	DWORD rc = GetFullPathName(+_name, sizeof(path), path, &file_part);
	return path;
#else
	UNIMPLEMENTED;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// UniqueFilename
///////////////////////////////////////////////////////////////////////////////////////////////

void UniqueFilename::ctor(const text &type, const Directory &dir)
{
	char name[MAX_PATH];
#ifdef _WIN32
	int rc = GetTempFileName(+dir.path(), "tmp", 0, name);
#else
	UNIMPLEMENTED;
#endif
	if (!rc)
		THROW_EX(Exception)("cannot generate temp filename");

	auto t = (type[0] == "." ? "" : ".") + type;
	assign(Filename(text(name) + t));
}

//---------------------------------------------------------------------------------------------

UniqueFilename::UniqueFilename(const text &type)
{
	ctor(type, TemporaryDirectory());
}

//---------------------------------------------------------------------------------------------

UniqueFilename::UniqueFilename(const text &type, const Directory &dir)
{
	ctor(type, dir);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
