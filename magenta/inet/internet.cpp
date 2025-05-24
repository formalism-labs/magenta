
///////////////////////////////////////////////////////////////////////////////////////////////
// Internet Classes
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma warning (disable : 4250)

#include "magenta/inet/internet.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Service
///////////////////////////////////////////////////////////////////////////////////////////////

servent Service::ByNameAndProtocol::entry() const
{
    const char *proto_name = _proto.name().empty() ? 0 : (const char *) _proto.name().c_str();
    servent *entry = getservbyname(_name.c_str(), proto_name);
    if (!entry)
        THROW_EX(Exception);
    return *entry;
}

//---------------------------------------------------------------------------------------------

servent Service::ByPortAndProtocol::entry() const
{
	const char *proto_name = _proto.name().empty() ? 0 : _proto.name().c_str();
	servent *entry = getservbyport(_port.physical(), proto_name);
	if (!entry)
		THROW_EX(Exception);
	return *entry;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// IP Address
///////////////////////////////////////////////////////////////////////////////////////////////

IPAddress::IPAddress(Byte n1, Byte n2, Byte n3, Byte n4)
{
    _addr.s_addr = htonl(n1 << 24 | n2 << 16 | n3 << 8 | n4);
}

//---------------------------------------------------------------------------------------------

IPAddress::IPAddress(const string &s)
{
    unsigned long n = inet_addr(s.c_str());
    if (n == INADDR_NONE)
        THROW_EX(Exception);
    ipStorage().s_addr = n;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
