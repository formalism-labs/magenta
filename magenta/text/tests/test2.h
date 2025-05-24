
#include "magenta/tests/gtest.h"

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////

using std::string;

class text : public string
{
	typedef string Super;
	
	string &super() { return *this; }
	const string &super() const { return *this; }

public:
	text(const char *p) : Super(p) {}
	text(const string &s) : Super(s) {}
	//text(string &&s) : Super(s) {}
	
	string &operator*() { return super(); }
	const string &operator*() const { return super(); }

	text &operator=(const string &s) { super() = s; return *this; }
	//text &&operator=(string &&s) { super() = s; return std::move(*this); }
};

///////////////////////////////////////////////////////////////////////////////////////////////
