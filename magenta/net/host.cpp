
#include "host.h"
#include "address.h"

#if defined(_WIN32)
#include <ws2tcpip.h>

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
// Host
///////////////////////////////////////////////////////////////////////////////////////////////

Host::Host(const char *name) : _name(name)
{
}

//---------------------------------------------------------------------------------------------

Host::Host(const IPAddress &addr)
{
	_name = addr.text();

#if 0
	char name[NI_MAXHOST];
	const struct sockaddr *sa = addr.sockaddr();
	socklen_t sa_len = addr.sockaddrLen();
	int rc = getnameinfo(sa, sa_len, name, sizeof(name), 0, 0, 0);
	if (rc)
	{
		rc = getnameinfo(sa, sa_len, name, sizeof(name), 0, 0, NI_NUMERICHOST);
		if (rc)
		{
			ERROR_EX(InvalidErr)("conversion to text (%s)", gai_strerror(rc));
			return;
		}
	}
	_name = name;
#endif
}

//---------------------------------------------------------------------------------------------

bool Host::isAddress() const
{
	IPAddress addr;
	return addr.set(_name);
}

//---------------------------------------------------------------------------------------------

IPAddress Host::explicitAddress() const
{
	return IPAddress(_name);
}

//---------------------------------------------------------------------------------------------

bool Host::isValidName() const
{
	if (!_name)
		return false;

	UINT32 n = _name.length();
	for (UINT32 i = 0; i < n; ++i)
	{
		char c = _name[i];
		if (!isalnum(c) && c != '.' && c != '-' && c != '_')
			return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------
// Host::Addresses
//---------------------------------------------------------------------------------------------

Host::Addresses::Addresses(const char *addr, int family)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;

	int rc = getaddrinfo(addr, 0, &hints, &_info);
	const char *p = gai_strerror(rc);
	if (rc || !_info)
		ERROR_EX(InvalidErr)("%s (addr=%s)", gai_strerror(rc), addr);
}

//---------------------------------------------------------------------------------------------

Host::Addresses Host::addresses() const
{
	return Addresses(_name.c_str()); 
}

//---------------------------------------------------------------------------------------------
// Host::Addresses::Iterator
//---------------------------------------------------------------------------------------------

Host::Addresses::Iterator::Iterator(Addresses &addrs) : _info(addrs._info) 
{
}

//---------------------------------------------------------------------------------------------

Host::Addresses::Iterator &Host::Addresses::Iterator::operator++()
{
	if (_info)
		_info = _info->ai_next;
	return *this;
}

//---------------------------------------------------------------------------------------------

bool Host::Addresses::Iterator::operator!() const 
{
	return !_info; 
}

//---------------------------------------------------------------------------------------------

IPAddress Host::Addresses::Iterator::operator*() const 
{
	return IPAddress(_info->ai_addr, _info->ai_addrlen); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// HostAndPort
///////////////////////////////////////////////////////////////////////////////////////////////

void HostAndPort::ctor(const char *str)
{
	set(str, true);
}

//---------------------------------------------------------------------------------------------

HostAndPort &HostAndPort::operator=(const char *str)
{
	ctor(str);
	return *this;
}

//---------------------------------------------------------------------------------------------

bool HostAndPort::set(const char *str)
{
	return set(str, false);
}

//---------------------------------------------------------------------------------------------

bool HostAndPort::set(const char *str, bool invalidate)
{
	int port_n;
	char host_s[HOSTNAME_MAX_SIZE];
	const char *p = strrchr(str, ':');
	if (!p)
	{
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
	strncpy(host_s, str, p - str);
	host_s[p - str] = '\0';

	_host = host_s;
	port() = port_n;
	_valid = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////

Text::text HostAndPort::text() const
{
	return std::string(_host.name()) + ":" + stringf("%d", _port.number());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Localhost
///////////////////////////////////////////////////////////////////////////////////////////////

Localhost::Localhost()
{
}

//---------------------------------------------------------------------------------------------

Host::Addresses Localhost::addresses() const
{
	return Addresses("");  // TODO
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
