
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define __GNU_LIBRARY__
#include "getopt.h"
#endif

#include "magenta/defs.h"

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

#define HOST	"localhost"
#define PORT	443

char *g_host = HOST;
int g_port = PORT;

char *g_ca = 0;
char *g_private_key = 0;
char *g_cert = 0;

bool g_zmode = false, g_noverify = false;
SecurityMode g_security_mode = SecurityMode::Secure;

bool g_verbose = true;

#define BUFSIZE 1024

static char *REQUEST_TEMPLATE=
   "GET / HTTP/1.0\r\nUser-Agent:"
   "Client\r\nHost: %s:%d\r\n\r\n";

char expected_response[] = 
	"HTTP/1.0 200 OK\r\n"
	"Server: magenta/net/tests/2/server\r\n\r\n"
	"Test page\r\n";

#ifdef _WIN32
#define snprintf _snprintf
#endif

OpenSSL open_ssl;

//---------------------------------------------------------------------------------------------

class MySocket : public SecureSocket
{
public:

protected:
	bool verify(bool verified, const Certificate &cert)
	{
		bool override = g_zmode || g_noverify;
		if (g_verbose)
			printf("Certificate [%s] verified: %s%s\n", cert.subjectName().oneLiner().c_str(),
				verified ? "yes" : "no", override ? " [override]" : "");
		return override ? true : verified;
	}
};

//---------------------------------------------------------------------------------------------

bool http_request(MySocket &ssock, const Host &host, const Port &port)
{
	// construct our HTTP request
	std::string request= stringf(REQUEST_TEMPLATE, host.name().c_str(), port.number());

	Socket::IOStatus rc = ssock.send(Block(request));
	if (rc != Socket::IOStatus::NoError)
		throw "SSL write problem";

	std::string page;
	// read the server's response, assuming that it's terminated by a close
	for (;;)
	{
		StaticBlock<BUFSIZE> b;
		rc = ssock.recv(b);
		if (rc != Socket::IOStatus::NoError)
			break;

		page += b.toString();
	}

	if (rc != Socket::IOStatus::NotConnected)
		throw "SSL read problem";

	if (g_verbose)
		puts(page.c_str());

	return page == expected_response;
}
	
//---------------------------------------------------------------------------------------------

/*
class MyContext : public ClientContext
{
public:
	MyContext(const char *ciphers = 0)
	{
		if (g_ca && !useCAFile(g_ca))
			throw "Cannot read read CA list";
		if (g_cert && !useCertificateFile(g_cert))
			throw "Can't read private key";
		if (g_private_key && !usePrivateKeyFile(g_private_key))
			throw "Can't read private key";
		
		// Set our cipher list
		if (ciphers)
			setCiphers(ciphers);
	}
};
*/

//---------------------------------------------------------------------------------------------

const char *help = 
"-c <file>     use certificate in <file> (.pem)\n"
"-C <file>     use CA list in <file> (.pem)\n"
"-k <file>     use private key in <file> (.pem)\n"
"-a <host>     connect to <host> (default is localhost)\n"
"-p <port>     connect to <port> (default is 4433)\n"
"-i            don't require the server to present a certificate.\n"
"              This is insecure but is useful for testing.\n"
"-v            override client certificate verification.\n"
"-s <ciphers>  restrict the cipher list to <ciphers>\n"
"              <ciphers> is a colon-separated list.\n"
"              do 'openssl ciphers' for a list of supported ciphers.\n"
"-z            use aNULL;eNULL as cipher list\n"
"-r            after the initial handshake, hang up and reconnect,\n"
"              attempting to resume the session\n"
"-b            best-effort security mode\n"
"-u            unsecure mode (pure tcp)\n"
"-q            quiet operation\n"
;

//---------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	extern char *optarg;

	try
	{
		ClientContext context;

		bool require_server_auth = true;
		char *ciphers = 0;
		bool reconnect = false;
	
		int c;
		while ((c = getopt(argc, argv, "c:C:k:a:p:is:rzvbuqh")) != -1)
		{
			switch(c)
			{
			case 'c':
				g_cert = strdup(optarg);
				if (!context.useCertificate(Certificate(g_cert)))
					throw "Cannot read certificate";
				break;

			case 'C':
				g_ca = strdup(optarg);
				if (!context.useCAFile(g_ca))
					throw "Cannot read CA list";
				break;

			case 'k':
				g_private_key = strdup(optarg);
				if (!context.usePrivateKey(PrivateKey(g_private_key)))
					throw "Cannot read private key";
				break;

			case 'a':
				g_host = strdup(optarg);
				break;
	
			case 'p':
				if (!(g_port = atoi(optarg)))
					throw "invalid port specified";
				break;
	
			case 'i':
				require_server_auth = false;
				break;
	
			case 's':
				ciphers = strdup(optarg);
				if (!context.setCiphers(ciphers))
					throw "Cannot set ciphers";
				break;
	
			case 'r':
				reconnect = true;
				break;

			case 'v':
				g_noverify = true;
				break;

			case 'z':
				g_zmode = true;
				break;

			case 'b':
				g_security_mode = SecurityMode::BestEffort;
				break;

			case 'u':
				g_security_mode = SecurityMode::None;
				break;

			case 'q':
				g_verbose = false;
				break;

			case 'h':
				puts(help);
				return 0;
			}
		}

		Host host(g_host);
		Port port(g_port);
		int n = Socket::IOStatus::Error;

	    MySocket *ssock = new MySocket;
		ssock->setContext(&context);
		ssock->setSecurityMode(g_security_mode);
		if (!ssock->connect(host, port))
		{
			delete ssock;
			throw "SSL connect error";
		}

		if (g_verbose)
			printf("connected to %s\n", g_host);

		// hang up and reconnect, if requested
		if (reconnect)
		{
			Session session(*ssock);
			delete ssock;
			ssock = 0;
	
			ssock = new MySocket();
			ssock->resume(session);
			if (!ssock->connect(host, port))
			{
				delete ssock;
				throw "SSL connect error";
			}
		}
		
		bool rc = http_request(*ssock, host, port);
		printf("%s\n", rc ? "OK" : "FAIL");
	}
	catch (std::exception &x)
	{
		if (g_verbose)
			fprintf(stderr, "%s\n", x.what());
		else
			printf("FAIL\n");
		exit(1);
	}
	catch (const char *x)
	{
		if (g_verbose)
			fprintf(stderr, "%s\n", x);
		else
			printf("FAIL\n");
		exit(1);
	}
}

//---------------------------------------------------------------------------------------------

void global_catch(const char *var, const char *x)
{
	fprintf(stderr, "global_catch: %s: %s\n", var, x);
}

void _rvfcLog(UINT32 level, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buf[1024] = "";
	if (g_verbose)
		vprintf(fmt, args);
	va_end(args);
}

RVFC_LOG_IMPLEMENTATION(_rvfcLog);

///////////////////////////////////////////////////////////////////////////////////////////////
