
#ifndef _magenta_gtest_
#define _magenta_gtest_

#include "gtest/gtest.h"

#include "magenta/text/defs.h"
#include "magenta/types/defs.h"

///////////////////////////////////////////////////////////////////////////////////////////////

namespace testing
{
namespace internal
{

inline GTEST_API_ AssertionResult CmpHelperSTREQ(const char* s1_expression, const char* s2_expression, const char* s1, const magenta::text &s2)
{
	return CmpHelperSTREQ(s1_expression, s2_expression, s1, +s2);
}

}
}

///////////////////////////////////////////////////////////////////////////////////////////////

namespace magenta
{
inline void PrintTo(const Vector<text> &v, std::ostream *o)
{
	if (o)
		*o << v;
}
}

#define VT(...) magenta::Vector<text>({__VA_ARGS__})
#define VT0     magenta::Vector<text>()

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_gtest_
