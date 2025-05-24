
#include "magenta/text/defs.h"
#include "magenta/files/defs.h"

#include "minIni.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

IniFile::IniFile(const Path &file) : _ini(new minIni(+file)) {}

IniFile::~IniFile()
{
	delete _ini;
}

//---------------------------------------------------------------------------------------------

IniFile::Section::Section(IniFile &ifile, const text &name) : _ifile(ifile), _name(name) {}

//---------------------------------------------------------------------------------------------

IniFile::Value::Value(Section &section, const text &name) : _section(section), _name(name) {}

//---------------------------------------------------------------------------------------------

IniFile::Value::operator text() const
{
	return _section._ifile._ini->gets(_section.name(), _name);
}

//---------------------------------------------------------------------------------------------

IniFile::Value &IniFile::Value::operator=(const text &val)
{
	_section._ifile._ini->put(_section.name(), _name, val);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta  