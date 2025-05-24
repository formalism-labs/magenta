
#if defined(_WIN32)
#include <time.h>
#include <winsock2.h>
#include <windows.h>

#elif defined(__VXWORKS__)
#include <vxworks.h>
#include <sysLib.h>
#include <tickLib.h>

#elif defined(__linux__)
#include <unistd.h>
#include <sys/times.h>
#endif

#include "time/ticks.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__VXWORKS__)
Milliseconds Ticks::tickDuration = Milliseconds(1000) / sysClkRateGet();

#elif defined(_WIN32)
Milliseconds Ticks::tickDuration(Milliseconds(1000) / CLOCKS_PER_SEC);

#elif defined(__linux__)
Milliseconds Ticks::tickDuration = Milliseconds(1000) / sysconf(_SC_CLK_TCK);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__VXWORKS__)
SystemTicks::SystemTicks() : Ticks(tickGet()) {}

#elif defined(_WIN32)
SystemTicks::SystemTicks() : Ticks(GetTickCount() / tickDuration.value()) {}

#elif defined(__linux__)
static tms dummy_tms;
SystemTicks::SystemTicks() : Ticks(times(&dummy_tms)) {}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
