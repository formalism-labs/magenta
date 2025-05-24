
////////////////////////////////////////////////////////////////////////////////
// Nested Expressions
////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_text_nexp_atom_
#define _magenta_text_nexp_atom_

#include "magenta/text/nexp/node.h"

#include "magenta/exceptions/general.h"
#include "magenta/types/defs.h"

namespace magenta
{


////////////////////////////////////////////////////////////////////////////////

class Atom : public Node
{
	String _text;
	
public:
	Atom(const String &s, int line = 0) : Node(line), _text(s) {}
	Atom(long n, int line = 0);
	Atom(double n, int line = 0);
	
	bool isList() const { return false; }
	bool isAtom() const { return true; }
	
	operator String() const { return _text; }
	
	void print() const;
};

////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_nexp_atom_

