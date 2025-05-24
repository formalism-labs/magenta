
#ifndef _magenta_lang_perl_general_
#define _magenta_lang_perl_general_

#include "magenta/lang/perl/classes.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"

#include <string>

struct sv;
struct av;
struct interpreter;

namespace magenta
{
	
namespace Perl
{

using std::string;

///////////////////////////////////////////////////////////////////////////////

class Value
{
	Script &my_perl;
	struct sv *_p;

public:
	Value(Script &script, const char *name);
	Value(Script &script, struct sv *p) : my_perl(script), _p(p) {}

	operator int();
	operator double();
	operator string();
	operator String() { return operator string(); }

	Value &operator=(int n);
	Value &operator=(double n);
	Value &operator=(const string &s);
};

///////////////////////////////////////////////////////////////////////////////

class Array
{
	Script &my_perl;
	struct av *_v;
	int row_size;

public:
	Array(Script &script, const string &name, int rowSize = 1, 
		bool create = false);

	class Element
	{
		Array &_array;
		int _row, _col;

		Value fetch();

	public:
		Element(Array &a, int row, int i = 0);

		operator int() { return fetch(); }
		operator double() { return fetch(); }
		operator string() { return fetch(); }
		operator String() { return fetch(); }
		operator Value() { return fetch(); }
		
		Element operator[](int i);
	};

	Element operator[](int i);

	int size() const;

	CLASS_EXCEPTION_DEF("magenta::Array");

	friend Element;
};

///////////////////////////////////////////////////////////////////////////////

class Script
{
	typedef struct interpreter PerlInterpreter;

	PerlInterpreter *my_perl;
	static char *_args[];
	
public:
	Script(char *env[] = 0);
	~Script();

	operator PerlInterpreter*() { return my_perl; }

	void run(const char *script);

	Value operator()(const char *name);
	Value operator[](const char *name) { return define(name);	}
	Value define(const char *name);

	Array array(const char *name, int rowSize = 1);
//	Array operator()(const char *name, int rowSize = 1) { return array(name, rowSize); }
	Array defineArray(const char *name, int rowSize = 1);
};

///////////////////////////////////////////////////////////////////////////////

} // namespace Perl
} // namespace magenta_general

#endif // _magenta_lang_perl_general_
