
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

#include "security/general.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// OpenSSL
///////////////////////////////////////////////////////////////////////////////////////////////

OpenSSL::OpenSSL()
{
	// Global system initialization
	SSL_library_init();
	SSL_load_error_strings();
	//OpenSSL_add_ssl_algorithms();

	CRYPTO_malloc_init();
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	//ENGINE_load_builtin_engines();
}
	
//---------------------------------------------------------------------------------------------

OpenSSL::~OpenSSL()
{
	ERR_free_strings();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Error
///////////////////////////////////////////////////////////////////////////////////////////////

SSLError::SSLError()
{
	has_info = true;
	_code = ERR_get_error();
}

//---------------------------------------------------------------------------------------------

std::string SSLError::text() const
{
	char err[1024];
	ERR_error_string_n(_code, err, sizeof(err));
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// BIO
///////////////////////////////////////////////////////////////////////////////////////////////

namespace _BIO
{

//---------------------------------------------------------------------------------------------

Bio::~Bio()
{
	if (_bio)
		BIO_free(_bio);
}

//---------------------------------------------------------------------------------------------

File::File(const char *name)
{
	_bio = BIO_new(BIO_s_file_internal());
	if (BIO_read_filename(_bio, name) <= 0)
		ERROR_EX(Error)("invalid file: %s", name);
}

//---------------------------------------------------------------------------------------------

Memory::Memory()
{
	_bio = BIO_new(BIO_s_mem());
}

//---------------------------------------------------------------------------------------------

Block Memory::block() const
{
	char *p;
	long n = BIO_get_mem_data(_bio, &p);
	return Block((UINT8 *) p, n);
}

//---------------------------------------------------------------------------------------------

} // namespace _BIO

///////////////////////////////////////////////////////////////////////////////////////////////
// Certificate
///////////////////////////////////////////////////////////////////////////////////////////////

Certificate::Certificate(const X509 *x509)
{
	_cert = X509_dup((X509*) x509);
}

//---------------------------------------------------------------------------------------------

Certificate::Certificate(const Block &block)
{
	// TBD
}

//---------------------------------------------------------------------------------------------

Certificate::Certificate(const char *filename) : _cert(0)
{
	_BIO::File file(filename);
	_cert = PEM_read_bio_X509_AUX(file, 0, 0, 0);
	if (!_cert)
		ERROR_EX(Error)("cannot read certificate from file %s", filename);
}

//---------------------------------------------------------------------------------------------

Certificate::~Certificate()
{
	if (_cert)
		X509_free(_cert);
}

//---------------------------------------------------------------------------------------------

std::string Certificate::text() const
{
	_BIO::Memory mem;
	X509_print_ex(mem, _cert, 0, 0);
	std::string s = mem.block().toString();
	return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// PrivateKey
///////////////////////////////////////////////////////////////////////////////////////////////

PrivateKey::PrivateKey(const char *filename)
{
	_BIO::File file(filename);
	_pkey = PEM_read_bio_PrivateKey(file, 0, 0, 0);
	if (!_pkey)
		ERROR_EX(Error)("cannot read private key from file %s", filename);
}

//---------------------------------------------------------------------------------------------

PrivateKey::PrivateKey(const PrivateKey &key)
{
	_BIO::Memory mem;
	PEM_write_bio_PrivateKey(mem, key._pkey, 0, 0, 0, 0, 0);
	_pkey = PEM_read_bio_PrivateKey(mem, 0, 0, 0);
}

//---------------------------------------------------------------------------------------------

PrivateKey::~PrivateKey()
{
	if (_pkey)
		EVP_PKEY_free(_pkey);
}

//---------------------------------------------------------------------------------------------

bool PrivateKey::operator==(const PrivateKey &key) const
{
	return !!EVP_PKEY_cmp(_pkey, key._pkey);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
