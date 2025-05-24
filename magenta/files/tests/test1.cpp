
#include "magenta/tests/gtest.h"

#include "magenta/files/defs.h"

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(Path, basics)
{
	Path p = "c:/root/somewhere/over/the/rainbow/";

	text t = p;
	EXPECT_STREQ("c:\\root\\somewhere\\over\\the\\rainbow\\", t);
	EXPECT_STREQ("c:", p.volume());
	EXPECT_STREQ("rainbow", +p.basename());
	EXPECT_STREQ("c:\\root\\somewhere\\over\\the\\rainbow\\", +p.file().path());
	auto d = p.dirname();
	EXPECT_STREQ("c:\\root\\somewhere\\over\\the", +d);
	EXPECT_STREQ("c:\\root\\somewhere\\over", +d.dirname());

	Path p1 = "c:/root/somewhere/over";
	EXPECT_EQ(Path("the/rainbow"), p - p1);
}

//---------------------------------------------------------------------------------------------

TEST(Path, winpath1)
{
	Path p = "c:/root/foo";
	EXPECT_STREQ("c:", +p.volume());
	EXPECT_EQ(Path("/root/foo"), p.invol());
	EXPECT_EQ(VT("c:", "root", "foo"), p.pieces());
	EXPECT_EQ(true, p.abs());
}

TEST(Path, winpath2)
{
	Path p = "c:";
	EXPECT_STREQ("c:", +p.volume());
	EXPECT_EQ(Path("/"), p.invol());
	EXPECT_EQ(VT("c:"), p.pieces());
	EXPECT_EQ(true, p.abs());
}

TEST(Path, winpath3)
{
	Path p = "c:/";
	EXPECT_STREQ("c:", +p.volume());
	EXPECT_EQ(Path("/"), p.invol());
	EXPECT_EQ(VT("c:"), p.pieces());
	EXPECT_EQ(true, p.abs());
}

TEST(Path, winpath4)
{
	Path p = "c:root/foo"; // exceptpion?
	EXPECT_STREQ("c:", +p.volume());
	EXPECT_EQ(Path("/root/foo"), p.invol());
	EXPECT_EQ(VT("c:", "root", "foo"), p.pieces());
	EXPECT_EQ(true, p.abs());
}

TEST(Path, winpath5)
{
	Path p = "c:root"; // exceptpion?
	EXPECT_STREQ("c:", +p.volume());
	EXPECT_EQ(Path("/root"), p.invol());
	EXPECT_EQ(VT("c:", "root"), p.pieces());
	EXPECT_EQ(true, p.abs());
}

//---------------------------------------------------------------------------------------------

TEST(Path, uncpath1)
{
	Path p = "//host1/share/root/foo";
	EXPECT_EQ(Path("//host1/share"), p.volume());
	EXPECT_EQ(Path("/root/foo"), p.invol());
	EXPECT_EQ(VT(+Path("//host1/share"), "root", "foo"), p.pieces());
	EXPECT_EQ(true, p.abs());
}

TEST(Path, uncpath2)
{
	Path p = "//host1/share";
	EXPECT_EQ(Path("//host1/share"), p.volume());
	EXPECT_EQ(Path("/"), p.invol());
	EXPECT_EQ(VT(+Path("//host1/share")), p.pieces());
	EXPECT_EQ(true, p.abs());
}

TEST(Path, uncpath3)
{
	EXPECT_THROW({
		Path p = "//host1";
		EXPECT_EQ(Path("//host1/share"), p.volume());
		}, Exception);
//	EXPECT_EQ(Path("/root/foo"), p.invol());
//	EXPECT_EQ(VT(+Path("//host1/share"), "root", "foo"), p.pieces());
//	EXPECT_EQ(true, p.abs());
}

TEST(Path, uncpath4)
{
	EXPECT_THROW({
		Path p = "//host1///share///root";
		EXPECT_EQ(Path("//host1/share"), p.volume());
		}, Exception);
//	EXPECT_EQ(Path("//host1/share"), p.volume());
//	EXPECT_EQ(Path("/root"), p.invol());
//	EXPECT_EQ(VT(+Path("//host1/share"), "root"), p.pieces());
//	EXPECT_EQ(true, p.abs());
}

TEST(Path, uncpath5)
{
	EXPECT_THROW({
		Path p = "////host1/share/root";
		EXPECT_EQ(Path("//host1/share"), p.volume());
		}, Exception);
//	EXPECT_EQ(Path("/root/foo"), p.invol());
//	EXPECT_EQ(VT(+Path("//host1/share"), "root", "foo"), p.pieces());
//	EXPECT_EQ(true, p.abs());
}

//---------------------------------------------------------------------------------------------

TEST(Path, uxpath1)
{
	Path p = "/root/foo";
	EXPECT_STREQ("", +p.volume());
	EXPECT_EQ(Path("/root/foo"), p.invol());
	EXPECT_EQ(VT("root", "foo"), p.pieces());
	EXPECT_EQ(true, p.abs());
}

//---------------------------------------------------------------------------------------------

TEST(Path, relpath1)
{
	Path p = "root/foo";
	EXPECT_STREQ("", +p.volume());
	EXPECT_EQ(Path("root/foo"), p.invol());
	EXPECT_EQ(VT("root", "foo"), p.pieces());
	EXPECT_EQ(false, p.abs());
}

//---------------------------------------------------------------------------------------------

TEST(Path, minus)
{
	EXPECT_EQ(Path("d/e"), Path("/a/b/c/d/e") - Path("/a/b/c"));
	EXPECT_EQ(Path("d/e"), Path("/a/b/c/d/e") - Path("a/b/c"));

	EXPECT_EQ(Path(), Path("/a/b/c") - Path("/a/b/c"));
	EXPECT_EQ(Path(), Path("/a/b/c") - Path("/x/b/c"));
	EXPECT_EQ(Path("/a/b/c"), Path("/a/b/c") - Path());
}

//---------------------------------------------------------------------------------------------

int arg1(const Path &path) { return 0; }
int arg2(Path path) { return 0; }

TEST(Path, arg)
{
	EXPECT_EQ(0, arg1("/a/b/c"));
	EXPECT_EQ(0, arg2("/a/b/c"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
