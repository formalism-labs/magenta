
#if _MSC_VER > 1300
#pragma warning(disable:4996)
#endif

#include "command-line.h"

namespace magenta
{

namespace CommandLine
{

///////////////////////////////////////////////////////////////////////////////////////////////

Options::Options(int argc, char *argv[])
{
	_args = 0;
	if (argc == 1)
		return;

	string args;
	push_back(args = argv[1]);
	for (int j = 2; j < argc; ++j)
	{
		args += " ";
		args += argv[j];
		push_back(argv[j]);
	}

	_args = new char[args.length() + 1];
}

//---------------------------------------------------------------------------------------------

Options::Options(const char *args)
{
	_args = 0;
	parse(args);
}

//---------------------------------------------------------------------------------------------

void Options::parse(const char *args)
{
	clear();
	_args = new char[strlen(args) + 1];
	strcpy(_args, args);

	char seps[] = " \t";
	char *token = strtok(_args, seps);
	while (token)
	{
		push_back(token);	
		token = strtok(0, seps);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

Options::ConstIterator Options::operator()(const string &opt) const
{
	ConstIterator i = begin();
	for (; !!i; ++i)
		if (*i == opt)
			break;
	return i;
}

//---------------------------------------------------------------------------------------------

Options::Iterator Options::operator()(const string &opt)
{
	Iterator i = begin();
	for (; !!i; ++i)
		if (*i == opt)
			break;
	return i;
}

//---------------------------------------------------------------------------------------------

bool Options::exist(const string &opt) const
{
	return !!(*this)(opt);
}

//---------------------------------------------------------------------------------------------

Options::ConstIterator Options::firstNonOption() const
{
	ConstIterator i = begin();
	for (; !!i; ++i)
		if ((*i)[0] != '-')
			break;
	return i;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace CommandLine
} // namespace magenta
