
#include <string>

#include "text/text.h"
#include "text/string.h"

#include "types/defs.h"
#include "files/path.h"

namespace magenta
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

Path text::operator~() const
{
	return *Path(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////

text text::operator[](int i) const
{
	return span(i, 1);
}

//---------------------------------------------------------------------------------------------

text text::operator()(const range &r) const
{
	auto s = r.span(length());
	if (!s)
		return "";
	return substr(s.at, s.size);
}

//---------------------------------------------------------------------------------------------

text text::operator()(const magenta::span &s) const
{
	return (*this)(s.range());
}

//---------------------------------------------------------------------------------------------

text text::span(int i, int n) const
{
	return (*this)(magenta::span(i, n));
}

///////////////////////////////////////////////////////////////////////////////////////////////

text text::ltrim() const
{
	static std::locale loc;
	string s = *this;
	s.erase(s.begin(), find_if(s.begin(), s.end(), [](char c){ return !std::isspace<char>(c , loc); }));
	return s;
}

//---------------------------------------------------------------------------------------------

text text::rtrim() const
{
	static std::locale loc;
	string s = *this;
	s.erase(find_if(s.rbegin(), s.rend(), [](char c){ return !std::isspace<char>(c , loc); }).base(), s.end());
	//s.erase(find_if(s.rbegin(), s.rend(), [](char c){ return !std::isspace<char>(c , loc); }), s.end());
	return s;   
}

///////////////////////////////////////////////////////////////////////////////////////////////

long text::to_int(const char *what) const
{
	long n;
	int k = sscanf(c_str(), "%ld", &n);
	if (!k)
		if (what)
			throw std::invalid_argument(stringf("%s: cannot convert '%s' to integer", what, c_str()));
		else
			throw std::invalid_argument(stringf("cannot convert '%s' to integer", c_str()));
	return n;
}

//---------------------------------------------------------------------------------------------

double text::to_num(const char *what) const
{
	double n;
	int k = sscanf(c_str(), "%lf", &n);
	if (!k)
	{
		if (what)
			throw std::invalid_argument(stringf("%s: cannot convert '%s' to number", what, c_str()));
		else
			throw std::invalid_argument(stringf("cannot convert '%s' to number", c_str()));
	}
	return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool text::ieq(const string &s) const
{
	struct icmp
	{
		bool operator()(int a, int b) const { return std::toupper(a) == std::toupper(b); }
	};

	return std::equal(begin(), end(), s.begin(), s.end(), icmp());
}

//---------------------------------------------------------------------------------------------

text text::tolower() const
{
	text t = *this;
	auto n = t.length();
	for (auto i = 0; i < n; ++i)
	{
		char &c = t.at(i);
		c = ::tolower(c);
	}
	return t; 
}

//---------------------------------------------------------------------------------------------

text text::toupper() const
{
	text t = *this;
	auto n = t.length();
	for (auto i = 0; i < n; ++i)
	{
		char &c = t.at(i);
		c = ::toupper(c);
	}
	return t; 
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool text::contains(const string &s) const
{
	return find(s) != string::npos;
}

//---------------------------------------------------------------------------------------------

bool text::startswith(const string &s) const
{
	return find(s) == 0;
}

//---------------------------------------------------------------------------------------------

bool text::endswith(const string &s) const
{
	size_t i = rfind(s);
	return i != string::npos && i == length() - s.length();
}

//---------------------------------------------------------------------------------------------

bool text::equalsto_one_of(const char *p, ...) const
{
	bool rc = false;
	va_list args;
	va_start(args, p);
	for (;;)
	{
		if (!p)
			break;
		if (*this == p)
		{
			rc = true;
			break;
		}
		p = va_arg(args, char*);
	}
	va_end(args);
	return rc;
}

//---------------------------------------------------------------------------------------------

bool text::startswith_one_of(const char *p, ...) const
{
	bool rc = false;
	va_list args;
	va_start(args, p);
	for (;;)
	{
		if (!p)
			break;
		if (startswith(p))
		{
			rc = true;
			break;
		}
		p = va_arg(args, char*);
	}
	va_end(args);
	return rc;
}

//---------------------------------------------------------------------------------------------

bool text::endswith_one_of(const char *p, ...) const
{
	bool rc = false;
	va_list args;
	va_start(args, p);
	for (;;)
	{
		if (!p)
			break;
		if (startswith(p))
		{
			rc = true;
			break;
		}
		p = va_arg(args, char*);
	}

	va_end(args);
	return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////

text text::replace(const text &t, const text &with)
{
	string s = *this;
	size_t n_t = t.length(), n_w = with.length();
	for (size_t i = 0; (i = s.find(t, i)) != string::npos; i += n_w)
		s.replace(i, n_t, +with);
	return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////

#if 0
bool text::search(const char *re_p, std::smatch &match) const
{
	std::regex re(re_p);
	bool b = regex_search(*this, match, re);
	return b;
}
#endif

//---------------------------------------------------------------------------------------------

bool text::match(const char *re_p, text::regex::Match &match) const
{
	text re_s(re_p);
	re_s += ".*";
	std::regex re(re_s);
	bool b = regex_match(begin(), end(), match, re);//, regex_constants::match_not_eol);
	return b;
}

//---------------------------------------------------------------------------------------------

text::regex::Match text::match(const char *re_p) const
{
	text::regex::Match match;

	text re_s(re_p);
	re_s += ".*";
	std::regex re(re_s);
	bool b = regex_match(begin(), end(), match, re);//, regex_constants::match_not_eol);
	return match;
}

//---------------------------------------------------------------------------------------------

text::regex::iterator text::scan(const char *re) const
{
	std::regex *pre = new std::regex(re);
	return text::regex::iterator(*this, pre);
}

//---------------------------------------------------------------------------------------------

Vector<text> text::split(const char *re_s, split_type type) const
{
	Vector<text> v;
	std::regex re(re_s);
	regex::iterator i(*this, re);
	if (!i)
	{
		v << *this;
		return v;
	}
		
	if (type == split_type::close || !!i.prefix())
		v << i.prefix();

	for (; !i.is_last(); ++i)
	{
		auto s = i.suffix();
		v << s;
	}
	auto p = i.prefix();
	auto s = i.suffix();
	if (type == split_type::close || !!i.suffix())
		v << s;

	return v;
}

//---------------------------------------------------------------------------------------------

Vector<text> text::csplit(const char *re) const
{
	return split(re, split_type::close);
}

Vector<text> text::osplit(const char *re) const
{
	return split(re, split_type::open);
}

///////////////////////////////////////////////////////////////////////////////////////////////

std::sregex_iterator text::regex::iterator::_end;

//---------------------------------------------------------------------------------------------

text::regex::iterator::~iterator()
{
	delete _re;
}

//---------------------------------------------------------------------------------------------

text text::regex::iterator::prefix() const
{
	try
	{
		return match().prefix();
	}
	catch (std::logic_error)
	{
		return ""_t;
	}
}

//---------------------------------------------------------------------------------------------

text text::regex::iterator::full_prefix() const
{
	return string(_str.begin(), _str.begin() + match().at());
}

//---------------------------------------------------------------------------------------------

text text::regex::iterator::suffix() const
{
	try
	{
		if (_la != _end)
			return Match(*_la).prefix();
		return match().suffix();
	}
	catch (std::logic_error)
	{
		return ""_t;
	}
}

//---------------------------------------------------------------------------------------------

text text::regex::iterator::full_suffix()
{
	try
	{
		return match().suffix();
	}
	catch (std::logic_error)
	{
		return ""_t;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

text join(const Vector<text> &strs, const text &sep)
{
	if (!strs)
		return "";
	text s = strs[0];
	foreach (t, strs.begin()+1)
		s += sep + *t;
	return s;
}

//---------------------------------------------------------------------------------------------

std::any ston(const string &s)
{
	char *p;
	long n = strtol(s.c_str(), &p, 10);
	if (!!p && p - s.c_str() == s.length())
		return n;
	double d;
	if (sscanf(s.c_str(), "%lf", &d) == 1)
		return d;
	return std::any();
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
