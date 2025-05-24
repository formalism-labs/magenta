
#ifndef _magenta_math_general_
#define _magenta_math_general_

#include <math.h>

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

inline double sqr(double x) { return x * x; }

inline double norm(double x, double y) { return sqrt(sqr(x) + sqr(y)); }
inline double norm(double x, double y, double z) { return sqrt(sqr(x) + sqr(y) + sqr(z)); }

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_math_general_
