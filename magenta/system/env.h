
#ifndef _magenta_system_env_
#define _magenta_system_env_

#include <string>

#include "magenta/text/defs.h"
#include "magenta/files/defs.h"

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class EnvVar
{
	text _name;
	text _val;

public:
	explicit EnvVar(text name, text defval = "") : _name(name)
	{
		const char *val = getenv(+name);
		if (val)
		{
			_val = val;
			return;
		}
		if (!!defval)
			*this = defval;
	}

	const text &name() const { return _name; }

	text &value() { return _val; }
	const text &value() const { return _val; }

	operator text&() { return value(); }
	operator const text&() const { return value(); }

	bool operator==(const text &t) const { return value() == t; }

	const text &operator*() const { return value(); }
	Path operator~() const { return ~value(); }
	bool operator!() const { return !value(); }

	EnvVar &operator=(const text &s)
	{
		value() = s;
		_putenv(+stringf("%s=%s", +_name, +value()));
		return *this;
	}
};

//inline text operator+(const EnvVar &v, const text &s) { return *v + s; }
//inline text operator+(const text &s, const EnvVar &v) { return s + *v; }

#define ENV_VAR(var, name, ...) EnvVar var(#name, ##__VA_ARGS__)
#define ENV_VAR1(var, ...) EnvVar var(#var, ##__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////////////////////

class Pathlet : public text
{
protected:
	void ctor(const string &s);

public:
	explicit Pathlet(const string &p) { ctor(p); }
	Pathlet(const Path &p) { ctor(p); }

	bool operator!() const { return blank(); }
};

//---------------------------------------------------------------------------------------------

class EnvPath : public text
{
	static text fix(const string &s);
	static text trim(const string &s);
	static text tolower(const string &s);

	void assign(const string &s) { string::assign(s); }

	static const char sep;

public:
	EnvPath() {}

	EnvPath(const string &s) { assign(fix(s)); }

	EnvPath &push_front(const Pathlet &p);
	EnvPath &push_back(const Pathlet &p);
	EnvPath &push_front(const EnvPath &p);
	EnvPath &push_back(const EnvPath &p);

	EnvPath &operator<<=(const string &s) { return push_back(EnvPath(s)); }
	EnvPath &operator<<=(const EnvPath &p) { return push_back(p); }

	EnvPath operator<<(const string &s) const { return EnvPath(*this).push_back(EnvPath(s)); }
	EnvPath operator<<(const EnvPath &p) const { return EnvPath(*this).push_back(p); }

	EnvPath &operator>>=(const string &s) { return push_front(EnvPath(s)); }
	EnvPath &operator>>=(const EnvPath &p) { return push_front(p); }

	EnvPath operator>>(const string &s) const { return EnvPath(*this).push_front(EnvPath(s)); }
	EnvPath operator>>(const EnvPath &p) const { return EnvPath(*this).push_front(p); }

	Pathlet pop_front();
	Pathlet pop_back();

	size_t find(const Pathlet &p) const;
	bool find_and_remove(const Pathlet &p);

	bool operator!() const { return empty(); }

	EnvPath filter(const string &t, bool out = false) const;
	EnvPath filter_out(const string &t) const { return filter(t, true); }

	text to_str() const;
};

//---------------------------------------------------------------------------------------------

class SystemPath : public EnvPath
{
public:
	SystemPath() : EnvPath(getenv("PATH")) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_env_
