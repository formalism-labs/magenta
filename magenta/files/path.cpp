
#include "path.h"

#include "exceptions/defs.h"
#include "types/defs.h"

#include "files/file.h"
#include "files/directory.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Construction
///////////////////////////////////////////////////////////////////////////////////////////////

text Path::_sep = 
#ifdef _WIN32
	"\\";
#else
	"/";
#endif

//---------------------------------------------------------------------------------------------

Path::Path() : _parse(0)
#ifdef _WIN32
	, _quoted(false)
#endif
{
}

//---------------------------------------------------------------------------------------------

Path::Path(const char *p) : _path(p), _parse(0)
#ifdef _WIN32
	, _quoted(false)
#endif
{
	fix();
}

//---------------------------------------------------------------------------------------------

Path::Path(const string &p) : _path(p), _parse(0)
#ifdef _WIN32
	, _quoted(false)
#endif
{
	fix();
}

//---------------------------------------------------------------------------------------------

Path::Path(const Path &path) : _parse(path._parse ? new Parse(*path._parse) : 0)
#ifdef _WIN32
	, _quoted(path._quoted)
#endif
{
	_path = path;
}

//---------------------------------------------------------------------------------------------

Path::Path(const Pieces &pieces) : _parse(0)
{
	if (!pieces)
		return;
	Pieces::ConstIterator i = pieces;
	_path = *i;
	for (++i; !!i; ++i)
		_path += _sep + *i;
	fix();
}

//---------------------------------------------------------------------------------------------

Path::~Path()
{
	delete _parse;
}

//---------------------------------------------------------------------------------------------

void Path::fix()
{
#ifdef _WIN32
	text::regex::Match m;
	_quoted = _path.match("\"(.*)\"", m);
	if (_quoted)
		_path = m[1];
	_path = _to_win();
#else
	_path = _to_ux();
#endif
}

//---------------------------------------------------------------------------------------------

text Path::_to_win() const
{
	text s = _path;
	return s.replace("/", "\\");
}

//---------------------------------------------------------------------------------------------

text Path::_to_ux() const
{
	text s = _path;
	return s.replace("\\", "/");
}

//---------------------------------------------------------------------------------------------

Path Path::join(text p) const
{
	if (!p)
		return *this;

	// prefix of / or \ will cause orignal path to be discarded
	text::regex::Match m;
	if (p.match(R"(^([/\\]*)(.*))", m))
		p = m[2];

#ifdef _WIN32
	if (_path[-1] == ":")
		return _path + "/" + p;
	else if (_path(-2, 2) == ":/" || _path(-2, 2) == ":\\" || _path == "/" || _path == "\\")
		return _path + p;
	else
		return _path + "/" + p;
	// Pathname.fix(s[-1] == ':' ? s + "/" + x : s + "/" + x)
#else
	return _path + "/" + p;
#endif
}

//---------------------------------------------------------------------------------------------

bool Path::abs() const
{
	text d = invol();
	return !!volume() || invol().str()[0] == _sep;
}

//---------------------------------------------------------------------------------------------

bool Path::under(const Path &p) const
{
	text s = *this - p;
	return !!s;
}

//---------------------------------------------------------------------------------------------

/*
/a/b/c - a/b/c
a/b/c - /a/b/c

/a/b/c             => [/, a, b, c]
v:/a/b/c           => [v: , a, b, c]
a/b/c              => [a, b, c]
//host/share/a/b/c => [//host/share, a, b, c]
*/

Path Path::operator-(const Path &p) const
{
	if (abs() != p.abs())
		return Path();

	const Pieces &p1 = pieces(), &p2 = p.pieces();
	Pieces common;
	auto n = min(p1.size(), p2.size());
	size_t i;
#ifdef _WIN32
	for (i = 0; i < n && p1[i].ieq(p2[i]); ++i) ;
#else
	for (i = 0; i < n && p1[i] == p2[i]; ++i) ;
#endif
	if (p2.size() > i )
		return Path();
	for (; i < p1.size(); ++i)
		common << p1[i];
	return Path(common);
}

//---------------------------------------------------------------------------------------------

Path::Pieces Path::pieces() const
{
	return parse().pieces();
}

///////////////////////////////////////////////////////////////////////////////////////////////

Filename Path::basename(const text &suffix) const
{
	return parse().basename();
}

//---------------------------------------------------------------------------------------------

Path Path::dirname() const
{
	return parse().dirname();
}

//---------------------------------------------------------------------------------------------

Path Path::volume() const
{
	return parse().volume();
}

//---------------------------------------------------------------------------------------------

File Path::file() const
{
	return File(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Filesystem
///////////////////////////////////////////////////////////////////////////////////////////////

Path Path::real() const
{
#ifdef _WIN32
	char *p = _fullpath(0, +_path, 0);
#else
	char *p = realpath(+_path, 0);
#endif
	if (!p)
		THROW_EX(Exception)("cannot determine real path for '%s' at '%s'", +_path, +CurrentDirectory().path());
	Path real(p);
	free(p);
	return real;
}

//---------------------------------------------------------------------------------------------

bool Path::exist() const
{
	return File(*this).exist();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Path::Parse
///////////////////////////////////////////////////////////////////////////////////////////////

Path::Parse::Parse(const Path &path)
{
#ifdef _WIN32
	text::regex::Match m;
	text p = path;
	if (p.match("^([[:alpha:]]:)(.*)", m))
	{
		_vol = m[1];
		_dir = m[2];
		if (!!_dir && _dir[0] != Path::_sep)
		{
			// optionally, throw exception here as these are source of touble
			_dir = Path::_sep + _dir;
		}
		_pieces << _vol;
	}
	else if (p.match(R"(^(\\\\\w+\\\w+)(.*))", m))
	{
		_vol = m[1];
		_dir = m[2];
		_pieces << _vol;
	}
	else if (p.match(R"(^\\\\\\+)", m))
		THROW_EX(Exception)("invalid volume spec: %s", +p);
	else if (p.match(R"(^\\\\\w+)", m))
		THROW_EX(Exception)("invalid volume spec: %s", +p);
	else if (p.match(R"(^\\\\\w+\\\\+)", m))
		THROW_EX(Exception)("invalid volume spec: %s", +p);
	else
	{
		_dir = p;
	}

	if (!!_vol && !_dir)
		_dir = Path::_sep;

	_pieces << _dir.osplit(R"([\\/]+)");

#else
#endif
}

//---------------------------------------------------------------------------------------------

Path Path::Parse::compose() const
{
	return magenta::join(_pieces, _sep);
}

//---------------------------------------------------------------------------------------------

Filename Path::Parse::basename() const
{
	return _pieces[-1];
}

//---------------------------------------------------------------------------------------------

Path Path::Parse::dirname() const
{
	return magenta::join(_pieces(0, -2), _sep);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// WildcardPath
///////////////////////////////////////////////////////////////////////////////////////////////

WildcardPath::WildcardPath(const Directory &dir, const text &wildcard) :
	Path(dir.subpath(wildcard))
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
