
#ifndef _magenta_security_general_
#define _magenta_security_general_

#include "magenta/classes.h"

#include "magenta/lang/general.h"
#include "magenta/net/block.h"

#ifndef _WIN32
#define OPENSSL_INC(f) TO_STRING(openssl/f)
#else
#define OPENSSL_INC(f) TO_STRING(contrib/open-ssl/0.9.8/windows/openssl/f)
#endif

#include OPENSSL_INC(ssl.h)
#include OPENSSL_INC(err.h)

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class OpenSSL
{
public:
	OpenSSL();
	~OpenSSL();
};

//---------------------------------------------------------------------------------------------

#define THROW_SSL(E) \
	throw (const E &) magenta::Exception::Adapter<E>(E(__FILE__, __LINE__, \
		magenta::Exception::SSLError()))

#define ERROR_SSL(E) \
	_invalidate(), magenta::LastException::Proxy() = (const E &) \
		magenta::Exception::Adapter<E, magenta::SSLError>(\
			E(__FILE__, __LINE__), \
			magenta::SSLError())

class SSLError : public magenta::Exception::SpecificError
{
public:
	SSLError();
	std::string text() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////

namespace _BIO
{

//---------------------------------------------------------------------------------------------

class Bio
{
protected:
	BIO *_bio;

public:
	Bio() : _bio(0) {}
	~Bio();

	operator BIO*() { return _bio; }
	operator const BIO*() const { return _bio; }
};

//---------------------------------------------------------------------------------------------

class File : public Bio
{
public:
	File(const char *name);

	CLASS_EXCEPTION_DEF("magenta::BIO::File");
};

//---------------------------------------------------------------------------------------------

class Memory : public Bio
{
public:
	Memory();

	Block block() const;

	CLASS_EXCEPTION_DEF("magenta::BIO::Memory");
};

//---------------------------------------------------------------------------------------------

} // namespace _BIO

///////////////////////////////////////////////////////////////////////////////////////////////

class Certificate
{
	X509 *_cert;

public:
	Certificate(const X509 *x509);
	Certificate(const Block &block);
	Certificate(const char *filename);
	~Certificate();

	operator const X509*() const { return _cert; }
	operator X509*() { return _cert; }

	//-----------------------------------------------------------------------------------------
	// Name
	//-----------------------------------------------------------------------------------------
	
	class Name
	{
		const X509_NAME *_name;

	public:
		Name(const X509_NAME *name) : _name(name) {}

		text oneLiner() const
		{
			char s[256];
			X509_NAME_oneline((X509_NAME *) _name, s, sizeof(s));
			return s;
		}

		const X509_NAME *name() const { return _name; }
		operator const X509_NAME*() const { return name(); }
	};
	
	//-----------------------------------------------------------------------------------------

	Name subjectName() const
	{
		return X509_get_subject_name((X509 *) _cert);
	}

	text peerCommonName() const
	{
		char common_name[256];
		X509_NAME_get_text_by_NID(const_cast<X509_NAME *>(subjectName().name()), NID_commonName, 
			common_name, sizeof(common_name));
		return common_name;
	}

	std::string text() const;

	//-----------------------------------------------------------------------------------------

	CLASS_EXCEPTION_DEF("magenta::Certificate");
};

//---------------------------------------------------------------------------------------------

class CACertificate : public Certificate
{
public:
	CACertificate(const X509 *x509) : Certificate(x509) {}
	CACertificate(const Block &block) : Certificate(block) {}
	CACertificate(const char *filename) : Certificate(filename) {}
};

//---------------------------------------------------------------------------------------------

class PrivateKey
{
	EVP_PKEY *_pkey;

public:
	PrivateKey(const char *filename);
	PrivateKey(const PrivateKey &key);
	~PrivateKey();

	operator const EVP_PKEY*() const { return _pkey; }
	operator EVP_PKEY*() { return _pkey; }

	bool operator==(const PrivateKey &key) const;
	bool operator!=(const PrivateKey &key) const { return !operator==(key); }

	CLASS_EXCEPTION_DEF("magenta::PrivateKey");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_security_general_
