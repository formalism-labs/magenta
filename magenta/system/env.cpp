
#include "env.h"

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

void Pathlet::ctor(const string &p)
{
	if (p.find_first_not_of(" \t\n") == string::npos)
		return;
	auto i = p.find_first_of(";");
	if (i != string::npos)
	{
		assign(p.substr(0, i));
		return;
	}
	assign(p);
	size_t j = 0;
	while (j < length() && (j = find_first_of("\\/", j)) != string::npos)
	{
		if (at(j) == '/')
			at(j) = '\\';
		++j;
		if (j < length() && (at(j) == '/' || at(j) == '\\'))
			erase(j, 1);
	}

	auto n = length();
	j = find_last_not_of("\\/");
	if (j != string::npos && j+1 < length())
		erase(j+1);
}

///////////////////////////////////////////////////////////////////////////////////////////////

const char EnvPath::sep = 
#ifdef _WIN32
	';';
#else
	':';
#endif

//---------------------------------------------------------------------------------------------

text EnvPath::fix(const string &s)
{
	EnvPath p, p1;
	p.assign(s);
	while (!p.empty())
	{
		Pathlet q = p.pop_front();
		if (!!q)
			p1.push_back(q);
	}
	return p1;
}

//---------------------------------------------------------------------------------------------

text EnvPath::trim(const string &s)
{
	auto i = s.find_first_not_of(" \t\n");
	auto j = s.find_last_not_of(" \t\n");
	if (i == string::npos || j == string::npos)
		return "";
	return s.substr(i, j - i + 1);
}

//---------------------------------------------------------------------------------------------

text EnvPath::tolower(const string &s)
{
	auto t = s;
	auto n = t.length();
	for (int i = 0; i < n; ++i)
	{
		char &c = t.at(i);
		c = ::tolower(c);
	}
	return t; 
}

//---------------------------------------------------------------------------------------------

EnvPath &EnvPath::push_front(const Pathlet &p)
{
	if (!p)
		return *this;

	find_and_remove(p);
	if (empty())
		assign(p);
	else
		assign(p + sep + *this);
	return *this;
}

//---------------------------------------------------------------------------------------------

EnvPath &EnvPath::push_back(const Pathlet &p)
{
	if (!p || find(p) != string::npos)
		return *this;

	if (!empty())
		append(text(sep));
	append(p);
	return *this;
}

//---------------------------------------------------------------------------------------------

EnvPath &EnvPath::push_front(const EnvPath &p)
{
	EnvPath p1 = p;
	while (!p1.empty())
		push_front(p1.pop_back());
	return *this;
}

//---------------------------------------------------------------------------------------------

EnvPath &EnvPath::push_back(const EnvPath &p)
{
	EnvPath p1 = p;
	while (!p1.empty())
		push_back(p1.pop_front());
	return *this;
}

//---------------------------------------------------------------------------------------------

Pathlet EnvPath::pop_front()
{
	auto i = find_first_of(sep);
	if (i == string::npos)
	{
		Pathlet p(string(*this));
		assign("");
		return p;
	}
	Pathlet p(substr(0, i));
	assign(substr(i + 1, string::npos));
	return p;
}

//---------------------------------------------------------------------------------------------

Pathlet EnvPath::pop_back()
{
	auto i = find_last_of(sep);
	if (i == string::npos)
	{
		Pathlet p(string(*this));
		assign("");
		return p;
	}
	Pathlet p(substr(i+1));
	assign(substr(0, i));
	return p;
}

//---------------------------------------------------------------------------------------------

size_t EnvPath::find(const Pathlet &p) const
{
	auto n = length();
	auto m = p.length();
	size_t i = 0;
	while (i < n && i != string::npos)
	{
		auto j = string::find(p, i);
		if (j == string::npos)
			return j;
		if (i > 0)
		{
			if (at(i - 1) != sep)
			{
				i = j + 1;
				continue;
			}
		}
		if (j + m == n || at(j + m) == sep)
			return j;
		i = find_first_of(sep);
	}
	return string::npos;
}

//---------------------------------------------------------------------------------------------

bool EnvPath::find_and_remove(const Pathlet &p)
{
	auto i = find(p);
	if (i == string::npos)
		return false;
	erase(i, p.length());
	if (at(--i) == sep)
		erase(i, 1);
	return true;
}

//---------------------------------------------------------------------------------------------

EnvPath EnvPath::filter(const string &t, bool out) const
{
	string t1 = tolower(t);
	EnvPath p = *this, p1;
	while (!p.empty())
	{
		Pathlet q = p.pop_front();
		string q1 = tolower(q);
		bool f = q1.find(t1) != string::npos;
		if (out && !f || !out && f)
			p1.push_back(q);
	}
	return p1;
}

//---------------------------------------------------------------------------------------------

text EnvPath::to_str() const
{
	text s;
	EnvPath p = *this;
	for (auto i = 0; !!p; ++i)
	{
		Pathlet q = p.pop_front();
		s += stringf("[%02d] %s\n", i+1, +q);
	}
	return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
