
#ifndef _magenta_text_docopt_
#define _magenta_text_docopt_

#include <strstream>

#include "magenta/text/classes.h"

#include "github.docopt.docopt.cpp/docopt.h"
#include "magenta/text/json.h"

///////////////////////////////////////////////////////////////////////////////////////////////

namespace docopt
{

void to_json(magenta::JSON &j, const std::map<std::string, docopt::value> &args);

} // namespace docopt

///////////////////////////////////////////////////////////////////////////////////////////////

namespace magenta
{
namespace Docopt
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

class Option
{
	text _name, _verb;
	docopt::value _val;
	
public:
	Option(const string &name, const string &verb, docopt::value &val) : _name(name), _verb(verb), _val(val) {}
	
	operator long() const;
	operator text() const;
	operator bool() const;
	operator Strings() const;
	
	Strings strings() const { return Strings(*this); }
	bool operator!() const;
};

//---------------------------------------------------------------------------------------------

class Options
{
	mutable map<string, docopt::value> _map;

protected:
	bool _exist(const string &s, string &verb) const;
	bool exist(const string &s, string &verb) const;

public:
	Options(const map<string, docopt::value> &docopt);
	
	bool exist(const string &s) const;

	Option operator[](const string &s) const;
	Option verbatim(const string &s) const;
	
	JSON json() const { JSON j; docopt::to_json(j, _map); return j; }
};

//---------------------------------------------------------------------------------------------

class Command
{
	bool _nop;
	Options *_options;

public:
	Command(const string &doc, int argc, char *argv[], const text &name);

	Options &options() const { return *_options; }
	Option option(const string &s) const { return options()[s]; }
	Option operator[](const string &s) const { return options()[s]; }
	bool exist(const string &s) const { return _options->exist(s); }

	void run();
	
	virtual int main() { return 0; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Commands
{
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace docopt
} // namespace magenta

#endif // _magenta_text_docopt_
