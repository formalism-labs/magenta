
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define __GNU_LIBRARY__
#include "getopt.h"
//#include <openssl/applink.c>
#endif

#include "magenta/defs.h"

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

char *ca_file = 0; // "ca.pem";
char *private_key_file = 0; // "srv-prkey.pem";
char *cert_file = 0; // "srv-cert.pem";

SecurityMode g_security_mode = SecurityMode::Secure;

bool g_verbose = true;
bool g_noverify = false;
int g_clients = 0;

#define PORT	443

#define BUFSIZE 1024

int g_port = PORT;

OpenSSL open_ssl;

//---------------------------------------------------------------------------------------------

static void http_serve(SecureSocket &s)
{
	if (g_verbose)
		printf("serving request from %s\n", s.peerAddress().text().c_str());

	for (;;)
	{
		string str;
		str.reserve(BUFSIZE);
		if (s.gets(str) != Socket::IOStatus::NoError)
		{
			s.shutdown();
			throw "SSL read problem";
		}

		// Look for the blank line that signals the end of the HTTP headers
		if (str == "\r\n" || str == "\n")
			break;
	}

	if (s.puts("HTTP/1.0 200 OK\r\n") != Socket::IOStatus::NoError)
		throw "Write error";
	if (s.puts("Server: magenta/net/tests/2/server\r\n\r\n") != Socket::IOStatus::NoError)
		throw "Write error";
	if (s.puts("Test page\r\n") != Socket::IOStatus::NoError)
		throw "Write error";
	
	if (s.flush() != Socket::IOStatus::NoError)
		throw "Error flushing BIO";

	s.shutdown();

	if (g_verbose)
		printf("transaction completed\n");
}

//---------------------------------------------------------------------------------------------

const char *help =
"-p <port>     use port <port>\n"
"-c <file>     use certificate in <file> (.pem)\n"
"-C <file>     use CA list in <file> (.pem)\n"
"-k <file>     use private key in <file> (.pem)\n"
"-r            request (but don't require) certificate based client authentication\n"
"-R            require certificate based client authentication\n"
"-x            after the client has sent its request, force\n"
"              a rehandshake requiring certificate based client\n"
"              authentication. This emulates a server which\n"
"              only requires client authentication for a proper\n"
"              subset of its content.\n"
"-s <ciphers>  restrict the cipher list to <ciphers>\n"
"              <ciphers> is a colon-separated list\n"
"              do 'openssl ciphers' for a list of supported ciphers\n"
"-z            use aNULL;eNULL as cipher list\n"
"-F            don't fork a new process for each client\n"
"-b            best-effort security mode\n"
"-u            unsecure mode (pure tcp)\n"
"-v            override certificate verification\n"
"-q            quiet operation\n"
"-# <num>      exit after <num> connections\n"
;

//---------------------------------------------------------------------------------------------

class MyContext : public ServerContext
{
public:

protected:
	bool verify(bool verified, const Certificate &cert)
	{
		if (g_verbose)
			printf("Certificate [%s] verified: %s%s\n", cert.subjectName().oneLiner().c_str(),
				verified ? "yes" : "no", g_noverify ? " [override]" : "");
		return g_noverify ? true : verified;
	}
};

//---------------------------------------------------------------------------------------------

int main(int argc,char *argv[])
{
	extern char *optarg;

	ServerContext::ClientAuthentication client_auth = ServerContext::ClientAuthentication::None;
	char *ciphers = 0;

	for (int c; (c = getopt(argc, argv, "p:c:C:k:rRxFs:zbvuhq#:")) != -1;)
	{
		switch (c)
		{
		case 'p':
			g_port = atoi(optarg);
			break;

		case 'c':
			cert_file = strdup(optarg);
			break;

		case 'C':
			ca_file = strdup(optarg);
			break;

		case 'k':
			private_key_file = strdup(optarg);
			break;

		case 'r':
			client_auth = ServerContext::ClientAuthentication::Request;
			break;

		case 'R':
			client_auth = ServerContext::ClientAuthentication::Require;
			break;

		case 'x':
			client_auth = ServerContext::ClientAuthentication::Rehandshake;
			break;

		case 's':
			ciphers = strdup(optarg);
			break;

		case 'z':
			ciphers = "aNULL:eNULL";
			break;

		case 'b':
			g_security_mode = SecurityMode::BestEffort;
			break;

		case 'u':
			g_security_mode = SecurityMode::None;
			break;

		case 'v':
			g_noverify = true;
			break;

		case 'q':
			g_verbose = false;
			break;

		case '#':
			g_clients = atoi(optarg);
			break;

		case 'h':
			puts(help);
			return 0;
		}
	}

	SecureServerSocket server = SecureServerSocket(Port(g_port));
	server.setSecurityMode(g_security_mode);

	MyContext ctx;
	if (g_security_mode != SecurityMode::None)
	{
		if (ca_file)
			ctx.useCAFile(ca_file);

		if (cert_file)
			ctx.useCertificate(Certificate(cert_file));
		if (private_key_file)
			ctx.usePrivateKey(PrivateKey(private_key_file));
		ctx.setClientAuthentication(client_auth);

		if (ciphers)
			ctx.setCiphers(ciphers);

		server.setContext(&ctx);
	}

	for (int k = 0; !g_clients || k < g_clients; ++k)
	{
		try
		{
			if (g_verbose)
				printf("accepting connections\n");

			SecureSocket ssock = server.accept();
			if (!ssock.isValid())
			{
				fprintf(stderr, "Problem accepting\n");
				continue;
			}

			http_serve(ssock);
		}
		catch (std::exception &x)
		{
			if (g_verbose)
				fprintf(stderr, "%s\n", x.what());
			else
				printf("FAIL\n");
		}
		catch (const char *x)
		{
			if (g_verbose)
				fprintf(stderr, "%s\n", x);
			else
				printf("FAIL\n");
		}
	}
	
	return 0;
}

//---------------------------------------------------------------------------------------------

void global_catch(const char *var, const char *x)
{
	fprintf(stderr, "global_catch: %s: %s\n", var, x);
}

///////////////////////////////////////////////////////////////////////////////////////////////
