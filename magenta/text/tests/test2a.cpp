
#include "test2.h"

///////////////////////////////////////////////////////////////////////////////////////////////

class Thing : public text
{
public:
	text &value() { return *this; }
	const text &value() const { return *this; }

public:
	Thing(const string &name) : text(name) {}

	const text &operator*() const { return value(); }
		
	Thing &operator=(const text &s) { value() = s; return *this; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

#define TEST_GROUP Text1

#include "test2.hxx"
