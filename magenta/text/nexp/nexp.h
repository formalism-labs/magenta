
///////////////////////////////////////////////////////////////////////////////////////////////
// Nested Expressions
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_text_nexp_nestexp_
#define _magenta_text_nexp_nestexp_

#include <stdio.h>

#include "magenta/exceptions/general.h"
#include "magenta/types/string.h"

#include "magenta/text/nexp/nodes.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Nexp : public Nodes
{
	//-----------------------------------------------------------------------------------------
	// Exceptions
	//-----------------------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::text::nexp::Nexp");
	CLASS_SUB_EXCEPTION_DEF(RefError, "reference error");
	CLASS_SUB_EXCEPTION_DEF(SyntaxError, "syntax error");

	//-----------------------------------------------------------------------------------------
	// Stream
	//-----------------------------------------------------------------------------------------
protected:
	class Stream
	{
		FILE *_file;
		bool new_line;
		int _line, _col;

	public:	
		Stream(const String &name);
		~Stream();

		Stream &operator>>(char &c);

		char readChar();
		bool isAtEnd();

		int line() const { return _line; }
		int col() const { return _col; }

		CLASS_EXCEPTION_DEF("magenta::nexp::stream error");
		CLASS_SUB_EXCEPTION_DEF(OpenError, "open error");
	};

	String _filename;
	Stream *_stream;
	Stream &stream() { return *_stream; }
	void defStream(Stream &stream) { _stream = &stream; }
	
	//-----------------------------------------------------------------------------------------
	// Parser
	//-----------------------------------------------------------------------------------------
protected:
	enum scanState
	{
		Scan_Sep,
		Scan_NewLine,
		Scan_Remark,
		Scan_Atom,
		Scan_List,
		Scan_End
	};

	int _line;

	Nodes *readList(bool root = false);

	//-----------------------------------------------------------------------------------------
	// Interface
	//-----------------------------------------------------------------------------------------
protected:
	Nodes &list() { return *this; }
	
public:
	Nexp() {}
	Nexp(const String &s) : _filename(s) { read(); }
	
	void read(String s);
	void read();
	//	void write(const string &s);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_nexp_nestexp_

