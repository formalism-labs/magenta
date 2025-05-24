
#if defined(_WIN32)
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#elif defined(__linux__)
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>

#include <string>
#include <map>

#elif defined(__VXWORKS__)
#include <wrn/coreip/net/utils/netstat.h>
#endif

#include "interface.h"

namespace magenta
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////
// Network Interfaces
///////////////////////////////////////////////////////////////////////////////////////////////

NetworkInterfaces::NetworkInterfaces()
{
#if defined(_WIN32)
	_adapters = 0;
    ULONG buf_len = 0x4000;
    for (int i = 0; i < 3; ++i)
    {
		if (!_adapters)
			_adapters = (IP_ADAPTER_ADDRESSES *) new char[buf_len];
		ULONG flags = 0;
        DWORD rc = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, _adapters, &buf_len);
		if (rc == NO_ERROR)
			break;
        if (rc != ERROR_BUFFER_OVERFLOW)
		{
			ERROR_OS(QueryErr)("GetAdaptersAddresses() failed");
        	return;
		}

		delete[] (char*) _adapters;
		_adapters = 0;
        buf_len *= 2;
    }

	if (!_adapters)
	{
		ERROR_EX(QueryErr)("GetAdaptersAddresses() failed");
		return;
	}

#elif defined(__linux__)
	if (getifaddrs(&_ifaddrs) != 0) 
	{
		ERROR_OS(QueryErr)("getifaddrs failed");
    	return;
	}

	typedef map<string, void*> Names;
	Names ifnames;
	for (struct ifaddrs *ifa = _ifaddrs; ifa; ifa = ifa->ifa_next) 
	{
		int f = ifa->ifa_addr->sa_family;
		if (f != AF_INET && f != AF_INET6)
			continue;
		ifnames.insert(pair<string,void*>(ifa->ifa_name, 0));
	}

	if (ifnames.size() == 0)
	{
		_names = 0;
		_count = 0;
		return;
	}

	_count = ifnames.size();
	_names = new text[_count];
	int i = 0;
	for (Names::iterator it = ifnames.begin(); it != ifnames.end(); ++it)
		_names[i++] = (*it).first;

/*
	Socket sock(SOCK_STREAM);
	int fd = sock.fd();

	struct ifconf ifcnf;
	memset(&ifcnf, 0, sizeof(ifcnf));
	if (ioctl(fd, SIOCGIFCONF, &ifcnf) != 0)

	_adaptres = new char[ifcnf.ifc_len];
	ifcnf.ifc_req = (struct ifreq *) _adaptres;
	if (ioctl(fd, SIOCGIFCONF, &ifcnf) != 0) 
	{
		ERROR_EX(QueryErr)("ioctl(SIOCGIFCONF) failed");
    	return;
	}

	struct ifreq *_ifreq = ifcnf.ifc_req;
	for (unsigned n = 0; n < ifcnf.ifc_len / sizeof(struct ifreq); ++n, ++_ifreq) 
	{
		if (ioctl(fd, SIOCGIFADDR, _ifreq) != 0) 
			continue;

		if (
		struct in_addr ip = ((struct sockaddr_in *) &(_ifreq->ifr_addr))->sin_addr;
		if (ip.s_addr != INADDR_ANY && ip.s_addr != INADDR_LOOPBACK && ip.s_addr != INADDR_NONE)
		{
			default_in_addr = ip;
			break;
		}
	}
*/
#endif
}

//---------------------------------------------------------------------------------------------

NetworkInterfaces::~NetworkInterfaces()
{
#if defined(_WIN32)
	delete[] (char*) _adapters;
#elif defined(__linux__)
	delete[] _names;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// NetworkInterfaces::Iterator
///////////////////////////////////////////////////////////////////////////////////////////////

NetworkInterfaces::Iterator::Iterator(NetworkInterfaces &interfaces) : _interfaces(interfaces)
{
#if defined(_WIN32)
	_adapter = interfaces._adapters;
#elif defined(__linux__)
	_number = 0;
#endif
}

//---------------------------------------------------------------------------------------------

NetworkInterface NetworkInterfaces::Iterator::operator*()
{
#if defined(_WIN32)
	return NetworkInterface(_adapter);
#elif defined(__linux__)
	return NetworkInterface(_interfaces._names[_number]);
#endif
}

//---------------------------------------------------------------------------------------------

NetworkInterfaces::Iterator &NetworkInterfaces::Iterator::operator++()
{
#if defined(_WIN32)
	_adapter = _adapter->Next;
#elif defined(__linux__)
	if (++_number == _interfaces._count)
		_number = -1;
#endif
	return *this;
}

//---------------------------------------------------------------------------------------------

bool NetworkInterfaces::Iterator::operator!() const
{
#if defined(_WIN32)
	return !_adapter;
#elif defined(__linux__)
	return _number < 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// NetworkInterface
///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)

NetworkInterface::NetworkInterface(IP_ADAPTER_ADDRESSES *adapter) : _adapter(adapter)
{
}

#endif

//---------------------------------------------------------------------------------------------

#if defined(__linux__)

NetworkInterface::NetworkInterface(const char *name) : _name(name)
{
}

#endif

//---------------------------------------------------------------------------------------------

UINT32 NetworkInterface::number() const
{
#if defined(_WIN32)
	return _adapter->IfIndex > 0 ? _adapter->IfIndex : _adapter->Ipv6IfIndex;
#elif defined(__linux__)
	return if_nametoindex(_name.c_str());
#endif
}

//---------------------------------------------------------------------------------------------

text NetworkInterface::name() const
{
#if defined(_WIN32)
	return stringf("%wS", _adapter->FriendlyName);

#elif defined(__linux__)
	return _name;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// NetworkInterface::Addresses
///////////////////////////////////////////////////////////////////////////////////////////////

NetworkInterface::Addresses::Addresses(NetworkInterface &ifc, int family) : 
	_interface(ifc), _family(family) 
{
#if defined(_WIN32)
	_addresses = ifc._adapter->FirstUnicastAddress;
#elif defined(__linux__)
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// NetworkInterface::Addresses::Iterator
///////////////////////////////////////////////////////////////////////////////////////////////

NetworkInterface::Addresses::Iterator::Iterator(NetworkInterface::Addresses &addresses) : 
	_addresses(addresses)
{
#if defined(_WIN32)
	_address = addresses._addresses;

#elif defined(__linux__)
	if (getifaddrs(&_ifaddrs) != 0) 
	{
		ERROR_OS(QueryErr)("getifaddrs failed");
    	return;
	}
	_address = _ifaddrs;
#endif

	_next(false);
}

//---------------------------------------------------------------------------------------------

LocalIPAddress NetworkInterface::Addresses::Iterator::operator*()
{
#if defined(_WIN32)
	return LocalIPAddress((struct sockaddr *) _address->Address.lpSockaddr, 
		(socklen_t) _address->Address.iSockaddrLength);

#elif defined(__linux__)
	socklen_t len = _address->ifa_addr->sa_family == AF_INET ? sizeof(struct sockaddr_in) : 
		sizeof(struct sockaddr_in6);
	return LocalIPAddress(_address->ifa_addr, len);
#endif
}

//---------------------------------------------------------------------------------------------

NetworkInterface::Addresses::Iterator &NetworkInterface::Addresses::Iterator::operator++()
{
	_next();	
	return *this;
}

//---------------------------------------------------------------------------------------------

void NetworkInterface::Addresses::Iterator::_next(bool b)
{
	int family = _addresses._family;

#if defined(_WIN32)
	if (b && _address)
		_address = _address->Next;

	for (; _address; _address = _address->Next)
	{
		int f = _address->Address.lpSockaddr->sa_family;
		if (family == AF_UNSPEC ? f != AF_INET && f != AF_INET6 : f != family)
			continue;

		break;
	}

#elif defined(__linux__)

	if (b && _address)
		_address = _address->ifa_next;

	for (; _address; _address = _address->ifa_next)
	{
		int f = _address->ifa_addr->sa_family;
		if (family == AF_UNSPEC ? f != AF_INET && f != AF_INET6 : f != family)
			continue;

		if (_addresses._interface._name == _address->ifa_name)
			break;
	}
#endif
}

//---------------------------------------------------------------------------------------------

bool NetworkInterface::Addresses::Iterator::operator!() const
{
	return !_address;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
