
#include "magenta/text/defs.h"

#include "datetime.h"

#include <time.h>

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

static int _compare(int n1, int n2, int &n)
{
    return n = n1 > n2 ? 1 : (n1 < n2 ? -1 : 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Date
///////////////////////////////////////////////////////////////////////////////////////////////

string Date::toString() const // yyyy-mm-dd
{
	return stringf("%04d-%02d-%02d", year(), month(), day());
}

//---------------------------------------------------------------------------------------------

int Date::compare(const Date &d) const
{
    int n;
    if (!_compare(year(), d.year(), n))
        if (!_compare(month(), d.month(), n))
           return _compare(day(), d.day(), n);
    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////////////////////

string Time::toString() const // hh:MM:ss.tt
{
	return stringf("%02d:%02d:%02d.%02d", hour(), minute(), second(), millisecond());
}

//---------------------------------------------------------------------------------------------

int Time::compare(const Time &d) const
{
    int n;
    if (!_compare(hour(), d.hour(), n))
        if (!_compare(minute(), d.minute(), n))
           if (!_compare(second(), d.second(), n))
           		return _compare(millisecond(), d.millisecond(), n);
    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// DateTime
///////////////////////////////////////////////////////////////////////////////////////////////

string DateTime::toString() const
{
	return stringf("%s %s", date().toString().c_str(), time().toString().c_str());
}

//---------------------------------------------------------------------------------------------

int DateTime::compare(const DateTime &dt) const
{
    int n = date().compare(dt.date());
    if (n)
    	return n;
    return time().compare(dt.time());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// SystemDateTime
///////////////////////////////////////////////////////////////////////////////////////////////

SystemDateTime::SystemDateTime()
{
	struct tm tm;

#if defined(_WIN32)
	time_t t = ::time(NULL);
	tm = *localtime(&t);
#else
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	localtime_r(&t.tv_sec, &tm);
	_millisecond = t.tv_nsec / 10000000;
#endif

	_year = tm.tm_year + 1900;
	_month =  tm.tm_mon + 1;
	_day = tm.tm_mday;
	_hour = tm.tm_hour;
	_minute = tm.tm_min;
	_second = tm.tm_sec;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
