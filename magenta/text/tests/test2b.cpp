
#include "test2.h"

///////////////////////////////////////////////////////////////////////////////////////////////

class Thing
{
	text _val;

public:
	text &value() { return _val; }
	const text &value() const { return _val; }

public:
	Thing(const char *name) : _val(name) {}
	Thing(const string &name) : _val(name) {}

	operator text&() { return value(); }
	operator const text&() const { return value(); }

	bool operator==(const Thing &t) const { return value() == t.value(); }

	const text &operator*() const { return value(); }
		
	Thing &operator=(const text &s) { value() = s; return *this; }
};

inline Thing operator+(const Thing &t, const Thing &s) { return Thing(*t + *s); }

///////////////////////////////////////////////////////////////////////////////////////////////

#define TEST_GROUP Text2

#include "test2.hxx"
