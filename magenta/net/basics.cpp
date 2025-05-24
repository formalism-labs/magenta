
#include "basics.h"

#if defined(_WIN32)
#include <ws2tcpip.h>

#elif defined(__linux__)
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>

#elif defined(__VXWORKS__)
#include <wrn/coreip/net/utils/netstat.h>
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Protocol
///////////////////////////////////////////////////////////////////////////////////////////////

Protocol::Protocol(const char *name)
{
	if (!name)
		name = "";
#ifndef __VXWORKS__
	struct protoent *proto = getprotobyname(name);
	if (!proto)
	{
		ERROR_OS(InvalidErr)("proto=%s", name);
		return;
	}
	_num = proto->p_proto;
#else
	struct protox *proto = name2protox((char *) name);
	if (!proto)
	{
		ERROR_EX(InvalidErr)("proto=%s", name);
		return;
	}
	_num = proto->pr_usesysctl;
#endif
}

//---------------------------------------------------------------------------------------------

text Protocol::name() const
{
#ifndef __VXWORKS__
	struct protoent *proto = getprotobynumber(_num);
	if (!proto)
		return "";
	return proto->p_name;
#else
	extern struct protox *protoprotox[];
	struct protox **tpp, *tp;
	for (tpp = protoprotox; *tpp; tpp++)
		for (tp = *tpp; tp->pr_name; tp++)
			if (tp->pr_usesysctl == _num)
				return tp->pr_name;
	return "";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Port
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// Service
///////////////////////////////////////////////////////////////////////////////////////////////

Service::Service(const char *name)
{
	if (!name)
		name = "";

	struct servent *serv = getservbyname(name, 0);
	if (!serv)
	{
		ERROR_EX(InvalidErr)("name=%s", name);
		return;
	}

	_port = Port(NetOrder<UINT16>(serv->s_port));
	if (!serv->s_proto || !*serv->s_proto)
	{		
		ERROR_OS(UnknownProtocol)("service name=%s", name);
		return;
	}
	_proto = Protocol(serv->s_proto);

#if 0
	struct addrinfo hints;  
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;

	auto_free<struct addrinfo*> res;
	int rc = getaddrinfo(0, name, &hints, &res);
	if (rc || !res)
	{
#ifdef _WIN32
		SetLastError(rc);
#endif
		ERROR_OS(InvalidErr)("name=%s", name);
		return;
	}

	_proto = res->ai_protocol;

	switch (res->ai_family)
	{
	case AF_INET:
		_port = ((struct sockaddr_in *) res->ai_addr)->sin_port;
		break;

	case AF_INET6:
		_port = ((struct sockaddr_in6 *) res->ai_addr)->sin6_port;
		break;

	default:
		ERROR_EX(InvalidErr)("name=%s", name);
		return;
	}
#endif // 0
}

//---------------------------------------------------------------------------------------------

Service::Service(const char *name, const Protocol &proto) : _proto(proto)
{
	if (!name)
		name = "";
	_name = name;
	struct servent *serv = getservbyname(name, proto.name().c_str());
	if (!serv)
	{
		ERROR_EX(InvalidErr)("name=%s proto=%s", name, proto.name().c_str());
		return;
	}
	_port = Port(NetOrder<UINT16>(serv->s_port));
}

//---------------------------------------------------------------------------------------------

Service::Service(const Port &port, const Protocol &proto) :
	_port(port), _proto(proto)
{
	struct servent *serv = getservbyport(port.number(), proto.name().c_str());
//	if (!serv)
//	{
//		ERROR_EX(InvalidErr)("port=%d proto=%d", port.number(), proto.number());
//		return
//	}
	if (serv)
		_name = serv->s_name;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
