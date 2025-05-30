
///////////////////////////////////////////////////////////////////////////////////////////////
// Internet Classes
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma warning (disable : 4250)

#include "magenta/inet/general.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Manager
///////////////////////////////////////////////////////////////////////////////////////////////

Manager::Manager()
{
#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
 	int rc = WSAStartup(version, &_data);
	if (rc != 0) 
		THROW_EX(Exception);
#endif
}

//---------------------------------------------------------------------------------------------

Manager::~Manager()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
