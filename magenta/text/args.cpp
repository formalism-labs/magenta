
#include "args.h"

#include <string.h>

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

void Arguments::append(int argc, char *argv[])
{
	for (int i = 0; i < argc; ++i)
		append(argv[i]);
}

//---------------------------------------------------------------------------------------------

void Arguments::append(const text &s)
{
	_args.push_back(s);
}

//---------------------------------------------------------------------------------------------

text Arguments::toString() const
{
	int j = 0;
	text s;
	for (ConstIterator i = *this; !!i; ++i, ++j)
	{
		// kind of half-assed quoting; won't handle strings containing quotes properly, 
		// but good enough for debug messages for the moment.
		bool needs_quotes = strpbrk(i->c_str(), " \t\n\"\';") != NULL;
		if (j)
			s += " ";
		if (needs_quotes)
			s += "\n";
		s += *i;
		if (needs_quotes)
			s += "\n";
	}

    return s;
}

//---------------------------------------------------------------------------------------------

text &Arguments::at(int i)
{
	std::list<text>::iterator j;
	for (j = _args.begin(); j != _args.end() && i > 0; ++j, --i) ;
	if (j == _args.end())
		throw std::range_error("argument index out of range");
	return *j;
}

//---------------------------------------------------------------------------------------------

Arguments::Iterator Arguments::find(const text &opt, bool strict)
{
	if (strict)
	{
		for (Iterator i = *this; !!i; ++i)
			if (*i == opt)
				return i;
	}
	else
	{
		for (Iterator i = *this; !!i; ++i)
			if (start_with(*i, opt))
				return i;
	}

	return end();
}

//---------------------------------------------------------------------------------------------

Arguments::Iterator Arguments::find(const text &opt, Arguments::Iterator i, bool strict)
{
	if (strict)
	{
		for (; !!i; ++i)
			if (*i == opt)
				return i;
	}
	else
	{
		for (; !!i; ++i)
			if (start_with(*i, opt))
				return i;
	}

	return end();
}

//---------------------------------------------------------------------------------------------

Arguments::ConstIterator Arguments::find(const text &opt, bool strict) const
{
	if (strict)
	{
		for (ConstIterator i = *this; !!i; ++i)
			if (*i == opt)
				return i;
	}
	else
	{
		for (ConstIterator i = *this; !!i; ++i)
			if (start_with(*i, opt))
				return i;
	}

	return end();
}

//---------------------------------------------------------------------------------------------

Arguments::ConstIterator Arguments::find(const text &opt, Arguments::ConstIterator i, bool strict) const
{
	if (strict)
	{
		for (; !!i; ++i)
			if (*i == opt)
				return i;
	}
	else
	{
		for (; !!i; ++i)
			if (start_with(*i, opt))
				return i;
	}

	return end();
}

//---------------------------------------------------------------------------------------------

Arguments Arguments::pop_front(size_t n)
{
	if (n > count())
		n = count();
	std::list<text> args;
	while (n--)
	{
		text s = _args.front();
		args.push_back(s);
		_args.pop_front();
	}
	return Arguments(args);
}

//---------------------------------------------------------------------------------------------

Arguments Arguments::pop_back(size_t n)
{
	if (n > count())
		n = count();
	std::list<text> args;
	while (n--)
	{
		args.push_front(_args.back());
		_args.pop_back();
	}
	return Arguments(args);
}

//---------------------------------------------------------------------------------------------

// This allocated a char** (array of char*-s in argv format)
char **Arguments::argv() const
{
	auto n = _args.size() + 1; // +1 for extra null pointer at and of args list
	auto size_in_chars = sizeof(char*) * n;
	for (ConstIterator i = *this; !!i; ++i)
		size_in_chars += i->length() + 1;
	char *argv = new char[size_in_chars];
	char **v = (char**) argv;
	char *s = (char *) &v[n];
	for (ConstIterator i = *this; !!i; ++i)
	{
		*v++ = s;
		strcpy(s, i->c_str());
		s += i->length() + 1;
	}
	*v = 0;
	return reinterpret_cast<char**>(argv);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Arguments::ConstIterator Arguments::ConstIterator::at_offset(int n) const
{
	ConstIterator i = *this;
	if (n > 0)
		while (n--)
			++i;
	else
		while (n++)
			--i;
	return i;
}

//---------------------------------------------------------------------------------------------

Arguments::Iterator Arguments::Iterator::at_offset(int n)
{
	Iterator i = *this;
	if (n > 0)
		while (n--)
			++i;
	else
		while (n++)
			--i;
	return i;
}

//---------------------------------------------------------------------------------------------

void Arguments::Iterator::remove(int n) 
{
	while (!!*this && n--)
		_iter = _args._args.erase(_iter);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
