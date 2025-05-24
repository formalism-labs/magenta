
#ifndef _magenta_net_secure_socket_
#define _magenta_net_secure_socket_

#include "magenta/classes.h"
#include "magenta/security/general.h"
#include "magenta/security/context.h"
#include "magenta/net/socket.h"

#include OPENSSL_INC(ssl.h)
#include OPENSSL_INC(err.h)

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

enum__(SecurityMode)
	None,
	BestEffort,
	Secure
__enum(SecurityMode);

//---------------------------------------------------------------------------------------------

class Session
{
	SSL_SESSION *_session;

public:
	Session(const SecureSocket &ss);
	~Session();
	
	operator SSL_SESSION*() const { return _session; }
};

//---------------------------------------------------------------------------------------------

class AcceptedSecureSocket
{
	friend class SecureSocket;
	friend class SecureServerSocket;

	AcceptedSocket acc_sock;

protected:
	ServerContext::ClientAuthentication cli_auth;
	ServerContext *_ctx;
	SecurityMode security_mode;

	AcceptedSecureSocket() : cli_auth(ServerContext::ClientAuthentication::None), _ctx(0), 
		security_mode(SecurityMode::Secure) {} // for server termination

	AcceptedSecureSocket(AcceptedSocket &sock, ServerContext::ClientAuthentication cliAuth, 
		ServerContext *ctx, SecurityMode securityMode);

	virtual void setAccepted() { acc_sock.setAccepted(); }

	virtual bool isValid() const { return acc_sock.isValid(); }
};

//---------------------------------------------------------------------------------------------

class SecureSocket : public ITcpSocket
{
	typedef SecureSocket This;

	friend class Session;

	static ClientContext *default_ctx;
	static int ssl_ex_data_index;

	Context *_ctx;
	TcpSocket _sock;
	SSL *_ssl;
	BIO *_bio, *bufio_bio;
	bool server_mode;
	ServerContext::ClientAuthentication cli_auth;

public:
	typedef Socket::Uninitialized Uninitialized;
	typedef Socket::IOStatus IOStatus;

	//-----------------------------------------------------------------------------------------
	// Construction
	//-----------------------------------------------------------------------------------------
protected:
	void ctor();
	void ctor(const IPAddress &ip, const Port &port);
	void ctor(const LocalSocketAddress &host, const IPAddress &ip, const Port &port);
	void ctor(const Host &peer, const Port &port);
	void ctor(const LocalSocketAddress &host, const Host &peer, const Port &port);
	void ctor(const AcceptedSecureSocket &sock);

	virtual bool materialize();

public:
	SecureSocket(Uninitialized);
	SecureSocket(bool real);
	SecureSocket();

	SecureSocket(const SocketAddress &peer);
	SecureSocket(const IPAddress &peer, const Port &port);
	SecureSocket(const Host &peer, const Port &port);

	SecureSocket(const LocalSocketAddress &host, const SocketAddress &peer);
	SecureSocket(const LocalSocketAddress &host, const Host &peer, const Port &port);

	SecureSocket(const AcceptedSecureSocket &sock);
	~SecureSocket();

	This &operator=(const AcceptedSecureSocket &sock);

	virtual int fd() const { return _sock.fd(); }

	virtual bool isValid() const { return _sock.isValid(); }

	virtual void setNoDelay(bool nodelay) { _sock.setNoDelay(nodelay); }
	
	void setKeepIdle(int idle = 0) { _sock.setKeepIdle(idle); }
	void setKeepIntvl(int intvl = 0) { _sock.setKeepIntvl(intvl); }
	void setKeepCnt(int cnt = 0) { _sock.setKeepCnt(cnt); }

	//-----------------------------------------------------------------------------------------
	// Context and authentication
	//-----------------------------------------------------------------------------------------
public:
	void setSecurityMode(SecurityMode mode) { security_mode = mode; }

protected:
	SecurityMode security_mode;
	bool is_secure;

public:
	static ClientContext &defaultContext();
	static void setDefaultContext(ClientContext *ctx);

	ClientContext &context();
	bool setContext(Context *ctx);

	void setServerAuthentication(bool b);

	bool renegotiate();
	bool rehandshake();

	Certificate peerCertificate() const;

protected:
	bool checkCertificate(const char *host);
	
	virtual bool verify(bool verified, const Certificate &cert);
	int _verify(int preverify_ok, X509_STORE_CTX *ctx);
	static int verify_callback(int preverify_ok, X509_STORE_CTX *ctx);

	//-----------------------------------------------------------------------------------------
	// Connection
	//-----------------------------------------------------------------------------------------
public:
	virtual bool bind(const LocalIPAddress &host) { return _sock.bind(host); }
	virtual bool bind(const LocalSocketAddress &host) { return _sock.bind(host); }
	virtual bool bind() { return _sock.bind(); }

protected:
	bool ssl_connect(const char *peer);

public:
	bool connect(const SocketAddress &peer) throw();
	bool connect(const IPAddress &peer, const Port &port) throw();
	bool connect(const Host &peer, const Port &port) throw();
	bool isConnected() const  throw() { return _sock.isConnected(); }
	This &waitForServer(bool wait = true) { _sock.waitForServer(wait); return *this; }
	This &waitForServer(int tries, const Milliseconds &interval = Seconds(1))
		{ _sock.waitForServer(tries, interval); return *this; }

	bool resume(Session &session);

	SocketAddress hostAddress() const throw() { return _sock.hostAddress(); }
	SocketAddress peerAddress() const  throw() { return _sock.peerAddress(); }

	//-----------------------------------------------------------------------------------------
	// I/O
	//-----------------------------------------------------------------------------------------
public:
	Socket::IOStatus recv(Block &block) throw();
	Socket::IOStatus send(const Block &block) throw();

	This &operator<<(const Block &block);
	This &operator>>(Block &block);

private:
	// not implemented
	virtual Socket::IOStatus recv(Block &block, const Milliseconds &timeout) throw() { return Socket::IOStatus::Error; }
	virtual Socket::IOStatus send(const Block &block, const Milliseconds &timeout) throw() { return Socket::IOStatus::Error; }

public:
	// String I/O
	Socket::IOStatus recv(string &s) throw();
	Socket::IOStatus send(const string &s) throw();

	This &operator<<(const string &s);
	This &operator>>(string &s);

	// Text line I/O
public:
	Socket::IOStatus gets(string &str) throw();
	Socket::IOStatus puts(const string &str) throw();
	Socket::IOStatus flush() throw();

protected:
	BIO *bufferedBIO();
	
	//-----------------------------------------------------------------------------------------
	// Termination
	//-----------------------------------------------------------------------------------------
public:
	virtual void shutdown();
	
private:
	virtual Socket::IOStatus shutdown(bool send, bool recv) throw() { shutdown(); return Socket::IOStatus::Error; }

	//-----------------------------------------------------------------------------------------
	// Errors
	//-----------------------------------------------------------------------------------------
private:
	void _invalidate();

public:
	CLASS_EXCEPTION_DEF("magenta::SecureSocket");
	CLASS_SUB_EXCEPTION_DEF(InitErr, "error initializing secure socket");
	CLASS_SUB_EXCEPTION_DEF(AcceptErr, "accept error");
	CLASS_SUB_EXCEPTION_DEF(AuthenticationErr, "authentication error");
	CLASS_SUB_EXCEPTION_DEF(ConnectErr, "connection error");
	CLASS_SUB_EXCEPTION_DEF(IOErr, "I/O error");
	CLASS_SUB_EXCEPTION_DEF(ShutdownErr, "shutdown error");

	//-----------------------------------------------------------------------------------------
	// Inherited
	//-----------------------------------------------------------------------------------------
public:
	void reopen() { _sock.reopen(); }

	void setNonBlocking(bool nb) { _sock.setNonBlocking(nb); }
	void setKeepAlive(bool ka) { _sock.setKeepAlive(ka); }
	void setLingerOff() { _sock.setLingerOff(); }
	void setLinger(const Seconds &time) { _sock.setLinger(time); }
	void setReuseAddr(bool reuse) { _sock.setReuseAddr(reuse); }
	void setSendBuff(int size) { _sock.setSendBuff(size); }
	void setRecvBuff(int size) { _sock.setRecvBuff(size); }

};

//---------------------------------------------------------------------------------------------

class SecureServerSocket : public ITcpServerSocket
{
protected:
	ServerContext *_ctx;
	static ServerContext *default_ctx;

	ServerContext::ClientAuthentication cli_auth;
	TcpServerSocket _sock;
	SecurityMode security_mode;

	void ctor();

public:
	typedef Socket::IOStatus IOStatus;

	//-----------------------------------------------------------------------------------------
	// Construction
	//-----------------------------------------------------------------------------------------
public:
	struct CInit
	{
		Port			mPort;
		int				mBacklog;
		LocalIPAddress	mIpAddr;

		CInit(const LocalIPAddress&	IpAddr, const Port& port, int backlog = 128) : 
			mIpAddr(IpAddr), mPort(port), mBacklog(backlog) {}
	};

	SecureServerSocket(const Port &port, int backlog = 128) : _sock(port, backlog) { ctor(); }
	SecureServerSocket(const LocalSocketAddress &host, int backlog = 128) : _sock(host, backlog) { ctor(); }
	SecureServerSocket(const CInit& init) : _sock(LocalSocketAddress(init.mIpAddr, init.mPort), init.mBacklog) { ctor(); }

	//-----------------------------------------------------------------------------------------
	// Authentication
	//-----------------------------------------------------------------------------------------
public:
	void setSecurityMode(SecurityMode mode) { security_mode = mode; }

	void setClientAuthentication(ServerContext::ClientAuthentication auth) { cli_auth = auth; }

	static ServerContext &defaultContext();
	static void setDefaultContext(ServerContext *ctx) { default_ctx = ctx; }

	ServerContext &context() { return *_ctx; }
	void setContext(ServerContext *ctx) { _ctx = ctx; }
	
	void setKeepAliveOpt();

	//-----------------------------------------------------------------------------------------
	// Accept
	//-----------------------------------------------------------------------------------------
public:
	virtual AcceptedSecureSocket accept();
	virtual AcceptedSecureSocket accept(const Milliseconds &timeout, Socket::IOStatus &status);
	AcceptedSecureSocket accept(const Milliseconds &timeout) { Socket::IOStatus status; return accept(timeout, status); }

	//-----------------------------------------------------------------------------------------
	// Termination
	//-----------------------------------------------------------------------------------------
public:
	virtual void terminate() { _sock.terminate(); }
	bool isTerminating() const { return _sock.isTerminating(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_net_secure_socket_
