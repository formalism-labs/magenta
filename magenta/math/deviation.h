
#ifndef _magenta_math_deviation_
#define _magenta_math_deviation_

#include <math.h>

#include "magenta/math/classes.h"

#include "magenta/types/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Deviation
{
	double sum_x, sum_xx;
	int _n;

public:
	Deviation();
	Deviation(const Vector<double> &v);
	Deviation(const Vector<double> &v, const Vector<int> &w);
	
	Deviation &operator<<(double x)
	{
		append(x);
		return *this;
	}

	void append(double x, int w = 1)
	{
		_n += w;
		sum_x += x * w;
		sum_xx += x * x * w;
	}

	double mean() const { return sum_x / _n; }
	double variance() const { return (sum_xx - sum_x * sum_x / _n) / _n; }
	double stddev() const { return sqrt(variance()); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_math_deviation_
