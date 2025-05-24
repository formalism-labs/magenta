
#include "gtest/gtest.h"

#include "magenta/net/defs.h"
#include "magenta/sync/defs.h"

using namespace magenta;

OpenSSL openssl;

extern bool g_rvfclog;
extern bool g_pthread_joinable;

///////////////////////////////////////////////////////////////////////////////////////////////

namespace secureSocket1
{

//---------------------------------------------------------------------------------------------

class Client1 : public NewThread 
{
	ClientContext ctx;

public:
	Client1()
	{
		ctx << Certificate("cli-cert.pem");
		ctx << CACertificate("ca.pem");
		ctx << PrivateKey("cli-prkey.pem");
	}

	int main()
	{
		test();
		return 0;
	}

	void test()
	{
		SecureSocket sock;
		sock.setContext(&ctx);
		ASSERT_TRUE(sock.waitForServer(3).connect("127.0.0.1:8081"));
		sock << "Client";
		string s;
		sock >> s;
		EXPECT_STREQ("ClientServer", s.c_str());
		sock << "Done";
		sock.shutdown();
	}
};

//---------------------------------------------------------------------------------------------

class Server1 : public NewThread 
{
	ServerContext ctx;

public:
	Server1()
	{
		ctx << Certificate("srv-cert.pem");
		ctx << CACertificate("ca.pem");
		ctx << PrivateKey("srv-prkey.pem");
	}

	int main()
	{
		test();
		return 0;
	}

	void test()
	{
		SecureServerSocket ssock(Port(8081));
		ssock.setContext(&ctx);
		ssock.setClientAuthentication(ServerContext::ClientAuthentication::Require);
		SecureSocket sock(false);
		sock = ssock.accept(Seconds(5));
//		SecureSocket sock = ssock.accept(Seconds(5));
		ASSERT_TRUE(sock.isValid());
		string s;
		sock >> s;
		EXPECT_STREQ("Client", s.c_str());
		sock << s + "Server";
		sock >> s;
		EXPECT_STREQ("Done", s.c_str());
		sock.shutdown();
	}
};

//---------------------------------------------------------------------------------------------

TEST(ClientServer, secureSocket1)
{
	g_pthread_joinable = true;

	Server1 srv;
	Client1 cli;

	srv.run();
	cli.run();

	srv << cli;
}

//---------------------------------------------------------------------------------------------

} // namespace SecureSocket1

//---------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////
