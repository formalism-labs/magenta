
///////////////////////////////////////////////////////////////////////////////////////////////
// Open File
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_files_open_file_
#define _magenta_files_open_file_

#include "magenta/files/classes.h"

#include "magenta/types/defs.h"

#include <string>

#include <stdio.h>

#ifdef _WIN32
#include "magenta/system/windows.h" 
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////
// OpenFile
///////////////////////////////////////////////////////////////////////////////////////////////

class OpenFile
{
	friend NewFile;

protected:
#ifdef _WIN32
	HANDLE _handle;
#endif

	void ctor(const Path &path);

	OpenFile() : _handle(0) {}

public:
	OpenFile(const Path &path);
	OpenFile(const File &file);
	OpenFile(HANDLE handle) : _handle(handle) {}
	~OpenFile();

#ifdef _WIN32
	operator HANDLE() const { return _handle; }
#endif

	PersistentDynablock read();
	void write(Block block);

	typedef std::uint64_t Size;
	Size size() const;

	CLASS_EXCEPTION_DEF("magenta::OpenFile");
	CLASS_SUB_EXCEPTION_DEF(TooBigError, "File too big to fit in memory");
	CLASS_SUB_EXCEPTION_DEF(IOError, "I/O Error");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class TextFile
{
	FILE *_file;

	void ctor(const Path &path, const char *mode);

public:
	TextFile(const Path &path, const char *mode);
	TextFile(const File &file, const char *mode);
	~TextFile();

	bool operator!() const;

	text read();
	void write(const string &text);

	text readln();
	void writeln(const string &line);

	CLASS_EXCEPTION_DEF("magenta::TextFile");
	CLASS_SUB_EXCEPTION_DEF(CannotOpen, "Cannot open file");
	CLASS_SUB_EXCEPTION_DEF(IOError, "I/O Error");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class NewFile : public OpenFile
{
public:
	NewFile(const Path &path);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_files_open_file_
