
#include "strings.h"
#include "text/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

Strings::Strings(const std::vector<string> &ss)
{
	for (auto i = ss.begin(); i != ss.end(); ++i)
		push_back(*i);
}

//---------------------------------------------------------------------------------------------

Strings::Strings(const Vector<string> &ss)
{
	foreach (i, ss.begin())
		push_back(*i);
}

//---------------------------------------------------------------------------------------------

Strings::Strings(const Vector<char*> &ss)
{
	foreach (i, ss.begin())
		push_back(*i);
}

//---------------------------------------------------------------------------------------------

Strings::Strings(const text &s, const text &sep)
{
}

//---------------------------------------------------------------------------------------------

text Strings::join(const text &sep) const
{
	return magenta::join(*this, sep);
}

//---------------------------------------------------------------------------------------------

void Strings::write(std::ostream &out) const
{
	foreach (i, begin())
		out << *i << " ";
}

//---------------------------------------------------------------------------------------------

#if 0
bool Strings::operator==(const Strings &ss) const
{
}

//---------------------------------------------------------------------------------------------

Strings Strings::common_prefix(const Strings &ss) const
{
}

//---------------------------------------------------------------------------------------------

Strings Strings::operator-(const Strings &ss) const
{
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
