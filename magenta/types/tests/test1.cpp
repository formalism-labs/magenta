
#include "gtest/gtest.h"

#include "magenta/types/defs.h"

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(Vector, ranges)
{
	Vector<int> v{ 1, 2, 3, 4, 5, 6 };
	EXPECT_EQ(6, v[-1]);
	EXPECT_EQ(Vector<int>({1, 2, 3}), v(0,2));
	EXPECT_EQ(Vector<int>({2, 3, 4, 5, 6}), v[range(1, -1)]);
	EXPECT_EQ(Vector<int>({1, 2}), v[span(0, 2)]);
}

//---------------------------------------------------------------------------------------------

TEST(Vector, map)
{
	Vector<int> v{ 1, 2, 3, 4, 5 };

	auto w1 = v.map([](int n) { return n * 2; });
	EXPECT_EQ(Vector<int>({2, 4, 6, 8, 10}), w1);

	auto w2 = w1.reduce([](int sum, int n) { return sum + n; }, 0);
	EXPECT_EQ(30, w2);
}

///////////////////////////////////////////////////////////////////////////////////////////////

struct P
{
	int x, y;
};

TEST(RefVector, basic)
{
	//RefVector<P> v = { new P{1,2}, new P{10,20} };
	RefVector<P> v, w;
	v << new P{1,2};
	v << new P{10, 20};
	w << v[0];
	v[0]->x = 5;
	EXPECT_EQ(5, w[0]->x);
}

///////////////////////////////////////////////////////////////////////////////////////////////

TEST(Lambda, name)
{
	auto la = [](int n) -> int { return n * 2; };
	EXPECT_STREQ("int __cdecl(int)", typeid(lambda<decltype(la)>::type).name());
}

///////////////////////////////////////////////////////////////////////////////////////////////
