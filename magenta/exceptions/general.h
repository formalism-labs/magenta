
#ifndef _maganta_exceptions_general_
#define _maganta_exceptions_general_

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4786)
#if _MSC_VER > 1300
#pragma warning(disable : 4996)
#endif
#endif

#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <string>

#include "magenta/lang/defs.h"
#include "magenta/text/string.h"
#include "magenta/text/text.h"

#if defined(_WIN32)
#include "magenta/system/windows.h"

#elif defined(__linux__)
#include <errno.h>

#elif defined(__VXWORKS__)
#include <errnoLib.h>
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class Exception : public std::exception
{
public:
	class SpecificError
	{
	public:
#if defined(__linux__)
		typedef int ErrorCode;
#elif defined(_WIN32)
		typedef DWORD ErrorCode;
#elif defined(__VXWORKS__)
		typedef int ErrorCode;
#endif

		SpecificError() : has_info(false), _code(0) {}

		bool hasInfo() const { return has_info; }
		ErrorCode code() const { return _code; }
		virtual string text() const;

		bool has_info;
		ErrorCode _code;
	};

	class NoError : public SpecificError
	{
	};

	class OSError : public SpecificError
	{
	public:
		OSError()
		{
			has_info = true;
#if defined(__linux__)
			_code = errno;
#elif defined(_WIN32)
			_code = GetLastError();
#elif defined(__VXWORKS__)
			_code = errnoGet();
#endif
		}

		string text() const;
	};

	class SocketError : public OSError
	{
	public:
		SocketError()
		{
			has_info = true;
#if defined(__linux__)
			_code = errno;
#elif defined(_WIN32)
			_code = WSAGetLastError();
#elif defined(__VXWORKS__)
			_code = errnoGet();
#endif
		}
	};

	//-----------------------------------------------------------------------------------------
public:
	string _file;
	int _line;
	mutable string _title, _report;

	void addDetails(const string &text) const;
	void addDetails(const char *fmt, ...) const;
	void addDetailsArgs(const char *fmt, va_list args) const;

	void addDetails(const SpecificError &e) const;

	void addReport() const;
	void addReport(const Exception &x) const;

protected:
	Exception() {}

public:
	Exception(const char *file, int line, const char *title = "");
	virtual ~Exception() throw () {}

	const string &report() const throw();
	const char *what() const throw() { return report().c_str(); }

	//-----------------------------------------------------------------------------------------
public:
	template <class E, class S>
	class Adapter
	{
	public:
		typedef Adapter<E,S> This;

		E _x;
		S _s;

		Adapter(const E &x, const S &s = Exception::NoError()) : _x(x), _s(s) {}

		This &operator()(const char *fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			_x.addDetailsArgs(fmt, args);
			va_end(args);
			return *this;
		}

		This &operator()(Exception &x)
		{
			_x.addReport(x);
			return *this;
		}

		This &operator()(Exception &x, const char *fmt, ...)
		{
			_x.addReport(x);

			va_list args;
			va_start(args, fmt);
			_x.addDetailsArgs(fmt, args);
			va_end(args);
			return *this;
		}

		operator const E&() const
		{                  
			_x.addDetails(_s);
			_x.addReport();
			return _x;
		}
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////

#define HERE __FILE__, __LINE__

//---------------------------------------------------------------------------------------------

#define THROW_EX(E) \
	throw (const E &) magenta::Exception::Adapter<E, magenta::Exception::NoError>(\
		E(__FILE__, __LINE__), \
		magenta::Exception::NoError())

#define THROW_OS(E) \
	throw (const E &) magenta::Exception::Adapter<E, magenta::Exception::OSError>(\
		E(__FILE__, __LINE__), \
		magenta::Exception::OSError())

#define THROW_SOCK(E) \
	throw (const E &) magenta::Exception::Adapter<E, magenta::Exception::SocketError>(\
		E(__FILE__, __LINE__), \
		magenta::Exception::SocketError())

#define magenta_fail(...)	THROW_EX(Exception)(__VA_ARGS__)
#define faiL(...) 			THROW_EX(Exception)(__VA_ARGS__)

//---------------------------------------------------------------------------------------------

class LastException : public Exception
{
public:
	LastException(const Exception &x) : Exception(x) {}

	struct Proxy
	{
		Proxy &operator=(const Exception &x);
	};
};

//---------------------------------------------------------------------------------------------

#define ERROR_EX(E) \
	_invalidate(), magenta::LastException::Proxy() = (const E &) \
		magenta::Exception::Adapter<E, magenta::Exception::NoError>(\
			E(__FILE__, __LINE__), \
			magenta::Exception::NoError())

#define ERROR_OS(E) \
	_invalidate(), magenta::LastException::Proxy() = (const E &) \
		magenta::Exception::Adapter<E, magenta::Exception::OSError>(\
			E(__FILE__, __LINE__), \
			magenta::Exception::OSError())

#define ERROR_SOCK(E) \
	_invalidate(), magenta::LastException::Proxy() = (const E &) \
		magenta::Exception::Adapter<E, magenta::Exception::SocketError>(\
			E(__FILE__, __LINE__), \
			magenta::Exception::SocketError())

void _invalidate();

///////////////////////////////////////////////////////////////////////////////////////////////

#define EXCEPTION_DEF(name, title) \
	class name : public magenta::Exception \
	{ \
	public: \
		name(const char *file, int line, const char *details = 0) : \
			magenta::Exception(file, line, title) \
		{ if (details) addDetails("%s", details); } \
	}

//---------------------------------------------------------------------------------------------

#define SUB_EXCEPTION_DEF(name, super, title) \
	class name : public super \
	{ \
	public: \
		name(const char *file, int line, const char *details = 0) : \
			super(file, line, oserr, title) \
		{ if (details) addDetails("%s", details); } \
	}

//---------------------------------------------------------------------------------------------

#define CLASS_EXCEPTION_DEF(title) \
	class Error : public magenta::Exception \
	{ \
	public: \
		Error(const char *file, int line, const char *details = 0) : \
			magenta::Exception(file, line, title) \
		{ if (details) addDetails("%s", details); } \
	}

//---------------------------------------------------------------------------------------------

#define CLASS_SUB_EXCEPTION_DEF(name, title) \
	class name : public Error \
	{ \
	public: \
		name(const char *file, int line, const char *details = 0) : \
			Error(file, line, title) \
		{ if (details) Error::addDetails("%s", details); } \
	}

//---------------------------------------------------------------------------------------------

EXCEPTION_DEF(Unimplemented, "Unimplemented feature");

#define UNIMPLEMENTED THROW_EX(Unimplemented)

//#define TOSSER throw (std::exception)
#define TOSSER throw (magenta::Exception)

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif // _magenta_exceptions_general_
