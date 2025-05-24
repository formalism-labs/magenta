
#ifndef _magenta_system_general_
#define _magenta_system_general_

// #include "magenta/system/classes.h"

namespace magenta
{
namespace System
{

///////////////////////////////////////////////////////////////////////////////////////////////

enum class OsType
{
	Linux,
	Solaris,
	Windows,
	MacOSX,
	FreeBSD
};

enum class OS
{
	UbuntuLinux,
	FeforaLinux,
	CentOSLinux,
	Windows,
	WindowsServer,
	Solaris,
	MacOSX,
	FreeBSD
};

OsType ostype();
OS os();
unsigned int cpu_count();

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace System
} // namespace magenta

#endif // _magenta_system_general_
