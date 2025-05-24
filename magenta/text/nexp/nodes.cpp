
#include "nodes.h"

#include <stdio.h>
#include <iostream>

namespace magenta
{

using std::cout;
using std::endl;

///////////////////////////////////////////////////////////////////////////////////////////////
// Nodes
///////////////////////////////////////////////////////////////////////////////////////////////

Nodes::operator String() const
{
	Nodes::Iterator i = begin();
	if (!i || i->isList())
		return String("");
	return String(*i);
}

//---------------------------------------------------------------------------------------------

Nodes::Iterator Nodes::operator()(const String &text)
{
	return begin()(text);
}

//---------------------------------------------------------------------------------------------

Nodes::Iterator Nodes::operator()()
{
	return Iterator(begin()).list();
}

//---------------------------------------------------------------------------------------------

Nodes::Iterator Nodes::operator[](const String &text)
{
	return begin()[text];
}

//---------------------------------------------------------------------------------------------

Nodes::Iterator Nodes::operator[](int index)
{
	return begin()[index];
}

//---------------------------------------------------------------------------------------------

Nodes::Iterator Nodes::operator[](const char *text)
{
	return (*this)[String(text)];
}

//---------------------------------------------------------------------------------------------

Nodes::Iterator Nodes::begin() { return NodesList::begin(); }
Nodes::Iterator Nodes::end() { return NodesList::end(); }
Nodes::Iterator Nodes::begin() const { return NodesList::begin(); }
Nodes::Iterator Nodes::end() const { return NodesList::end(); }

//---------------------------------------------------------------------------------------------

void Nodes::print() const
{
	Iterator(*this).print();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Nodes::Iterator
///////////////////////////////////////////////////////////////////////////////////////////////

namespace _Nodes_
{

///////////////////////////////////////////////////////////////////////////////////////////////

Iterator::Iterator(const RefList<Node>::Iterator &i) : NodesListIterator(i)
{
}

//---------------------------------------------------------------------------------------------
Iterator::Iterator(const Nodes &nodes) : NodesListIterator(nodes.begin())
{
}

//---------------------------------------------------------------------------------------------

Iterator::Iterator(const Iterator &i) : NodesListIterator(i)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

Iterator & Iterator::operator=(const Nodes &nodes)
{
	raw_iterator() = nodes.begin(); 
	return *this;
}

//---------------------------------------------------------------------------------------------

Iterator & Iterator::operator=(const Iterator &i)
{
	raw_iterator() = ((This &) i).raw_iterator(); 
	return *this;
}

//---------------------------------------------------------------------------------------------

/*
Iterator &Iterator::operator=(const NodesList::Iterator &i)
{ 
	raw_iterator() = i; 
	return *this; 
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////

Iterator Iterator::operator()(const String &text)
{
	try
	{
		Iterator i = raw_iterator()(Atom(text));
		if (!i->isList())
			return i;
		Nodes &list = i->list();
		if (list.begin() == list.end())
			i = list.end();
		else
			i = ++list.begin();
		return i;
	}
	catch (NodesList::Iterator::NotFoundError &x)
	{
		THROW_EX(NotFound)(x, "%s", text.c_str());
	}
}

//---------------------------------------------------------------------------------------------

Iterator Iterator::operator()()
{
	return list();
}

///////////////////////////////////////////////////////////////////////////////////////////////

Iterator Iterator::operator[](const String &text)
{
	try
	{
		return raw_iterator()(Atom(text));
	}
	catch (NodesList::Iterator::NotFoundError &x)
	{
		THROW_EX(NotFound)(x, "%s", text.c_str());
	}
}

//---------------------------------------------------------------------------------------------

Iterator Iterator::operator[](int index)
{
	return raw_iterator()[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////

static void indent(int n)
{
	for (int j = 1; j < n; j++)
		cout << '\t';
}

//---------------------------------------------------------------------------------------------

void Iterator::print(int level) const
{
	if (level)
	{
		cout << endl;
		indent(level);
		cout << '(';
	}

	level++;

	int sublist = false;
	int j = 0;
	for (Iterator i = *this; !!i; ++i, j++)
		if (i->isList())
		{
			sublist = true;
			Iterator(i->list()).print(level);
		}
		else
		{
			j && cout << ' ';
			cout << String(*i).c_str();
		}

	if (--level)
	{
		if (sublist)
		{
			cout << endl;
			indent(level);
		}
		cout << ')';
	}
	else
		cout << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace _Nodes_

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

