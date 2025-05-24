
#ifndef _magenta_net_address_
#define _magenta_net_address_

#include "magenta/exceptions/defs.h"
#include "magenta/lang/defs.h"

#include "magenta/net/classes.h"
#include "magenta/net/basics.h"

#include "rvfc/Include/rvfcString.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

#define IP_ADDRESS_TEXT_SIZE 64

///////////////////////////////////////////////////////////////////////////////////////////////
// Addresses
///////////////////////////////////////////////////////////////////////////////////////////////

class IPAddress
{
	friend class Host;
	friend class IPv4Address;
	friend class IPv4MappedAddress;
	friend class IPv6Address;
	friend class SocketAddress;
	friend class Socket;
	friend class TcpSocket;
	friend class UdpPeerSocket;

	typedef IPAddress This;

public:
	//-----------------------------------------------------------------------------------------
	// State
	//-----------------------------------------------------------------------------------------
protected:
	union
	{
#ifndef __VXWORKS__
		short ss_family;
#else
		struct
		{
			UINT8 len;
			UINT8 family;
		} ss;
#endif
		sockaddr_in in4;
		sockaddr_in6 in6;
	} _addr;

	//-----------------------------------------------------------------------------------------
	// Construction
	//-----------------------------------------------------------------------------------------
protected:
	void ctor();
	void ctor(int family, const char *text, bool numeric);
	void ctor(const This &addr);
	void ctor(const struct sockaddr *sa, socklen_t len);
	void ctor(const Host &host);

	bool set(const char *text, bool numeric, bool invalidate) throw()
		{ return set(AF_UNSPEC, text, numeric, invalidate); }
	bool set(int family, const char *text, bool numeric, bool invalidate) throw();
	bool setIp(int family, const char *text, bool numeric, bool invalidate) throw();
	void setIp(const IPAddress &ip) throw();
	void setNull(int family);

	struct __Empty__ {};
	IPAddress(__Empty__) {}

public:
	IPAddress() { ctor(); }
	IPAddress(const Null &null) { ctor(); }
	IPAddress(const Host &host) { ctor(host); }
	IPAddress(const std::string &addr, int family = AF_UNSPEC) { ctor(family, addr.c_str(), true); }
	IPAddress(const char *addr, int family = AF_UNSPEC) { ctor(family, addr, true); }
	IPAddress(const struct sockaddr *sa, socklen_t len) { ctor(sa, len); }

	This &operator=(const IPAddress &addr) { setIp(addr); return *this; }
	This &operator=(const std::string &addr) { return operator=(addr.c_str()); }
	This &operator=(const char *addr) { setIp(AF_UNSPEC, addr, true, true); return *this; }

	// read address from text, return false on error
	bool set(const std::string &addr, int family = AF_UNSPEC) throw() { return setIp(family, addr.c_str(), true, false); }
	bool set(const char *addr, int family = AF_UNSPEC) throw() { return setIp(family, addr, true, false); }

	//-----------------------------------------------------------------------------------------
	// Attributes
	//-----------------------------------------------------------------------------------------
public:
	int family() const;
	socklen_t addrLen() const;
	socklen_t sockaddrLen() const;

	NetOrder<UINT16, UINT16&> sockaddr_port();
	NetOrder<UINT16> sockaddr_port() const { return const_cast<This*>(this)->sockaddr_port(); }

	NetOrder<UINT32, UINT32&> in_addr_ulong();
	NetOrder<UINT32> in_addr_ulong() const { return const_cast<This*>(this)->in_addr_ulong(); }

public:
	struct sockaddr *sockaddr() { return reinterpret_cast<struct sockaddr *>(&_addr); }
	const struct sockaddr *sockaddr() const { return const_cast<This*>(this)->sockaddr(); }

	struct sockaddr_in *sockaddr_in() { return reinterpret_cast<struct sockaddr_in *>(sockaddr()); }
	struct sockaddr_in6 *sockaddr_in6() { return reinterpret_cast<struct sockaddr_in6 *>(sockaddr()); }

	const struct in_addr *in_addr() const { return &_addr.in4.sin_addr; }
	struct in_addr *in_addr() { return &_addr.in4.sin_addr; }
	const struct in6_addr *in6_addr() const { return &_addr.in6.sin6_addr; }
	struct in6_addr *in6_addr() { return &_addr.in6.sin6_addr; }

	static Block ipBlock(const IPAddress &ip);
	Block ipBlock() const;

public:
	bool isValid() const;
	bool operator!() const { return isNull(); }
	bool isNull() const;

	bool isV4() const;
	bool isV4Mapped() const;
	bool isV6() const;
	bool isLoopback() const;

	text text() const;

	// is reverse lookup possible?
	bool hasHostname() const;

	//-----------------------------------------------------------------------------------------
	// Comparison
	//-----------------------------------------------------------------------------------------
protected:
	int compare(const IPAddress &addr) const;

public:
	bool operator==(const IPAddress &addr) const;
	bool operator!=(const IPAddress &addr) const { return !operator==(addr); }
	bool operator<(const IPAddress &addr) const { return compare(addr) < 0; }
	bool operator<=(const IPAddress &addr) const { return compare(addr) <= 0; }
	bool operator>(const IPAddress &addr) const { return compare(addr) > 0; }
	bool operator>=(const IPAddress &addr) const { return compare(addr) >= 0; }

	IPv4Address v4Address() const;
	IPv6Address v6Address() const;
	IPv4MappedAddress v4MappedAddress() const;

	//-----------------------------------------------------------------------------------------
	// Errors
	//-----------------------------------------------------------------------------------------
protected:
	static int invalidFamily();
	void assertValid() const;
	void _invalidate();
	void _invalidate() const { magenta::_invalidate(); }

public:
	CLASS_EXCEPTION_DEF("magenta::IPAddress");
	CLASS_SUB_EXCEPTION_DEF(InvalidErr, "invalid address");
	CLASS_SUB_EXCEPTION_DEF(HostAddrErr, "host does not have an address");
};

//---------------------------------------------------------------------------------------------

class IPv4Address : public IPAddress
{
	typedef IPAddress Super;

public:
	IPv4Address() {}
	IPv4Address(const HostOrder<UINT32> &addr);
	IPv4Address(const struct in_addr &addr);
	IPv4Address(const Null &null) {}
	IPv4Address(const Host &host);
	IPv4Address(const std::string &addr);
	IPv4Address(const char *addr);
	IPv4Address(const struct sockaddr *sa, socklen_t len);
	IPv4Address(const IPAddress &addr);
	IPv4Address(const IPv4MappedAddress &addr);

	bool set(const std::string &addr) throw() { return set(addr.c_str()); }
	bool set(const char *addr) throw();

	CLASS_EXCEPTION_DEF("magenta::IPv4Address");
	CLASS_SUB_EXCEPTION_DEF(NotV4Err, "not IPv4 address");
};

//---------------------------------------------------------------------------------------------

class IPv4MappedAddress : public IPAddress
{
	typedef IPAddress Super;

protected:
	void ctor(const struct in_addr *ip);

public:
	IPv4MappedAddress() throw() {}
	IPv4MappedAddress(const Null &null) {}
	IPv4MappedAddress(const Host &host);
	IPv4MappedAddress(const std::string &addr);
	IPv4MappedAddress(const char *addr);
	IPv4MappedAddress(const struct sockaddr *sa, socklen_t len);
	IPv4MappedAddress(const IPv4Address &addr);
	IPv4MappedAddress(const IPAddress &addr);

	bool set(const std::string &addr) throw() { return set(addr.c_str()); }
	bool set(const char *addr) throw();

	CLASS_EXCEPTION_DEF("magenta::IPvMapped4Address");
	CLASS_SUB_EXCEPTION_DEF(NotV4MappedErr, "not IPv4-Mapped address");
};

//---------------------------------------------------------------------------------------------

class IPv6Address : public IPAddress
{
	typedef IPAddress Super;

public:
	IPv6Address() {}
	IPv6Address(const struct in6_addr &addr);
	IPv6Address(const Null &null) {}
	IPv6Address(const Host &host);
	IPv6Address(const std::string &addr);
	IPv6Address(const char *addr);
	IPv6Address(const struct sockaddr *sa, socklen_t len);
	IPv6Address(const IPAddress &addr);

	bool set(const std::string &addr) throw() { return set(addr.c_str()); }
	bool set(const char *addr) throw();

	CLASS_EXCEPTION_DEF("magenta::IPv6Address");
	CLASS_SUB_EXCEPTION_DEF(NotV6Err, "not IPv6 address");
};

//---------------------------------------------------------------------------------------------

class IPv6AddressPrefix : public IPv6Address
{
	typedef IPv6AddressPrefix This;
	typedef IPv6Address Super;

	UINT16 prefix_len;
	
	static const UINT16 defalt_prefix_len;

protected:
	void ctor(const char *addr);
	bool set(const char *addr, bool invalidate);

public:
	IPv6AddressPrefix() : prefix_len(0) {}
	IPv6AddressPrefix(const Null &null) : prefix_len(0)  {}

	IPv6AddressPrefix(const std::string &addr) { ctor(addr.c_str()); }
	IPv6AddressPrefix(const char *addr) { ctor(addr); }

	IPv6AddressPrefix(const IPv6Address &addr, int length = defalt_prefix_len);

	This &operator=(const IPv6Address &ip);
	This &operator=(const char *addr) { ctor(addr); return *this; }
	bool set(const char *addr);
	
	IPv6Address &ip() { return *this; }
	const IPv6Address &ip() const { return *this; }

	UINT16 prefixLength() const { return prefix_len; }
	UINT16 &prefixLength() { return prefix_len; }

	CLASS_EXCEPTION_DEF("magenta::IPv6AddressPrefix");
	CLASS_SUB_EXCEPTION_DEF(InvalidPrefixLengthErr, "invalid prefix length");
};

//---------------------------------------------------------------------------------------------

class LoopbackIPAddress : public IPAddress
{
protected:
	LoopbackIPAddress() {}
};

//---------------------------------------------------------------------------------------------

class LoopbackIPv4Address : public LoopbackIPAddress
{
	typedef LoopbackIPAddress Super;

public:
	LoopbackIPv4Address();
};

//---------------------------------------------------------------------------------------------

class LoopbackIPv6Address : public LoopbackIPAddress
{
	typedef LoopbackIPAddress Super;

public:
	LoopbackIPv6Address();
};

//---------------------------------------------------------------------------------------------

//typedef LoopbackIPv4Address LoopbackIPAddress;

//---------------------------------------------------------------------------------------------

class LocalIPAddress : public IPAddress
{
	typedef IPAddress Super;

public:
	LocalIPAddress();

	 // for the time being
	LocalIPAddress(const IPAddress &addr) : Super(addr) {}
	LocalIPAddress(const std::string &addr) : Super(addr) {}
	LocalIPAddress(const char *addr) : Super(addr) {}
	LocalIPAddress(const struct sockaddr *sa, socklen_t len) : Super(sa, len) {}
};

//---------------------------------------------------------------------------------------------

class AnyIPAddress : public IPAddress
{
public:
	AnyIPAddress();
};

///////////////////////////////////////////////////////////////////////////////////////////////

class SocketAddress : public IPAddress
{
	typedef SocketAddress This;
	typedef IPAddress Super;

	//-----------------------------------------------------------------------------------------
	// Construction
	//-----------------------------------------------------------------------------------------
protected:
	bool set(const char *addr, bool numeric, bool invalidate);
	void ctor(const char *addr);
	void ctor(const IPAddress &ip, const Port &port);

public:
	SocketAddress() {}
	SocketAddress(const Null &null) {}
	SocketAddress(const char *addr) : IPAddress(__Empty__()) { ctor(addr); }
	SocketAddress(const std::string &str) : IPAddress(__Empty__()) { ctor(str.c_str()); }
	SocketAddress(const char *ip, const Port &port) : IPAddress(__Empty__()) { ctor(IPAddress(ip), port); }
	SocketAddress(const IPAddress &ip, const Port &port) : IPAddress(__Empty__()) { ctor(ip, port); }
	SocketAddress(const SocketAddress &sa) : Super(sa.sockaddr(), sa.sockaddrLen()) {}
	SocketAddress(const Host &host, const Port &port);
	SocketAddress(struct sockaddr *sa, socklen_t len) : Super(sa, len) {}

//	This &operator=(const IPAddress &addr);
	This &operator=(const SocketAddress &sa);
	This &operator=(const std::string &addr) { return operator=(addr.c_str()); }
	This &operator=(const char *addr) { set(addr, true, true); return *this; }

	// read address from text, return false on error
	bool set(const CStringAbs &addr) throw() { return set(addr.c_str()); }
	bool set(const char *addr) throw() { return set(addr, true, false); }

	//-----------------------------------------------------------------------------------------
	// Attributes
	//-----------------------------------------------------------------------------------------
public:
	const IPAddress &ip() const { return *this; }
	IPAddress &ip() { return *this; }

	bool isNull() const { return !ip() && !port(); }
	bool operator!() const { return isNull(); }

	text text() const; // not virtual!

	//-----------------------------------------------------------------------------------------
	// Port
	//-----------------------------------------------------------------------------------------
public:
	template <class T>
	class _Port
	{
		typedef _Port<T> This;

		friend class SocketAddress;

		T &_sa;

	protected:
		_Port(T &sa) : _sa(sa) {}

		NetOrder<UINT16> netOrder() const { return _sa.ip().sockaddr_port(); }
		HostOrder<UINT16> hostOrder() const { return netOrder(); }

	public:
		_Port &operator=(const Port &port)
		{
			NetOrder<UINT16, UINT16&> sa_port = _sa.ip().sockaddr_port();
			sa_port = port;
			return *this;
		}

		operator Port() const { return Port(hostOrder()); }

		operator HostOrder<UINT16>() const { return hostOrder(); }
		operator NetOrder<UINT16>() const { return netOrder(); }

		bool operator==(const Port &p) const { return number() == p.number(); }
		bool operator!=(const Port &p) const { return !operator==(p); }

		bool operator!() const { return number() == 0; }

		int number() const { return hostOrder().number(); }
	};

	_Port<const SocketAddress> port() const { return _Port<const SocketAddress>(*this); }
	_Port<SocketAddress> port() { return _Port<SocketAddress>(*this); }

	//-----------------------------------------------------------------------------------------
	// Comparison
	//-----------------------------------------------------------------------------------------
protected:
	static Block addrBlock(const SocketAddress &ip);

	int compare(const SocketAddress &addr) const;

public:
	bool operator==(const SocketAddress &addr) const { return ip() == addr.ip() && port() == addr.port(); }
	bool operator!=(const SocketAddress &addr) const { return !operator==(addr); }
	bool operator<(const SocketAddress &addr) const { return compare(addr) < 0; }
	bool operator<=(const SocketAddress &addr) const { return compare(addr) <= 0; }
	bool operator>(const SocketAddress &addr) const { return compare(addr) > 0; }
	bool operator>=(const SocketAddress &addr) const { return compare(addr) >= 0; }

	//-----------------------------------------------------------------------------------------
	// Errors
	//-----------------------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::SocketAddress");
	CLASS_SUB_EXCEPTION_DEF(InvalidErr, "invalid socket address");
};

//---------------------------------------------------------------------------------------------

class LocalSocketAddress : public SocketAddress
{
public:
	LocalSocketAddress() {}
	LocalSocketAddress(const Port &port) : SocketAddress(AnyIPAddress(), port) {}
	LocalSocketAddress(const LocalIPAddress &ip) : SocketAddress(ip, Port(0)) {}
	LocalSocketAddress(const LocalIPAddress &ip, const Port &port) : SocketAddress(ip, port) {}
	LocalSocketAddress(const SocketAddress &sa) : SocketAddress(sa) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_net_address_
