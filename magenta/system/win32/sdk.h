
#ifndef _maganta_system_win32_sdk_
#define _maganta_system_win32_sdk_

///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4786)
#if _MSC_VER > 1300
#pragma warning(disable : 4996)
#endif

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#endif // _WIN32

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _maganta_system_win32_sdk_
