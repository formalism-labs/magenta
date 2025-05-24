
#ifndef _magenta_matlab_mex_file_
#define _magenta_matlab_mex_file_

#include "magenta/matlab/mex/general.h"

#include "magenta/exceptions/defs.h"
#include "magenta/files/defs.h"

namespace magenta
{

namespace MEX
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class File
{
	MATFile *mat_file;

public:
	File(const Path &path, const string &mode);
	virtual ~File();

	mxArray *read(const string &name);
	mxArray *operator[](const string &name);

	void write(mxArray *array);
	void writeAsGlobal(mxArray *array);
	void erase(const string &name);

	CLASS_EXCEPTION_DEF("magenta::File");

public:
	class Iterator
	{
		File &_file;
		mxArray *_array;
		
	public:
		Iterator(File &file);

		mxArray *operator*();
		Iterator &operator++();
		bool operator!() const { return !_array; }
	};

	friend Iterator;
};


///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta

#endif // _magenta_matlab_mex_file_
