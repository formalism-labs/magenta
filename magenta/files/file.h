
///////////////////////////////////////////////////////////////////////////////////////////////
// File
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_files_file_
#define _magenta_files_file_

#include <string>

#ifdef _WIN32
#include "magenta/system/windows.h"
#endif

#include "magenta/files/classes.h"

#include "magenta/exceptions/defs.h"

#include "magenta/files/path.h"
#include "magenta/files/filename.h"

// #include "magenta/types/defs.h"

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////
// File
///////////////////////////////////////////////////////////////////////////////////////////////

class File
{
	Path _path;

public:
	File(Directory &dir, const Filename &filename);
	File(const Path &path) : _path(path) {}
	explicit File(const text &path) : _path(path) {}

	Filename basename() const { return _path.basename(); }
	const Path &path() const { return _path; }
	Path operator~() const { return _path; }
	operator Path() const { return _path; }
	Path name() const { return _path; }

	bool exist() const;

	void copy(const Path &path, bool force = false);
	void copy(const Directory &dir, bool force = false);

	bool remove(bool force = false);

	CLASS_EXCEPTION_DEF("magenta::File");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class TemporaryFile : public File
{
public:
	TemporaryFile(const text &text);
};

typedef TemporaryFile TempFile;

///////////////////////////////////////////////////////////////////////////////////////////////

class Files
{
	WildcardPath _filespec;

public:
	Files(const WildcardPath &filespec) : _filespec(filespec) {}

	struct MapFunction
	{
		virtual void operator()(_Directory_::Iterator &i) {}
		virtual void down(_Directory_::Iterator &i) {}
		virtual void up() {}
	};

protected:
	static void preOrderMap(MapFunction &f, _Directory_::Iterator i);
	static void postOrderMap(MapFunction &f, _Directory_::Iterator i);

public:
	void preOrderMap(MapFunction &&f);
	void preOrderMap(MapFunction &f);
	void postOrderMap(MapFunction &&f);
	void postOrderMap(MapFunction &f);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_files_file_
