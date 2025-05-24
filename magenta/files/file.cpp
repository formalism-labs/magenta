
///////////////////////////////////////////////////////////////////////////////////////////////
// File
///////////////////////////////////////////////////////////////////////////////////////////////

#include "file.h"

#include "magenta/files/defs.h"
#include "magenta/text/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// File
///////////////////////////////////////////////////////////////////////////////////////////////

File::File(Directory &dir, const Filename &filename) :
	_path(dir.file(filename).path())
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool File::exist() const
{
	Directory::Iterator i(path());
	return !!i;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void File::copy(const Path &destPath, bool force)
{
#ifdef _WIN32
	BOOL rc = ::CopyFile(+path(), +destPath, force);
	if (!rc)
		THROW_EX(Exception)("cannot copy file %s to %s", +path(), +destPath);
#else
#endif
}

//---------------------------------------------------------------------------------------------

void File::copy(const Directory &dir, bool force)
{
	copy(dir[path().basename()].path(), force);
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool File::remove(bool force)
{
#ifdef _WIN32
	struct F : public Files::MapFunction
	{
		BOOL rc;

		F() : rc(1) {}

		void operator()(Directory::Iterator &di)
		{
			text s = di.path();
			if (di->isDir())
				rc = !!::RemoveDirectory(+s) && rc;
			else
				rc = !!::DeleteFile(+s) && rc;
		}
	};

	BOOL rc = !!::DeleteFile(+_path);
	if (!rc)
		rc  = !!::RemoveDirectory(+_path);
	if (!rc && force)
	{
		F f;
		Files(WildcardPath(_path, "*.*")).postOrderMap(f);
		rc = f.rc && !!::RemoveDirectory(+_path);
	}

	return !!rc;
#else
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// TemporaryFile
///////////////////////////////////////////////////////////////////////////////////////////////

TemporaryFile::TemporaryFile(const text &text) : File(UniqueFilename())
{
	TextFile file(*this, "w");
	file.write(text);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Files
///////////////////////////////////////////////////////////////////////////////////////////////

void Files::preOrderMap(MapFunction &f, Directory::Iterator i)
{
	f.down(i);
	for (; !!i; ++i)
	{
		f(i);
		if (i->isDir())
			preOrderMap(f, Directory::Iterator(i, *i));
	}
	f.up();
}

//---------------------------------------------------------------------------------------------

void Files::preOrderMap(MapFunction &&f)
{
	preOrderMap(f, Directory::Iterator(_filespec));
}

void Files::preOrderMap(MapFunction &f)
{
	preOrderMap(f, Directory::Iterator(_filespec));
}

//---------------------------------------------------------------------------------------------

void Files::postOrderMap(MapFunction &f, Directory::Iterator i)
{
	f.down(i);
	for (; !!i; ++i)
	{
		if (i->isDir())
			postOrderMap(f, Directory::Iterator(i, *i));
		f(i);
	}
	f.up();
}

//---------------------------------------------------------------------------------------------

void Files::postOrderMap(MapFunction &&f)
{
	postOrderMap(f, Directory::Iterator(_filespec));
}

void Files::postOrderMap(MapFunction &f)
{
	postOrderMap(f, Directory::Iterator(_filespec));
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
