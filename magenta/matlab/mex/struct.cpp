
#include "struct.h"

namespace magenta
{

namespace MEX
{

///////////////////////////////////////////////////////////////////////////////////////////////
// ConstStruct
///////////////////////////////////////////////////////////////////////////////////////////////

ConstStruct::ConstStruct(const mxArray *s, int index)
{
    if (!s || !mxIsStruct(s)) 
    	THROW_EX(Exception)("mxArray is not a struct");
    _s = s;
	_index = index;
}


///////////////////////////////////////////////////////////////////////////////////////////////

const mxArray *ConstStruct::operator[](const char name[]) const
{
	return mxGetField(_s, _index, name);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Struct
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////////////////////
// Struct::Field
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// ConstStructArray
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// StructArray
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace MEX
} // namespace magenta
