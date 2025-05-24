
#ifndef _magenta_files_filename_
#define _magenta_files_filename_

#include "magenta/files/classes.h"

#include "magenta/exceptions/defs.h"
#include "magenta/text/defs.h"
#include "magenta/types/defs.h"

#ifdef _WIN32
#include "magenta/system/windows.h"
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Filename
{
	friend UniqueFilename;
	friend Directory;

protected:
	text _name;
	const text &str() const { return _name; }

	Filename() {}

	void assign(const Filename &filename);
	Filename &operator=(const Filename &filename) { assign(filename); return *this; }

public:
	Filename(const text &name) : _name(name) {}
	Filename(const Filename &filename) : _name(filename) {}

	Filename forename() const;
	text surname() const { return ext(); }
	text ext() const;
	Path fullPath() const;

	operator text() const { return _name; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class UniqueFilename : public Filename
{
	void ctor(const text &type, const Directory &dir);

public:
	UniqueFilename(const text &type = "");
	UniqueFilename(const text &type, const Directory &dir);

	CLASS_EXCEPTION_DEF("magenta::UniqueFilename");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_files_filename_
