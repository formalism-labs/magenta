
#include "magenta/tests/gtest.h"

#include "magenta/text/defs.h"
#include "magenta/types/defs.h"

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(Text, substring)
{
	text s = "abcdefg";

	EXPECT_STREQ("b", s[1]);
	EXPECT_STREQ("g", s[-1]);
	EXPECT_STREQ("bc", s(1, 2));
	EXPECT_STREQ("c", s(-5, 2));
	EXPECT_STREQ("cd", s(span(-5, 2)));
	EXPECT_STREQ("cd", s.span(-5, 2));
}

//---------------------------------------------------------------------------------------------

TEST(Text, regexp_prefix_suffix)
{
	{
	text::regex::Match m;
	auto p1 = m.prefix();
	auto s1 = m.suffix();
	}
	
	text s0 = "\\V:\\rafi_f7n-1\\freemason.framework\\\\bootstrap\\vs14\\";

	{
	Vector<text> ps, ss;
	foreach (i, s0.scan(R"([\\/]+)"))
	{
		ps << i.prefix();
		ss << i.suffix();
	}
	EXPECT_EQ(VT("", "V:", "rafi_f7n-1", "freemason.framework", "bootstrap", "vs14"), ps);
	EXPECT_EQ(VT("V:", "rafi_f7n-1", "freemason.framework", "bootstrap", "vs14", ""), ss);
	}

	{
		Vector<text> ps, ss;
		foreach (i, s0.scan(R"([\\/]+)"))
		{
			ps << i.full_prefix();
			ss << i.full_suffix();
		}

		Vector<text> c_ps = 
		{
			"",
			"\\V:",
			"\\V:\\rafi_f7n-1",
			"\\V:\\rafi_f7n-1\\freemason.framework",
			"\\V:\\rafi_f7n-1\\freemason.framework\\\\bootstrap",
			"\\V:\\rafi_f7n-1\\freemason.framework\\\\bootstrap\\vs14"
		};

		Vector<text> c_ss = 
		{
			"V:\\rafi_f7n-1\\freemason.framework\\\\bootstrap\\vs14\\",
			"rafi_f7n-1\\freemason.framework\\\\bootstrap\\vs14\\",
			"freemason.framework\\\\bootstrap\\vs14\\",
			"bootstrap\\vs14\\",
			"vs14\\",
			""
		};

		EXPECT_EQ(c_ps, ps);
		EXPECT_EQ(c_ss, ss);
	}

	{
	Vector<text> ps, ss;
	foreach (i, s0.scan(R"([\\/])"))
	{
		ps << i.prefix();
		ss << i.suffix();
	}
	EXPECT_EQ(VT("", "V:", "rafi_f7n-1", "freemason.framework", "", "bootstrap", "vs14"), ps);
	EXPECT_EQ(VT("V:", "rafi_f7n-1", "freemason.framework", "", "bootstrap", "vs14", ""), ss);
	}

	{
	text t("V:\\rafi_f7n-1\\freemason.mk\\vs14\\");
	auto v1 = t.osplit(R"([\\/])");
	EXPECT_EQ(VT("V:", "rafi_f7n-1", "freemason.mk", "vs14"), v1);
	auto v2 = t.split(R"([\\/])");
	EXPECT_EQ(VT("V:", "rafi_f7n-1", "freemason.mk", "vs14", ""), v2);
	}
}

//---------------------------------------------------------------------------------------------

TEST(Text, regexp_with_map)
{
	text s1 = "V:\\rafi_f7n-1\\freemason.framework\\\\bootstrap\\vs14\\";
	auto v1 = s1.osplit(R"([\\/]+)").map([](text s) { return text("*" + s + "*"); });
	EXPECT_EQ(VT("*V:*", "*rafi_f7n-1*", "*freemason.framework*", "*bootstrap*", "*vs14*"), v1);

	text s2 = "\\\\host1\\share\\rafi_f7n-1\\freemason.framework\\\\bootstrap\\vs14\\";
	auto v3 = s2.csplit(R"([\\/]+)").map([](text s) { return text("*" + s + "*"); });
	EXPECT_EQ(VT("**", "*host1*", "*share*", "*rafi_f7n-1*", "*freemason.framework*", "*bootstrap*", "*vs14*", "**"), v3);
}

//---------------------------------------------------------------------------------------------

TEST(Text, split0)
{
	EXPECT_EQ(VT("", "a", "b", "c", ""), "abc"_t.split(R"()"));
	EXPECT_EQ(VT("a", "b", "c"), "abc"_t.osplit(R"()"));
}

TEST(Text, split1)
{
	text t("");
	EXPECT_EQ(VT(""), t.osplit(R"(/)"));
	auto c = t.csplit(R"(/)");
	EXPECT_EQ(VT(""), c);
}

TEST(Text, split2)
{
	text ab("a/b");
	EXPECT_EQ(VT("a", "b"), ab.osplit(R"(/)"));
	EXPECT_EQ(VT("a", "b"), ab.csplit(R"(/)"));

	text a("a/");
	EXPECT_EQ(VT("a"),     a.osplit(R"(/)"));
	EXPECT_EQ(VT("a", ""), a.csplit(R"(/)"));

	text b("/b");
	EXPECT_EQ(VT("b"),     b.osplit(R"(/)"));
	EXPECT_EQ(VT("", "b"), b.csplit(R"(/)"));

	text e("/");
	EXPECT_EQ(VT0,     e.osplit(R"(/)"));
	EXPECT_EQ(VT("", ""), e.csplit(R"(/)"));
}

TEST(Text, split3)
{
	EXPECT_EQ(VT(""),    ""_t.split(R"(/)"));
	BB
	EXPECT_EQ(VT("abc"), "abc"_t.split(R"(/)"));
}

TEST(Text, split4)
{
	EXPECT_EQ(VT0,        "/"_t.osplit(R"(/)"));
	EXPECT_EQ(VT("", ""), "/"_t.split(R"(/)"));
	EXPECT_EQ(VT0,        "/////"_t.osplit(R"(/+)"));
}

TEST(Text, split5)
{
	EXPECT_EQ(VT("a", "b", "c"),         "a  b   c"_t.osplit(R"(\s+)"));
	EXPECT_EQ(VT("a", "b", "c"),         "  a  b   c   "_t.osplit(R"(\s+)"));
	EXPECT_EQ(VT("a", "b", "c"),         "a  b   c"_t.split(R"(\s+)"));
	EXPECT_EQ(VT("a", "b", "c", ""),     "a  b   c   "_t.split(R"(\s+)"));
	EXPECT_EQ(VT("", "a", "b", "c"),     "  a  b   c"_t.split(R"(\s+)"));
	EXPECT_EQ(VT("", "a", "b", "c", ""), "  a  b   c   "_t.split(R"(\s+)"));
}

//---------------------------------------------------------------------------------------------

TEST(Text, join)
{
	EXPECT_STREQ("a b c d e", join(Vector<text>({"a", "b", "c", "d", "e"}), " "));
	EXPECT_STREQ("", join(Vector<text>(), " "));
}

///////////////////////////////////////////////////////////////////////////////////////////////
