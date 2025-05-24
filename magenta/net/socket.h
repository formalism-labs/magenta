
#ifndef _magenta_net_socket_
#define _magenta_net_socket_

#include "magenta/net/classes.h"
#include "magenta/net/address.h"
#include "magenta/net/host.h"
#include "magenta/net/block.h"

#include "magenta/lang/defs.h"
#include "magenta/time/defs.h"

#ifdef _WIN32
#pragma warning(disable : 4250)
#endif

namespace magenta
{

using magenta::Milliseconds;

///////////////////////////////////////////////////////////////////////////////////////////////

class SocketError
{
public:
	static bool isInProgress();

public:
	CLASS_EXCEPTION_DEF("magenta::SocketError");
	CLASS_SUB_EXCEPTION_DEF(BindErr, "local address is already in use - bind problem");
	CLASS_SUB_EXCEPTION_DEF(ResourceErr, "not enough available resources");
	CLASS_SUB_EXCEPTION_DEF(NetworkErr, "network error");
};

//---------------------------------------------------------------------------------------------

class ISocket
{
public:
	enum__(Type)
		TCP,
		UDP
	__enum(Type);

	virtual ~ISocket() {}

	virtual bool isValid() const = 0;

	virtual void reopen(Type type) = 0;

	virtual bool bind(const LocalIPAddress &host) = 0;
	virtual bool bind(const LocalSocketAddress &host) = 0;

	virtual void setNonBlocking(bool nb) = 0;
	virtual void setKeepAlive(bool ka) = 0;
	virtual void setLingerOff() = 0;
	virtual void setReuseAddr(bool reuse) = 0;
	virtual void setLinger(const Seconds &time) = 0;
	virtual void setSendBuff(int size) = 0;
	virtual void setRecvBuff(int size) = 0;

	virtual void setKeepIdle(int idle) = 0;
	virtual void setKeepIntvl(int intvl) = 0;
	virtual void setKeepCnt(int cnt) = 0;

	virtual SocketAddress hostAddress() const = 0;
};

//---------------------------------------------------------------------------------------------

class Socket : virtual public ISocket
{
public:
	struct Invalid {};
	struct Uninitialized {};

	//-----------------------------------------------------------------------------------------
	// State
	//-----------------------------------------------------------------------------------------
public:
	int _fd;

	int fd() const { return _fd; }

	static int badFd();
	static int errorRc();
	static int lastError();

	//-----------------------------------------------------------------------------------------
	// Construction
	//-----------------------------------------------------------------------------------------
protected:
	Socket(Invalid) { ctor(Uninitialized()); }
	Socket(Uninitialized) { ctor(Uninitialized()); }
	Socket(int family, int type, int proto) { ctor(family, type, proto); }

	void ctor(Uninitialized);
	void ctor(int family, int type, int proto);
	void ctor(Type type);

	void assertValid() const;
	void _invalidate();
	void _invalidate() const { magenta::_invalidate(); }

	virtual void reopen(Type type);
	void _close();

public:
	Socket(Type type) { ctor(type); }
	virtual ~Socket();

	//-----------------------------------------------------------------------------------------
	// Connection
	//-----------------------------------------------------------------------------------------
public:
	virtual bool bind(const LocalIPAddress &host);
	virtual bool bind(const LocalSocketAddress &host);

	virtual SocketAddress hostAddress() const;

	//-----------------------------------------------------------------------------------------
	// I/O
	//-----------------------------------------------------------------------------------------
public:
	enum__(IOStatus)
		NoError,
		TimeOut,
		Error,
		NotConnected
	__enum(IOStatus);

protected:
	IOStatus _recv(Block &block, SocketAddress *addr = 0) throw();
	IOStatus _recv(Block &block, const Milliseconds &timeout, SocketAddress *addr = 0) throw();

	IOStatus _send(const Block &block, const SocketAddress *addr = 0) throw();
	IOStatus _send(const Block &block, const Milliseconds &timeout, const SocketAddress *addr = 0) throw();

	IOStatus _select(bool read, const Milliseconds &timeout, bool except);

public:
	virtual bool isValid() const { return _fd != badFd(); }

	//-----------------------------------------------------------------------------------------
	// Socket options
	//-----------------------------------------------------------------------------------------
public:
	virtual void setNonBlocking(bool nb);
	virtual void setKeepAlive(bool ka);
	virtual void setLingerOff();
	virtual void setLinger(const Seconds &time);
	virtual void setReuseAddr(bool reuse);
	virtual void setSendBuff(int size);
	virtual void setRecvBuff(int size);
	
	virtual void setKeepIdle(int idle = 0); //default value determine by the static type of the obj
	virtual void setKeepIntvl(int intvl = 0);
	virtual void setKeepCnt(int cnt = 0);

	//-----------------------------------------------------------------------------------------
	// Exceptions
	//-----------------------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::Socket");
	CLASS_SUB_EXCEPTION_DEF(InitErr, "initialization error");
	CLASS_SUB_EXCEPTION_DEF(BindErr, "local address is already in use - bind problem");
	CLASS_SUB_EXCEPTION_DEF(ResourceErr, "not enough available resources");
	CLASS_SUB_EXCEPTION_DEF(NetworkErr, "network error");
};

//---------------------------------------------------------------------------------------------

#if defined(_WIN32)
inline int Socket::badFd() { return INVALID_SOCKET; }
inline int Socket::errorRc() { return SOCKET_ERROR; }
inline int Socket::lastError() { return ::WSAGetLastError(); }
#elif defined(__linux__)
inline int Socket::badFd() { return -1; }
inline int Socket::errorRc() { return -1; }
inline int Socket::lastError() { return errno; }
#elif defined(__VXWORKS__)
inline int Socket::badFd() { return ERROR; }
inline int Socket::errorRc() { return ERROR; }
inline int Socket::lastError() { return errno; }
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

class AcceptedSocket : public Socket
{
	friend class TcpSocket;
	friend class TcpServerSocket;
	friend class AcceptedSecureSocket;

protected:
	SocketAddress _peer;

	AcceptedSocket() : Socket(Invalid()) {} // for server termination
	AcceptedSocket(int fd, const SocketAddress &peer);

	virtual void setAccepted() { _fd = badFd(); }

public:
	virtual bool isValid() const { return _fd != badFd(); }
};

//---------------------------------------------------------------------------------------------

class ITcpSocket : virtual public ISocket
{
public:
	virtual ~ITcpSocket() {}

	virtual int fd() const = 0;

	virtual void reopen() = 0;

	virtual bool bind(const LocalSocketAddress &host) = 0;
	virtual bool bind() = 0;

	virtual bool connect(const SocketAddress &peer) throw() = 0;
	virtual bool connect(const IPAddress &peer, const Port &port) throw() = 0;
	virtual bool connect(const Host &peer, const Port &port) throw() = 0;
	virtual bool isConnected() const throw() = 0;

	virtual Socket::IOStatus recv(Block &block) throw() = 0;
	virtual Socket::IOStatus recv(Block &block, const Milliseconds &timeout) throw() = 0;
	virtual Socket::IOStatus send(const Block &block) throw() = 0;
	virtual Socket::IOStatus send(const Block &block, const Milliseconds &timeout) throw() = 0;

	virtual Socket::IOStatus shutdown(bool send = true, bool recv = true) throw() = 0;

	virtual SocketAddress hostAddress() const throw() = 0;
	virtual SocketAddress peerAddress() const throw() = 0;

	virtual void setNoDelay(bool nodelay) = 0;

protected:
	virtual void reopen(Type type) { return reopen(); }
};

//---------------------------------------------------------------------------------------------

class TcpSocket : public Socket, virtual public ITcpSocket
{
	friend class SecureSocket;

	typedef Socket Super;
	typedef TcpSocket This;

	//-----------------------------------------------------------------------------------------
	// State
	//-----------------------------------------------------------------------------------------
protected:
	enum__(State)
		Uninitialized,
		Created,
		Connected,
		Disconnected
	__enum(State);

	State _state;

	LocalSocketAddress host_addr;
	SocketAddress peer_addr;

	bool wait_for_server;
	int connect_tries;
	Milliseconds connect_interval;

public:
	virtual int fd() const { return Socket::fd(); }

	//-----------------------------------------------------------------------------------------
	// Construction
	//-----------------------------------------------------------------------------------------
protected:
	TcpSocket(Uninitialized);
	TcpSocket(bool real);

	void ctor(bool real = true);
	void ctor(const IPAddress &ip, const Port &port);
	void ctor(const LocalSocketAddress &host, const IPAddress &ip, const Port &port);
	void ctor(const Host &peer, const Port &port);
	void ctor(const LocalSocketAddress &host, const Host &peer, const Port &port);
	void ctor(const AcceptedSocket &sock);

public:
	TcpSocket() throw ();

	TcpSocket(const SocketAddress &peer);
	TcpSocket(const IPAddress &peer, const Port &port);
	TcpSocket(const Host &peer, const Port &port);

	TcpSocket(const LocalSocketAddress &host, const SocketAddress &peer);
	TcpSocket(const LocalSocketAddress &host, const Host &peer, const Port &port);

	TcpSocket(const AcceptedSocket &sock);

	virtual ~TcpSocket();

	This &operator=(const AcceptedSocket &sock);

	virtual void reopen(Type type) { reopen(); }
	virtual void reopen();

	//-----------------------------------------------------------------------------------------
	// Connection
	//-----------------------------------------------------------------------------------------
public:
	virtual bool bind(const LocalSocketAddress &host);
	virtual bool bind();

	virtual bool connect(const SocketAddress &peer) throw();
	virtual bool connect(const IPAddress &peer, const Port &port) throw() { return connect(SocketAddress(peer, port)); }
	virtual bool connect(const Host &peer, const Port &port) throw();
	virtual bool isConnected() const throw() { return _state == State::Connected; }
	This &waitForServer(bool wait = true);
	This &waitForServer(int tries, const Milliseconds &interval = Seconds(1));

	virtual IOStatus shutdown(bool send = true, bool recv = true) throw();

	virtual SocketAddress hostAddress() const throw() { return Socket::hostAddress(); }
	virtual SocketAddress peerAddress() const throw() { return peer_addr; }

protected:
	void assertConnected();

	//-----------------------------------------------------------------------------------------
	// I/O
	//-----------------------------------------------------------------------------------------
public:
	virtual IOStatus recv(Block &block) throw();
	virtual IOStatus recv(Block &block, const Milliseconds &timeout) throw();
	virtual IOStatus send(const Block &block) throw();
	virtual IOStatus send(const Block &block, const Milliseconds &timeout) throw();

	This &operator<<(const Block &block);
	This &operator>>(Block &block);

public:
	// String I/O
	IOStatus recv(string &str) throw();
	IOStatus send(const string &str) throw();

	This &operator<<(string &s);
	This &operator>>(string &s);

	virtual void setNoDelay(bool nodelay);

	//-----------------------------------------------------------------------------------------
	// Exceptions
	//-----------------------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::TcpSocket");
	CLASS_SUB_EXCEPTION_DEF(InitErr, "initialization error"); 
	CLASS_SUB_EXCEPTION_DEF(NotConnected, "socket is not connected"); 
	CLASS_SUB_EXCEPTION_DEF(IOErr, "I/O error");
};

//---------------------------------------------------------------------------------------------

class ITcpServerSocket //: virtual public ISocket
{
public:
	virtual ~ITcpServerSocket() {}

	virtual void terminate() = 0;
};

//---------------------------------------------------------------------------------------------

class TcpServerSocket : public Socket, virtual public ITcpServerSocket
{
	typedef TcpServerSocket This;
	typedef Socket Super;

	int _backlog;
	LocalSocketAddress host_addr;
	bool _terminating;
	TcpSocket *terminate_socket;

	void ctor(const LocalSocketAddress &host, int backlog);
private:
	TcpServerSocket(const TcpServerSocket&);				//not implemented
	TcpServerSocket& operator=(const TcpServerSocket&);		//not implemented
public:
	TcpServerSocket(const Port &port, int backlog = 128) : Socket(Uninitialized()) { terminate_socket = NULL; ctor(LocalSocketAddress(port), backlog); }
	TcpServerSocket(const LocalSocketAddress &host, int backlog = 128) : Socket(Uninitialized()) { terminate_socket = NULL; ctor(host, backlog); }
	virtual ~TcpServerSocket();

	virtual AcceptedSocket accept();
	virtual AcceptedSocket accept(const Milliseconds &timeout, Socket::IOStatus &status);
	AcceptedSocket accept(const Milliseconds &timeout) { Socket::IOStatus status; return accept(timeout, status); }

	virtual void terminate();

	bool isTerminating() const { return _terminating; }

	CLASS_EXCEPTION_DEF("magenta::TcpServerSocket");
	CLASS_SUB_EXCEPTION_DEF(BindErr, "local address is already in use - bind problem");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class PeerMessage
{
	Block _block;
	IPAddress _addr;

public:
	PeerMessage(const Block &block, const IPAddress &addr);
};

//---------------------------------------------------------------------------------------------

class UdpSocket : public Socket
{
	typedef UdpSocket This;
	typedef Socket Super;

protected:
	void ctor();
	void ctor(const LocalSocketAddress &host);

public:
	// client socket
	UdpSocket() throw() : Socket(Uninitialized()) { ctor(); }
	virtual ~UdpSocket() {}

	// server socket
	UdpSocket(const Port &port) : Socket(Uninitialized()) { ctor(LocalSocketAddress(port)); }
	UdpSocket(const LocalIPAddress &ip) : Socket(Uninitialized()) { ctor(LocalSocketAddress(ip)); }
	UdpSocket(const LocalSocketAddress &host) : Socket(Uninitialized()) { ctor(host); }

	virtual bool bind(const Port &port) { return bind(LocalSocketAddress(port)); }
	virtual bool bind(const LocalIPAddress &ip) { return Super::bind(LocalSocketAddress(ip)); }
	virtual bool bind(const LocalSocketAddress &host) { return Super::bind(host); }

	virtual IOStatus recv(Block &block, SocketAddress &addr) throw();
	virtual IOStatus recv(Block &block, const Milliseconds &timeout, SocketAddress &addr) throw();
	virtual IOStatus send(const Block &block, SocketAddress &addr) throw();
	virtual IOStatus send(const Block &block, const Milliseconds &timeout, SocketAddress &addr) throw();
};

//---------------------------------------------------------------------------------------------

class UdpPeerSocket : public Socket
{
	typedef UdpPeerSocket This;
	typedef Socket Super;

protected:
	SocketAddress peer_addr;

public:
	UdpPeerSocket() throw();
	virtual ~UdpPeerSocket() {}

	UdpPeerSocket(const Host &peer, const Port &peerPort); // connects to "default" peer address
	UdpPeerSocket(const SocketAddress &peer); // connects to peer
	UdpPeerSocket(const LocalSocketAddress &host, const Host &peer, const Port &peerPort); // binds to host, connects to "default" peer address
	UdpPeerSocket(const LocalSocketAddress &host, const SocketAddress &peer); // binds host, connects to peer
	UdpPeerSocket(const LocalIPAddress &host, const IPAddress &peer, const Port &port); // binds host, connects to peer, both use same port

	virtual bool bind(const LocalSocketAddress &host);

	virtual void connect(const SocketAddress &peer);
	virtual void connect(const Host &peer, const Port &peerPort);

	virtual IOStatus recv(Block &block) throw();
	virtual IOStatus recv(Block &block, const Milliseconds &timeout) throw();
	virtual IOStatus send(const Block &block) throw();
	virtual IOStatus send(const Block &block, const Milliseconds &timeout) throw();

	virtual SocketAddress peerAddress() const { return peer_addr; }

	CLASS_EXCEPTION_DEF("magenta::UdpPeerSocket");
	CLASS_SUB_EXCEPTION_DEF(InvalidPeer, "peer does not resolve to an address");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_net_socket_
