
#ifndef _magenta_text_text_
#define _magenta_text_text_

#include "magenta/common.h"

#include <stdexcept>
#include <string>
#include <any>

#ifdef MAGENTA_FEATURE_BOOST_REGEXP // def _WIN32
#include "boost/regex.hpp"
namespace std
{
	using boost::regex;
   	using boost::sregex_iterator;
   	using boost::smatch;
	using boost::sub_match;
   	using boost::regex_search;
   	namespace regex_constants 
	{
		using boost::regex_constants::match_not_null;
   	}
}
#else
#include <regex>
#endif

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "magenta/types/classes.h"
#include "magenta/types/general.h"

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class text : public string
{
public:
	text() {}
	text(char c) : string(1, c) {}
	text(const char *cp) : string(cp) {}
	text(const char *cp, size_t n) : string(cp, n) {}
	text(const string &s) : string(s) {}
	text(const text &s) : string(s) {}
//	text(long n) : string(std::to_string()) {}
	text(const Vector<text> &v, const text &sep = "");

	bool startwith(const text &s) const{ return find(s) == 0; }

	text ltrim() const;
	text rtrim() const;
	text trim() const { return rtrim().ltrim(); }

	long to_int(const char *what = 0) const;
	double to_num(const char *what = 0) const;

	text tolower() const;
	text toupper() const;

	bool blank() const { return empty() || trim().empty(); }
	bool operator!() const { return blank(); }

	bool ieq(const string &s) const;

	text operator[](int i) const;
	text operator()(int i, int j) const { return (*this)(range(i, j)); }
	text operator()(const range &r) const;
	text operator()(const span &s) const;
	text span(int i, int n) const;

	bool contains(const string &s) const;
	bool startswith(const string &s) const;
	bool endswith(const string &s) const;

	bool equalsto_one_of(const char *p, ...) const;
	bool startswith_one_of(const char *p, ...) const;
	bool endswith_one_of(const char *p, ...) const;

	text replace(const text &t, const text &with);

	class Path operator~() const;

	//-----------------------------------------------------------------------------------------

	class regex
	{
	public:
		class Match;
		class Group;

		//-------------------------------------------------------------------------------------

		class Match : protected std::smatch
		{
			friend class text;
			typedef std::smatch Super;

			const Super &super() const { return *this; }

		public:
			Match() {}
			Match(const Super &m) : Super(m) {}

			bool operator!() const { return super().size() == 0; }

			text str() const 
			{
				if (super().empty())
					return "";
				return super().str(); 
			}

			text prefix() const 
			{ 
				if (super().empty())
					return "";
				return (const string &) super().prefix(); 
			}
			
			text suffix() const 
			{ 
				if (super().empty())
					return "";
				return (const string &) super().suffix(); 
			}

			size_t at() const { return super().position(); }

			text operator*() const { return ngroups() == 0 ? str() : group(1); }
			const Match *operator->() const { return this; }

			Group group(int i) const { return super()[i]; }
			Group operator[](int i) { return group(i); }

			size_t ngroups() const
			{
				if (super().empty())
					return 0;
				return super().size() - 1; 
			}
		};

		//-------------------------------------------------------------------------------------

		class Group : protected std::sub_match<string::const_iterator>
		{
			friend class text;
			typedef std::sub_match<string::const_iterator> Super;

			const Super &super() const { return *this; }

		public:
			Group(const Super &g) : Super(g) {}

			operator string() const { return super().str(); }
			operator text() const { return super().str(); }

			text str() const { return super().str(); }
			text operator*() const { return str(); }

			//int at() const { return super().}
		};

		//-------------------------------------------------------------------------------------

		class iterator : protected std::sregex_iterator
		{
			friend class text;
			typedef std::sregex_iterator Super;

			std::regex *_re;

			static std::sregex_iterator _end;

			Super &super() { return *this; }
			const Super &super() const { return *this; }

			Super _la;
			const string &_str;

		protected:
			void ctor()
			{
				_la = super();
				if (_la != _end)
					++_la;
			}

			iterator(const text &text, std::regex *re) : Super(text.begin(), text.end(), *re), _str(text), _re(re)
			{
				ctor();
			}

		public:
			iterator(const text &text, std::regex &re) : Super(text.begin(), text.end(), re), _str(text), _re(0)
			{
				ctor();
			}

			~iterator();

			bool operator!() const { return *this == _end; }

			Match match() const
			{
				if (!*this)
					return Match();
				return Match(*super());
			}

			const Match operator->() const { return match(); }

			text prefix() const;
			text full_prefix() const;
			text suffix() const;
			text full_suffix();

			size_t ngroups() const { return match().ngroups(); }
			Group group(int i) const { return match().group(i); }
			Group operator[](int i) const { return group(i); }
			// int at(int i) const { return group(i).at(); }

			text operator*() const { return group(1).str(); }

			iterator &operator++() { next(); return *this; }
			iterator operator++(int) { auto i = *this; next(); return i; }

			bool is_last() const { return !*this || _la == _end; }

		protected:
			void next()
			{
				++super();
				if (_la != _end)
					++_la;
			}
		};

	};

//	text(const regex::match &m) : string(m.str()) {}
//	text(const std::ssub_match &m) : string(m.str()) {}

//	text(const Match &m) : string(m.str()) {}
//	text(const std::ssub_match &m) : string(m.str()) {}

//	bool search(const char *re, regex::match &match) const;

	bool match(const char *re, regex::Match &match) const;
	regex::Match match(const char *re) const;
	regex::iterator scan(const char *re) const;
	regex::iterator matches(const char *re) const { return scan(re); }

	enum class split_type
	{
		open,
		close
	};

	Vector<text> split(const char *re, split_type type = split_type::close) const;
	Vector<text> osplit(const char *re) const;
	Vector<text> csplit(const char *re) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////

#if 0

class Strings : public Vector<text>
{
public:
	Strings(const Vector<string> &ss);
	Strings(const Vector<text> &ss);
	Strings(const Vector<char*> &ss);
	Strings(const text &s, const text &sep);

	text join(const text &sep) const;// { return join(*this, sep); }
};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////

text join(const Vector<text> &strs, const text &sep);

//inline text operator+(const text &s, const char *p) { return std::operator+(+s, p); }
//inline text operator+(const char *p, const text &s) { return std::operator+(p, +s); }

//inline text operator+(const text &s, const string &t) { return std::operator+(+s, t); }
//inline text operator+(const string &t, const text &s) { return std::operator+(t, +s); }

inline string operator "" _s(const char *p, size_t n)
{
	return string(p, n);
}

inline text operator "" _t(const char *p, size_t n)
{
	return text(p, n);
}

std::any ston(const string &s);

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_text_
