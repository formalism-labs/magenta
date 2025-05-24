
///////////////////////////////////////////////////////////////////////////////////////////////
// General
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_inet_general_
#define _magenta_inet_general_

#include "magenta/inet/classes.h"

#ifdef _WIN32
#include "winsock2.h"
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"

#include <string>

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class Manager
{
#ifdef _WIN32
	WSADATA _data;
#endif

public:
	Manager();
	virtual ~Manager();

	CLASS_EXCEPTION_DEF("inet_magenta::Manager");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_inet_general_

