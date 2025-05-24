
#ifndef _magenta_text_strings_
#define _magenta_text_strings_
 
#include <iostream>

#include "magenta/text/text.h"
#include "magenta/types/vector.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Strings : public Vector<text>
{
	typedef Strings This;
	typedef Vector<text> Super;

public:
	Strings() {}
	Strings(const Vector<string> &ss);
	Strings(const std::vector<string> &ss);
	Strings(const Vector<text> &ss)  : Super(ss) {}
	Strings(const Vector<char*> &ss);
	Strings(const text &s, const text &sep);

	text join(const text &sep) const;// { return join(*this, sep); }

	bool operator==(const Strings &ss) const;
	Strings common_prefix(const Strings &ss) const;
	Strings operator-(const Strings &ss) const;

	void write(std::ostream &out) const;

	friend std::ostream &operator<<(std::ostream &out, const This &v)
	{
		v.write(out);
		return out;
	} 
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_strings_
