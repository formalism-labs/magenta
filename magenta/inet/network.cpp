
///////////////////////////////////////////////////////////////////////////////////////////////
// Network Classes
///////////////////////////////////////////////////////////////////////////////////////////////

#include "network.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Host
///////////////////////////////////////////////////////////////////////////////////////////////

HostAddress Host::ByName::hostAddress() const
{
   hostent *entry = gethostbyname((char *) _name.c_str());
   if (!entry)
        THROW_EX(Host::Exception);
   return *entry;
}

//---------------------------------------------------------------------------------------------

Host::Name Host::ByAddress::name() const
{
    hostent *entry = gethostbyaddr((char *) _addr.physical().data(),
		_addr.physical().length(), _addr.type());
    if (!entry)
        THROW_EX(Host::Exception);
    return string(entry->h_name, sizeof(entry->h_name));
}

///////////////////////////////////////////////////////////////////////////////////////////////

LocalHost::LocalHost()
{
    char s[256];
    if (gethostname(s, sizeof(s)))
        THROW_EX(Exception);
	_host = new ByName(string(s, sizeof(s)));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Protorol
///////////////////////////////////////////////////////////////////////////////////////////////

Protocol::Number Protocol::ByName::number() const
{
    protoent *entry = getprotobyname((char *) _name.c_str());
    if (!entry)
        THROW_EX(Protocol::Exception);
    return entry->p_proto;
}

//---------------------------------------------------------------------------------------------

Protocol::Name Protocol::ByNumber::name() const
{
    protoent *entry = getprotobynumber(_number);
    if (!entry)
        THROW_EX(Protocol::Exception);
    return string(entry->p_name);
}

///////////////////////////////////////////////////////////////////////////////////////////////

AnyProtocol::AnyProtocol()
{
}

//---------------------------------------------------------------------------------------------

IProtocol *AnyProtocol::duplicate() const
{
    return new AnyProtocol();
}

//---------------------------------------------------------------------------------------------

Protocol::Name AnyProtocol::name() const
{
    return Name();
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespece magenta_inet_1
