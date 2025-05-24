
///////////////////////////////////////////////////////////////////////////////
// Nested Expressions: Nodes
///////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_text_nexp_nodes_
#define _magenta_text_nexp_nodes_

#include <stdio.h>
/*
#ifdef _WIN32
#include <iostream>
#else
#include <iostream.h>
#endif
*/
#include "magenta/exceptions/general.h"
#include "magenta/types/defs.h"

#include "magenta/text/nexp/node.h"
#include "magenta/text/nexp/atom.h"

namespace magenta
{



///////////////////////////////////////////////////////////////////////////////

typedef RefList<Node> NodesList;

EXCEPTION_DEF(NotFound, "nexp element does not exist");

//-----------------------------------------------------------------------------

class Nodes : public Node, public NodesList
{
	friend class Nexp;
//	friend class Iterator;
	
public:
	typedef _Nodes_::Iterator Iterator;

public:
	Nodes(int line = 0) : Node(line) {}
	Nodes(Nodes &list, int line) : Node(line), NodesList(list) {}
	~Nodes() {}
	
	bool isList() const { return true; }
	bool isAtom() const { return false; }
	
	Nodes &list() { return *this; }
	
	Iterator operator()(const String &text);
//	Iterator operator()(int index);
	Iterator operator()();

	Iterator operator[](const String &text);
	Iterator operator[](const char *text);
	Iterator operator[](int index);

	Iterator begin();
	Iterator end();
	Iterator begin() const;
	Iterator end() const;
						
	operator String() const;
	
	void print() const;
};

///////////////////////////////////////////////////////////////////////////////

namespace _Nodes_
{


typedef NodesList::Iterator NodesListIterator;

//-----------------------------------------------------------------------------

class Iterator : public NodesListIterator, public Node
{
	typedef Iterator This;

	friend Nodes;

public:
	CLASS_EXCEPTION_DEF("magenta::text::nexp::2::Nodes::Iterator");
	CLASS_SUB_EXCEPTION_DEF(NullIteratorError, "error trying to access a null iterator");

protected:
	NodesList::Iterator &raw_iterator() { return *this; }
	const NodesList::Iterator &raw_iterator() const { return *this; }

	Node &node() 
	{ 
		if (!raw_iterator())
			THROW_EX(NullIteratorError);
		return **raw_iterator(); 
	}

	const Node &node() const 
	{ 
		if (!raw_iterator())
			THROW_EX(NullIteratorError);
		return **raw_iterator(); 
	}

protected:

public:
	Iterator(const NodesListIterator &i);
	Iterator(const Nodes &nodes);
	Iterator(const Iterator &i);

	This &operator=(const Nodes &nodes);
	This &operator=(const Iterator &i);
//	This &operator=(const NodesList::Iterator &i);
		
	virtual bool isList() const { return node().isList(); }
	virtual bool isAtom() const { return node().isAtom(); }

	virtual operator String() const { return node(); }
	virtual Nodes &list() { return node().list(); }

	Node &operator*() { return node(); }
	const Node &operator*() const { return node(); }

	This operator()(const String &text);
	This operator()();

	This operator[](const String &text);
	This operator[](const char *text) { return (*this)[String(text)]; }
	This operator[](int index);

	This &operator++() { ++raw_iterator(); return *this; }
	This operator++(int) { This i = raw_iterator()++; return i; }
	This &operator--() { --raw_iterator(); return *this; }
	This operator--(int) { This i = raw_iterator()--; return i; }

	void print() const { print(0); }
	void print(int level) const;
	
	bool operator==(const Iterator &iterator) { return raw_iterator() == iterator; }
};

//-----------------------------------------------------------------------------

} // namespace _Nodes_

///////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_nexp_nodes_
