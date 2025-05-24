
#ifndef _magenta_system_windows_
#define _magenta_system_windows_

#include <stdio.h>
#include <algorithm>

using std::min;
using std::max;

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winsock2.h>

#endif // _magenta_system_windows_
