
#include "gtest/gtest.h"

#include "magenta/net/defs.h"
#include "magenta/sync/defs.h"

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

namespace tcpSenario1
{

//---------------------------------------------------------------------------------------------

class Client : public NewThread
{
public:
	int state;

	Client() : state(1) {}

	int main()
	{
		LocalSocketAddress la(LoopbackIPv4Address(), Port(888));
		//printf("client addr: %s\n", la.text().c_str());
		TcpSocket s(la);
		EXPECT_TRUE(s.isConnected());
		if (!s.isConnected())
		{
			s.shutdown();
			state = 0;
			return 1;
		}

		state = 2;

		Block b("message in a bottle");
		Socket::IOStatus rc = s.send(b);
		EXPECT_EQ(Socket::IOStatus::NoError, rc);

		StaticBlock<1024> b1;
		rc = s.recv(b1);
		EXPECT_EQ(Socket::IOStatus::NoError, rc);

		s.shutdown();
		state = 0;
		return 0;
	}
};

//---------------------------------------------------------------------------------------------

class Server : public NewThread
{
public:
	int state;

	Server() : state(1) {}

	int main()
	{
		LocalSocketAddress la(AnyIPAddress(), Port(888));
		TcpServerSocket s(la);
		EXPECT_TRUE(s.isValid());
		state = 2;

		TcpSocket s1 = s.accept();
		EXPECT_TRUE(s1.isValid());
		state = 3;

		StaticBlock<1024> b;
		Socket::IOStatus rc = s1.recv(b);
		EXPECT_EQ(Socket::IOStatus::NoError, rc);
		if (rc != Socket::IOStatus::NoError)
		{
			s1.shutdown();
			state = 0;
			return 1;
		}

		rc = s1.send(b);
		EXPECT_EQ(Socket::IOStatus::NoError, rc);

		s1.shutdown();
		state = 0;
		return 0;
	}
};

//---------------------------------------------------------------------------------------------

TEST(ClientServer, tcpSenario1)
{
	int i;
	Server srv;
	Client cli;

	srv.run();

	for (i = 0; srv.state < 2 && i < 10; ++i)
		sleep(Milliseconds(500));
	EXPECT_LT(i , 10);

	cli.run();

	for (i = 0; (srv.state != 0 || cli.state != 0) && i < 10; ++i)
		sleep(Milliseconds(500));

	EXPECT_LT(i , 10);
}

//---------------------------------------------------------------------------------------------

} // namespace tcpSenario1

///////////////////////////////////////////////////////////////////////////////////////////////

namespace tcpSenario2
{

//---------------------------------------------------------------------------------------------

class Client : public NewThread
{
public:
	Milliseconds wait;
	bool connect_ok;

	Client(Milliseconds wait) : wait(wait), connect_ok(false) {}

	int main()
	{
		sleep(wait);

		LocalSocketAddress la(LoopbackIPv4Address(), Port(888));
		TcpSocket s(la);
		EXPECT_TRUE(s.isConnected());
		connect_ok = s.isConnected();

		s.shutdown();
		return 0;
	}
};

//---------------------------------------------------------------------------------------------

class Server : public NewThread
{
public:
	Milliseconds wait;
	bool accept_timeout, accept_ok, socket_valid;

	Server(Milliseconds wait) : wait(wait) 
	{
		accept_timeout = accept_ok = socket_valid = false;
	}

	int main()
	{
		LocalSocketAddress la(AnyIPAddress(), Port(888));
		TcpServerSocket s(la);
		EXPECT_TRUE(s.isValid());

		Socket::IOStatus status;
		TcpSocket s1 = s.accept(wait, status);
		accept_ok = status == Socket::IOStatus::NoError;
		accept_timeout = status == Socket::IOStatus::TimeOut;
		socket_valid = s1.isValid();

		s1.shutdown();
		return 0;
	}
};

//---------------------------------------------------------------------------------------------

TEST(ClientServer, tcpSenario2_accept_ok)
{
	Server srv(Milliseconds(10000));
	Client cli(Seconds(5));

	srv.run();
	cli.run();

	srv << cli;

	EXPECT_TRUE(srv.accept_ok);
	EXPECT_FALSE(srv.accept_timeout);
	EXPECT_TRUE(srv.socket_valid);
	EXPECT_TRUE(cli.connect_ok);
}

//---------------------------------------------------------------------------------------------

TEST(ClientServer, tcpSenario2_accept_timeout)
{
	Server srv(Milliseconds(5000));
	Client cli(Seconds(10));

	srv.run();
	cli.run();

	srv << cli;

	EXPECT_FALSE(srv.accept_ok);
	EXPECT_TRUE(srv.accept_timeout);
	EXPECT_FALSE(srv.socket_valid);
	EXPECT_FALSE(cli.connect_ok);
}

//---------------------------------------------------------------------------------------------

} // namespace tcpSenario2

///////////////////////////////////////////////////////////////////////////////////////////////
// udpSenario1

namespace udpSenario1
{

//---------------------------------------------------------------------------------------------

class Client : public NewThread
{
public:
	int state;

	Client() : state(1) {}

	int main()
	{
		LocalSocketAddress la(LoopbackIPv4Address(), Port(888));
		//printf("client addr: %s\n", la.text().c_str());
		UdpPeerSocket s;
		s.connect(la);
		EXPECT_TRUE(s.isValid());
		state = 2;

		char m1[] = "message in a bottle";
		Block b(m1);
		EXPECT_EQ(sizeof(m1), b.length());

		Socket::IOStatus rc = s.send(b);
		EXPECT_EQ(Socket::IOStatus::NoError, rc);

		StaticBlock<1024> b1;
		rc = s.recv(b1);
		EXPECT_EQ(Socket::IOStatus::NoError, rc);

		state = 0;
		return 0;
	}
};

//---------------------------------------------------------------------------------------------

class Server : public NewThread
{
public:
	int state;

	Server() : state(1) {}

	int main()
	{
		LocalSocketAddress la(AnyIPAddress(), Port(888));
		UdpPeerSocket s;
		s.bind(la);
		EXPECT_TRUE(s.isValid());
		state = 2;

		StaticBlock<1024> b;
		Socket::IOStatus rc = s.recv(b);
		//printf("server got message: %s\n", (char *) b.data());
		EXPECT_EQ(Socket::IOStatus::NoError, rc);
		if (rc != Socket::IOStatus::NoError)
		{
			state = 0;
			return 1;
		}

		char m1[] = "message in a bottle";
		EXPECT_EQ(sizeof(m1), b.length());

		rc = s.send(b);
		EXPECT_EQ(Socket::IOStatus::NoError, rc);

		state = 0;
		return 0;
	}
};

//---------------------------------------------------------------------------------------------

TEST(ClientServer, DISABLED_udpSenario1)
{
	int i;
	Server srv;
	Client cli;

	srv.run();

	i = 0;
	do
	{
		sleep(Milliseconds(500));
	}
	while (srv.state < 2 && ++i < 10);
	EXPECT_LT(i , 10);

	cli.run();

	for (i = 0; (srv.state != 0 || cli.state != 0) && i < 10; ++i)
		sleep(Milliseconds(500));

	EXPECT_LT(i , 10);
}

//---------------------------------------------------------------------------------------------

} // namespace udpSenario1

///////////////////////////////////////////////////////////////////////////////////////////////
