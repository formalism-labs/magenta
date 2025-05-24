
#ifndef _magenta_files_path_
#define _magenta_files_path_

#include "magenta/files/classes.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"
#include "magenta/text/defs.h"

#ifdef _WIN32
#include "magenta/system/windows.h"
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Path
///////////////////////////////////////////////////////////////////////////////////////////////

class Path
{
public:
	typedef Vector<text> Pieces;

protected:
	class Parse
	{
		text _vol;
		text _dir;
		Path::Pieces _pieces;

	public:
		Parse(const Path &path);

		text volume() const { return _vol; }
		Path invol() const { return _dir; }
		Filename basename() const;
		Path dirname() const;

		const Path::Pieces pieces() const { return _pieces; }
		text operator[](int i) const { return _pieces[i]; }

		Path compose() const;
	};

	//--------------------------------------------------------------------------------------------

	text _path;
#ifdef _WIN32
	bool _quoted;
#endif
	mutable Parse *_parse;

	Parse &parse() const
	{
		if (!_parse)
			_parse = new Parse(*this);
		return *_parse;
	}

	static text _sep;

public:
	Path();
	Path(const char *p);
	Path(const string &p);
	Path(const Path &path);
	Path(const Pieces &pieces);
	~Path();

	//Path &operator=(const string &p) { _path = p; fix(); /* refresh(); */ return *this; }
	Path &operator=(const Path &path) { _path = path; /* refresh(); */ return *this; }

	void fix();
	Path fixed() const { return Path(str()); }

	const Path &operator*() const { parse(); return *this; }
	Path &operator*() { parse(); return *this; }

protected:
	text _to_win() const;
	text _to_ux() const;

public:
	Path to_win() const { return _to_win(); }
	Path to_ux() const  { return _to_ux(); }

	//--------------------------------------------------------------------------------------------

	bool operator!() const { return !_path; }
	size_t size() const { return _path.length(); }
	operator const text&() const { return _path; }
	const text &str() const { return _path; }

	bool operator==(const Path &p) const { return _path == p._path; }

	//--------------------------------------------------------------------------------------------

	Path volume() const;
	Path dirname() const;
	Filename basename(const text &suffix = "") const;
	Path invol() const { return parse().invol(); }

	Directory dir() const;
	File file() const;

	bool abs() const;

	//--------------------------------------------------------------------------------------------

	Path join(text p) const;
	Path operator/(const text &p) const { return join(p); }
	Path operator+(const text &p) const { return join(p); }
	Path operator-(const Path &p) const;
	Path operator&(const Path &p) const;

	Pieces pieces() const;
	text operator[](int i) const { return pieces()[i]; }

	bool under(const Path &p) const;
	bool startswith(const Path &p) const;

	//--------------------------------------------------------------------------------------------

	Path real() const;
	bool exist() const;

	//--------------------------------------------------------------------------------------------

	friend std::ostream &operator<<(std::ostream &out, const Path &p)
	{
		out << p.str();
		return out;
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////

inline Path operator "" _path(const char *p, size_t n)
{
	return Path(p);
}

///////////////////////////////////////////////////////////////////////////////////////////////

class WildcardPath : public Path
{
public:
	WildcardPath(const Path &path) : Path(path) {}
	WildcardPath(const Directory &dir, const text &wildcard = "*.*");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_files_path_
