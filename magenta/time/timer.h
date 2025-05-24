
#ifndef _magenta_time_timer_
#define _magenta_time_timer_

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Timer
{
	clock_t t0, t1, dt;

public:
	void start()
	{
		t0 = clock();
	}
	
	void stop()
	{
		t1 = clock();
		dt = t1 - t0;
	}

	int hours() const { return dt_sec() / 3600; }
	int minutes() const { return (dt_sec() - hours() * 3600) / 60; }
	int seconds() const { return dt_sec() % 60; }
	int milliseconds() const { return dt % CLOCKS_PER_SEC; }

	text delta() const
	{
		return stringf("%02.2d:%02.2d:%02.2d.%d", hours(), minutes(), seconds(), milliseconds());
	}

protected:
	long dt_sec() const { return dt / CLOCKS_PER_SEC; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_time_timer_ 
