
#include <memory.h>

#include "file.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////

File::File(const Path &path, const string &mode)
{
	mat_file = matOpen(path.c_str(), mode.c_str());
	if (!mat_file)
		THROW_EX(Exception)("cannot open file %s", path.c_str());
}

//---------------------------------------------------------------------------------------------

File::~File()
{
	matClose(mat_file);
}

///////////////////////////////////////////////////////////////////////////////////////////////

mxArray *File::read(const string &name)
{
	return matGetArray(mat_file, name.c_str());
}

//---------------------------------------------------------------------------------------------

mxArray *File::operator[](const string &name)
{
	mxArray *array = read(name);
	if (!array)
		THROW_EX(Exception)("cannot read variable \"%s\"", name.c_str());
	return array;
}

//---------------------------------------------------------------------------------------------

void File::write(mxArray *array)
{
	int rc = matPutArray(mat_file, array);
	if (rc)
		THROW_EX(Exception)("error writing array");
}

//---------------------------------------------------------------------------------------------

void File::writeAsGlobal(mxArray *array)
{
	int rc = matPutArrayAsGlobal(mat_file, array);
	if (rc)
		THROW_EX(Exception)("error writing array");
}

//---------------------------------------------------------------------------------------------

void File::erase(const string &name)
{
	int rc = matDeleteArray(mat_file, name.c_str());
	if (rc)
		THROW_EX(Exception)("cannot delete variable \"%s\"", name.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// File::Iterator
///////////////////////////////////////////////////////////////////////////////////////////////

File::Iterator::Iterator(File &file) : _file(file), _array(0) 
{
	_array = matGetNextArray(_file.mat_file);
}

///////////////////////////////////////////////////////////////////////////////////////////////

mxArray *File::Iterator::operator*()
{
	return _array;
}

//---------------------------------------------------------------------------------------------

File::Iterator &File::Iterator::operator++()
{
	_array = matGetNextArray(_file.mat_file);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta
