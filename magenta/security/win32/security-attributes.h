
///////////////////////////////////////////////////////////////////////////////////////////////
// Security Attributes
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_security_win32_security_attributes_
#define _magenta_security_win32_security_attributes_

#ifdef _WIN32
#include "magenta/system/windows.h"
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class SecurityAttributes
{
	SECURITY_ATTRIBUTES sa;

public:
	SecurityAttributes()
	{
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
	}

	operator const LPSECURITY_ATTRIBUTES() const { return (const LPSECURITY_ATTRIBUTES) &sa; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_security_win32_security_attributes_
