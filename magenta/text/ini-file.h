
#ifndef _magenta_text_ini_file_
#define _magenta_text_ini_file_

#include "magenta/text/classes.h"
#include "magenta/files/classes.h"

class minIni;

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class IniFile
{
	::minIni *_ini;

public:
	IniFile(const Path &file);
	~IniFile();

	class Section;
	class Value;
	friend Section;
	friend Value;

	class Value
	{
		Section &_section;
		text _name;
		
	public:
		friend Section;
	
		Value(Section &section, const text &name);
		
		Value &operator=(const text &val);
		operator text() const;
		bool operator!() const { return !text(); }
	};

	class Section
	{
		IniFile &_ifile;
		text _name;

	public:
		friend Value;

		Section(IniFile &ifile, const text &name);
		
		const text &name() const { return _name; }
		Value operator[](const text &name) { return Value(*this, name); }
	};

	Section operator[](const text &name) { return Section(*this, name); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_ini_file_
