
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

#include "context.h"

namespace magenta
{

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////
// Context
///////////////////////////////////////////////////////////////////////////////////////////////

int Context::ex_data_index = -1;

//---------------------------------------------------------------------------------------------

Context::Context()
{
	if (ex_data_index == -1)
		ex_data_index = SSL_CTX_get_ex_new_index(0, "magenta::Context", 0, 0, 0);
}

//---------------------------------------------------------------------------------------------

void Context::ctor()
{
	// _ctx is initialized by a subclass
	// we can default to a client context if !_ctx
	if (!_ctx)
	{
		ERROR_EX(InitErr);
		return;
	}

	SSL_CTX_set_ex_data(_ctx, ex_data_index, this);

	SSL_CTX_set_verify_depth(_ctx, 1);

	SSL_CTX_set_default_passwd_cb_userdata(_ctx, this);
	SSL_CTX_set_default_passwd_cb(_ctx, _copyPassword);
}

//---------------------------------------------------------------------------------------------

Context::~Context()
{
	SSL_CTX_free(_ctx);
}

//---------------------------------------------------------------------------------------------

bool Context::useCAFile(const char *caFile)
{
	if (!SSL_CTX_load_verify_locations(_ctx, caFile, 0))
	{
		SSL_CTX_set_default_verify_paths(_ctx);
		ERROR_EX(CertErr)("Can't read CA list");
		return false;
	}

	STACK_OF(X509_NAME) *ca_certs = SSL_load_client_CA_file(caFile);
	SSL_CTX_set_client_CA_list(_ctx, ca_certs);
	return true;
}

//---------------------------------------------------------------------------------------------

bool Context::useCA(const Certificate &ca)
{
	X509_STORE *store = SSL_CTX_get_cert_store(_ctx);
	if (!X509_STORE_add_cert(store, const_cast<X509*>((const X509 *) ca)))
	{
		ERROR_EX(CertErr)("Can't read CA '%s'", ca.peerCommonName().c_str());
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------

bool Context::usePrivateKeyFile(const char *file, const char *password)
{
	if (password)
		_passwd = password;

	if (!SSL_CTX_use_PrivateKey_file(_ctx, file, SSL_FILETYPE_PEM))
	{
		ERROR_EX(CertErr)("Can't read private key file");
		return false;
	}

	if (!SSL_CTX_check_private_key(_ctx))
		ERROR_EX(CertErr)("Can't verify private key (no certificate introduced?)");
	
	return true;
}

//---------------------------------------------------------------------------------------------

bool Context::usePrivateKey(const PrivateKey &pkey)
{
	if (!SSL_CTX_use_PrivateKey(_ctx, const_cast<PrivateKey &>(pkey)))
	{
		ERROR_EX(CertErr)("Can't use private key");
		return false;
	}
	
	if (!SSL_CTX_check_private_key(_ctx))
		ERROR_EX(CertErr)("Can't verify private key (no certificate introduced?)");

	return true;
}

//---------------------------------------------------------------------------------------------

bool Context::useCertificateFile(const char *file)
{
	if (!SSL_CTX_use_certificate_chain_file(_ctx, file))
	{
		ERROR_EX(CertErr)("Can't read certificate file");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------

bool Context::useCertificate(const Certificate &cert)
{
	if (!SSL_CTX_use_certificate(_ctx, const_cast<Certificate &>(cert)))
	{
		ERROR_EX(CertErr)("Can't use certificate for '%s'", cert.subjectName().oneLiner().c_str());
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------

bool Context::setCiphers(const char *ciphers)
{
	return !!SSL_CTX_set_cipher_list(_ctx, ciphers);
}

//---------------------------------------------------------------------------------------------

bool Context::allowNullCiphers()
{
	return !!SSL_CTX_set_cipher_list(_ctx, "ALL:aNULL:eNULL");
}

//---------------------------------------------------------------------------------------------

int Context::_copyPassword(char *buf, int size, int rwflag, void *userdata)
{
	return reinterpret_cast<const Context *>(userdata)->copyPassword(buf, size, rwflag);
}

//---------------------------------------------------------------------------------------------

int Context::copyPassword(char *buf, int size, int rwflag) const
{
	int n = _passwd.Size();
	if (size < n + 1)
	  return 0;

	strcpy(buf, (const char *) _passwd);
	return n;
}

//---------------------------------------------------------------------------------------------

int Context::verify_callback(int preverify_ok, X509_STORE_CTX *store)
{
	SSL *ssl = (SSL *) X509_STORE_CTX_get_ex_data(store, SSL_get_ex_data_X509_STORE_CTX_idx());
	SSL_CTX *ctx = SSL_get_SSL_CTX(ssl);
	Context *self = (Context *) SSL_CTX_get_ex_data(ctx, Context::ex_data_index);
	return self->_verify(preverify_ok, store);
}

//---------------------------------------------------------------------------------------------

int Context::_verify(int preverify_ok, X509_STORE_CTX *store)
{
	Certificate cert(X509_STORE_CTX_get_current_cert(store));
	int err = X509_STORE_CTX_get_error(store);
	int depth = X509_STORE_CTX_get_error_depth(store);

	if (!preverify_ok)
		ERROR_EX(VerifyErr)("certificate %s [%s]", X509_verify_cert_error_string(err), 
			cert.subjectName().oneLiner().c_str());

	return verify(!!preverify_ok, cert) ? 1 : 0;
}

//---------------------------------------------------------------------------------------------

bool Context::verify(bool verified, const Certificate &cert)
{
	return verified;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ClientContext::ClientContext()
{
	_ctx = SSL_CTX_new(SSLv23_method());

	Context::ctor();

	SSL_CTX_set_verify(_ctx, SSL_VERIFY_PEER, verify_callback);
}
	
///////////////////////////////////////////////////////////////////////////////////////////////

ServerContext::ServerContext()
{
	_ctx = SSL_CTX_new(SSLv23_server_method());

	Context::ctor();

	SSL_CTX_set_verify(_ctx, SSL_VERIFY_NONE, verify_callback);
//	SSL_CTX_set_session_id_context(ctx, (void*) &sid_ctx,sizeof(sid_ctx));
}
	
//---------------------------------------------------------------------------------------------

void ServerContext::setClientAuthentication(ClientAuthentication auth)
{
	switch (auth)
	{
	case ClientAuthentication::Request:
		SSL_CTX_set_verify(_ctx, SSL_VERIFY_PEER, verify_callback);
		break;

	case ClientAuthentication::Require:
		SSL_CTX_set_verify(_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT | 
			SSL_VERIFY_CLIENT_ONCE, verify_callback);
		break;

	case ClientAuthentication::None:
	default:
		SSL_CTX_set_verify(_ctx, SSL_VERIFY_NONE, verify_callback);
		// Do nothing
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
