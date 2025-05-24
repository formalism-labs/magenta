
#ifndef _magenta_text_commnad_line_
#define _magenta_text_commnad_line_

#include "magenta/types/defs.h"

#include <string>

namespace magenta
{

namespace CommandLine
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class Options : public List<string>
{
	char *_args;

	void parse(const char *args);

public:
	Options(int argc, char *argv[]);
	Options(const char *args);
	~Options() { delete _args; }

//	typedef ConstIterator Iterator;

	ConstIterator operator()(const string &opt) const;
	Iterator operator()(const string &opt);
	bool exist(const string &opt) const;

	ConstIterator firstNonOption() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
struct Argument
{
	T value;
	bool exist;

	Argument() : exist(false) {}
	Argument(T v) : value(v), exist(false) {}
	Argument(CommandLine::Options &options, string arg) : exist(false) 
		{ read(options, arg); }
	Argument(CommandLine::Options &options, string arg, T v) : value(v), exist(false) 
		{ read(options, arg); }

	operator const T() const { return value; }

	Argument &operator=(String s)
	{
		set(s);
		return *this;
	}

	void set(String s)
	{
		value = s;
		exist = true;
	}

	void read(CommandLine::Options &options, string arg)
	{
		CommandLine::Options::Iterator i = options(arg);
		if (!i)
			return;

		set(*i[1]);
		i.remove(2);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace CommandLine
} // namespace magenta

#endif // _magenta_text_commnad_line_
