
#include "general.h"

#define HAS_BOOL
#include <perl-5.8/lib/core/perl.h>
#include <perl-5.8/lib/core/extern.h>

#include "magenta/lang/cxx/defs.h"

namespace magenta
{

namespace Perl
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////
// Script
///////////////////////////////////////////////////////////////////////////////////////////////

char *Script::_args[] = { "", "-e", "0", 0 };

//---------------------------------------------------------------------------------------------

Script::Script(char *env[])
{
   my_perl = perl_alloc();	// allocate the interpreter 
   perl_construct(my_perl);   // now build it 

   perl_parse                 // parse any perl script given to use 
   (
      my_perl,				// perl interpreter pointer 
      NULL,					// pointer to module init glue code: default=NONE=NULL 
      lengthof(_args) - 1,	// # of args received 
      _args,                  // list of arguments - make perl run empty script 
      env                     // pointer to environment 
  );
   perl_run(my_perl);         // run the opcode tree parse just made 
}

//---------------------------------------------------------------------------------------------

Script::~Script()
{
	perl_destruct(my_perl);
    perl_free(my_perl);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Script::run(const char *script)
{
	perl_eval_pv(script, TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Value Script::operator()(const char *name)
{
	return Value(*this, perl_get_sv(name, FALSE));
}

//---------------------------------------------------------------------------------------------

Value Script::define(const char *name)
{
	return Value(*this, perl_get_sv(name, TRUE));
}

///////////////////////////////////////////////////////////////////////////////////////////////

Array Script::array(const char *name, int rowSize)
{
	return Array(*this, name, rowSize, false);
}

//---------------------------------------------------------------------------------------------

Array Script::defineArray(const char *name, int rowSize)
{
	return Array(*this, name, rowSize, true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Value
///////////////////////////////////////////////////////////////////////////////////////////////

Value::operator int()
{
	return SvIV(_p);
}

//---------------------------------------------------------------------------------------------

Value::operator string()
{
	return SvPV_nolen(_p);
}

//---------------------------------------------------------------------------------------------

Value::operator double()
{
	return SvNV(_p);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Value &Value::operator=(int n)
{
	sv_setiv(_p, n);
	return *this;
}

//---------------------------------------------------------------------------------------------

Value &Value::operator=(double n)
{
	sv_setnv(_p, n);
	return *this;
}

//---------------------------------------------------------------------------------------------

Value &Value::operator=(const string &s)
{
	sv_setpv(_p, s.c_str());
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Array
///////////////////////////////////////////////////////////////////////////////////////////////

Array::Array(Script &script, const string &name, int rowSize, bool create) : 
	my_perl(script), row_size(rowSize)
{
	_v = perl_get_av(name.c_str(), create);
	if (!_v)
		THROW_EX(Exception)("Invalid array specification");
}

///////////////////////////////////////////////////////////////////////////////////////////////

Array::Element Array::operator[](int i)
{
	return Element(*this, i);
}

//---------------------------------------------------------------------------------------------

int Array::size() const
{
	return (av_len(_v) + 1) / row_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Array::Element
///////////////////////////////////////////////////////////////////////////////////////////////

Array::Element::Element(Array &a, int row, int i) : _array(a), _row(row), _col(i)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

Value Array::Element::fetch()
{
#define my_perl _array.my_perl
	return Value(my_perl, *av_fetch(_array._v, _row * _array.row_size + _col, FALSE));
#undef my_perl
}

//---------------------------------------------------------------------------------------------

Array::Element Array::Element::operator[](int i)
{
	return Element(_array, _row, _col + i);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
} // namespace Perl

