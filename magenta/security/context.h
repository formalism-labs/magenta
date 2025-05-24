
#ifndef _magenta_security_context_
#define _magenta_security_context_

#include "magenta/classes.h"
#include "magenta/security/general.h"

#include OPENSSL_INC(ssl.h)
#include OPENSSL_INC(err.h)

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Context
{
	typedef Context This;

protected:
	SSL_CTX *_ctx;
	text _passwd;
	static int ex_data_index;

	void ctor();

	virtual void _abstract() = 0;

public:
	Context();
	virtual ~Context();

	bool useCAFile(const char *ca_file);
	bool useCertificateFile(const char *file);
	bool usePrivateKeyFile(const char *file, const char *password = 0);

	bool useCA(const Certificate &cert);
	bool useCertificate(const Certificate &cert);
	bool usePrivateKey(const PrivateKey &key);

	This &operator<<(const CACertificate &caCert) { useCA(caCert); return *this; }
	This &operator<<(const Certificate &cert) { useCertificate(cert); return *this; }
	This &operator<<(const PrivateKey &key) { usePrivateKey(key); return *this; }

	bool setCiphers(const char *ciphers);
	bool allowNullCiphers();

	operator SSL_CTX*() const { return _ctx; }

protected:
	static int _copyPassword(char *buf, int size, int rwflag, void *userdata);
	virtual int copyPassword(char *buf, int size, int rwflag) const;

	virtual bool verify(bool verified, const Certificate &cert);
	int _verify(int preverify_ok, X509_STORE_CTX *ctx);
	static int verify_callback(int preverify_ok, X509_STORE_CTX *ctx);

public:
	CLASS_EXCEPTION_DEF("magenta::Context");
	CLASS_SUB_EXCEPTION_DEF(InitErr, "error initializing SSL context");
	CLASS_SUB_EXCEPTION_DEF(CertErr, "error reading certificate");
	CLASS_SUB_EXCEPTION_DEF(VerifyErr, "verification error");
};

//---------------------------------------------------------------------------------------------

class ClientContext : public Context
{
	void _abstract() {}

public:
	ClientContext();
};

//---------------------------------------------------------------------------------------------

class ServerContext : public Context
{
	void _abstract() {}

public:
	ServerContext();

	enum__(ClientAuthentication)
		None,
		Request,
		Require,
		Rehandshake
	__enum(ClientAuthentication);

	void setClientAuthentication(ClientAuthentication auth);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_security_context_
