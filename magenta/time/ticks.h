
#ifndef _magenta_time_ticks_
#define _magenta_time_ticks_

#include "magenta/time/time-units.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Ticks
{
	typedef unsigned long Number;

	Number n;

protected:
	static Milliseconds tickDuration;

public:
	Ticks() : n(0) {}
	explicit Ticks(Number n) : n(n) {}

	Ticks(const Microseconds &t) : n(Milliseconds(t) / tickDuration) {}
	Ticks(const Milliseconds &t) : n(t / tickDuration) {}
	Ticks(const Seconds &t) : n(Milliseconds(t) / tickDuration) {}
	Ticks(const Minutes &t) : n(Milliseconds(t) / tickDuration) {}
	Ticks(const Hours &t) : n(Milliseconds(t) / tickDuration) {}
	Ticks(const Days &t) : n(Milliseconds(t) / tickDuration) {}

	Ticks operator+(const Ticks &t) const { return Ticks(n + t.n); }
	Ticks operator-(const Ticks &t) const { return Ticks(n - t.n); }
	Ticks operator*(unsigned int m) const { return Ticks(n * m); }
	Ticks operator/(unsigned int m) const { return Ticks(n / m); }

	bool operator==(const Ticks &t) const { return n == t.n; }
	bool operator!=(const Ticks &t) const { return n != t.n; }
	bool operator<(const Ticks &t) const  { return n < t.n; }
	bool operator<=(const Ticks &t) const { return n <= t.n; }
	bool operator>(const Ticks &t) const  { return n > t.n; }
	bool operator>=(const Ticks &t) const { return n >= t.n; }

#ifndef _WIN32
	operator Microseconds() const { return Microseconds(tickDuration * n); }
	operator Milliseconds() const { return Milliseconds(tickDuration * n); }
    operator Seconds() const      { return Seconds(tickDuration * n); }
    operator Minutes() const      { return Minutes(tickDuration * n); }
    operator Hours() const        { return Hours(tickDuration * n); }
    operator Days() const         { return Days(tickDuration * n); }
#else
	operator Microseconds() const { Milliseconds t = tickDuration; return t *= n; }
	operator Milliseconds() const { Milliseconds t = tickDuration; return t *= n; }
    operator Seconds() const      { Milliseconds t = tickDuration; return t *= n; }
    operator Minutes() const      { Milliseconds t = tickDuration; return t *= n; }
    operator Hours() const        { Milliseconds t = tickDuration; return t *= n; }
    operator Days() const         { Milliseconds t = tickDuration; return t *= n; }
#endif

	Number value() const { return n; }
};

//---------------------------------------------------------------------------------------------

class SystemTicks : public Ticks
{
public:
	SystemTicks();
};
	
///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_time_ticks_
