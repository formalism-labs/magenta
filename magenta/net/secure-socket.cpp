
#if defined(__linux__)
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#endif

#include "secure-socket.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Session
///////////////////////////////////////////////////////////////////////////////////////////////

Session::Session(const SecureSocket &ss)
{
	_session = SSL_get1_session(ss._ssl);
}
	
//---------------------------------------------------------------------------------------------

Session::~Session()
{
	SSL_SESSION_free(_session);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// SecureSocket
///////////////////////////////////////////////////////////////////////////////////////////////

ClientContext *SecureSocket::default_ctx = 0;

int SecureSocket::ssl_ex_data_index = -1;

//---------------------------------------------------------------------------------------------

ClientContext &SecureSocket::defaultContext()
{
	if (!default_ctx)
		default_ctx = new ClientContext;
	return *default_ctx;
}

//---------------------------------------------------------------------------------------------
// Construction
//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket(Uninitialized) : _sock(Uninitialized())
{
}

//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket(bool real) : _sock(real)
{
}

//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket()
{
	ctor();
}

//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket(const SocketAddress &peer)
{
	ctor(peer.ip(), peer.port());
}

//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket(const IPAddress &peer, const Port &port)
{
	ctor(peer, port);
}

//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket(const Host &peer, const Port &port)
{
	ctor(peer, port);
}

//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket(const LocalSocketAddress &host, const SocketAddress &peer)
{
	ctor(host, peer.ip(), peer.port());
}

//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket(const LocalSocketAddress &host, const Host &peer, const Port &port)
{
	ctor(host, peer, port);
}

//---------------------------------------------------------------------------------------------

SecureSocket::SecureSocket(const AcceptedSecureSocket &sock) : _sock(Uninitialized())
{
	ctor(sock);
}

//---------------------------------------------------------------------------------------------

SecureSocket::~SecureSocket()
{
	if (bufio_bio)
		BIO_free_all(bufio_bio);

	if (_ssl)
	{
		if (is_secure)
			SSL_shutdown(_ssl);
		SSL_free(_ssl); // also frees _bio
	}
}

//---------------------------------------------------------------------------------------------

void SecureSocket::ctor()
{
	_ctx = &defaultContext();
	_ssl = 0;

	_bio = 0;
	bufio_bio = 0;

	server_mode = false;

	security_mode = SecurityMode::Secure;
	is_secure = true;
	
	if (ssl_ex_data_index == -1)
		ssl_ex_data_index = SSL_get_ex_new_index(0, const_cast<void*>((void *)"magenta::SecureSocket"), 0, 0, 0);
}

//---------------------------------------------------------------------------------------------

void SecureSocket::ctor(const IPAddress &ip, const Port &port)
{
	ctor();
	_sock.ctor(ip, port);
}

//---------------------------------------------------------------------------------------------

void SecureSocket::ctor(const LocalSocketAddress &host, const IPAddress &ip, const Port &port)
{
	ctor();
	_sock.ctor(host, ip, port);
}

//---------------------------------------------------------------------------------------------

void SecureSocket::ctor(const Host &peer, const Port &port)
{
	ctor();
	_sock.ctor(peer, port);
}

//---------------------------------------------------------------------------------------------

void SecureSocket::ctor(const LocalSocketAddress &host, const Host &peer, const Port &port)
{
	ctor();
	_sock.ctor(host, peer, port);
}

//---------------------------------------------------------------------------------------------

void SecureSocket::ctor(const AcceptedSecureSocket &sock)
{
	ctor();

	_sock.ctor(sock.acc_sock);
	_ctx  = sock._ctx;
	server_mode = true;
	security_mode = sock.security_mode;

	if (!isValid())
		return;

	if (!materialize())
		_invalidate();
	const_cast<AcceptedSecureSocket&>(sock).setAccepted();

	if (is_secure && cli_auth == ServerContext::ClientAuthentication::Rehandshake)
		rehandshake();
}

//---------------------------------------------------------------------------------------------

void SecureSocket::_invalidate()
{
	_sock._invalidate();
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::materialize()
{
	if (_ssl)
	{
		SSL_shutdown(_ssl);
		SSL_free(_ssl);
	}

	_bio = BIO_new_socket(_sock.fd(), BIO_NOCLOSE);

	if (security_mode == SecurityMode::None)
	{
		is_secure = false;
		return true;
	}

	_ssl = SSL_new(*_ctx);
	SSL_set_ex_data(_ssl, ssl_ex_data_index, this);
	SSL_set_bio(_ssl, _bio, _bio);

	if (!server_mode)
	{
		SSL_set_connect_state(_ssl);
		//@@ SSL_set_verify(_ssl, SSL_VERIFY_PEER, verify_callback);
	}
	else
	{
		SSL_set_accept_state(_ssl);
		//@@ SSL_set_verify(_ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT | SSL_VERIFY_CLIENT_ONCE, verify_callback);
		int rc = SSL_accept(_ssl);
		if (rc != 1)
		{
			// due to SGC (Server Gated Cryptography)
			int rc1 = SSL_get_error(_ssl, rc);
			if (rc1 == SSL_ERROR_WANT_READ || rc1 == SSL_ERROR_WANT_WRITE)
				rc = SSL_accept(_ssl);
		}

		if (rc != 1)
		{
			if (security_mode == SecurityMode::Secure)
			{
				is_secure = true;

				ERROR_SSL(AcceptErr);
				return false;
			}
			else
				is_secure = false;
		}
		else
			is_secure = true;
	}
	
	return true;
}

//---------------------------------------------------------------------------------------------

SecureSocket &SecureSocket::operator=(const AcceptedSecureSocket &sock)
{
	if (_sock.isValid())
		L0G(warning, "socket with fd=%d is about to be closed by assignment", _sock.fd());
	_sock._close();

	ctor(sock);

	return *this;
}

//---------------------------------------------------------------------------------------------
// Context
//---------------------------------------------------------------------------------------------

bool SecureSocket::setContext(Context *ctx)
{
	if (_ssl)
	{
		// TODO: log something
		return false;
	}
	_ctx = ctx;
	return true;
}

//---------------------------------------------------------------------------------------------

void SecureSocket::setDefaultContext(ClientContext *ctx)
{
	default_ctx = ctx;
}

//---------------------------------------------------------------------------------------------
// Connection
//---------------------------------------------------------------------------------------------

bool SecureSocket::ssl_connect(const char *peer)
{
	if (security_mode == SecurityMode::None)
	{
		is_secure = false;
		return true;
	}

	int rc = SSL_connect(_ssl);
	if (rc <= 0)
	{
		if (security_mode == SecurityMode::Secure)
		{
			is_secure = true;

			int io_err = SSL_get_error(_ssl, rc);
			ERROR_SSL(ConnectErr);
			return false;
		}
		else
		{
			is_secure = false;
			return true;
		}
	}
	else
		is_secure = true;

	if (server_mode && !checkCertificate(peer))
	{
		ERROR_SSL(ConnectErr)("cannot verify %s", peer);
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::connect(const SocketAddress &peer) throw()
{
	if (!_sock.connect(peer))
		return false;
	if (!materialize())
		return false;
	return ssl_connect(peer.text().c_str());
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::connect(const IPAddress &peer, const Port &port) throw()
{
	if (!_sock.connect(peer, port))
		return false;
	if (!materialize())
		return false;
	return ssl_connect(peer.text().c_str());
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::connect(const Host &peer, const Port &port) throw()
{
	if (!_sock.connect(peer, port))
		return false;
	if (!materialize())
		return false;
	return ssl_connect(peer.name().c_str());
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::resume(Session &session)
{
	return !!SSL_set_session(_ssl, session);
}

//---------------------------------------------------------------------------------------------
// Block I/O
//---------------------------------------------------------------------------------------------

Socket::IOStatus SecureSocket::recv(Block &block) throw()
{
	if (!is_secure)
		return _sock.recv(block);

	for (;;)
	{
		int rc = SSL_read(_ssl, (char *) block.data(), block.size());
		if (rc > 0)
		{
			block.setLength(rc);
			return IOStatus::NoError;
		}

		int err = SSL_get_error(_ssl, rc);
		int shutdown_state = SSL_get_shutdown(_ssl);
		if (shutdown_state & SSL_RECEIVED_SHUTDOWN)
			err = SSL_ERROR_ZERO_RETURN;
		switch (err)
		{
		case SSL_ERROR_NONE:
			block.setLength(rc);
			return IOStatus::NoError;
	
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			continue;
	
		case SSL_ERROR_ZERO_RETURN:
			shutdown();
			return IOStatus::NotConnected;

		case SSL_ERROR_SSL:
		case SSL_ERROR_SYSCALL:
			shutdown();
			return !rc ? IOStatus::NotConnected : IOStatus::Error;
	
		default:
			return IOStatus::Error;
		}
	}	
	return IOStatus::Error;
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus SecureSocket::send(const Block &block) throw()
{
	if (!is_secure)
		return _sock.send(block);

	const char *p = (char *) block.data();
	int len = block.length();
	while (len > 0)
	{
		int rc = SSL_write(_ssl, p, len);
		switch (SSL_get_error(_ssl, rc))
		{
		case SSL_ERROR_NONE:
			p += rc;
			len -= rc;
			break;
	
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			continue;

		case SSL_ERROR_ZERO_RETURN:
		case SSL_ERROR_SSL:
		case SSL_ERROR_SYSCALL:
			shutdown();
			return IOStatus::Error;

		default:
			return IOStatus::Error;
		}
	}
	
	return IOStatus::NoError;
}

//---------------------------------------------------------------------------------------------

SecureSocket &SecureSocket::operator<<(const Block &block)
{
	if (send(block) != IOStatus::NoError)
		THROW_EX(IOErr);
	return *this; 
}

//---------------------------------------------------------------------------------------------

SecureSocket &SecureSocket::operator>>(Block &block)
{
	if (recv(block) != IOStatus::NoError)
		THROW_EX(IOErr);
	return *this; 
}

//---------------------------------------------------------------------------------------------
// String I/O
//---------------------------------------------------------------------------------------------

Socket::IOStatus SecureSocket::recv(string &str) throw()
{
#define BUFSIZE 2
	char s[BUFSIZE+1];
	Block b1((UINT8*) s, BUFSIZE);
	IOStatus rc = recv(b1);
	if (rc == IOStatus::NoError)
	{
		s[b1.length()] = '\0';
		//s[BUFSIZE] = '\0';
		str = s;
		int n = SSL_pending(_ssl);
		if (n > 0)
		{
			str.resize(str.length() + n);
			Block b2((UINT8*)str.c_str() + BUFSIZE, n);
			rc = recv(b2);
		}
	}
	return rc;
#undef BUFSIZE
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus SecureSocket::send(const string &s) throw()
{
	return send(Block(s));
}

//---------------------------------------------------------------------------------------------

SecureSocket &SecureSocket::operator>>(string &s)
{
	if (recv(s) != IOStatus::NoError)
		THROW_EX(IOErr);
	return *this;
}

//---------------------------------------------------------------------------------------------

SecureSocket &SecureSocket::operator<<(const string &s)
{
	if (send(s) != IOStatus::NoError)
		THROW_EX(IOErr);
	return *this;
}

//---------------------------------------------------------------------------------------------
// Text line I/O
//---------------------------------------------------------------------------------------------

Socket::IOStatus SecureSocket::gets(string &s) throw()
{
	BIO *bio = bufferedBIO();
	for (;;)
	{
		int rc = BIO_gets(bio, const_cast<char *>(s.c_str()), s.capacity());
		if (SSL_get_error(_ssl, rc) == SSL_ERROR_NONE)
		{
			s.resize(rc);
			break;
		}

		if (! BIO_should_retry(bio))
			return IOStatus::Error;
	}

	return IOStatus::NoError;
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus SecureSocket::puts(const string &str) throw()
{
	BIO *bio = bufferedBIO();
	const char *p = reinterpret_cast<const char *>(str.c_str());
//	if (p[str.length()] != '\0')
//		return IOStatus::Error;

	for (;;)
	{
		int rc = BIO_puts(bio, p);
		if (SSL_get_error(_ssl, rc) == SSL_ERROR_NONE)
		{
			p += rc;
			if (*p == '\0')
				break;
		}

		if (! BIO_should_retry(bio))
			return IOStatus::Error;
	}

	return IOStatus::NoError;
}

//---------------------------------------------------------------------------------------------

Socket::IOStatus SecureSocket::flush() throw()
{ 
	BIO *bio = bufferedBIO();
	int rc = BIO_flush(bio);
	return rc < 0 ? IOStatus::Error : IOStatus::NoError;
}

//---------------------------------------------------------------------------------------------

BIO *SecureSocket::bufferedBIO()
{
	if (!bufio_bio)
	{
		bufio_bio = BIO_new(BIO_f_buffer());
		BIO *bio;
		if (is_secure)
		{
			bio = BIO_new(BIO_f_ssl());
			BIO_set_ssl(bio, _ssl, BIO_NOCLOSE);
		}
		else
			bio = _bio;

		BIO_push(bufio_bio, bio);
	}
	return bufio_bio;
}

//---------------------------------------------------------------------------------------------
// Termination
//---------------------------------------------------------------------------------------------

void SecureSocket::shutdown()
{
	if (is_secure)
	{
		//SSL_set_shutdown(_ssl, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
		int rc = SSL_shutdown(_ssl);
		switch (rc)
		{
		case 1:
			break; // success

		case 0:
			// ERROR_SSL(ShutdownErr)("rc=0. shutting down once more");
			rc = SSL_shutdown(_ssl);
			if (rc < 0)
				ERROR_SSL(ShutdownErr);
			break;

		case -1:
		default:
			ERROR_SSL(ShutdownErr);
			break;
		}
	}

	_sock.shutdown();
}

//---------------------------------------------------------------------------------------------
// Authentication
//---------------------------------------------------------------------------------------------

Certificate SecureSocket::peerCertificate() const
{
	return SSL_get_peer_certificate(_ssl);
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::checkCertificate(const char *host)
{
	if (SSL_get_verify_result(_ssl) != X509_V_OK)
	{
		ERROR_SSL(AuthenticationErr)("host %s verification failed", host);
		return false;
	}

	text peer_name = peerCertificate().peerCommonName();

#ifdef _WIN32
	if (stricmp(peer_name, host))
#else
	if (strcasecmp(peer_name, host))
#endif
	{
		ERROR_SSL(AuthenticationErr)("host %s: common name doesn't match host name", host);
		return false;
	}
	
	return true;
}

//---------------------------------------------------------------------------------------------

int SecureSocket::verify_callback(int preverify_ok, X509_STORE_CTX *store)
{
	// Retrieve the pointer to the SSL of the connection currently treated and 
	// the application specific data stored into the SSL object.
	SSL *ssl = (SSL *) X509_STORE_CTX_get_ex_data(store, SSL_get_ex_data_X509_STORE_CTX_idx());
	SecureSocket *self = (SecureSocket *) SSL_get_ex_data(ssl, SecureSocket::ssl_ex_data_index);
	return self->_verify(preverify_ok, store);
}

//---------------------------------------------------------------------------------------------

int SecureSocket::_verify(int preverify_ok, X509_STORE_CTX *store)
{
	Certificate cert(X509_STORE_CTX_get_current_cert(store));
	int err = X509_STORE_CTX_get_error(store);
	int depth = X509_STORE_CTX_get_error_depth(store);

	if (!preverify_ok)
		ERROR_EX(AuthenticationErr)("SecureSocket verify error: %s [%s]", X509_verify_cert_error_string(err), 
			cert.subjectName().oneLiner().c_str());

	return verify(!!preverify_ok, cert) ? 1 : 0;
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::verify(bool verified, const Certificate &cert)
{
	return verified;
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::renegotiate()
{
	return true;
}

//---------------------------------------------------------------------------------------------

bool SecureSocket::rehandshake()
{
	if (!is_secure)
		return true;

	SSL_set_verify(_ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);

	// Stop the client from just resuming the un-authenticated session
	// SSL_set_session_id_context(_ssl, (void *)&id, sizeof(id));
  
	if (SSL_renegotiate(_ssl) <= 0)
	{
		ERROR_SSL(AuthenticationErr)("renegotiation error");
		return false;
	}
	if (SSL_do_handshake(_ssl) <= 0)
	{
		ERROR_SSL(AuthenticationErr)("handshake error");
		return false;
	}
	_ssl->state = SSL_ST_ACCEPT;
	if (SSL_do_handshake(_ssl) <= 0)
	{
		ERROR_SSL(AuthenticationErr)("handshake error");
		return false;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// SecureServerSocket
///////////////////////////////////////////////////////////////////////////////////////////////

ServerContext *SecureServerSocket::default_ctx = 0;

//---------------------------------------------------------------------------------------------

ServerContext &SecureServerSocket::defaultContext()
{
	if (!default_ctx)
		default_ctx = new ServerContext;
	return *default_ctx;
}

//---------------------------------------------------------------------------------------------

void SecureServerSocket::setKeepAliveOpt()
{
	_sock.setKeepIdle();
	_sock.setKeepIntvl();
	_sock.setKeepCnt();
}

//---------------------------------------------------------------------------------------------

void SecureServerSocket::ctor()
{
	if (!default_ctx)
		default_ctx = new ServerContext;
	_ctx = default_ctx;
	cli_auth = ServerContext::ClientAuthentication::None;
	security_mode = SecurityMode::Secure;
}

//---------------------------------------------------------------------------------------------

AcceptedSecureSocket SecureServerSocket::accept()
{
	AcceptedSocket s = _sock.accept();
	return AcceptedSecureSocket(s, cli_auth, _ctx, security_mode);
}

//---------------------------------------------------------------------------------------------

AcceptedSecureSocket SecureServerSocket::accept(const Milliseconds &timeout, Socket::IOStatus &status)
{
	AcceptedSocket s = _sock.accept(timeout, status);
	if (!s.isValid() || status != IOStatus::NoError)
		return AcceptedSecureSocket();
		
	return AcceptedSecureSocket(s, cli_auth, _ctx, security_mode);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// AcceptedSecureServerSocket
///////////////////////////////////////////////////////////////////////////////////////////////

AcceptedSecureSocket::AcceptedSecureSocket(AcceptedSocket &sock, ServerContext::ClientAuthentication 
	cliAuth, ServerContext *ctx, SecurityMode securityMode) : 
		acc_sock(sock), cli_auth(cliAuth), _ctx(ctx), security_mode(securityMode)
{
	sock.setAccepted();
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
