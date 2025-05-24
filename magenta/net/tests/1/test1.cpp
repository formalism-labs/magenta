
#include "gtest/gtest.h"

#include "magenta/net/defs.h"
#include "magenta/sync/defs.h"

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(Basics1, port)
{
	Port p1(80);
	EXPECT_EQ(80, p1.number());
}

//---------------------------------------------------------------------------------------------

TEST(Basics1, service)
{
	Service s1("http");
	EXPECT_STREQ("tcp", s1.protocol().name());
	EXPECT_EQ(80, s1.port().number());

	Service s2(Port(80), Protocol("tcp"));
	EXPECT_STREQ("tcp", s2.protocol().name());
	EXPECT_EQ(80, s2.port().number());

	Service s3(Port(80), "udp");
	EXPECT_STREQ("udp", s3.protocol().name());
	EXPECT_EQ(80, s3.port().number());
}

//---------------------------------------------------------------------------------------------

TEST(Basics1, byteOrdering)
{
#ifdef RV_TARGET_ARCH_LITTLE_ENDIAN
#define N  0x3412
#define NN 0x78563412
#else
#define N 0x1234
#define NN 0x12345678
#endif

	{
	HostOrder<UINT16> h = 0x1234;
	NetOrder<UINT16> n = h;
	EXPECT_EQ(N, n.number());
	h = n;
	EXPECT_EQ(N, n.number());
	}

	{
	HostOrder<UINT32> h = 0x12345678;
	NetOrder<UINT32> n = h;
	EXPECT_EQ(NN, n.number());
	h = n;
	EXPECT_EQ(NN, n.number());
	}

	{
	UINT16 h_ = 0x1234, n_;
	HostOrder<UINT16, UINT16&> h = h_;
	NetOrder<UINT16, UINT16&> n = n_;
	n = h;
	EXPECT_EQ(0x1234, h_);
	EXPECT_EQ(N, n_);
	h = n;
	EXPECT_EQ(0x1234, h_);
	EXPECT_EQ(N, n_);
	}
}

//---------------------------------------------------------------------------------------------

TEST(Basic1, block)
{
	Block b1("12");
	Block b2("1234");
	Block b3("1243");
	
	EXPECT_TRUE(b2 == b2);
	EXPECT_TRUE(b1 < b2);
	EXPECT_TRUE(b2 < b3);
	EXPECT_TRUE(b3 > b2);
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(IPAddress, explicit)
{
	const char *addrs[][2] = 
	{
		{ "5", "0.0.0.5" },
		{ "172.16.86.144" },
		{ "fe80::250:56ff:fec0:8" },
		{ "[fe80::250:56ff:fec0:8]", "fe80::250:56ff:fec0:8" },
#ifdef _WIN32
		{ "fe80::5efe:172.16.82.61" },
		{ "fe80::214:f1ff:fe52:d400%6" },
#else
		{ "fe80::5efe:172.16.82.61", "fe80::5efe:ac10:523d" },
		{ "fe80::214:f1ff:fe52:d400%eth0" },
#endif
		{ "2001:2001:1:80:b104:bf26:102f:be72" },
		{ "2001:2001:1:80:392a:e45a:47ab:c0fc" },
		{ "2001:2001:1:80:5106:227f:d661:9a4c" },
		{ "2001:2001:1:80:94b3:74b8:e3d5:9e8b" },
		{ "2001:2001:1:80:211:43ff:febf:63a" }
	};

	for (int i = 0; i < sizeof(addrs)/sizeof(*addrs); ++i)
	{
		IPAddress a(addrs[i][0]);
		if (addrs[i][1])
			EXPECT_STREQ(addrs[i][1], a.text().c_str());
		else
			EXPECT_STREQ(addrs[i][0], a.text().c_str());
	}
}

//---------------------------------------------------------------------------------------------

TEST(IPAddress, explicitNegative)
{
	EXPECT_FALSE(IPAddress("172.16.86").isValid());
	EXPECT_FALSE(IPAddress("172.16").isValid());
	EXPECT_FALSE(IPAddress("172").isValid());
	EXPECT_FALSE(IPAddress("172.16. 86").isValid());
	EXPECT_FALSE(IPAddress(" 172.16.86.144").isValid());
	EXPECT_FALSE(IPAddress("fe80::250:56ff:fec0:8 ").isValid());
}

//---------------------------------------------------------------------------------------------

TEST(IPAddress, assignment1)
{
	IPAddress ip;
	EXPECT_TRUE(ip.isNull());
	ip = "172.16.82.74";
	EXPECT_STREQ("172.16.82.74", ip.text().c_str());
	ip = "fe80::250:56ff:fec0:8";
	EXPECT_STREQ("fe80::250:56ff:fec0:8", ip.text().c_str());
}

//---------------------------------------------------------------------------------------------

TEST(IPAddress, assignmentNull)
{
	IPAddress ip4 = "0.0.0.0";
	EXPECT_TRUE(ip4.isNull());
	IPAddress ip6 = "::";
	EXPECT_TRUE(ip6.isNull());
	EXPECT_TRUE(ip4 == ip6);
}

//---------------------------------------------------------------------------------------------

TEST(IPAddress, assignmentLoopback)
{
	IPAddress ip4 = "127.0.0.1";
	EXPECT_TRUE(ip4.isLoopback());
	IPAddress ip6 = "::1";
	EXPECT_TRUE(ip6.isLoopback());
	EXPECT_FALSE(ip4 == ip6);
}

//---------------------------------------------------------------------------------------------

TEST(IPAddress, overwrite)
{
	IPAddress ip;
	memset(&ip, 0, sizeof(ip));
	const char *ip4s = ip.text().c_str();
	EXPECT_TRUE(ip.isValid());
	EXPECT_TRUE(ip.isNull());
	EXPECT_TRUE(ip.isV4());
	EXPECT_TRUE(ip.isV6());
}

//---------------------------------------------------------------------------------------------

TEST(IPAddress, compare)
{
	{
	IPAddress a1 = "1.2.3.4", a2 = "1.2.3.4";
	EXPECT_TRUE(a1 == a2);
	EXPECT_FALSE(a1 != a2);
	EXPECT_TRUE(a1 >= a2);
	EXPECT_TRUE(a1 <= a2);
	}

	{
	IPAddress a1 = "1.2.3.4", a2 = "1.2.3.5";
	EXPECT_TRUE(a1 != a2);
	EXPECT_TRUE(a1 < a2);
	EXPECT_TRUE(a1 <= a2);
	EXPECT_FALSE(a1 > a2);
	EXPECT_FALSE(a1 >= a2);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(IPv4Address, general)
{
	{
	IPv4Address a("fe80::250:56ff:fec0:8");
	EXPECT_FALSE(a.isValid());
	}

	{
	IPv4Address a;
	EXPECT_FALSE(a.set("fe80::250:56ff:fec0:8"));
	a = "fe80::250:56ff:fec0:8";
	EXPECT_FALSE(a.isValid());
	}

	{
	IPv6Address a1("fe80::250:56ff:fec0:8");
	IPv4Address a2;
	a2 = a1;
	EXPECT_FALSE(a2.isValid());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(IPv4MappedAddress, general)
{
	{
	IPv4MappedAddress a("::ffff:192.0.2.128");
	EXPECT_TRUE(a.isValid());
	EXPECT_FALSE(a.isV4());
	EXPECT_TRUE(a.isV6());
	EXPECT_TRUE(a.isV4Mapped());
	}

	{
	IPv4MappedAddress a("192.0.2.128");
	EXPECT_FALSE(a.isValid());
	}

	{
	IPv4Address a("192.0.2.128");
	IPv4MappedAddress a1(a);
	EXPECT_TRUE(a1.isValid());
	}

	{
	IPv6Address a("::ffff:192.0.2.128");
	IPv4MappedAddress a1(a);
	EXPECT_TRUE(a1.isValid());
	}

	{
	IPv6Address a("fe80::250:56ff:fec0:8");
	IPv4MappedAddress a1(a);
	EXPECT_FALSE(a1.isValid());
	}

	{
	IPv4MappedAddress a1;
	EXPECT_TRUE(a1.set("::ffff:192.0.2.128"));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(IPv6Address, general)
{
	{
	IPv6Address a("1.2.3.4");
	EXPECT_FALSE(a.isValid());
	}

	{
	IPv6Address a;
	EXPECT_FALSE(a.set("1.2.3.4"));
	a = "1.2.3.4";
	EXPECT_FALSE(a.isValid());
	}

	{
	IPv4Address a1("1.2.3.4");
	IPv6Address a2;
	a2 = a1;
	EXPECT_FALSE(a2.isValid());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(IPv6AddressPrefix, general)
{   
	{
	IPv6AddressPrefix ip;
	EXPECT_TRUE(ip.isNull());
	EXPECT_TRUE(ip.prefixLength() == 0);
	}

	{
	IPv6AddressPrefix ip("1.2.3.4");
	EXPECT_FALSE(ip.isValid());
	}

	{
	IPv6AddressPrefix ip("1.2.3.4/16");
	EXPECT_FALSE(ip.isValid());
	}

	{
	IPv6AddressPrefix ip("fe80::250:56ff:fec0:8/32");
	EXPECT_STREQ("fe80::250:56ff:fec0:8", ip.text().c_str());
	EXPECT_EQ(32, ip.prefixLength());
	}
	
	{
	IPv6AddressPrefix ip("");
	EXPECT_FALSE(ip.isValid());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __linux__

TEST(LocalIPAddress, basic)
{
	LocalIPAddress ip;
	EXPECT_TRUE(ip.isValid());
}

#endif // __linux__

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(SocketAddress, fromText)
{
	{
	SocketAddress sa("172.16.82.74"); // case (a)
	EXPECT_FALSE(sa.isValid());
	}

	{
	SocketAddress sa("172.16.82.74:8080"); // case (b)
	EXPECT_TRUE(sa.isValid());
	EXPECT_STREQ("172.16.82.74:8080", sa.text().c_str());
	}

	{
	SocketAddress sa("2001:2001:1:80:5106:227f:d661:9a4c"); // case (c)
	EXPECT_FALSE(sa.isValid());
	}

	{
	SocketAddress sa("2001:2001:1:80:5106:227f:d661:9a4c:80"); // case (d)
	EXPECT_FALSE(sa.isValid());
	}

	{
	SocketAddress sa1("[2001:2001:1:80:5106:227f:d661:9a4c]"); // case (e)
	EXPECT_FALSE(sa1.isValid());
	SocketAddress sa2("[fe80::250:56ff:fec0:8]");
	EXPECT_FALSE(sa2.isValid());
	}

	{
	SocketAddress sa("[fe80::250:56ff:fec0:8]:8080"); // case (f)
	EXPECT_STREQ("[fe80::250:56ff:fec0:8]:8080", sa.text().c_str());
	}
}

//---------------------------------------------------------------------------------------------

TEST(SocketAddress, assignment)
{
	{
	SocketAddress sa1("172.16.82.74", 23);
	EXPECT_STREQ("172.16.82.74:23", sa1.text().c_str());
	SocketAddress sa2 = sa1;
	EXPECT_STREQ("172.16.82.74:23", sa2.text().c_str());
	}

	{
	SocketAddress sa1("1.2.3.4", 42);
	SocketAddress sa2("172.16.82.74", 23);
	sa2 = sa1;
	EXPECT_STREQ("1.2.3.4:42", sa2.text().c_str());
	sa2.ip() = IPAddress("4.3.2.1");
	EXPECT_STREQ("4.3.2.1:42", sa2.text().c_str());
	}

	{
	SocketAddress sa("172.16.82.74", 23);
	EXPECT_TRUE(sa.set("1.2.3.4:42"));
	EXPECT_STREQ("1.2.3.4:42", sa.text().c_str());
	EXPECT_TRUE(sa.ip().set("4.3.2.1"));
	EXPECT_STREQ("4.3.2.1:42", sa.text().c_str());
	}

	{
	SocketAddress sa("172.16.82.74:23");
	EXPECT_FALSE(sa.set("1.2.3.4"));
	EXPECT_STREQ("172.16.82.74:23", sa.text().c_str());
	EXPECT_TRUE(sa.ip().set("1.2.3.4"));
	EXPECT_STREQ("1.2.3.4:23", sa.text().c_str());
	}

	{
	SocketAddress sa;
	sa.port() = 10;
	EXPECT_FALSE(sa.isNull());
	EXPECT_TRUE(sa.ip().isNull());
	EXPECT_EQ(10, sa.port().number());
	}

	{
	SocketAddress sa1 = "172.16.82.74:80", sa2 = "172.16.82.74:81";
	EXPECT_TRUE(sa1 != sa2);
	EXPECT_TRUE(sa1.ip() == sa2.ip());
	sa1.ip() = "172.16.82.74";
	EXPECT_EQ(80, sa1.port().number());
	}
}


//---------------------------------------------------------------------------------------------

TEST(SocketAddress, compare)
{
	{
	SocketAddress sa1 = "172.16.82.74:80", sa2 = "172.16.82.74:81";
	EXPECT_TRUE(sa1 != sa2);
	EXPECT_TRUE(sa1.ip() == sa2.ip());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(Host, basic)
{
	{
	Host h("storage");
	EXPECT_TRUE(h.isValid());
	EXPECT_STREQ("storage", h.name().c_str());
	EXPECT_FALSE(h.isAddress());
	}

	{
	Host h("172.20.2.81");
	EXPECT_TRUE(h.isValid());
	EXPECT_TRUE(h.isAddress());
	}

	{
	Host h("172.20.2.81.1");
	EXPECT_FALSE(h.isAddress());
	EXPECT_TRUE(h.isValid());
	}

	{
	Host h("not@ok");
	EXPECT_FALSE(h.isValidName());
	}
}

//---------------------------------------------------------------------------------------------

TEST(Host, hostAddress)
{
	Host h1("storage");
	Host::Addresses h1a = h1.addresses();
	for (Host::Addresses::Iterator i = h1a; !!i; ++i)
	{
		IPAddress a = *i;
		EXPECT_FALSE(a.isNull());
		EXPECT_TRUE(a.isValid());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(HostAndPort, basic)
{
	HostAndPort hp = "hostname:8080";
	EXPECT_STREQ("hostname", hp.host().name().c_str());
	EXPECT_EQ(8080, hp.port().number());

	HostAndPort hp1 = hp;
	EXPECT_STREQ("hostname", hp1.host().name().c_str());
	EXPECT_EQ(8080, hp1.port().number());

	hp1 = "hostname:8080";
	EXPECT_STREQ("hostname", hp1.host().name().c_str());
	EXPECT_EQ(8080, hp1.port().number());
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(NetworkInterfaces, basic)
{
	NetworkInterfaces ifs;
	for (NetworkInterfaces::Iterator i_ifs = ifs; !!i_ifs; ++i_ifs)
	{
		NetworkInterface if0(*i_ifs);
		// printf("[%d] %s:\n", if0.number(), if0.name().c_str());
		NetworkInterface::Addresses addrs(if0, AF_INET);
		for (NetworkInterface::Addresses::Iterator addr = addrs; !!addr; ++addr)
		{
			LocalIPAddress ip = *addr;
			// printf("\tip=%s\n", ip.text().c_str());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
