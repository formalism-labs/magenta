
#include "nexp.h"

#include <stdio.h>

namespace magenta
{
	
///////////////////////////////////////////////////////////////////////////////////////////////
// Nexp
///////////////////////////////////////////////////////////////////////////////////////////////
	
Nodes *Nexp::readList(bool root)
{
	char token[256] = "", *tail = token, c;
	bool read_ch;
	Nodes *nodes = new Nodes, *list_node;
	
	for (scanState state = Scan_Sep;;)
	{
		stream() >> *tail;
		for (read_ch = false; !read_ch;)
			switch (state)
			{
			case Scan_Sep:
				if (stream().isAtEnd())
					state = Scan_End;
				else
					switch (*tail)
					{
					case ')':   state = Scan_End; break;
					case '(':   state = Scan_List; break;
					case '\n':
						read_ch = true;
						state = Scan_NewLine; 
						break;

					default:
						if (isAtomChar(c = *tail))
						{
							tail = token;
							*tail = c;
							state = Scan_Atom;
						}
						else
							read_ch = true;
					}
				break;
			
			case Scan_NewLine:
				_line++;
				if (*tail == '#')
				{
					read_ch = true;
					state = Scan_Remark;
				}
				else
					state = Scan_Sep;
				break;

			case Scan_Remark:
				read_ch = true;
				if (*tail == '\n')
					state = Scan_NewLine;
				break;

			case Scan_Atom:
				if (stream().isAtEnd() || isSepChar(*tail))
				{
					*nodes << new Atom(String(token, tail - token));
					state = Scan_Sep;
					break;
				}
				if (isEscChar(*tail))
					stream() >> *tail;
				tail++;
				read_ch = true;
				break;
				
			case Scan_List:
				if (list_node = readList())
				{
					*nodes << list_node;
					read_ch = true;
					state = Scan_Sep;
				}
				else
					state = Scan_End;
				break;

			case Scan_End:
				if (!root && *tail != ')')
					THROW_EX(SyntaxError)("expected) at end of file");
				return nodes;
			}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Nexp::read(String s)
{
	_filename = s;
	read();
}

//---------------------------------------------------------------------------------------------

void Nexp::read()
{
	if (_filename.isEmpty())
		THROW_EX(Exception);
	
	Stream stream(_filename);
	defStream(stream);
	_line = 1;
	list() = *readList(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Nexp::Stream
///////////////////////////////////////////////////////////////////////////////////////////////

Nexp::Stream::Stream(const String &name)
{
	_file = fopen(name.c_str(), "r");
	if (!_file)
		THROW_EX(OpenError);
	_line = 0;
	_col = 0;
}

//---------------------------------------------------------------------------------------------

Nexp::Stream::~Stream() 
{ 
	_file && fclose(_file); 
}

//---------------------------------------------------------------------------------------------

Nexp::Stream &Nexp::Stream::operator>>(char &c)
{
	c = readChar();
	return *this;
}

//---------------------------------------------------------------------------------------------

char Nexp::Stream::readChar() 
{
	++_col;
	char c = fgetc(_file);
	if (new_line)
	{
		_col = 0;
		++_line;
		while (c == ';')
		{
			char s[256];
			fgets(s, sizeof(s), _file);
			c = fgetc(_file);
		}
	}
	new_line = c == '\n';
	return c;
}

//---------------------------------------------------------------------------------------------

bool Nexp::Stream::isAtEnd()  
{ 
	return !!feof(_file);
}

///////////////////////////////////////////////////////////////////////////////////////////////
	
} // namespace magenta

