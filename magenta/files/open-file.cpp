
///////////////////////////////////////////////////////////////////////////////////////////////
// OpenFile
///////////////////////////////////////////////////////////////////////////////////////////////

#include "file.h"
#include "open-file.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// OpenFile
///////////////////////////////////////////////////////////////////////////////////////////////

void OpenFile::ctor(const Path &path)
{
	_handle = CreateFile(+path, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (_handle == INVALID_HANDLE_VALUE)
		THROW_EX(Exception)("cannot open file %s", +path);
}

//---------------------------------------------------------------------------------------------

OpenFile::OpenFile(const Path &path)
{
	ctor(path);
}

//---------------------------------------------------------------------------------------------

OpenFile::OpenFile(const File &file)
{
	ctor(file.path());
}

//---------------------------------------------------------------------------------------------

OpenFile::~OpenFile()
{
	CloseHandle(_handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////

PersistentDynablock OpenFile::read()
{
	Size n_ = size();
	DWORD n = (DWORD) n_;
	if (n != n_)
		THROW_EX(TooBigError)("");
	DWORD bytes_read;
	PersistentDynablock block(n);
//	Block block(new char[n], 0, n);
	BOOL rc = ::ReadFile(_handle, block.data(), n, &bytes_read, 0);
	if (!rc)
		THROW_EX(IOError)("read error");
	block.length() = bytes_read;
	return block;
}

//---------------------------------------------------------------------------------------------

void OpenFile::write(Block block)
{
	DWORD n;
	DWORD len = block.length();
	BOOL rc = ::WriteFile(_handle, block.data(), len, &n, 0);
	if (!rc || len < n)
		THROW_EX(IOError)("write error");
}

///////////////////////////////////////////////////////////////////////////////////////////////

OpenFile::Size OpenFile::size() const
{
	unsigned long hi, lo;
	lo = GetFileSize(_handle, &hi);
	if (lo == 0xFFFFFFFF && GetLastError() != NO_ERROR)
		THROW_EX(IOError)("cannot determine file size");
	return (OpenFile::Size) hi << 32 | lo;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////
// Text File
///////////////////////////////////////////////////////////////////////////////////////////////

void TextFile::ctor(const Path &path, const char *mode)
{
	_file = fopen(+path, mode);
	if (!_file)
		THROW_EX(CannotOpen);

	// validate EOF
	int c = fgetc(_file);
	ungetc(c, _file);
}

//---------------------------------------------------------------------------------------------

TextFile::TextFile(const Path &path, const char *mode)
{
	ctor(path, mode);
}

//---------------------------------------------------------------------------------------------

TextFile::TextFile(const File &file, const char *mode)
{
	ctor(file.path(), mode);
}

//---------------------------------------------------------------------------------------------

TextFile::~TextFile()
{
	fclose(_file);
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool TextFile::operator!() const
{
	int c = fgetc(_file);
	if (c == EOF)
		return TRUE;
	ungetc(c, _file);
	return FALSE;
}

//---------------------------------------------------------------------------------------------

text TextFile::read()
{
	int c = fgetc(_file);
	if (c == EOF)
		return "";
	text s;
	char ch = c;
	s.append(&ch, 1);
	return s;
}

//---------------------------------------------------------------------------------------------

void TextFile::write(const string &text)
{
	if (fprintf(_file, "%s", text.c_str()) < 0)
		THROW_EX(IOError);
}

//---------------------------------------------------------------------------------------------

text TextFile::readln()
{
	char s[1025];
	text line;
	for (;;)
	{
		int n = fscanf(_file, "%1024[^\n]", s);
		if (n == EOF)
			return line;
		if (n > 0)
			line += s;
		int c = fgetc(_file);
		if (c == EOF)
			return line;
		if (c == '\n')
		{
			char ch = c;
			line.append(&ch, 1);
			return line;
		}
		ungetc(c, _file);
	}
}

//---------------------------------------------------------------------------------------------

void TextFile::writeln(const string &line)
{
	if (!fprintf(_file, "%s\n", +line))
		THROW_EX(IOError);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// NewFile
///////////////////////////////////////////////////////////////////////////////////////////////

NewFile::NewFile(const Path &path)
{
#ifdef _WIN32
	_handle = CreateFile(+path, GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (!_handle)
		THROW_EX(Exception)("cannot create file %s", path);
#else
	UNIMPLEMENTED;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
