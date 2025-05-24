
#include "address.h"
#include "host.h"
#include "block.h"

#if defined(_WIN32)
#include <ws2tcpip.h>
#include <mstcpip.h>

#elif defined(__linux__)
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>

#elif defined(__VXWORKS__)
#include <wrn/coreip/net/utils/netstat.h>
#endif

#include "magenta/text/defs.h"

namespace magenta
{

using magenta::stringf;

///////////////////////////////////////////////////////////////////////////////////////////////
// IPAddress
///////////////////////////////////////////////////////////////////////////////////////////////

void IPAddress::ctor()
{                                           
	setNull(AF_INET); //@@
}

//---------------------------------------------------------------------------------------------

void IPAddress::ctor(int family, const char *addr, bool numeric)
{
	set(family, addr, numeric, true);
}

//---------------------------------------------------------------------------------------------

void IPAddress::ctor(const This &addr)
{
	if (this == &addr)
		return;
	memcpy(&_addr, &addr._addr, sizeof(_addr));
}

//---------------------------------------------------------------------------------------------

void IPAddress::ctor(const struct sockaddr *sa, socklen_t len)
{
	if (!sa)
	{
		ERROR_EX(InvalidErr);
		return;
	}

	switch (sa->sa_family)
	{
	case AF_INET:
		if (len != sizeof(struct sockaddr_in))
		{
			ERROR_EX(InvalidErr)("assert failed: len == sizeof(struct sockaddr_in), len=%d", len);
			return;
		}
		break;

	case AF_INET6:
		if (len != sizeof(struct sockaddr_in6))
		{
			ERROR_EX(InvalidErr)("assert failed: len == sizeof(struct sockaddr_in6), len=%d", len);
			return;
		}
		break;

	default:
		ERROR_EX(InvalidErr)("unrecognized sa_family: %d", sa->sa_family);
		return;
	}

	memcpy(&_addr, sa, len);
}

//---------------------------------------------------------------------------------------------

void IPAddress::ctor(const Host &host)
{
	if (host.isAddress())
		ctor(host.explicitAddress());
	else
	{
		Host::Addresses addrs = host.addresses();
		Host::Addresses::Iterator i = addrs;
		if (!i)
		{
			ERROR_EX(HostAddrErr)("host %s", host.name().c_str());
			return;
		}
		ctor(*i);
	}
}

//---------------------------------------------------------------------------------------------

bool IPAddress::set(int family, const char *addr, bool numeric, bool invalidate) throw()
{
	if (*addr == '\0')
	{
		setNull(family);
		return true;
	}

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family; //AF_UNSPEC;
	hints.ai_protocol = 0;
	if (numeric)
		hints.ai_flags = AI_NUMERICHOST;

#ifdef __linux__
	char ip_s[IP_ADDRESS_TEXT_SIZE];
	if (*addr == '[')
	{
		int n = strlen(addr);
		if (addr[n-1] != ']')
		{
			if (invalidate)
				ERROR_EX(InvalidErr)("missing ']': addr=%s", addr);
			return false;
		}
		strncpy(ip_s, addr + 1, sizeof(ip_s));
		ip_s[n-2] = '\0';
		addr = ip_s;
	}
#endif // __linux__

	auto_free<struct addrinfo*> info;
	int rc = getaddrinfo(addr, 0, &hints, &info);
	if (rc || !info)
	{
		if (invalidate)
			ERROR_EX(InvalidErr)("%s (addr=%s)", gai_strerror(rc), addr);
		return false;
	}

	if (info->ai_family == AF_INET || info->ai_family == AF_INET6)
		memcpy(&_addr, info->ai_addr, info->ai_addrlen);
	else
	{
		if (invalidate)
			ERROR_EX(InvalidErr)("invalid address family [%d]", info->ai_family);
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------------

// set ip, preserve port

bool IPAddress::setIp(int family, const char *text, bool numeric, bool invalidate) throw()
{
	Port::NetType p(sockaddr_port());
	if (! set(family, text, numeric, invalidate))
		return false;
	sockaddr_port() = p;
	return true;
}

//---------------------------------------------------------------------------------------------

// set ip, preserve port

void IPAddress::setIp(const IPAddress &ip) throw()
{
	Port::NetType p(sockaddr_port());
	ctor(ip);
	sockaddr_port() = p;
}

//---------------------------------------------------------------------------------------------

void IPAddress::setNull(int family)
{
	memset(&_addr, 0, sizeof(_addr));
	if (family == AF_INET)
	{
		_addr.ss_family = AF_INET;
		_addr.in4.sin_addr.s_addr = INADDR_ANY;
	}
	else if (family == AF_INET6)
	{
		_addr.ss_family = AF_INET6;
		_addr.in6.sin6_addr = in6addr_any;
	}
	else
		ERROR_EX(InvalidErr)("invalid address family [%d]", family);
}

//---------------------------------------------------------------------------------------------

void IPAddress::_invalidate()
{
	_addr.ss_family = invalidFamily();
}

///////////////////////////////////////////////////////////////////////////////////////////////

int IPAddress::invalidFamily()
{
	return -1;
}

//---------------------------------------------------------------------------------------------

IPAddress::String IPAddress::text() const
{
	char host[NI_MAXHOST];
	int rc = getnameinfo(sockaddr(), sockaddrLen(), host, sizeof(host), 0, 0, NI_NUMERICHOST);
	if (rc)
	{
		ERROR_EX(InvalidErr)("conversion to text (%s)", gai_strerror(rc));
		return "";
	}
	return host;
}

//---------------------------------------------------------------------------------------------

bool IPAddress::hasHostname() const
{
	if (isNull())
		return false;

	char host[NI_MAXHOST];
	int rc = getnameinfo(sockaddr(), sockaddrLen(), host, sizeof(host), 0, 0, 0);
	return !rc;
}

//---------------------------------------------------------------------------------------------

int IPAddress::family() const
{
	if (!_addr.ss_family)
		const_cast<This*>(this)->_addr.ss_family = AF_INET;
	return _addr.ss_family;
}

//---------------------------------------------------------------------------------------------

socklen_t IPAddress::addrLen() const
{
	switch (family())
	{
	case AF_INET:
		return sizeof(struct in_addr);

	case AF_INET6:
		return sizeof(struct in6_addr);

	default:
		return 0;
	}
}

//---------------------------------------------------------------------------------------------

socklen_t IPAddress::sockaddrLen() const 
{
	switch (family())
	{
	case AF_INET:
		return sizeof(_addr.in4);

	case AF_INET6:
		return sizeof(_addr.in6);

	default:
		return 0;
	}
}

//---------------------------------------------------------------------------------------------

NetOrder<UINT16, UINT16&> IPAddress::sockaddr_port()
{
	if (isNull() || family() == AF_INET)
		return sockaddr_in()->sin_port;
	else if (family() == AF_INET6)
		return sockaddr_in6()->sin6_port;
	else
	{
		ERROR_EX(Error)("unknown address family");
		return sockaddr_in()->sin_port;
	}
}

//---------------------------------------------------------------------------------------------

NetOrder<UINT32, UINT32&> IPAddress::in_addr_ulong()
{
#if defined(_WIN32)
	return (UINT32 &) _addr.in4.sin_addr.S_un.S_addr;
#elif defined(__linux__)
	return _addr.in4.sin_addr.s_addr;
#endif
}

//---------------------------------------------------------------------------------------------

Block IPAddress::ipBlock(const IPAddress &ip)
{
	if (ip.isV4())
		return Block((UINT8 *) ip.in_addr(), sizeof(struct in_addr), sizeof(struct in_addr));
	else
		return Block((UINT8 *) ip.in6_addr(), sizeof(struct in6_addr), sizeof(struct in6_addr));
}

Block IPAddress::ipBlock() const
{
	return ipBlock(*this);
}

//---------------------------------------------------------------------------------------------
// Predicates
//---------------------------------------------------------------------------------------------

bool IPAddress::isValid() const
{
	return family() != invalidFamily();
}

//---------------------------------------------------------------------------------------------

bool IPAddress::isNull() const
{
	static const NetOrder<UINT32> inaddr_any = HostOrder<UINT32>(INADDR_ANY);

	switch (_addr.ss_family)
	{
	case 0:
		return true;

	case AF_INET:
		return in_addr_ulong() == inaddr_any;

	case AF_INET6:
		return !memcmp(in6_addr(), &in6addr_any, sizeof(in6addr_any));

	default:
		ERROR_EX(InvalidErr)("unrecognized sa_family: %d", _addr.ss_family);
		return false;
	}
}

//---------------------------------------------------------------------------------------------

bool IPAddress::isV4() const
{
	if (isNull())
		return true;
	return sockaddr()->sa_family == AF_INET;
}

//---------------------------------------------------------------------------------------------

bool IPAddress::isV4Mapped() const
{
	if (isNull())
		return true;
	return isV6() && IN6_IS_ADDR_V4MAPPED(in6_addr());
}

//---------------------------------------------------------------------------------------------

bool IPAddress::isV6() const
{
	if (isNull())
		return true;
	return sockaddr()->sa_family == AF_INET6;
}

//---------------------------------------------------------------------------------------------

bool IPAddress::isLoopback() const
{
	static const NetOrder<UINT32> inaddr_loopback = HostOrder<UINT32>(INADDR_LOOPBACK);

	switch (_addr.ss_family)
	{
	case AF_INET:
		return in_addr_ulong() == inaddr_loopback;

	case AF_INET6:
		return !memcmp(in6_addr(), &in6addr_loopback, sizeof(in6addr_loopback));

	default:
		ERROR_EX(InvalidErr)("unrecognized sa_family: %d", _addr.ss_family);
		return false;
	}
}

//---------------------------------------------------------------------------------------------
// Comparison
//---------------------------------------------------------------------------------------------

bool IPAddress::operator==(const IPAddress &addr) const
{
	if (isNull() && addr.isNull())
		return true;

	return family() == addr.family() && ipBlock() == addr.ipBlock();
}

//---------------------------------------------------------------------------------------------

int IPAddress::compare(const IPAddress &addr) const
{
	Block ip1 = ipBlock(*this), ip2 = ipBlock(addr);
	return ip1.compare(ip2);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// IPv4Address
///////////////////////////////////////////////////////////////////////////////////////////////

IPv4Address::IPv4Address(const HostOrder<UINT32> &addr) : IPAddress(__Empty__())
{
	_addr.ss_family = AF_INET;
#if defined(_WIN32)
	_addr.in4.sin_addr.S_un.S_addr = addr.net().number();
#elif defined(__linux__)
	_addr.in4.sin_addr.s_addr = addr.net().number();
#endif
}

//---------------------------------------------------------------------------------------------

IPv4Address::IPv4Address(const Host &host)
{
	if (host.isAddress())
	{
		Super::ctor(host.explicitAddress());
		if (family() != AF_INET)
			ERROR_EX(NotV4Err)("address: %s", host.name().c_str());
	}
	else
	{
		Host::Addresses addrs = host.addresses();
		for (Host::Addresses::Iterator i = addrs; !!i; ++i)
		{
			if ((*i).isV4())
			{
				ctor(*i);
				return;
			}
		}
		ERROR_EX(HostAddrErr)("host %s", host.name().c_str());
	}
}

//---------------------------------------------------------------------------------------------

IPv4Address::IPv4Address(const CStringAbs &addr) : IPAddress(addr)
{
	if (family() != AF_INET)
		ERROR_EX(NotV4Err)("address: %s", addr.c_str());
}

//---------------------------------------------------------------------------------------------

IPv4Address::IPv4Address(const char *addr) : IPAddress(addr)
{
	if (family() != AF_INET)
		ERROR_EX(NotV4Err)("address: %s", addr);
}

//---------------------------------------------------------------------------------------------

IPv4Address::IPv4Address(const struct sockaddr *sa, socklen_t len) : IPAddress(sa, len)
{
	if (family() != AF_INET)
		ERROR_EX(NotV4Err)("address: %s", text().c_str());
}

//---------------------------------------------------------------------------------------------

IPv4Address::IPv4Address(const IPAddress &addr) : IPAddress(addr)
{
	if (family() != AF_INET)
		ERROR_EX(NotV4Err)("address: %s", text().c_str());
}

//---------------------------------------------------------------------------------------------

bool IPv4Address::set(const char *addr) throw()
{
	IPAddress ip;
	if (! ip.set(addr) || ip.family() != AF_INET)
		return false;
	*this = ip;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// IPv4MappedAddress
///////////////////////////////////////////////////////////////////////////////////////////////

void IPv4MappedAddress::ctor(const struct in_addr *ip)
{
#ifdef _WIN32
	IN6_SET_ADDR_V4MAPPED(in6_addr(), ip);
#else
//	UINT32 *p = (UINT32 *) &sockaddr_in6()->sin6_addr.s6_addr[0];
	UINT32 *p = &sockaddr_in6()->sin6_addr.s6_addr32[0];
	p[0] = 0;
	p[1] = 0;
	p[2] = htonl(0xffff);
	p[3] = ip->s_addr;
#endif
}

//---------------------------------------------------------------------------------------------

IPv4MappedAddress::IPv4MappedAddress(const Host &host)
{
	if (host.isAddress())
	{
		Super::ctor(host.explicitAddress());
		if (family() != AF_INET)
			ERROR_EX(NotV4MappedErr)("address: %s", host.name().c_str());
	}
	else
	{
		Host::Addresses addrs = host.addresses();
		for (Host::Addresses::Iterator i = addrs; !!i; ++i)
		{
			if ((*i).isV4Mapped())
			{
				ctor((*i).in_addr());
				return;
			}
		}
		ERROR_EX(HostAddrErr)("host %s", host.name().c_str());
	}
}

//---------------------------------------------------------------------------------------------

IPv4MappedAddress::IPv4MappedAddress(const CStringAbs &addr) : IPAddress(addr)
{
	if (!isV4Mapped())
		ERROR_EX(NotV4MappedErr)("address: %s", addr.c_str());
}

//---------------------------------------------------------------------------------------------

IPv4MappedAddress::IPv4MappedAddress(const char *addr) : IPAddress(addr)
{
	if (!isV4Mapped())
		ERROR_EX(NotV4MappedErr)("address: %s", addr);
}

//---------------------------------------------------------------------------------------------

IPv4MappedAddress::IPv4MappedAddress(const struct sockaddr *sa, socklen_t len) : IPAddress(sa, len)
{
	if (!isV4Mapped())
		ERROR_EX(NotV4MappedErr)("address: %s", text().c_str());
}

//---------------------------------------------------------------------------------------------

IPv4MappedAddress::IPv4MappedAddress(const IPv4Address &addr)
{
	ctor(addr.in_addr());
}

//---------------------------------------------------------------------------------------------

IPv4MappedAddress::IPv4MappedAddress(const IPAddress &addr) : IPAddress(__Empty__())
{
	if (addr.isV4())
		ctor(addr.in_addr());
	else if (addr.isV4Mapped())
		_addr = addr._addr;
	else
		ERROR_EX(NotV4MappedErr)("address: %s", addr.text().c_str());
}

//---------------------------------------------------------------------------------------------

bool IPv4MappedAddress::set(const char *addr) throw()
{
	IPAddress ip;
	if (!ip.set(addr) || !ip.isV4Mapped())
		return false;
	*this = ip;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// IPv6Address
///////////////////////////////////////////////////////////////////////////////////////////////

IPv6Address::IPv6Address(const struct in6_addr &addr)
{
	_addr.in6.sin6_family = AF_INET6;
	_addr.in6.sin6_port = 0;
	_addr.in6.sin6_flowinfo = 0;
	_addr.in6.sin6_addr = addr;
	_addr.in6.sin6_scope_id = 0;
}

//---------------------------------------------------------------------------------------------

IPv6Address::IPv6Address(const Host &host)
{
	if (host.isAddress())
	{
		Super::ctor(host.explicitAddress());
		if (family() != AF_INET6)
			ERROR_EX(NotV6Err)("address: %s", host.name().c_str());
	}
	else
	{
		Host::Addresses addrs = host.addresses();
		for (Host::Addresses::Iterator i = addrs; !!i; ++i)
		{
			if ((*i).isV6())
			{
				ctor(*i);
				return;
			}
		}
		ERROR_EX(HostAddrErr)("host %s", host.name().c_str());
	}
}

//---------------------------------------------------------------------------------------------

IPv6Address::IPv6Address(const CStringAbs &addr) : IPAddress(addr)
{
	if (family() != AF_INET6)
		ERROR_EX(NotV6Err)("address: %s", addr.c_str());
}

//---------------------------------------------------------------------------------------------

IPv6Address::IPv6Address(const char *addr) : IPAddress(addr)
{
	if (family() != AF_INET6)
		ERROR_EX(NotV6Err)("address: %s", addr);
}

//---------------------------------------------------------------------------------------------

IPv6Address::IPv6Address(const struct sockaddr *sa, socklen_t len) : IPAddress(sa, len)
{
	if (family() != AF_INET6)
		ERROR_EX(NotV6Err)("address: %s", text().c_str());
}

//---------------------------------------------------------------------------------------------

IPv6Address::IPv6Address(const IPAddress &addr) : IPAddress(addr)
{
	if (family() != AF_INET6)
		ERROR_EX(NotV6Err)("address: %s", text().c_str());
}

//---------------------------------------------------------------------------------------------

bool IPv6Address::set(const char *addr) throw()
{
	IPAddress ip;
	if (! ip.set(addr) || ip.family() != AF_INET6)
		return false;
	*this = ip;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// IPv6AddressPrefix
///////////////////////////////////////////////////////////////////////////////////////////////

const UINT16 IPv6AddressPrefix::defalt_prefix_len = 64;

//---------------------------------------------------------------------------------------------

void IPv6AddressPrefix::ctor(const char *addr)
{
	set(addr, true);
}

//---------------------------------------------------------------------------------------------

bool IPv6AddressPrefix::set(const char *addr, bool invalidate)
{
	const char *p = strchr(addr, '/');
	if (!p)
	{
		if (! IPAddress::set(AF_INET6, addr, true, invalidate))
			return false;
		prefix_len = defalt_prefix_len;
		return true;
	}

	char ip_s[IP_ADDRESS_TEXT_SIZE];
	strncpy(ip_s, addr, p - addr);
	ip_s[p - addr] = '\0';

	if (! IPAddress::set(AF_INET6, ip_s, true, invalidate))
		return false;

	int n;
	int rc = sscanf(p + 1, "%d", &n);
	if (rc != 1 || n < 0 || n > 128)
	{
		if (invalidate)
			ERROR_EX(InvalidPrefixLengthErr)("%s", p + 1);
		return false;
	}

	prefix_len = n;
	return true;
}

//---------------------------------------------------------------------------------------------

IPv6AddressPrefix::IPv6AddressPrefix(const IPv6Address &addr, int prefixLen)
{
	Super::ctor(addr);
	prefix_len = prefixLen;
}

//---------------------------------------------------------------------------------------------

IPv6AddressPrefix &IPv6AddressPrefix::operator=(const IPv6Address &ip)
{
	Super::operator=(ip);
	prefix_len = defalt_prefix_len;
	return *this;
}

//---------------------------------------------------------------------------------------------

bool IPv6AddressPrefix::set(const char *addr)
{
	return set(addr, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// LoopbackIPAddress
///////////////////////////////////////////////////////////////////////////////////////////////

LoopbackIPv4Address::LoopbackIPv4Address()
{
	Super::ctor(AF_INET, "127.0.0.1", true);
}

///////////////////////////////////////////////////////////////////////////////////////////////

LoopbackIPv6Address::LoopbackIPv6Address()
{
	Super::ctor(AF_INET6, "::1", true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// LocalIPAddress
///////////////////////////////////////////////////////////////////////////////////////////////

LocalIPAddress::LocalIPAddress()
{
	char name[NI_MAXHOST];
	int rc = gethostname(name, sizeof(name));
	if (rc)
	{
		ERROR_EX(InvalidErr)("cannot determine hostname: %s", gai_strerror(rc));
		return;
	}
	Super::ctor(AF_UNSPEC, name, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// LocalIPAddress
///////////////////////////////////////////////////////////////////////////////////////////////

AnyIPAddress::AnyIPAddress()
{
	setNull(AF_INET); //@@
}

///////////////////////////////////////////////////////////////////////////////////////////////
// SocketAddress
///////////////////////////////////////////////////////////////////////////////////////////////

bool SocketAddress::set(const char *addr, bool numeric, bool invalidate)
{
	if (!addr)
	{
		Super::ctor();
		return true;
	}

	bool brackets = *addr == '[';

	if (!brackets)
	{
		// the following options are possible:
		// (a) 1.2.3.4 - invalid (no port)
		// (b) 1.2.3.4:port - ok
		// (c) 1.2.3.4:5:6:7:8 - invalid (no port)
		// (d) 1.2.3.4:5:6:7:8:port - invalid (should be [1:2:3:4]:port)
		// (x) something entirely different

		// does string is just an ip address (w/o port)?
		IPAddress ip;
		if (ip.set(addr, true, false))
		{
			// if so, fail: this excludes (a) and (c)
			if (invalidate)
				ERROR_EX(InvalidErr)("no port specification");
			return false;
		}

		// we're left with (b), (d), and (x)
	}

	// the following options are possible:
	// (b) 1.2.3.4:port - ok
	// (d) 1:2:3:4:5:6:7:8:port - invalid (should be [1:2:3:4:5:6:7:8]:port)
	// (e) [1:2:3:4:5:6:7:8] - invalid (no port)
	// (f) [1:2:3:4:5:6:7:8]:port - ok
	// (x) something entirely different

	// (d), (e), and (x) should fail

	int port_n;
	char ip_s[IP_ADDRESS_TEXT_SIZE];
	const char *p = strrchr(addr, ':');
	if (!p)
	{
		// exclude (e)
		if (invalidate)
			ERROR_EX(InvalidErr)("no port specification");
		return false;
	}

	int rc = sscanf(p + 1, "%d", &port_n);
	if (rc != 1 || port_n < 0)
	{
		if (invalidate)
			ERROR_EX(InvalidErr)("invalid port specification");
		return false;
	}

	strncpy(ip_s, addr, p - addr);
	ip_s[p - addr] = '\0';
	if (! Super::set(ip_s, numeric, invalidate))
	{
		// exclude (x)
		return false;
	}
	
	// exclude (d)
	if (isV6() && !brackets)
	{
		ERROR_EX(InvalidErr)("IPv6 socket address should have square brackets");
		return false;
	}

	port() = port_n;
	return true;
}

//---------------------------------------------------------------------------------------------

void SocketAddress::ctor(const IPAddress &ip, const Port &port_)
{
	if (!ip.isValid())
	{
		_invalidate();
		return;
	}

	_addr = ip._addr;
	port() = port_;
}

//---------------------------------------------------------------------------------------------

void SocketAddress::ctor(const char *addr)
{
	set(addr, true, true);
}

//---------------------------------------------------------------------------------------------

SocketAddress::SocketAddress(const Host &host, const Port &port) : IPAddress(__Empty__())
{
	IPAddress ip(host);
	ctor(ip, port);
}

//---------------------------------------------------------------------------------------------
/*
SocketAddress &SocketAddress::operator=(const IPAddress &addr)
{
	if (this == &addr)
		return *this;

	Super::setIp(addr);
	return *this; 
}
*/
//---------------------------------------------------------------------------------------------

SocketAddress &SocketAddress::operator=(const SocketAddress &sa)
{ 
	if (this == &sa)
		return *this;
	Super::ctor(sa.sockaddr(), sa.sockaddrLen());
	return *this; 
}
/*
//---------------------------------------------------------------------------------------------

bool SocketAddress::set(const char *addr) throw() 
{
	set
	This sa(addr);
	if (!sa.isValid())
		return false;
	*this = sa;
	return true;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////

IPAddress::String SocketAddress::text() const
{
	return stringf(isV4() && !ip().isNull() ? "%s:%d" : "[%s]:%d", ip().text().c_str(), port().number());
}

//---------------------------------------------------------------------------------------------

Block SocketAddress::addrBlock(const SocketAddress &addr)
{
	UINT32 n = addr.sockaddrLen();
	return Block((UINT8 *) addr.sockaddr(), n, n);
}

//---------------------------------------------------------------------------------------------

int SocketAddress::compare(const SocketAddress &addr) const
{
	Block a1 = addrBlock(*this), a2 = addrBlock(addr);
	return a1.compare(a2);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
