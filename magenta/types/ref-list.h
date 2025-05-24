
#ifndef _magenta_types_ref_list_
#define _magenta_types_ref_list_

#include "magenta/types/classes.h"
#include "magenta/types/ref.h"

#include "magenta/exceptions/defs.h"

#include <list>

///////////////////////////////////////////////////////////////////////////////////////////////

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class RefT = Ref<T> >
class RefList : public std::list<RefT>
{
public:
	typedef _RefList_::Iterator<T, RefT> Iterator;
	typedef _RefList_::ConstIterator<T, RefT> ConstIterator;

	friend Iterator;
	friend ConstIterator;

	//--------------------------------------------------------------------------
	// Internals
	//--------------------------------------------------------------------------
protected:
	typedef RefList This;
	typedef std::list<RefT> STLList;

	STLList &stlList() { return *this; }
	const STLList &stlList() const { return *this; }

	//--------------------------------------------------------------------------
	// Exceptions
	//--------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::types::2::RefList");

	//--------------------------------------------------------------------------
	// Navigation
	//--------------------------------------------------------------------------
public:
	Iterator begin() { return Iterator(*this, stlList().begin()); }
	Iterator begin() const { return ((This *) this)->begin(); }
	
	Iterator end() { return Iterator(*this, stlList().end()); }
	Iterator end() const { return ((This *) this)->end(); }

	//--------------------------------------------------------------------------
	// Search
	//--------------------------------------------------------------------------
public:		
	template <class K>
	Iterator find(const K &k) const
	{
		return begin().find(k);
	}

	template <class K>
	Iterator operator()(const K &k) const
	{
		return begin().find(k);
	}

	//--------------------------------------------------------------------------
	// Mutation
	//--------------------------------------------------------------------------
public:
	void prepend(const RefT &t) { push_front(t); }
	void append(const RefT &t) { push_back(t); }
	This &operator<<(const RefT &t) { append(t); return *this; }

	void erase(Iterator &i)
	{
		stlList().erase(i.stlIterator());
	}
		
	//--------------------------------------------------------------------------
	// Diagnostics
	//--------------------------------------------------------------------------
public:
	void print() const
	{
		printf("(");
		for (Iterator i = begin(); !!i; ++i)
			printf("%s ", string(*i).c_str());
		printf(")\n");
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

namespace _RefList_
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Iterators
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class RefT = Ref<T> >
class Iterator
{
	friend RefList<T, RefT>;
	friend ConstIterator<T, RefT>;
	
	typedef Iterator This;

	//----------------------------------------------------------------------
	// Internals
	//----------------------------------------------------------------------
protected:
	typedef RefList<T, RefT> _List;
	_List *_list;
	_List &list() { return *_list; }
	const _List &list() const { return *_list; }

	typedef typename _List::STLList STLList;
	STLList &stlList() { return _list->stlList(); }
	const STLList &stlList() const { return _list->stlList(); }

	typedef typename STLList::iterator STLIterator;
	STLIterator _iterator;
	STLIterator &stlIterator() { return _iterator; }
	const STLIterator &stlIterator() const { return _iterator; }

	//----------------------------------------------------------------------
	// Exceptions
	//----------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::types::2::RefList::Iterator");
	CLASS_SUB_EXCEPTION_DEF(NotFoundError, "not found error");
	CLASS_SUB_EXCEPTION_DEF(NullIteratorError, "null iterator error");
	
	//----------------------------------------------------------------------
	// Construction
	//----------------------------------------------------------------------
protected:
	Iterator(_List &list, const STLIterator &i) :  _list(&list), _iterator(i) {}
	
public:
	Iterator(_List &list) : _list(&list), _iterator(list.stlList().begin()) {}
		
	Iterator(const Iterator<T, RefT> &i) : _list(static_cast<_List *>(i._list)), 
		_iterator(i.stlIterator()) {}

	Iterator &operator=(const Iterator<T, RefT> &i) 
	{ 
		_list = (_List *) &i.list();
		stlIterator() = i.stlIterator(); 
		return *this; 
	}
	
	//----------------------------------------------------------------------
	// Navigation
	//----------------------------------------------------------------------
public:
	bool operator!() const 
	{ 
		return stlList().end() == stlIterator(); 
	    }

	bool operator==(const Iterator &i) const 
	{ 
		return _list == i._list && _iterator == i._iterator; 
	}
	
	Iterator &operator++() 
	{ 
		++stlIterator(); 
		return *this; 
	}
	
	Iterator operator++(int) 
	{ 
		Iterator i(list(), stlIterator()); 
		stlIterator()++; 
		return i; 
	}

	Iterator &operator--()
	{
		--stlIterator(); 
		return *this; 
	}
	
	Iterator operator--(int) 
	{ 
		Iterator i(list(), stlIterator()); 
		stlIterator()--; 
		return i; 
	}
	
	Iterator skip(int i)
	{
		if (i > 0)
			for (; i && !!*this; --i)
				++*this;
		else
			for (; i && stlIterator() != stlList().begin(); ++i)
				--*this;
		return *this;
	}
	
	Iterator operator[](int index)
	{
		Iterator i = *this;
		return i.skip(index);
	}

	//----------------------------------------------------------------------
	// Search
	//----------------------------------------------------------------------
public:
	template <class K>
	Iterator find(const K &k) const
	{
		Iterator j = *this;
		for (; !!j; ++j)
			if (static_cast<const K &>(**j) == k)
				break;
		return j;
	}

	template <class K>
	Iterator operator()(const K &k) const
	{
		Iterator i = find(k);
		if (!i)
			THROW_EX(NotFoundError);
		return i;
	}

	//----------------------------------------------------------------------
	// Access
	//----------------------------------------------------------------------
public:
	T *operator->() { return **this; } 
	const T *operator->() const { return **this; } 
	
	RefT &operator*() 
	{ 
		if (!*this)
			THROW_EX(NullIteratorError);
		return *stlIterator(); 
	}

	typename RefT::Const operator*() const 
	{ 
		if (!*this)
			THROW_EX(NullIteratorError);
		return *stlIterator(); 
	}
	
	//----------------------------------------------------------------------
	// Mutation
	//----------------------------------------------------------------------
public:
	void prepend(const RefT &data)
	{
		STLIterator i = stlIterator();
		stlList().insert(i, data);
	}
	
	void append(const RefT &data)
	{
		STLIterator i = stlIterator();
		stlList().insert(++i, data);
	}
	
	This &operator<<(const RefT &data) { append(data); return *this; }
	
	void remove(int k)
	{
		Iterator i = *this + k;
		stlList().erase(stlIterator(), i.stlIterator());
		_iterator = i.stlIterator();
	}

	//----------------------------------------------------------------------
	// Diagnostics
	//----------------------------------------------------------------------
public:
	void _dump()
	{
		printf("%lx %lx %lx : %lx\n", (long) node->prev, (long) node, 
			(long) node->next, (long) node->data);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class RefT = Ref<T> >
class ConstIterator
{
	friend RefList<T, RefT>;
	typedef ConstIterator This;
	typedef typename RefT::Const ConstRefT;

	//----------------------------------------------------------------------
	// Internals
	//----------------------------------------------------------------------
protected:
	typedef RefList<T, RefT> _List;

	const _List *_list;
	const _List &list() const { return *_list; }

	typedef typename _List::STLList STLList;
	const STLList &stlList() const { return _list->stlList(); }

	typedef typename STLList::const_iterator STLIterator;
	STLIterator _iterator;
	STLIterator &stlIterator() { return _iterator; }
	const STLIterator &stlIterator() const { return _iterator; }

	//----------------------------------------------------------------------
	// Exceptions
	//----------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::types::2::RefList::ConstIterator");
	CLASS_SUB_EXCEPTION_DEF(NotFoundError, "not found error");
	CLASS_SUB_EXCEPTION_DEF(NullIteratorError, "null iterator error");
	
	//----------------------------------------------------------------------
	// Construction
	//----------------------------------------------------------------------
protected:
	ConstIterator(const _List &list, const STLIterator &i) : 
		 _list(&list), _iterator(i) {}
	
public:
	ConstIterator(const _List &list) : _list(&list), 
		_iterator(list.stlList().begin()) {}
		
	ConstIterator(const ConstIterator &i) : _list(&i.list()), 
		_iterator(i.stlIterator()) {}

	ConstIterator(const Iterator<T, RefT> &i) : _list(&i.list()), 
		_iterator(i.stlIterator()) {}

	ConstIterator &operator=(const Iterator<T> &i) 
	{ 
		_list = &i.list();
		stlIterator() = i.stlIterator(); 
		return *this; 
	}
	
	ConstIterator &operator=(const ConstIterator &i) 
	{ 
		_list = &i.list();
		stlIterator() = i.stlIterator(); 
		return *this; 
	}
	
	//----------------------------------------------------------------------
	// Navigation
	//----------------------------------------------------------------------
public:
	bool operator!() const 
	{ 
		return stlList().end() == stlIterator(); 
	}

	bool operator==(const Iterator<T, RefT> &i) const 
	{ 
		return _list == i._list && _iterator == i._iterator; 
	}
	
	bool operator==(const ConstIterator &i) const 
	{ 
		return _list == i._list && _iterator == i._iterator; 
	}
	
	ConstIterator &operator++() 
	{ 
		++stlIterator(); 
		return *this; 
	}
	
	ConstIterator operator++(int) 
	{
		ConstIterator i(list(), stlIterator()); 
		stlIterator()++; 
		return i; 
	}

	ConstIterator &operator--()
	{
		--stlIterator(); 
		return *this; 
	}
	
	ConstIterator operator--(int) 
	{ 
		Iterator i(list(), stlIterator()); 
		stlIterator()--; 
		return i; 
	}
	
	ConstIterator skip(int i)
	{
		if (i > 0)
			for (; i && !!*this; --i)
				++*this;
		else
			for (; i && !!*this; ++i)
				--*this;
		return *this;
	}
	
	//----------------------------------------------------------------------
	// Search
	//----------------------------------------------------------------------
public:
	ConstIterator operator[](int index)
	{
		ConstIterator i = *this;
		while (index--)
			++i;
		return i;				
	}

	template <class K>
	ConstIterator find(const K &k) const
	{
		ConstIterator j = *this;
		for (; !!j; ++j)
			if (static_cast<const K &>(**j) == k)
				break;
		return j;
	}

	template <class K>
	ConstIterator operator()(const K &k) const
	{
		ConstIterator i = find(k);
		if (!i)
			THROW_EX(NotFoundError);
		return i;
	}

	//----------------------------------------------------------------------
	// Access
	//----------------------------------------------------------------------
public:
	const T *operator->() const { return **this; } 
	
	ConstRefT operator*() const 
	{ 
		if (!*stlIterator())
			THROW_EX(NullIteratorError);
		return *stlIterator(); 
	}
	
	//----------------------------------------------------------------------
	// Diagnostics
	//----------------------------------------------------------------------
public:
	void _dump()
	{
		printf("%lx %lx %lx : %lx\n", (long) node->prev, (long) node, 
			(long) node->next, (long) node->data);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace _RefList_

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class PtrList : public RefList< T, Ptr<T> >
{
public:
	PtrList() {}
	PtrList(int n) : PtrList< T, Ptr<T> >(n) {}
	PtrList(PtrList &v) : PtrList< T, Ptr<T> >(v.stlVector()) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_ref_list_
