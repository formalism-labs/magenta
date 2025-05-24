
///////////////////////////////////////////////////////////////////////////////////////////////
// Internet Classes
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_inet_internet_
#define _magenta_inet_internet_

#include "magenta/inet/classes.h"
#include "magenta/inet/general.h"
#include "magenta/inet/network.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

typedef unsigned char Byte;

typedef in_addr PhysicalIPAddress;
typedef sockaddr_in PhysicalInetAddress;

///////////////////////////////////////////////////////////////////////////////////////////////
// IP Address
///////////////////////////////////////////////////////////////////////////////////////////////

class IIPAddress : public INetworkAddress
{
public:
    AddressFamily type() const { return AF_INET; }

    virtual PhysicalIPAddress &ipStorage() const = 0;
    unsigned long physical() const { return ipStorage().s_addr; }

    virtual operator string() const { return string(inet_ntoa(ipStorage())); }
    virtual operator PhysicalIPAddress&() const { return ipStorage(); }
    virtual bool operator!() const { return !ntohl(ipStorage().s_addr); }

    CLASS_EXCEPTION_DEF("IP address error");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class IPAddress : public IIPAddress
{
    PhysicalIPAddress _addr;
    virtual PhysicalIPAddress &ipStorage() const { return (PhysicalIPAddress &) _addr; }

public:
    IPAddress() { _addr.s_addr = ntohl(0); }
    IPAddress(const IIPAddress &address) : _addr(address.ipStorage()) {}

    IPAddress(Byte n1, Byte n2, Byte n3, Byte n4);
    IPAddress(unsigned long n) { _addr.s_addr = htonl(n); }

    IPAddress(const string &s);
    IPAddress(const PhysicalIPAddress &ip) { _addr.s_addr = ip.s_addr; }

	virtual ~IPAddress() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class AnyIPAddress : public IIPAddress
{
    PhysicalIPAddress _addr;
    virtual PhysicalIPAddress &ipStorage() const { return (PhysicalIPAddress &) _addr; }

public:
    AnyIPAddress() { _addr.s_addr = INADDR_ANY; }

    bool operator!() const { return true; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Port
///////////////////////////////////////////////////////////////////////////////////////////////

class IPort
{
public:
    typedef unsigned short Number;

    virtual Number number() const = 0;

    operator Number() const { return number(); }
    Number physical() const { return htons(number()); }

    virtual bool operator!() { return !number(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Port : public IPort
{
    Number _number;

public:
    Port() : _number(0) {}
    Port(Number n) : _number(n) {}
    Port(const IPort &port) : _number(port.number()) {}

    Number number() const { return _number; }
    Port &operator=(Number n) { _number = n; return *this; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class AnyPort : public IPort
{
    Port _port;

public:
    AnyPort() : _port(0) {}

    Number number() const { return _port.number(); }
    operator const Port() const { return _port; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Service
///////////////////////////////////////////////////////////////////////////////////////////////

class IService
{
public:
    typedef magenta::Name Name;

    virtual Name name() const = 0;
    virtual Port port() const = 0;
    virtual Protocol protocol() const = 0;
    virtual IService *duplicate() const = 0;

    operator Name() const { return name(); }
    operator Port() const { return port(); }
    operator Protocol() const { return protocol(); }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Service : public IService
{
    class ByNameAndProtocol : public IService
    {
        Name _name;
        Protocol _proto;

        servent entry() const;

    public:
        ByNameAndProtocol(const Name &name, const Protocol &proto = AnyProtocol()) :
            _name(name), _proto(proto) {}

        Name name() const { return _name; }
        Port port() const { return ntohs(entry().s_port); }
        Protocol protocol() const { if (!_proto) return Name(entry().s_proto); else return _proto; }

        IService *duplicate() const { return new ByNameAndProtocol(_name); }
    };

    class ByPortAndProtocol : public IService
    {
        Port _port;
        Protocol _proto;

        servent entry() const;

    public:
        ByPortAndProtocol(const Port &port, const Protocol &proto = AnyProtocol()) :
            _port(port), _proto(proto) {}

        Name name() const { return Name(entry().s_name); }
        Port port() const { return _port; }
        Protocol protocol() const { if (!_proto) return Name(entry().s_proto); else return _proto; }

        IService *duplicate() const { return new ByPortAndProtocol(_port, _proto); }
    };

    IService *_service;
    IService &service() const { if (!_service) THROW_EX(Exception); return *_service; }
    IService *duplicate() const { return service().duplicate(); }

public:
	Service() : _service(0) {}
    Service(const Name &s) : _service(new ByNameAndProtocol(s)) {}
    Service(const IService &service) : _service(service.duplicate()) {}
    Service(const Service &service) : _service(service.duplicate()) {}
    Service(const Port &port, const Protocol &proto) :
        _service(new ByPortAndProtocol(port, proto)) {}

    string name() const { return service().name(); }
    Port port() const { return service().port(); }
    Protocol protocol() const { return service().protocol(); }

	bool operator!() const { return !_service; }

	CLASS_EXCEPTION_DEF("service error");
};

//=============================================================================================
// Internet Address (interface)
//=============================================================================================

class IInetAddress : public INetworkAddress
{
    friend InetAddress;

protected:
    virtual IInetAddress *duplicate() const { THROW_EX(Unimplemented); }

public:
    AddressFamily type() const { return AF_INET; }

    virtual IPAddress ip() const = 0;
    virtual Port port() const = 0;

    virtual PhysicalInetAddress physical() const
    {
        PhysicalInetAddress oAddr;
        oAddr.sin_family = AF_INET;
        oAddr.sin_port = port().physical();
        oAddr.sin_addr.s_addr = ip().physical();
        return oAddr;
    }

    virtual operator IPAddress() const { return ip(); }
    virtual operator Port() const { return port(); }

    virtual operator PhysicalInetAddress() const { return physical(); }

    virtual bool operator!() const { return !ip() && !port(); }
};

//---------------------------------------------------------------------------------------------

class InetSocketAddress : 
	public SocketAddress,
    public IInetAddress, 
	protected IIPAddress, 
	protected IPort
{
    in_addr &ipStorage() const { return (in_addr &) saStorage().sin_addr; }
    PhysicalInetAddress &saStorage() const { return (PhysicalInetAddress &) SocketAddress::saStorage(); }

    virtual Number number() const { return saStorage().sin_port; }

public:
	CLASS_EXCEPTION_DEF("internet socket address error");
	CLASS_SUB_EXCEPTION_DEF(WrongAddressTypeError, "wrong address type");

    InetSocketAddress() : SocketAddress(AF_INET) {}

    InetSocketAddress(const SocketAddress &addr) : SocketAddress(addr)
	{
		if (addr.type() != AF_INET) 
			THROW_EX(WrongAddressTypeError); 
	}

    InetSocketAddress(const IIPAddress &ip, const IPort &port)
    {
        PhysicalInetAddress &addr = saStorage();
        addr.sin_family = AF_INET;
        addr.sin_port = port.physical();
        addr.sin_addr.s_addr = ip.physical();
    }

    InetSocketAddress(const IInetAddress &addr)
    {
        saStorage() = addr.physical();
    }

    virtual IPAddress ip() const { return (const IPAddress &) *this; }
    virtual Port port() const { return (const IPort &) *this; }

    virtual PhysicalInetAddress physical() const { return saStorage(); }
    virtual operator PhysicalInetAddress() const { return saStorage(); }
};

//=============================================================================================
// Internet Host
//=============================================================================================

class IInetHost : virtual public IHost
{
public:
    IPAddress ipAddress() const
    {
        const HostAddress &addr = hostAddress();
        if (addr.type() != AF_INET || long(addr.physical().length()) != sizeof(in_addr))
            THROW_EX(WrongAddressTypeError);
        return *(in_addr *) addr.physical().data();
    }

    InetSocketAddress address() const { return InetSocketAddress(ipAddress(), AnyPort()); }

	operator IPAddress() const { return ipAddress(); }

	CLASS_EXCEPTION_DEF("internet host error");
	CLASS_SUB_EXCEPTION_DEF(WrongAddressTypeError, "wrong address type");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class InetHost : public Host, virtual public IInetHost
{
    virtual HostAddress hostAddress() const { return Host::hostAddress(); }

public:
    InetHost() {}
    InetHost(const Name &name) : Host(name) {}
    InetHost(const HostAddress &addr) : Host(addr)
	{ 
		if (hostAddress().type() != AF_INET) 
			THROW_EX(WrongAddressTypeError); 
	}

	virtual ~InetHost() {}

    InetHost(const IIPAddress &addr) : Host(addr) {}
    InetHost(const IHost &host) : Host(host) {}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class InetLocalHost : public InetHost
{
public:
    InetLocalHost() : InetHost(LocalHost()) {}
};

//=============================================================================================
// Internet Address (cont.)
//=============================================================================================

class InetAddress : public IInetAddress
{
    class ByIPAndPort : public InetSocketAddress
    {
        friend InetAddress;

    protected:
        IInetAddress *duplicate() const { return new ByIPAndPort(ip(), port()); }

    public:
        ByIPAndPort(const IIPAddress &ip, const IPort &port) :
            InetSocketAddress(ip, port) {}

//        IPAddress ip() const { return address().sin_addr; }
//        Port port() const    { return ntohs(address().sin_port); }
//        PhysicalInetAddress physical() const { return address(); }
    };

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    class ByHostAndPort : public IInetAddress
    {
        friend InetAddress;

        InetHost _host;
        Port _port;

    protected:
        IInetAddress *duplicate() const { return new ByHostAndPort(_host, port()); }

    public:
        ByHostAndPort(const IInetHost &host, const IPort &port) :
            _host(host), _port(port) {}

        IPAddress ip() const { return _host.ipAddress(); }
        Port port() const    { return _port; }
   };

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    class ByHostAndService : public IInetAddress
    {
        friend InetAddress;

        InetHost _host;
        Service _service;

    protected:
        IInetAddress *duplicate() const { return new ByHostAndService(_host, (IService &) _service); }

    public:
        ByHostAndService(const IInetHost &host, const IService &service) :
            _host(host), _service(service) {}

        IPAddress ip() const { return _host.ipAddress(); }
        Port port() const    { return _service.port(); }
    };

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	CLASS_EXCEPTION_DEF("internet address error");

    IInetAddress *_addr;
    IInetAddress &address() const { if (!_addr) THROW_EX(Exception); return (InetAddress &) *_addr; }
    IInetAddress *duplicate() const { return address().duplicate(); }

public:
    InetAddress() : _addr(0) {}
    InetAddress(const IInetAddress &ip, const Port &port) : _addr(new ByIPAndPort(ip, port)) {}
    InetAddress(const IInetHost &host, const Port &port) : _addr(new ByHostAndPort(host, port)) {}
    InetAddress(const IInetHost &host, const IService &service) : _addr(new ByHostAndService(host, service)) {}

//    InetAddress(const PhysicalInetAddress &o) {}

    IPAddress ip() const { return address().ip(); }
    Port port() const { return address().port(); }
	PhysicalInetAddress physical() const { return address().physical(); }

    operator PhysicalInetAddress() const { return physical(); }
    operator SocketAddress() const { return InetSocketAddress(address()); }
    operator InetSocketAddress() const { return address(); }
};

//---------------------------------------------------------------------------------------------

class AnyInetAddress : private InetAddress
{
public:
    AnyInetAddress() : InetAddress(AnyInetAddress(), (const Port &) AnyPort()) {}

    virtual bool operator!() const { return true; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif // _magenta_inet_internet_
