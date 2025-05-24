
///////////////////////////////////////////////////////////////////////////////////////////////
// File
///////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include "directory.h"

#include "filename.h"
#include "file.h"

#include <direct.h>

namespace magenta
{

using namespace _Directory_;

///////////////////////////////////////////////////////////////////////////////////////////////
// Directory
///////////////////////////////////////////////////////////////////////////////////////////////

File Directory::file(const Filename &name) const
{
	return File(subpath(name));
}

//---------------------------------------------------------------------------------------------

Directory Directory::subdir(const Path &path) const
{
	return Directory(subpath(path));
}

//---------------------------------------------------------------------------------------------

Path Directory::subpath(const text &filespec) const
{
	return path()/filespec;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Directory::copy(Directory &dir)
{
	struct F : Files::MapFunction
	{
		Directory src_dir, dest_dir;
		size_t _bias;

		F(Directory &srcDir, Directory &destDir) : src_dir(srcDir), dest_dir(destDir) 
		{
			_bias = src_dir.path().size() + 1;
		}

		void operator()(Iterator &i)
		{
			try
			{
				Path dest(dest_dir.path()/(i.path().str().substr(_bias)));
				if (i->isDir())
				{
					NewDirectory new_dir(dest);
				}
				else
					File(i.path()).copy(dest);
			} catch (...) {}
		}
	};

	Files(WildcardPath(*this)).preOrderMap(F(*this, dir));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// NewDirectory
///////////////////////////////////////////////////////////////////////////////////////////////

NewDirectory::NewDirectory(Path &path)
{
#ifdef _WIN32
	BOOL rc = CreateDirectory(+path, 0);
	if (!rc)
	{
		WIN32_FIND_DATA data;
		HANDLE h = FindFirstFile(+path, &data);
		if (h == INVALID_HANDLE_VALUE)
			THROW_EX(Exception);
		FindClose(h);
	}
	_path = path;
#else
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// TemporaryDirectory
///////////////////////////////////////////////////////////////////////////////////////////////

TemporaryDirectory::TemporaryDirectory()
{
#ifdef _WIN32
	char dir[1024];
	DWORD rc = GetTempPath(sizeof(dir)-1, dir);
	_path = dir;
#else
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CurrentDirectory
///////////////////////////////////////////////////////////////////////////////////////////////

CurrentDirectory::CurrentDirectory()
{
	char s[MAX_PATH];
	_getcwd(s, sizeof(s));
	_path = s;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////////////////////

namespace _Directory_
{

///////////////////////////////////////////////////////////////////////////////////////////////

void Iterator::ctor()
{
#ifdef _WIN32
	text d = _dirspec;
	if (*d.begin() == '\"' && *d.rbegin() == '\"')
		d = d.substr(1, d.length() - 2);
	_handle = FindFirstFile(+d, &file_data);
	_valid = _handle != INVALID_HANDLE_VALUE;
	while (_valid && (!strcmp(file_data.cFileName, ".") || !strcmp(file_data.cFileName, "..")))
		++(*this);
#else
	UNIMPLEMENTED;
#endif
}

//---------------------------------------------------------------------------------------------

Iterator::Iterator(const WildcardPath &dirspec) : 
	_dirspec(dirspec), file_data(*this)
{
	ctor();
}

//---------------------------------------------------------------------------------------------

Iterator::Iterator(Iterator &iterator, const FileData &data) :
	_dirspec(text(iterator.path()) + text(data.name()) + "\\*.*"),
	file_data(*this)
{
	ctor();
}

//---------------------------------------------------------------------------------------------

Iterator::~Iterator()
{
#ifdef _WIN32
	FindClose(_handle);
#else
	UNIMPLEMENTED;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

Iterator &Iterator::operator++()
{
#ifdef _WIN32
	_valid = !!FindNextFile(_handle, &file_data);
#else
	UNIMPLEMENTED;
#endif
	return *this;
}

//---------------------------------------------------------------------------------------------

bool Iterator::operator!() const
{
	return !_valid;
}

///////////////////////////////////////////////////////////////////////////////////////////////

magenta::File Iterator::file() const
{
	return magenta::File(path());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// DirectoryIterator :: File
///////////////////////////////////////////////////////////////////////////////////////////////

Filename Iterator::FileData::name() const 
{ 
	return Filename(cFileName);
}

//---------------------------------------------------------------------------------------------

Path Iterator::FileData::path() const 
{ 
	// return Directory(_iterator._dirspec.dir()).subpath(name());
	return Path();
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace _Directory_

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
