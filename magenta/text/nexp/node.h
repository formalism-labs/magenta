
////////////////////////////////////////////////////////////////////////////////
// Nested Expressions
////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_text_nexp_node_
#define _magenta_text_nexp_node_

#include "magenta/text/nexp/classes.h"

#include "magenta/exceptions/general.h"
#include "magenta/lang/cxx/defs.h"
#include "magenta/types/defs.h"

namespace magenta
{


////////////////////////////////////////////////////////////////////////////////

class Node
{
public:
	CLASS_EXCEPTION_DEF("magenta::text::nexp::2::Node");
	CLASS_SUB_EXCEPTION_DEF(FormatError, "format error");

	String atom() const { return String(*this); }

	MAGENTA_CXX_CONST_ATTRIBUTE(int, line);

public:
	Node(int line_ = 0) { _line() = line_; }
	Node(const Node &node) { _line() = node.line(); }
	virtual ~Node() {}
	
	bool isAtomChar(char c) const { return c > ' '; }
	bool isSepChar(char c) const { return c <= ' ' || c == '(' || c == ')'; }
	bool isEscChar(char c) const { return c == '\\'; }

	bool operator==(const Node &node) const { return String(*this) == String(node); }

	operator int() const
	{
		long n;
		String s = atom();
		if (!sscanf(s.c_str(), "%ld", &n))
			THROW_EX(FormatError);
		return n;
	}

	operator double() const
	{
		double n;
		String s = atom();
		if (!sscanf(s.c_str(), "%lf", &n))
			THROW_EX(FormatError);
		return n;
	}

	virtual operator String() const { THROW_EX(Unimplemented); }
	virtual Nodes &list() { THROW_EX(Unimplemented); }
	
	virtual bool isList() const { THROW_EX(Unimplemented); }
	virtual bool isAtom() const { THROW_EX(Unimplemented); }
	
	bool operator+() const { return isList(); }

	virtual void print() const { THROW_EX(Unimplemented); }
};

////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_nexp_general_
