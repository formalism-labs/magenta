
///////////////////////////////////////////////////////////////////////////////////////////////
// Directory
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_files_directory_
#define _magenta_files_directory_

#include "magenta/files/classes.h"

#include "magenta/types/defs.h"
#include "magenta/files/filename.h"
#include "magenta/files/file.h"

#include <string>
#include <sys/stat.h>

#ifdef _WIN32
#include "magenta/system/windows.h"

#ifdef _MSC_VER
typedef int mode_t;
#endif
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////
// Directory
///////////////////////////////////////////////////////////////////////////////////////////////

class Directory
{
	friend NewDirectory;
	friend TemporaryDirectory;
	friend CurrentDirectory;

	Path _path;

protected:
	Directory() {}

public:
	Directory(const Path &path) : _path(path) {}
	explicit Directory(const text &path) : _path(Path(path)) {}

	const Path &path() const { return _path; }
	operator Path() const { return path(); }

	bool operator!() const { return !_path; }

	bool exist() const;

	File file(const Filename &filename) const;
	File operator[](const Filename &filename) const { return file(filename); }
	File operator[](const text &filename) const { return file(Filename(filename)); }
	Path operator/(const text &p) const { return _path/p; }
	
	Directory subdir(const Path &path) const;
	Directory operator()(const text &path) const { return subdir(Path(path)); }

	Path subpath(const text &name) const;

	void create();
	void create(mode_t mode, bool strict = true);
	
	void copy(Directory &dir);

	CLASS_EXCEPTION_DEF("magenta::Directory");

	typedef _Directory_::Iterator Iterator;
};

///////////////////////////////////////////////////////////////////////////////////////////////

class NewDirectory : public Directory
{
public:
	NewDirectory(Path &name);

	CLASS_EXCEPTION_DEF("magenta::NewDirectory");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class TemporaryDirectory : public Directory
{
public:
	TemporaryDirectory();
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CurrentDirectory : public Directory
{
public:
	CurrentDirectory();
};

///////////////////////////////////////////////////////////////////////////////////////////////

namespace _Directory_
{

class Iterator
{
public:
#ifdef _WIN32
	class FileData : protected WIN32_FIND_DATA
	{
		friend Iterator;
		Iterator &_iterator;

	protected:
		FileData(Iterator &iterator) : _iterator(iterator) {}
		FileData(Iterator &iterator, WIN32_FIND_DATA &find_data) : 
			_iterator(iterator), WIN32_FIND_DATA(find_data) {}

	public:
		Filename name() const;
		Path path() const;

		DWORD attributes() const { return dwFileAttributes; }
		bool isDir() const { return !!(attributes() & FILE_ATTRIBUTE_DIRECTORY); }
	};
#else
	class FileData
	{
	};
#endif
	friend FileData;

private:
	WildcardPath _dirspec;
#ifdef _WIN32
	HANDLE _handle;
#else
#endif
	FileData file_data;
	bool _valid;

	void ctor();

public:
	Iterator(const WildcardPath &dirspec);
	Iterator(Iterator &iterator, const FileData &fname);
	~Iterator();

	Iterator &operator++();
	bool operator!() const;

	FileData &operator*() { return file_data; }
	FileData *operator->() { return &file_data; }

	magenta::File file() const;

	Path path() const { return file_data.path(); }
	Path commonPath() const { return _dirspec; }

	CLASS_EXCEPTION_DEF("magenta::Directory::Iterator");
};

} // namespace _Directory_

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_files_directory_
