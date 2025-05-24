
///////////////////////////////////////////////////////////////////////////////////////////////
// Localhost
///////////////////////////////////////////////////////////////////////////////////////////////

#include "localhost.h"

#include <lmcons.h>

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////

NetBIOSHostName Localhost::netBIOSName() const
{
	char name[MAX_COMPUTERNAME_LENGTH + 1];
	unsigned long n = sizeof(name);
	if (!GetComputerName(name, &n))
		THROW_OS(Exception)("cannot get computer name");
	return name;
}

//---------------------------------------------------------------------------------------------

string Localhost::loggedOnUserName() const
{
	char name[UNLEN + 1];
	unsigned long n = sizeof(name);
	if (!GetUserName(name, &n))
		THROW_OS(Exception)("cannot get logged-on user name");
	return name;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
