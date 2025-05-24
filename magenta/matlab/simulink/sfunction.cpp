
#include "sfunction.h"

#include <stdarg.h>

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// SFunction
///////////////////////////////////////////////////////////////////////////////////////////////

Strings SFunction::_messages;

///////////////////////////////////////////////////////////////////////////////////////////////

#if 0

double SFunction::safeiport(int i, int j) const
{
	if (i < 0 || i >= inputPortsNum())
	{
		error("invalid port number");
		return 0;
	}
	if (j < 0 || j >= ssGetInputPortWidth(S, i))
	{
		error("invalid port signal number");
		return 0;
	}

	return iport(i, j);
}

//---------------------------------------------------------------------------------------------

double &SFunction::safeoport(int i, int j)
{
	if (i < 0 || i >= outputPortsNum())
	{
		error("invalid port number");
		return 0;
	}
	if (j < 0 || j >= ssGetOutputPortWidth(S, i))
	{
		error("invalid port signal number");
		return 0;
	}
	return oport(i, j);
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////

void SFunction::error(const char *fmt, va_list args)
{
	_messages.append(vstringf(fmt, args));
	const char *p = _messages.last()->c_str();
	ssSetErrorStatus(S, p);
}

//---------------------------------------------------------------------------------------------

void SFunction::error(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	error(fmt, args);

	va_end(args);
}

//---------------------------------------------------------------------------------------------

void SFunction::warning(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	warning(fmt, args);

	va_end(args);
}

//---------------------------------------------------------------------------------------------

void SFunction::warning(const char *fmt, va_list args)
{
	_messages.append(vstringf(fmt, args));
	const char *p = _messages.last()->c_str();
	ssWarning(S, p);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void SFunction::setInputPortWidth(int_T port, int_T width)
{
	setIPortWidth(port, width);
}

//---------------------------------------------------------------------------------------------

void SFunction::setOutputPortWidth(int_T port, int_T width)
{
	setOPortWidth(port, width);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

