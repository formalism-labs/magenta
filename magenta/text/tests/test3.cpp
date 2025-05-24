
#include "magenta/tests/gtest.h"

#include "magenta/text/defs.h"
#include "magenta/types/defs.h"
#include "magenta/files/defs.h"
#include "magenta/text/ini-file.h"

using namespace magenta;

///////////////////////////////////////////////////////////////////////////////////////////////

const char inifile1[] = R"(
[global]
views          = w:/
framework      = framework
framework.site = site/framework
repo           = repo
repo.site      = site/repo
perl.windows   = sys/stbperl-5.24.0.1-windows
perl.linux     = /tmp/perl-5.25.0.0-linux
msys2          = sys/msys2-20161025
scaffolds      = sys/scaffolds
)";
 
TEST(Text, inifile1)
{
	TempFile f1(inifile1);
	IniFile ini(f1.name());
	text framework = ini["global"]["framework"];
	ASSERT_STREQ("framework", +framework);
	ini["global"]["framework"] = "workframe";
	ASSERT_STREQ("workframe", +ini["global"]["framework"]);
}

///////////////////////////////////////////////////////////////////////////////////////////////
