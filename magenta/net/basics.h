
#ifndef _magenta_net_basics_
#define _magenta_net_basics_

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <in6addr.h>
#include <windows.h>

#pragma warning(disable : 4290)

#elif defined(__linux__)
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

#elif defined(__VXWORKS__)
#include <inetLib.h> 
#endif

#include "magenta/exceptions/defs.h"
#include "magenta/net/classes.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class auto_free
{
	T p;

public:
	auto_free(T t = 0) : p(t) {}
	~auto_free() {}

	operator T() const { return p; }
	T operator->() { return p; }

	bool operator!() const { return !p; }

	T *operator&() { return &p; }
	const T *operator&() const { return &p; }
};

//---------------------------------------------------------------------------------------------

template <>
inline auto_free<struct addrinfo *>::~auto_free() 
{
	if (p)
		freeaddrinfo(p);
}

#ifdef __linux__

template <>
inline auto_free<struct ifaddrs *>::~auto_free() 
{
	if (p)
		freeifaddrs(p);
}

#endif // __linux__

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class M = T> class HostOrder;
template <class T, class M = T> class NetOrder;

//---------------------------------------------------------------------------------------------

template <class T, class M>
class HostOrder
{
	typedef HostOrder<T, M> This;

	M _m;

public:
	HostOrder() : _m(0) {}
	HostOrder(M m) : _m(m) {}

	const This &operator=(const NetOrder<T> &n) { _m = n.host().number(); return *this; }

	HostOrder<T> host() const { return _m; }
	NetOrder<T> net() const;
	T number() const { return _m; }

	operator HostOrder<T>() const { return host(); }
	operator NetOrder<T>() const { return net(); }

	bool operator==(const This &x) const { return _m == x.number(); }
	bool operator!=(const This &x) const { return !operator==(x); }
};

//---------------------------------------------------------------------------------------------

#ifdef _WIN32
// 'magenta::NetOrder<T,M>' : assignment operator could not be generated
#pragma warning(push)
#pragma warning(disable : 4512)
#endif

template <class T, class M>
class NetOrder
{
	typedef NetOrder<T, M> This;

	M _m;

public:
	NetOrder() : _m(0) {}
	NetOrder(M m) : _m(m) {}

	const This &operator=(const HostOrder<T> &h) { _m = h.net().number(); return *this; }

	HostOrder<T> host() const;
	NetOrder<T> net() const { return _m; }
	T number() const { return _m; }

	operator HostOrder<T>() const { return host(); }
	operator NetOrder<T>() const { return net(); }

	bool operator==(const This &x) const { return _m == x.number(); }
	bool operator!=(const This &x) const { return !operator==(x); }
};

#ifdef _WIN32
#pragma warning(pop)
#endif

//---------------------------------------------------------------------------------------------

template <> inline HostOrder<UINT16> NetOrder<UINT16>::host() const { return ntohs(_m); }

template <> inline HostOrder<UINT32> NetOrder<UINT32>::host() const { return ntohl(_m); }

template <> inline HostOrder<UINT16> NetOrder<UINT16, UINT16&>::host() const { return ntohs(_m); }

template <> inline HostOrder<UINT32> NetOrder<UINT32, UINT32&>::host() const { return ntohl(_m); }

//---------------------------------------------------------------------------------------------

template <> inline NetOrder<UINT16> HostOrder<UINT16>::net() const { return htons(_m); }

template <> inline NetOrder<UINT32> HostOrder<UINT32>::net() const { return htonl(_m); }

template <> inline NetOrder<UINT16> HostOrder<UINT16, UINT16&>::net() const { return htons(_m); }

template <> inline NetOrder<UINT32> HostOrder<UINT32, UINT32&>::net() const { return htonl(_m); }

///////////////////////////////////////////////////////////////////////////////////////////////

class Protocol
{
	typedef Protocol This;

	friend class Service;

protected:
	UINT16 _num;

	Protocol() : _num(-1) {}

	void ctor(int n) { _num = n; }

public:
	Protocol(int n) { ctor(n); }
	Protocol(const char *name);

	This &operator=(int n) { ctor(n); return *this; }

	int number() const { return _num; }
	text name() const;

	CLASS_EXCEPTION_DEF("magenta::Protocol");
	CLASS_SUB_EXCEPTION_DEF(InvalidErr, "invalid protocol"); 
};

//---------------------------------------------------------------------------------------------

class Port
{
	typedef Port This;

	friend class Service;

public:
	typedef HostOrder<UINT16> Raw;

	typedef HostOrder<UINT16> HostType;
	typedef NetOrder<UINT16> NetType;

protected:
	Raw _raw;

public:
	Port() : _raw(0) {}
	Port(UINT16 n) : _raw(n) {}
	Port(const Raw &n) : _raw(n) {}

	bool operator==(const Port &p) const { return _raw == p._raw; }
	bool operator!=(const Port &p) const { return !operator==(p); }

	bool operator!() const { return !number(); }

	operator const Raw &() const { return _raw; }
	operator NetOrder<UINT16>() const { return _raw; }

	UINT16 number() const { return _raw.number(); }

	CLASS_EXCEPTION_DEF("magenta::Port");
	CLASS_SUB_EXCEPTION_DEF(InvalidErr, "invalid port"); 
};

//---------------------------------------------------------------------------------------------

class AnyPort : public Port
{
public:
	AnyPort() : Port(0) {}
};

//---------------------------------------------------------------------------------------------

class Service
{
protected:
	text _name;
	Port _port;
	Protocol _proto;

public:
	Service(const char *name);
	Service(const char *name, const Protocol &proto);
	Service(const Port &port, const Protocol &proto);
	
	const text &name() const { return _name; }
	const Port &port() const { return _port; }
	const Protocol &protocol() const { return _proto; }

	operator const Port&() const;
	operator const Protocol&() const;

	CLASS_EXCEPTION_DEF("magenta::Service");
	CLASS_SUB_EXCEPTION_DEF(InvalidErr, "invalid service"); 
	CLASS_SUB_EXCEPTION_DEF(UnknownProtocol, "unknown protocol"); 
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_net_basics_
