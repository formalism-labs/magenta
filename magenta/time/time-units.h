
#ifndef _magenta_time_time_units_
#define _magenta_time_time_units_

#include "magenta/lang/general.h"
#include "magenta/time/classes.h"

#ifdef _WIN32
#include "magenta/system/windows.h"
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class TimeUnit
{
public:
	typedef unsigned long Number;
	Number n;

public:
	TimeUnit() : n(0) {}
	explicit TimeUnit(Number n) : n(n) {}
	TimeUnit(const TimeUnit &t) : n(t.n) {}

	T operator+(const T &t) const      { return T(n + t.n); }
	T operator-(const T &t) const      { return T(n - t.n); }
	T operator*(unsigned long m) const { return T(n * m); }
	T operator/(unsigned long m) const { return T(n / m); }
	Number operator/(const T &t) const { return n / t.n; }
	T operator%(long m) const          { return T(n % m); }

	T operator=(const T &t)  { return T(n = t.n); }
	T operator+=(const T &t) { return T(n += t.n); }
	T operator-=(const T &t) { return T(n -= t.n); }
	T operator*=(Number m)   { return T(n *= m); }
	T operator/=(Number m)   { return T(n /= m); }
	T operator%=(long m)     { long k = long(n); return T(n = k %= m); }

	T absdiff(const T &t) const { return T(t.n > n ? t.n - n : (~Number(0)) - n + t.n); }

	static T absdiff(const T &t0, const T &t1) { return t0.absdiff(t1); }

	bool operator> (const T &t) const { return n > t.n; }
	bool operator>=(const T &t) const { return n >= t.n; }
	bool operator< (const T &t) const { return n < t.n; }
	bool operator<=(const T &t) const { return n <= t.n; }
	bool operator==(const T &t) const { return n == t.n; }
	bool operator!=(const T &t) const { return n != t.n; }

	bool operator!() const { return !n; }

	Number value() const { return n; }
};

//---------------------------------------------------------------------------------------------

template <class T>
T operator*(typename TimeUnit<T>::Number m, const T &t) { return T(t.n * m); }

//---------------------------------------------------------------------------------------------

class Microseconds : public TimeUnit<Microseconds>
{
	friend class Milliseconds;
	friend class Seconds;
	friend class Minutes;
	friend class Hours;
	friend class Days;

	typedef TimeUnit<Microseconds> Super;

public:
	Microseconds() {}
	explicit Microseconds(Number n) : Super(n) {}
	Microseconds(const Microseconds &t) : Super(t) {}

	operator Milliseconds() const;
    operator Seconds() const;
    operator Minutes() const;
    operator Hours() const;
	operator Days() const;
};

//---------------------------------------------------------------------------------------------

class Milliseconds : public TimeUnit<Milliseconds>
{
	friend class Microseconds;
	friend class Seconds;
	friend class Minutes;
	friend class Hours;
	friend class Days;

	typedef TimeUnit<Milliseconds> Super;

public:
	Milliseconds() {}
	explicit Milliseconds(Number n) : Super(n) {}
	Milliseconds(const Milliseconds &t) : Super(t) {}

	operator Microseconds() const;
    operator Seconds() const;
    operator Minutes() const;
    operator Hours() const;
	operator Days() const;

	struct timeval timeval() const
	{
		struct timeval tv;
		tv.tv_sec = value() / _1e3;
		tv.tv_usec = (value() % _1e3) * _1e3;
		return tv;
	}

	struct timespec	timespec() const
	{
		struct timespec ts;
		ts.tv_sec = value() / _1e3;
		ts.tv_nsec = (value() % _1e3) * _1e6;
		return ts;
	}
};

//---------------------------------------------------------------------------------------------

class Seconds : public TimeUnit<Seconds>
{
	friend class Microseconds;
	friend class Milliseconds;
	friend class Minutes;
	friend class Hours;
	friend class Days;

	typedef TimeUnit<Seconds> Super;

public:
	Seconds() {}
	explicit Seconds(Number n) : Super(n) {}
	Seconds(const Seconds &t) : Super(t) {}

	operator Microseconds() const;
    operator Milliseconds() const;
    operator Minutes() const;
    operator Hours() const;
	operator Days() const;
};

//---------------------------------------------------------------------------------------------

class Minutes : public TimeUnit<Minutes>
{
	friend class Microseconds;
	friend class Milliseconds;
	friend class Seconds;
	friend class Hours;
	friend class Days;

	typedef TimeUnit<Minutes> Super;

public:
	Minutes() {}
	explicit Minutes(Number n) : Super(n) {}
    Minutes(const Super &t) : Super(t) {}

	operator Microseconds() const;
    operator Milliseconds() const;
    operator Seconds() const;
    operator Hours() const;
	operator Days() const;
};

//---------------------------------------------------------------------------------------------

class Hours : public TimeUnit<Hours>
{
	friend class Microseconds;
	friend class Milliseconds;
	friend class Seconds;
	friend class Minutes;
	friend class Days;

	typedef TimeUnit<Hours> Super;

public:
	Hours() {}
	explicit Hours(Number n) : Super(n) {}
	Hours(const Hours &t) : Super(t) {}

	operator Microseconds() const;
	operator Milliseconds() const;
	operator Seconds() const;
	operator Minutes() const;
	operator Days() const;
};

//---------------------------------------------------------------------------------------------

class Days : public TimeUnit<Days>
{
	friend class Microseconds;
	friend class Milliseconds;
	friend class Seconds;
	friend class Minutes;
	friend class Hours;

	typedef TimeUnit<Days> Super;

public:
	Days() {}
	explicit Days(Number n) : Super(n) {}
	Days(const Days &t) : Super(t) {}

	operator Microseconds() const;
	operator Milliseconds() const;
	operator Seconds() const;
	operator Minutes() const;
	operator Hours() const;
};

//---------------------------------------------------------------------------------------------

inline Microseconds::operator Milliseconds() const { return Microseconds(n / 1000L); }
inline Microseconds::operator Seconds()      const { return Seconds(n / (1000L * 1000)); }
inline Microseconds::operator Minutes()      const { return Minutes(n / (1000L * 1000 * 60)); }
inline Microseconds::operator Hours()        const { return Hours(n / (1000L * 1000) / 3600); }
inline Microseconds::operator Days()         const { return Days(n / (1000L * 1000) / 3600 / 24); }

inline Milliseconds::operator Microseconds() const { return Microseconds(n * 1000L); }
inline Milliseconds::operator Seconds()      const { return Seconds(n / 1000); }
inline Milliseconds::operator Minutes()      const { return Minutes(n / (1000 * 60)); }
inline Milliseconds::operator Hours()        const { return Hours(n / (1000L * 3600)); }
inline Milliseconds::operator Days()         const { return Days(n / (1000L * 3600) / 24); }

inline Seconds::operator Microseconds() const { return Microseconds(n * (1000L * 1000)); }
inline Seconds::operator Milliseconds() const { return Milliseconds(n * 1000); }
inline Seconds::operator Minutes()      const { return Minutes(n / 60); }
inline Seconds::operator Hours()        const { return Hours(n / 3600); }
inline Seconds::operator Days()         const { return Days(n / 3600 / 24); }
          
inline Minutes::operator Microseconds() const { return Microseconds(n * (1000L * 1000 * 60)); }
inline Minutes::operator Milliseconds() const { return Milliseconds(n * (1000L * 60)); }
inline Minutes::operator Seconds()      const { return Seconds(n * 60); }
inline Minutes::operator Hours()        const { return Hours(n / 60); }
inline Minutes::operator Days()         const { return Days(n / 60 / 24); }

inline Hours::operator Microseconds() const { return Microseconds(n * (1000L * 1000) * 3600); }
inline Hours::operator Milliseconds() const { return Milliseconds(n * (1000L * 3600)); }
inline Hours::operator Seconds()      const { return Seconds(n * 3600); }
inline Hours::operator Minutes()      const { return Minutes(n * 60); }
inline Hours::operator Days()         const { return Days(n / 24); }

inline Days::operator Microseconds() const { return Microseconds(n * (1000L * 1000) * 3600 * 24); }
inline Days::operator Milliseconds() const { return Milliseconds(n * (1000L * 3600 * 24)); }
inline Days::operator Seconds()      const { return Seconds(n * 3600 * 24); }
inline Days::operator Minutes()      const { return Minutes(n * 60 * 24); }

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_time_time_units_
