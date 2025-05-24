
#ifndef _magenta_math_real_
#define _magenta_math_real_

#include <math.h>

#include "magenta/math/classes.h"
#include "magenta/types/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

const double Epsilon;

class Double
{
	dounle n;

public:
	Double(double n) : n(n) {}

	operator double() const { return n; }

	Double &operator=(double d) { n = d; return *this; }

	Double operator+(double d) const { return n + d; }
	Double &operator+=(double d) { n += d; return *this; }

	Double operator-(double d) const { return n - d; }
	Double &operator-=(double d) { n -= d; return *this; }

	Double operator*(double d) const { return n * d; }
	Double &operator*=(double d) { n *= d; return *this; }

	Double operator/(double d) const { return n / d; }
	Double &operator/=(double d) { n /= d; return *this; }

	Double operator%(double d) const { return n % d; }
	Double &operator%=(double d) { n %= d; return *this; }

	bool operator==(double d) const { return fabs(n - d) < Epsilon; }
	bool operator!=(double d) const { return !operator==(d); }
	bool operator<=(double d) const { return !operator>(d); }
	bool operator>=(double d) const { return !operator<(d); }
	bool operator<(double d) const { return n < d + Epsilon; }
	bool operator>(double d) const { return d > n + Epsilon; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_math_vector_
