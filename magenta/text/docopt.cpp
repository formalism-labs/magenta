
//#include <stdexcept>

#include "exceptions/defs.h"

#include "text/string.h"
#include "text/strings.h"
#include "text/text.h"
#include "text/docopt.h"

#include "types/defs.h"

///////////////////////////////////////////////////////////////////////////////////////////////

namespace docopt
{

void to_json(magenta::JSON &j, const std::map<std::string, docopt::value> &args)
{
	for (auto const &arg : args)
	{
		if (arg.second.isStringList())
			j[arg.first] = arg.second.asStringList();
		else
		{
			std::strstream ss;
			ss << arg.second << std::ends;
			j[arg.first] = ss.str();
		}
	}
}

} // namespace docopt

///////////////////////////////////////////////////////////////////////////////////////////////

namespace magenta
{
namespace Docopt
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Option

Option::operator long() const
{
	return _val.asLong();
}

Option::operator text() const
{
	if (_val.isString())
		return _val.asString();
	if (_val.isLong())
		return _val.asLong();
	if (_val.isBool())
		return _val.asBool() ? "1" : "0";
	return "";
}

Option::operator bool() const
{
	if (!bool(_val))
		return false;
	return _val.isBool() ? _val.asBool() : true;
}

Option::operator Strings() const
{
	if (_val.isStringList())
		return _val.asStringList();
	Strings ss;
	ss << text(*this);
	return ss;
}

bool Option::operator!() const
{
	return !bool(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Options

Options::Options(const map<string, docopt::value> &docopt) : _map(docopt)
{
}
	
//---------------------------------------------------------------------------------------------

bool Options::_exist(const string &s, string &v) const
{
	auto it = _map.find(s);
	auto b = it != _map.end();
	if (b)
		v = s;
	return b;
}

//---------------------------------------------------------------------------------------------

bool Options::exist(const string &s, string &verb) const
{
	if (_exist(s, verb))
		return true;
	if (_exist("--"_t + s, verb))
		return true;
	if (_exist("<"_t + s + ">", verb))
		return true;
	if (_exist(text(s).toupper(), verb))
		return true;
	//faiL("invalid option: %s", +s);
	return false;
}

//---------------------------------------------------------------------------------------------

bool Options::exist(const string &s) const
{
	string v;
	return exist(s, v);
}

//---------------------------------------------------------------------------------------------

Option Options::operator[](const string &s) const
{
	string t;
	if (exist(s, t))
		return Option(s, t, _map[t]);

	faiL("invalid option: %s", +s);
	//return Option(docopt::value();
/*
	if (b && bool(it->second))
	{
		if (it->second.isBool())
			return it->second.asBool();
		auto b2 =  && 
		b = b1 && b2;
		if (b)
			v = s;
	}*/
}

//---------------------------------------------------------------------------------------------

Option Options::verbatim(const string &s) const
{
	string t;
	if (_exist(s, t))
		return Option(s, t, _map[t]);

	faiL("invalid option: %s", +s);
	//return docopt::value();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Command

Command::Command(const string &doc, int argc, char *argv[], const text &name) : _nop(false)
{
	try
	{
		map<string, ::docopt::value> args = ::docopt::docopt(doc, 
			{ argv + 1, argv + argc }, true, +name, true);
		_options = new Options(args);
		if (_options->exist("nop"))
			_nop = !!(*_options)["nop"];
	}
	catch (exception &x)
	{
		cerr << x.what() << endl;
		exit(1);
	}
	catch (...)
	{
		cerr << "fatal error" << endl;
	}
}

//---------------------------------------------------------------------------------------------

void Command::run()
{
	try
	{
		exit(main());
	}
	catch (exception &x)
	{
		cerr << "fatal error: " << x.what() << endl;
		exit(1);
	}
}
	
///////////////////////////////////////////////////////////////////////////////////////////////
// Commands

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Docopt
} // namespace magenta
