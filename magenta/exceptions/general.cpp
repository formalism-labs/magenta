
#include "exceptions/general.h"

#if defined(_WIN32)

#elif defined(__linux__)
#include <string.h>

#elif defined(__VXWORKS__)
#endif

namespace magenta
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

Exception::Exception(const char *file, int line, const char *title) :
	_title(title ? title : ""),
	_file(file),
	_line(line)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

const string &Exception::report() const throw()
{
	try
	{
		if (_report.empty())
			addReport();
	}
	catch (...) {}
	return _report;
}

//---------------------------------------------------------------------------------------------

void Exception::addDetails(const string &text) const
{ 
	if (_title.empty())
		_title += text;
	else
		_title += ", " + text;
}

//---------------------------------------------------------------------------------------------

void Exception::addDetails(const char *fmt, ...) const
{
	if (!fmt)
		return;

	va_list args;
	va_start(args, fmt);
	addDetailsArgs(fmt, args);
	va_end(args);
}

//---------------------------------------------------------------------------------------------

void Exception::addDetailsArgs(const char *fmt, va_list args) const
{
	if (!fmt)
		return;

	char s[1024];
	vsprintf(s, fmt, args);
	addDetails(string(s));
}

//---------------------------------------------------------------------------------------------

void Exception::addDetails(const SpecificError &s) const
{
	if (s.hasInfo())
		addDetails(s.text());
}

//---------------------------------------------------------------------------------------------

void Exception::addReport() const
{
#ifdef _DEBUG
	_report += stringf("=> at (%s, %d): %s\n", _file.c_str(), _line, _title.empty() ? "exception" : _title.c_str());
#else
	_report += stringf("=> %s\n", _title.empty() ? "exception" : _title.c_str());
#endif
}

//---------------------------------------------------------------------------------------------

void Exception::addReport(const Exception &x) const
{
	_report += x.report();
}

//---------------------------------------------------------------------------------------------

string Exception::SpecificError::text() const
{
	return stringf("error code %d", _code);
}

///////////////////////////////////////////////////////////////////////////////////////////////

std::string Exception::OSError::text() const
{
#if defined(__linux__)
	char buf[256] = "";
	// it appears that strerror_r does not use buf (!)
	char *err = strerror_r(code(), buf, sizeof(buf));
	return stringf("os error: '%s' (%ld)", err, code());

#elif defined(_WIN32)
	char *msg;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, code(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
	if (msg)
	{
		auto m = strlen(msg);
		if (m > 1 && (msg[m-2] == '\r' || msg[m-2] == '\n'))
			msg[m-2]='\0';
		string err = stringf("os error: '%s' (%lu)", msg, code());
		LocalFree(msg);
		return err;
	}
	else
		return stringf("os error: %lu", code());
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

#if 0 // !defined(DEBUG) && !defined(_DEBUG)

//---------------------------------------------------------------------------------------------

__thread char __lastException_storage[sizeof(LastException)];
__thread LastException *__lastException = 0;

//---------------------------------------------------------------------------------------------

LastException::Proxy &LastException::Proxy::operator=(const Exception &x)
{
	if (!__lastException)
		__lastException = reinterpret_cast<LastException*>(&__lastException_storage);
	else
		__lastException->~LastException();
	new (__lastException) LastException(x);

	L0G(debug4, "%s" , __lastException->what());

	return *this;
}

//---------------------------------------------------------------------------------------------

#endif // defined(DEBUG) || defined(_DEBUG)

//---------------------------------------------------------------------------------------------

void _invalidate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
