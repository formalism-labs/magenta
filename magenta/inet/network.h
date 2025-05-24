
///////////////////////////////////////////////////////////////////////////////////////////////
// Network Classes
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_inet_network_
#define _magenta_inet_network_

#include "magenta/inet/classes.h"
#include "magenta/inet/general.h"

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

typedef sockaddr PhysicalSocketAddress;

///////////////////////////////////////////////////////////////////////////////////////////////

typedef string Name;

#if 0
class Name : public string
{
public:
    Name(const string &name) : string(name) {}
    bool operator!() { return is_null(); }
};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
// Addresses
///////////////////////////////////////////////////////////////////////////////////////////////

class INetworkAddress
{
public:
    typedef u_short AddressFamily;

    virtual AddressFamily type() const = 0;

    virtual operator string() const { THROW_EX(Unimplemented); }
    virtual bool operator!() const { return type() == AF_UNSPEC; }
};

//---------------------------------------------------------------------------------------------

class HostAddress : public INetworkAddress
{
    AddressFamily _type;
    Dynablock *_addr;

public:
	CLASS_EXCEPTION_DEF("host address error");

    HostAddress() : _addr(0), _type(AF_UNSPEC) {}

    HostAddress(hostent &entry)
    {
        _type = entry.h_addrtype;
        _addr = new Dynablock(entry.h_addr, entry.h_length);
    }

    HostAddress(const HostAddress &address) : _type(address._type),
        _addr(new Dynablock(address.physical())) {}

    ~HostAddress() { delete _addr; }

    virtual AddressFamily type() const { return _type; }

    virtual ConstBlock physical() const 
	{ 
		if (!_addr) 
			THROW_EX(Exception); 
		return *_addr; 
	}
};


//---------------------------------------------------------------------------------------------

class SocketAddress : public INetworkAddress, public Block
{
    friend Host;

protected:
    PhysicalSocketAddress _addr;

public:
    SocketAddress(AddressFamily nFamily = AF_UNSPEC)
    {
        _addr.sa_family = nFamily;
        IMutableBlock::set((char *) &_addr, sizeof(_addr));
    }

    SocketAddress(const SocketAddress &o) : _addr(o._addr)
        { IMutableBlock::set((char *)&_addr, sizeof(_addr)); }

    SocketAddress(const PhysicalSocketAddress &o) : _addr(o) { IMutableBlock::set((char *) &_addr, sizeof(_addr)); }

    SocketAddress &operator=(const SocketAddress &o) { _addr = o.saStorage(); return *this; }

    virtual AddressFamily type() const { return _addr.sa_family; }

    PhysicalSocketAddress &physical() const { return (PhysicalSocketAddress &) _addr; }
    PhysicalSocketAddress &saStorage() const { return physical(); }
    operator PhysicalSocketAddress&() const { return physical(); }

    bool operator!() const { return INetworkAddress::operator!(); }
};

//=============================================================================================
// Host
//=============================================================================================

class IHost
{
    friend Host;

protected:
    virtual IHost *duplicate() const = 0;

public:
    typedef magenta::Name Name;

    virtual Name name() const = 0;
    virtual HostAddress hostAddress() const = 0;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Host : virtual public IHost
{
protected:
    class ByName : public IHost
    {
        Name _name;

        IHost *duplicate() const { return new ByName(_name); }

    public:
        ByName(const Name &s) : _name(s) {}
        Name name() const { return _name; }
        HostAddress hostAddress() const;
    };

    class ByAddress : public IHost
    {
        HostAddress _addr;

        IHost *duplicate() const { return new ByAddress(_addr); }

    public:
        ByAddress(const HostAddress &o) : _addr(o) {}
        string name() const;
        HostAddress hostAddress() const { return _addr; }
    };

    IHost *_host;
    IHost &host() const { if (!_host) THROW_EX(Exception); return *_host;  }
    IHost *duplicate() const { return host().duplicate(); }

public:
    Host() : _host(0) {}
    Host(const Name &_name) : _host(new ByName(_name)) {}
    Host(const HostAddress &_addr) : _host(new ByAddress(_addr)) {}
    Host(const IHost &host) : _host(host.duplicate()) {}
    Host(const Host &host) : _host(host.duplicate()) {}
    ~Host() { delete _host; }

    Name name() const { return host().name(); }
    virtual HostAddress hostAddress() const { return host().hostAddress(); }

    bool operator!() const { return !_host; }

	CLASS_EXCEPTION_DEF("host error");
};

//---------------------------------------------------------------------------------------------

class LocalHost : public Host
{
public:
    LocalHost();
};

//=============================================================================================
// Protocol
//=============================================================================================

class IProtocol
{
    friend Protocol;

protected:
    virtual IProtocol *duplicate() const = 0;

public:
    typedef magenta::Name Name;
    typedef unsigned int Number;

    virtual Number number() const = 0;
    virtual Name name() const = 0;
    Number physical() { return htons(number()); }

    virtual bool operator!() { return !number(); }

    operator Number() const { return number(); }
    operator Name() const { return name(); }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Protocol : public IProtocol
{
public:
	CLASS_EXCEPTION_DEF("protocol error");

private:
    class ByName : public IProtocol
    {
        Name _name;

    protected:
        IProtocol *duplicate() const { return new ByName(_name); }

    public:
        ByName(const Name &s) : _name(s) {}
        Number number() const;
        Name name() const { return _name; }
    };

    class ByNumber : public IProtocol
    {
        Number _number;

    protected:
        IProtocol *duplicate() const { return new ByNumber(_number); }

    public:
        ByNumber(Number n) : _number(n) {}
        Number number() const { return _number; }
        Name name() const;
    };

    IProtocol *_proto;
    IProtocol &proto() const { if (!_proto) THROW_EX(Exception); return *_proto; }
    virtual IProtocol *duplicate() const { return proto().duplicate(); }

public:
	Protocol() : _proto(0) {}
	Protocol(Number n) : _proto(new ByNumber(n)) {}
	Protocol(const Name &name) : _proto(new ByName(name)) {}
	Protocol(const IProtocol &proto) : _proto(proto.duplicate()) {}
	Protocol(const Protocol &proto) : _proto(proto.duplicate()) {}
	~Protocol() { delete _proto; }

	virtual Number number() const { return proto().number(); }
	virtual Name name() const { return proto().name(); }

	virtual bool operator!() { return !_proto; }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class AnyProtocol : public IProtocol
{
protected:
    IProtocol *duplicate() const;

public:
    AnyProtocol();
    Number number() const { return 0; }
    Name name() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_inet_network_

