
#ifndef _magenta_net_interface_
#define _magenta_net_interface_

#include "rvfc/Include/rvfc.h"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iptypes.h>
#include <iphlpapi.h>

#elif defined(__linux__)
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <net/if.h>
#include <ifaddrs.h>
#endif

#include "magenta/net/classes.h"
#include "magenta/net/basics.h"
#include "magenta/net/address.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Network Interface
///////////////////////////////////////////////////////////////////////////////////////////////

class NetworkInterface
{
protected:
#if defined(_WIN32)
	IP_ADAPTER_ADDRESSES *_adapter;
#elif defined(__linux__)
	text _name;
#endif

public:
	NetworkInterface(const char *name);

#if defined(_WIN32)
	NetworkInterface(IP_ADAPTER_ADDRESSES *adapter);
#endif

	bool isUp() const;
	bool isLoopback() const;

	UINT32 number() const;
	text name() const;

	class Addresses
	{
		NetworkInterface &_interface;
		int _family;

#if defined(_WIN32)
		IP_ADAPTER_UNICAST_ADDRESS *_addresses;
#endif

	public:
		Addresses(NetworkInterface &ifc, int family = AF_UNSPEC);
		
		class Iterator
		{
			Addresses &_addresses;

#if defined(_WIN32)
			IP_ADAPTER_UNICAST_ADDRESS *_address;
#elif defined(__linux__)
			auto_free<struct ifaddrs *> _ifaddrs;
			struct ifaddrs *_address;
#endif

			void _next(bool b = true);

		public:
			Iterator(Addresses &addresses);

			LocalIPAddress operator*();
			Iterator &operator++();
			bool operator!() const;
		};

		CLASS_EXCEPTION_DEF("magenta::NetworkInterface::Addresses::Iterator");
		CLASS_SUB_EXCEPTION_DEF(QueryErr, "query error");
	};
};

//---------------------------------------------------------------------------------------------

class NetworkInterfaces
{
protected:
#if defined(_WIN32)
	IP_ADAPTER_ADDRESSES *_adapters;

#elif defined(__linux__)
	auto_free<struct ifaddrs *> _ifaddrs;

	text *_names;
	int _count;
#endif

public:
	NetworkInterfaces();
	~NetworkInterfaces();

	class Iterator
	{
		NetworkInterfaces &_interfaces;
#if defined(_WIN32)
		IP_ADAPTER_ADDRESSES *_adapter;
#elif defined(__linux__)
		int _number;
#endif

	public:
		Iterator(NetworkInterfaces &interfaces);

		NetworkInterface operator*();
		Iterator &operator++();
		bool operator!() const;
	};

	void _invalidate() {}

public:
	CLASS_EXCEPTION_DEF("magenta::NetworkInterfaces");
	CLASS_SUB_EXCEPTION_DEF(QueryErr, "query error");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_net_interface_
