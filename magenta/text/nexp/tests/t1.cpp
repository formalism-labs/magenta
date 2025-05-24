
#include "magenta/text/nexp/defs.h"

using namespace magenta;
using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

#define PRINT(x) printf("%s: ", #x); x.print()

void t1()
{
	try
	{
		Nexp ne("t1.nexp");
		PRINT(ne);
		PRINT(ne("numbers"));
		Node &n1 = ne("numbers")("first")[0];

		PRINT(ne["numbers"]);
		PRINT(ne("numbers"));
		PRINT(ne("numbers")["first"]);
		PRINT(ne("numbers")("first"));

		int n = ne("numbers")("first");
		PRINT(n1);
	}
	catch (Exception &x)
	{
		THROW_EX(Exception)(x, "t1 failed");
	}
}

int main()
{
	try
	{
		t1();
	}
	catch (Exception &x)
	{
		printf("%s\n", x.report().c_str());
	}
	catch (...) 
	{
		printf("error!\n");
		return -1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
