
TEST(TEST_GROUP, plus)
{
	char *a = "a";
	string b("b");
	text c("c");
	Thing d("d");
#define x(s) Thing(#s)

	ASSERT_EQ(x(abcd), a + b + c + d);
	ASSERT_EQ(x(abdc), a + b + d + c);
	ASSERT_EQ(x(acbd), a + c + b + d);
	ASSERT_EQ(x(acdb), a + c + d + b);
	ASSERT_EQ(x(adbc), a + d + b + c);
	ASSERT_EQ(x(adcb), a + d + c + b);
	ASSERT_EQ(x(bacd), b + a + c + d);
	ASSERT_EQ(x(badc), b + a + d + c);
	ASSERT_EQ(x(bcad), b + c + a + d);
	ASSERT_EQ(x(bcda), b + c + d + a);
	ASSERT_EQ(x(bdac), b + d + a + c);
	ASSERT_EQ(x(bdca), b + d + c + a);
	ASSERT_EQ(x(cabd), c + a + b + d);
	ASSERT_EQ(x(cadb), c + a + d + b);
	ASSERT_EQ(x(cbad), c + b + a + d);
	ASSERT_EQ(x(cbda), c + b + d + a);
	ASSERT_EQ(x(cdab), c + d + a + b);
	ASSERT_EQ(x(cdba), c + d + b + a);
	ASSERT_EQ(x(dabc), d + a + b + c);
	ASSERT_EQ(x(dacb), d + a + c + b);
	ASSERT_EQ(x(dbac), d + b + a + c);
	ASSERT_EQ(x(dbca), d + b + c + a);
	ASSERT_EQ(x(dcab), d + c + a + b);
	ASSERT_EQ(x(dcba), d + c + b + a);
	
#undef x	
}

TEST(TEST_GROUP, plus1)
{
	Thing t("a");
	Thing p1(
		t + "1" +
		t + "2" +
		t + "3" +
		t + "4");
	ASSERT_EQ(Thing("a1a2a3a4"), p1);

	Thing p2 =
		t + "1" +
		t + "2" +
		t + "3" +
		t + "4";
	ASSERT_EQ(Thing("a1a2a3a4"), p2);
}
