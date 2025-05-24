
#ifndef _magenta_time_datetime_
#define _magenta_time_datetime_

#include "magenta/time/time-units.h"

#include <string>

namespace magenta
{

class DateTime;

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

#define COMPARE_OPERATORS(T) \
	bool operator==(const T &t) const { return compare(t) == 0; } \
	bool operator!=(const T &t) const { return compare(t) != 0; } \
	bool operator<(const T &t) const  { return compare(t) < 0;  } \
	bool operator>(const T &t) const  { return compare(t) > 0;  } \
	bool operator<=(const T &t) const { return compare(t) <= 0; } \
	bool operator>=(const T &t) const { return compare(t) >= 0; }

///////////////////////////////////////////////////////////////////////////////////////////////

class Date
{
	friend class DateTime;

public:
	typedef int Day;
	typedef int Month;
	typedef int Year;

protected:
	Year _year;
	Month _month;
	Day _day;

public:
	Day day() const { return _day; }
	Month month() const { return _month; }
	Year year() const { return _year; }

	bool operator!() const { return !_year && !_month && !_day; }
	string toString() const; // yyyy-mm-dd

public:
	Date() : _year(0), _month(0), _day(0) {}
	Date(Year year, Month month, Day day) : _year(year), _month(month), _day(day) {}
	Date(unsigned long days); // unimplemented

protected:
	int compare(const Date &d) const;

public:
	COMPARE_OPERATORS(Date)
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Time
{
	friend class DateTime;

public:
	typedef int Hour;
	typedef int Minute;
	typedef int Second;
	typedef int Millisecond;

protected:
	Hour _hour;
	Minute _minute;
	Second _second;
	Millisecond _millisecond;

public:
	Hour hour() const { return _hour; }
	Minute minute() const { return _minute; }
	Second second() const { return _second; }
	Millisecond millisecond() const { return _millisecond; }

	bool operator!() const { return !_hour && !_minute && !_second && !_millisecond; }
	string toString() const; // hh:MM:ss.tt

public:
	Time() : _hour(0), _minute(0), _second(0), _millisecond(0) {}
	Time(Hour h, Minute m, Second s, Millisecond ms = 0) : _hour(h), _minute(m), _second(s), _millisecond(ms) {}

protected:
	int compare(const Time &d) const;

public:
	COMPARE_OPERATORS(Time)
};

///////////////////////////////////////////////////////////////////////////////////////////////

class DateTime : public Date, public Time
{
public:
	DateTime() {}
	DateTime(const Date &date, const Time &time) : Date(date), Time(time) {}

public:
	const Date &date() const { return *this; }
	const Time &time() const { return *this; }

	bool operator!() const { return !date() && !time(); }
	string toString() const; // yyyy-mm-dd hh:MM:ss.tt

protected:
	int compare(const DateTime &d) const;

public:
	COMPARE_OPERATORS(DateTime)
};

//---------------------------------------------------------------------------------------------

class SystemDateTime : public DateTime
{
public:
	SystemDateTime();
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_time_datetime_
