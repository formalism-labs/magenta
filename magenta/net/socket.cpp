
#include "rvfc/Include/rvfcTime.h"
#include "magenta/lang/defs.h"
#include "magenta/sync/thread.h"

#if defined(__linux__)
#include <fcntl.h>
#include <netinet/tcp.h>
#endif

#include "socket.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

class Winsock
{
public:
	Winsock()
	{
		WORD wVersionRequested = MAKEWORD(2, 2);
	
		WSADATA wsaData;
		int rc = WSAStartup(wVersionRequested, &wsaData);
		if (rc != 0) 
			THROW_EX(Error);
	
		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
			THROW_EX(VersionMismatch);
	}
	
	~Winsock()
	{
		WSACleanup();
	}

	CLASS_EXCEPTION_DEF("magenta::Winsock");
	CLASS_SUB_EXCEPTION_DEF(InitErr, "Winsock API initialization error"); 
	CLASS_SUB_EXCEPTION_DEF(VersionMismatch, "Winsock API version mismatch"); 
};

GLOBAL(Winsock, winsock);

#endif // _WIN32

///////////////////////////////////////////////////////////////////////////////////////////////
// SocketError
///////////////////////////////////////////////////////////////////////////////////////////////

#define CHECK_ERROR(rc, details) \
	do \
	{ \
		if ((rc) == errorRc()) \
		{ \
			ERROR_SOCK(Error) details; \
		} \
	} \
	while (0)

#define CHECK_ERROR_1(pred, details) \
	do \
	{ \
		if (pred) \
		{ \
			ERROR_SOCK(Error) details; \
		} \
	} \
	while (0)

//---------------------------------------------------------------------------------------------

bool SocketError::isInProgress()
{
#if defined(__linux__)
	return Socket::lastError() == EINPROGRESS;
#elif defined(_WIN32)
	return Socket::lastError() == WSAEINPROGRESS;
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Socket
///////////////////////////////////////////////////////////////////////////////////////////////

void Socket::ctor(Uninitialized)
{
	_fd = badFd();
}

//---------------------------------------------------------------------------------------------

void Socket::ctor(int family, int type, int proto)
{
	if (_fd != badFd())
		THROW_EX(InitErr)("socket already initialized");

	_fd = socket(family, type, proto);
	if (_fd == badFd())
	{
		ERROR_OS(InitErr)("cannot create more sockets");
		return;
	}
#ifdef _WIN32
	DWORD v6only = 0;
	::setsockopt(_fd, SOL_SOCKET, IPV6_V6ONLY, (char *) &v6only, sizeof(v6only));
#endif
}

//---------------------------------------------------------------------------------------------

void Socket::ctor(Type type)
{
	switch (type)
	{
	case Type::TCP:
		ctor(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		break;

	case Type::UDP:
		ctor(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		break;

	default:
		THROW_EX(InitErr)("invalid socket type: %d", (int)type);
	}
}

//---------------------------------------------------------------------------------------------

Socket::~Socket()
{
	_close();
}

//---------------------------------------------------------------------------------------------

void Socket::reopen(Type type)
{
	_close();
	ctor(type);
}

//---------------------------------------------------------------------------------------------

void Socket::_close()
{
	if (_fd == badFd())
		return;
#ifndef _WIN32
	::close(_fd);
#else
	::closesocket(_fd);
#endif
	_fd = badFd();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Socket::_invalidate()
{
	// _fd = badFd(); // this will cause a socket leak
	// _close();
}

//---------------------------------------------------------------------------------------------
// Connection
//---------------------------------------------------------------------------------------------

bool Socket::bind(const LocalIPAddress &host)
{
	return bind(LocalSocketAddress(host, AnyPort()));
}

//---------------------------------------------------------------------------------------------

bool Socket::bind(const LocalSocketAddress &host)
{
	setReuseAddr(true);
	int rc = ::bind(_fd, host.sockaddr(), host.sockaddrLen());
	CHECK_ERROR(rc, ("bind() on address %s failed", host.text().c_str()));
	return !rc;
}

//---------------------------------------------------------------------------------------------

SocketAddress Socket::hostAddress() const
{
	sockaddr_storage sa;
	socklen_t sa_len = sizeof(sa);
	int rc = getsockname(_fd, (struct sockaddr *) &sa, &sa_len);
	CHECK_ERROR(rc, ("getsockname() failed"));
	return SocketAddress((struct sockaddr *) &sa, sa_len);
}

//---------------------------------------------------------------------------------------------
// I/O
//---------------------------------------------------------------------------------------------

Socket::IOStatus Socket::_recv(Block &block, SocketAddress *addr) throw()
{
	int rc;
	if (addr)
	{
		sockaddr_storage sa;
#ifdef _WIN32
		int sa_len = sizeof(sa);
#else
		socklen_t sa_len = sizeof(sa);
#endif
		rc = ::recvfrom(_fd, (char *) block.data(), block.size(), 0, (struct sockaddr *) &sa, &sa_len);
		*addr = SocketAddress((struct sockaddr *) &sa, sa_len);
	}
	else
		rc = ::recv(_fd, (char *) block.data(), block.size(), 0);

	CHECK_ERROR(rc, ("recv() failed"));
	if (rc == errorRc())
		return IOStatus::Error;
			
	block.setLength(rc);
	return rc == 0 ? IOStatus::NotConnected : IOStatus::NoError;
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus Socket::_recv(Block &block, const Milliseconds &timeout, SocketAddress *addr) throw()
{
	IOStatus rc = _select(true, timeout, false);
	if (rc != IOStatus::NoError)
		return rc;
	return _recv(block, addr);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus Socket::_send(const Block &block, const SocketAddress *addr) throw()
{
	int rc;
	if (addr)
		rc = ::sendto(_fd, (char *) block.data(), block.size(), 0, addr->sockaddr(), addr->sockaddrLen());
	else
		rc = ::send(_fd, (char *) block.data(), block.size(), 0);
	CHECK_ERROR(rc, ("send() failed"));
	return rc != errorRc() ? IOStatus::NoError : IOStatus::Error;
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus Socket::_send(const Block &block, const Milliseconds &timeout, const SocketAddress *addr) throw()
{
	IOStatus rc = _select(false, timeout, false);
	if (rc != IOStatus::NoError)
		return rc;
	return _send(block, addr);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus Socket::_select(bool read, const Milliseconds &timeout, bool except)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(_fd, &set);
	fd_set *rd_set = read ? &set : 0, *wr_set = read ? 0 : &set;
	struct timeval to = timeout.timeval();
	int rc = ::select(1 + _fd, rd_set, wr_set, 0, &to);
	if (except)
		CHECK_ERROR(rc, ("select() failed"));
	else if (rc == errorRc())
		return IOStatus::Error;
	return rc == 1 ? IOStatus::NoError : IOStatus::TimeOut;
}

//---------------------------------------------------------------------------------------------
// Socket options
//---------------------------------------------------------------------------------------------

void Socket::setNonBlocking(bool nb)
{
#if defined(_WIN32)
	u_long arg = nb ? 1 : 0;
	int rc = ::ioctlsocket(_fd, FIONBIO, &arg);
	CHECK_ERROR(rc, ("ioctlsocket(FIONBIO) failed"));
#else
	int flags = ::fcntl(_fd, F_GETFL, 0);
	if (nb)
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;
	int rc = fcntl(_fd, F_SETFL, flags);
	CHECK_ERROR(rc, ("fcntl(O_NONBLOCK) failed"));
#endif
}

//---------------------------------------------------------------------------------------------

void Socket::setKeepAlive(bool ka)
{
	int opt = ka ? 1 : 0;
	int rc = ::setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, (const char *) &opt, sizeof(opt));
	CHECK_ERROR(rc, ("setsockopt(SO_KEEPALIVE) failed"));
}

//---------------------------------------------------------------------------------------------

void Socket::setLingerOff()
{
	struct linger linger;
	linger.l_onoff = 0;
	int rc = ::setsockopt(_fd, SOL_SOCKET, SO_LINGER, (const char *) &linger, sizeof(linger));
	CHECK_ERROR(rc, ("setsockopt(SO_LINGER) failed"));
}

//---------------------------------------------------------------------------------------------

void Socket::setLinger(const Seconds &time)
{
	struct linger linger;
	linger.l_onoff = 1;
	linger.l_linger = (u_short) time.value();
	int rc = ::setsockopt(_fd, SOL_SOCKET, SO_LINGER, (const char *) &linger, sizeof(linger));
	CHECK_ERROR(rc, ("setsockopt(SO_LINGER) failed"));
}

//---------------------------------------------------------------------------------------------

void Socket::setReuseAddr(bool reuse)
{
	int n = reuse ? 1 : 0;
	int rc = ::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &n, sizeof(n));
	CHECK_ERROR(rc, ("setsockopt(SO_REUSEADDR) failed"));
}

//---------------------------------------------------------------------------------------------

void Socket::setKeepIdle(int idle)
{
#if defined(__linux__)
	int opt = idle ? idle : 120;
	int rc = ::setsockopt(_fd, IPPROTO_TCP, TCP_KEEPIDLE, (const char *) &opt, sizeof(opt));
	CHECK_ERROR(rc, ("setsockopt(TCP_KEEPIDLE) failed"));
#endif
}

//---------------------------------------------------------------------------------------------

void Socket::setKeepIntvl(int intvl)
{
#if defined(__linux__)	
	int opt = intvl ? intvl : 30;
	int rc = ::setsockopt(_fd, IPPROTO_TCP, TCP_KEEPINTVL, (const char *) &opt, sizeof(opt));
	CHECK_ERROR(rc, ("setsockopt(TCP_KEEPINTVL) failed"));
#endif
}

//---------------------------------------------------------------------------------------------

void Socket::setKeepCnt(int cnt)
{
#if defined(__linux__)	
	int opt = cnt ? cnt : 3;
	int rc = ::setsockopt(_fd, IPPROTO_TCP, TCP_KEEPCNT, (const char *) &opt, sizeof(opt));
	CHECK_ERROR(rc, ("setsockopt(TCP_KEEPCNT) failed"));
#endif
}

//---------------------------------------------------------------------------------------------

void Socket::setSendBuff(int size)
{
	int rc = ::setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, (const char *) &size, sizeof(size));
	CHECK_ERROR(rc, ("setsockopt(SO_SNDBUF) failed"));
}

//---------------------------------------------------------------------------------------------

void Socket::setRecvBuff(int size)
{
	int rc = ::setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, (const char *) &size, sizeof(size));
	CHECK_ERROR(rc, ("setsockopt(SO_RCVBUF) failed"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// TcpSocket
///////////////////////////////////////////////////////////////////////////////////////////////

TcpSocket::TcpSocket() throw () : Socket(Uninitialized())
{
	ctor();
}

//---------------------------------------------------------------------------------------------

TcpSocket::TcpSocket(Uninitialized) : Socket(Uninitialized())
{
	ctor(false);
}

//---------------------------------------------------------------------------------------------

TcpSocket::TcpSocket(bool real) : Socket(Uninitialized())
{
	ctor(real);
}

//---------------------------------------------------------------------------------------------

TcpSocket::TcpSocket(const SocketAddress &peer) : Socket(Uninitialized())
{
	ctor(peer.ip(), peer.port());
}

//---------------------------------------------------------------------------------------------

TcpSocket::TcpSocket(const IPAddress &peer, const Port &port) : Socket(Uninitialized())
{
	ctor(peer, port); 
}

//---------------------------------------------------------------------------------------------

TcpSocket::TcpSocket(const Host &peer, const Port &port) : Socket(Uninitialized()) 
{
	ctor(peer, port); 
}

//---------------------------------------------------------------------------------------------

TcpSocket::TcpSocket(const LocalSocketAddress &host, const SocketAddress &peer) : Socket(Uninitialized())
{
	ctor(host, peer.ip(), peer.port());
}

//---------------------------------------------------------------------------------------------

TcpSocket::TcpSocket(const LocalSocketAddress &host, const Host &peer, const Port &port) : Socket(Uninitialized())
{
	ctor(host, peer, port);
}

//---------------------------------------------------------------------------------------------

TcpSocket::TcpSocket(const AcceptedSocket &sock) : Socket(Uninitialized())
{
	ctor(sock);
}

//---------------------------------------------------------------------------------------------

TcpSocket::~TcpSocket()
{
}

//---------------------------------------------------------------------------------------------

void TcpSocket::ctor(bool real)
{
	if (real)
	{
		Super::ctor(Type::TCP);
		_state = State::Created;
	}
	else
	{
		Super::ctor(Uninitialized());
		_state = State::Uninitialized;
	}

	wait_for_server = false;
	connect_tries = 10;
	connect_interval = Seconds(1);
}

//---------------------------------------------------------------------------------------------

void TcpSocket::ctor(const IPAddress &ip, const Port &port)
{
	ctor();
	connect(ip, port);
}

//---------------------------------------------------------------------------------------------

void TcpSocket::ctor(const LocalSocketAddress &host, const IPAddress &ip, const Port &port)
{
	ctor();
	if (bind(host))
		connect(ip, port);
}

//---------------------------------------------------------------------------------------------

void TcpSocket::ctor(const Host &peer, const Port &port)
{
	ctor();
	connect(peer, port);
}

//---------------------------------------------------------------------------------------------

void TcpSocket::ctor(const LocalSocketAddress &host, const Host &peer, const Port &port)
{
	ctor();
	if (bind(host))
		connect(peer, port);
}

//---------------------------------------------------------------------------------------------

void TcpSocket::ctor(const AcceptedSocket &sock)
{
	_fd = sock._fd;
	if (isValid())
	{
		_state = State::Connected;
		peer_addr = sock._peer;
	}
	else
		_state = State::Disconnected;

	const_cast<AcceptedSocket&>(sock).setAccepted();
}

//---------------------------------------------------------------------------------------------

TcpSocket &TcpSocket::operator=(const AcceptedSocket &sock)
{
	_close();
	ctor(sock);
	return *this;
}

//---------------------------------------------------------------------------------------------

void TcpSocket::reopen()
{
	Super::reopen(Type::TCP);
	bind();
}

///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
// Connection
//---------------------------------------------------------------------------------------------

bool TcpSocket::bind(const LocalSocketAddress &host)
{
	if (Super::bind(host))
		return false;
	host_addr = host;
	return true;
}

//---------------------------------------------------------------------------------------------

bool TcpSocket::bind()
{
	return Super::bind(host_addr);
}

//---------------------------------------------------------------------------------------------

void TcpSocket::assertConnected()
{
	if (!isConnected())
		ERROR_EX(NotConnected);
}

//---------------------------------------------------------------------------------------------

bool TcpSocket::connect(const SocketAddress &sa) throw()
{
	int tries = connect_tries;
	for (;;)
	{
		int rc = ::connect(_fd, sa.sockaddr(), sa.sockaddrLen());
		if (rc == errorRc())
		{
			if (wait_for_server && --tries > 0)
			{
				sleep(connect_interval);
				continue;
			}

			_state = State::Disconnected;
			CHECK_ERROR(rc, ("connect() failed"));
			return false;
		}
		_state = State::Connected;
		return true;
	}
}

//---------------------------------------------------------------------------------------------

bool TcpSocket::connect(const Host &peer, const Port &port) throw()
{
	Host::Addresses addrs = peer.addresses();
	for (Host::Addresses::Iterator i_addr = addrs; !!i_addr; ++i_addr)
	{
		SocketAddress sa(*i_addr, port);
		if (connect(sa))
			return true;
		reopen();
	}
	return false;
}

//---------------------------------------------------------------------------------------------

TcpSocket &TcpSocket::waitForServer(bool wait)
{
	wait_for_server = wait;
	return *this;
}

//---------------------------------------------------------------------------------------------

TcpSocket &TcpSocket::waitForServer(int tries, const Milliseconds &interval)
{
	wait_for_server = true;
	connect_tries = tries;
	connect_interval = interval;
	return *this;
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus TcpSocket::shutdown(bool send, bool recv) throw()
{
	assertConnected();

#ifdef _WIN32
#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH
#endif

	int what = 0;
	if (send && !recv)
		what = SHUT_WR;
	else if (!send && recv)
		what = SHUT_RD;
	else if (!send && !recv)
		what = SHUT_RDWR;

	int rc = ::shutdown(_fd, what);
	return rc == errorRc() ? IOStatus::NoError : IOStatus::Error;
}

//---------------------------------------------------------------------------------------------
// I/O
//---------------------------------------------------------------------------------------------

Socket::IOStatus TcpSocket::recv(Block &block) throw()
{
	if (!isConnected())
		return IOStatus::NotConnected;
	return _recv(block);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus TcpSocket::recv(Block &block, const Milliseconds &timeout) throw()
{
	if (!isConnected())
		return IOStatus::NotConnected;
	return _recv(block, timeout);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus TcpSocket::send(const Block &block) throw()
{
	if (!isConnected())
		return IOStatus::NotConnected;
	return _send(block);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus TcpSocket::send(const Block &block, const Milliseconds &timeout) throw()
{
	if (!isConnected())
		return IOStatus::NotConnected;
	return _send(block, timeout);
}

//---------------------------------------------------------------------------------------------

TcpSocket &TcpSocket::operator<<(const Block &block)
{
	if (send(block) != IOStatus::NoError)
		THROW_EX(IOErr);
	return *this; 
}

//---------------------------------------------------------------------------------------------

TcpSocket &TcpSocket::operator>>(Block &block)
{
	if (recv(block) != IOStatus::NoError)
		THROW_EX(IOErr);
	return *this; 
}

//---------------------------------------------------------------------------------------------
// String I/O
//---------------------------------------------------------------------------------------------

/*
Socket::IOStatus TcpSocket::recv(string &str) throw()
{
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus TcpSocket::send(const string &str) throw()
{
}
*/

//---------------------------------------------------------------------------------------------

TcpSocket &TcpSocket::operator<<(string &s)
{
	return *this;
}

//---------------------------------------------------------------------------------------------

TcpSocket &TcpSocket::operator>>(string &s)
{
	return *this;
}

//---------------------------------------------------------------------------------------------
// Socket options
//---------------------------------------------------------------------------------------------

void TcpSocket::setNoDelay(bool nodelay)
{
	int n = nodelay ? 1 : 0;
	int rc = setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (char *) &n, sizeof(n));
	CHECK_ERROR(rc, ("setsockopt(TCP_NODELAY) failed"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// AcceptedSocket
///////////////////////////////////////////////////////////////////////////////////////////////

AcceptedSocket::AcceptedSocket(int fd, const SocketAddress &peer) : 
	Socket(Uninitialized()), _peer(peer)
{
	_fd = fd;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// TcpServerSocket
///////////////////////////////////////////////////////////////////////////////////////////////

TcpServerSocket::~TcpServerSocket()
{
	if (terminate_socket)
	{
		terminate_socket->shutdown();
		delete terminate_socket;
	}
}

//---------------------------------------------------------------------------------------------

void TcpServerSocket::ctor(const LocalSocketAddress &host, int backlog)
{
	Super::ctor(Type::TCP);
	_terminating = false;
	terminate_socket = 0;
	_backlog = backlog;
	if (!Super::bind(host))
		return;
	host_addr = host;

	int rc = ::listen(_fd, _backlog);
	CHECK_ERROR(rc, ("listen() failed"));
}

//---------------------------------------------------------------------------------------------

AcceptedSocket TcpServerSocket::accept()
{
	if (_terminating)
		return AcceptedSocket();

	sockaddr_storage sa;
	socklen_t len = sizeof(sa);
	int fd = ::accept(_fd, (struct sockaddr *) &sa, &len);
	if (_terminating)
	{
		if (fd != badFd())
		{
			::shutdown(fd, SHUT_RDWR);
#ifndef _WIN32
			::close(fd);
#else
			::closesocket(fd);
#endif
		}
		return AcceptedSocket();
	}

	CHECK_ERROR_1(fd == badFd(), ("accept() failed"));
	return AcceptedSocket(fd, SocketAddress((struct sockaddr *) &sa, len));
}

//---------------------------------------------------------------------------------------------

AcceptedSocket TcpServerSocket::accept(const Milliseconds &timeout, Socket::IOStatus &status)
{
	if (_terminating)
		return AcceptedSocket();

	status = _select(true, timeout, false);
	if (status != IOStatus::NoError)
		return AcceptedSocket();
	
	return accept();
}

//---------------------------------------------------------------------------------------------

void TcpServerSocket::terminate()
{
	_terminating = true;
	terminate_socket = new TcpSocket;
	SocketAddress peer;
	if (!host_addr.ip())
		peer = SocketAddress(host_addr.isV4() ? (const IPAddress &) LoopbackIPv4Address() : 
			(const IPAddress &)LoopbackIPv6Address(), host_addr.port());
	else
		peer = host_addr;
	terminate_socket->setNonBlocking(true);
	terminate_socket->connect(peer);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// UdpSocket
///////////////////////////////////////////////////////////////////////////////////////////////

void UdpSocket::ctor()
{
	Super::ctor(Type::UDP);
}

//---------------------------------------------------------------------------------------------

void UdpSocket::ctor(const LocalSocketAddress &host)
{
	Super::ctor(Type::UDP);
	bind(host);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Socket::IOStatus UdpSocket::recv(Block &block, SocketAddress &addr) throw()
{
	return _recv(block, &addr);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus UdpSocket::recv(Block &block, const Milliseconds &timeout, SocketAddress &addr) throw()
{
	return _recv(block, timeout, &addr);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus UdpSocket::send(const Block &block, SocketAddress &addr) throw()
{
	return _send(block, &addr);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus UdpSocket::send(const Block &block, const Milliseconds &timeout, SocketAddress &addr) throw()
{
	return _send(block, timeout, &addr);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// UdpPeerSocket
///////////////////////////////////////////////////////////////////////////////////////////////

UdpPeerSocket::UdpPeerSocket() throw() : Socket(Type::UDP)
{
}

//---------------------------------------------------------------------------------------------

UdpPeerSocket::UdpPeerSocket(const Host &peer, const Port &peerPort) : Socket(Type::UDP)
{
	connect(peer, peerPort);
}

//---------------------------------------------------------------------------------------------

UdpPeerSocket::UdpPeerSocket(const SocketAddress &peer) : Socket(Type::UDP)
{
	connect(peer);
}

//---------------------------------------------------------------------------------------------

UdpPeerSocket::UdpPeerSocket(const LocalSocketAddress &host, const Host &peer, const Port &peerPort) :
	Socket(Type::UDP)
{
	if (bind(host))
		connect(peer, peerPort);
}

//---------------------------------------------------------------------------------------------

UdpPeerSocket::UdpPeerSocket(const LocalSocketAddress &host, const SocketAddress &peer) : 
	Socket(Type::UDP)
{
	if (bind(host))
		connect(peer);
}

//---------------------------------------------------------------------------------------------

UdpPeerSocket::UdpPeerSocket(const LocalIPAddress &host, const IPAddress &peer, const Port &port) : 
	Socket(Type::UDP)
{
	if (bind(LocalSocketAddress(host, port)))
		connect(SocketAddress(peer, port));
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool UdpPeerSocket::bind(const LocalSocketAddress &host)
{
	return Super::bind(host);
}

//---------------------------------------------------------------------------------------------

void UdpPeerSocket::connect(const SocketAddress &peer)
{
	int rc = ::connect(_fd, peer.sockaddr(), peer.sockaddrLen());
	CHECK_ERROR(rc, ("connect() failed"));
}

//---------------------------------------------------------------------------------------------

void UdpPeerSocket::connect(const Host &peer, const Port &peerPort)
{
	Host::Addresses addrs = peer.addresses();
	Host::Addresses::Iterator i_addr = addrs;
	if (!i_addr)
	{
		ERROR_EX(InvalidPeer)("peer=%s", peer.name().c_str());
		return;
	}
	SocketAddress sa(*i_addr, peerPort);
	connect(sa);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus UdpPeerSocket::recv(Block &block) throw()
{
	return _recv(block);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus UdpPeerSocket::recv(Block &block, const Milliseconds &timeout) throw()
{
	return _recv(block, timeout);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus UdpPeerSocket::send(const Block &block) throw()
{
	return _send(block);
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus UdpPeerSocket::send(const Block &block, const Milliseconds &timeout) throw()
{
	return _send(block, timeout);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
