
#ifndef _magenta_types_list_
#define _magenta_types_list_

#include "magenta/types/classes.h"

#include "magenta/exceptions/defs.h"

#include <list>

///////////////////////////////////////////////////////////////////////////////////////////////

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class List : public std::list<T>
{
	typedef List This;
	typedef std::list<T> Super;

public:
	typedef _List_::Iterator<T> Iterator;
	typedef _List_::ConstIterator<T> ConstIterator;

	friend Iterator;
	friend ConstIterator;

	//--------------------------------------------------------------------------
	// Internals
	//--------------------------------------------------------------------------
protected:
	Super &super() { return *this; }
	const Super &super() const { return *this; }

	//--------------------------------------------------------------------------
	// Exceptions
	//--------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::types::2::List");
	CLASS_SUB_EXCEPTION_DEF(EmptyErr, "List is empty");

	//--------------------------------------------------------------------------
	// Navigation
	//--------------------------------------------------------------------------
public:
	Iterator begin() { return Iterator(*this, super().begin()); }
	ConstIterator begin() const { return ConstIterator(*this, super().begin()); }
	
	Iterator end() { return Iterator(*this, super().end()); }
	ConstIterator end() const { return ConstIterator(*this, super().end()); }

	Iterator operator[](int k) { return begin().skip(k); }
	ConstIterator operator[](int k) const { return begin().skip(k); }

	//--------------------------------------------------------------------------
	// Search
	//--------------------------------------------------------------------------
public:		
	template <class K>
	Iterator find(const K &k)
	{
		return begin().find(k);
	}

	template <class K>
	ConstIterator operator()(const K &k) const
	{
		return begin().find(k);
	}

	//--------------------------------------------------------------------------
	// Mutation
	//--------------------------------------------------------------------------
public:
	void prepend(const T &t) { push_front(t); }
	void append(const T &t) { push_back(t); }
	This &operator<<(const T &t) { append(t); return *this; }

	T popFront()
	{
		if (Super::empty())
			THROW_EX(EmptyErr);
		T e = Super::front();
		Super::pop_front();
		return e;
	}
		
	T popBack()
	{
		if (Super::empty())
			THROW_EX(EmptyErr);
		T e = Super::back();
		Super::pop_back();
		return e;
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

namespace _List_
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Iterators
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Iterator
{
	friend List<T>;
	typedef Iterator This;

	//-----------------------------------------------------------------------------------------
	// Internals
	//-----------------------------------------------------------------------------------------
protected:
	List<T> *_list;
	List<T> &list() { return *_list; }
	const List<T> &list() const { return *_list; }

	typedef typename List<T>::Super SuperL;
	
	SuperL &superl() { return _list->super(); }
	const SuperL &superl() const { return _list->super(); }

	typedef typename SuperL::iterator SuperIt;
	SuperIt _iterator;
	SuperIt &superit() { return _iterator; }
	const SuperIt &superit() const { return _iterator; }

	//-----------------------------------------------------------------------------------------
	// Exceptions
	//-----------------------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::types::2::List::Iterator");
	CLASS_SUB_EXCEPTION_DEF(NotFoundError, "not found error");
	
	//-----------------------------------------------------------------------------------------
	// Construction
	//-----------------------------------------------------------------------------------------
protected:
	Iterator(List<T> &list, const SuperIt &i) : _list(&list), _iterator(i) {}
	
public:
	Iterator(List<T> &list) : _list(&list), 
		_iterator(list.super().begin()) {}
		
	Iterator(const Iterator &i) : _list(static_cast<List<T> *>(i._list)), 
		_iterator(i.superit()) {}

	Iterator &operator=(const Iterator &i) 
	{ 
		_list = (List<T> *) &i.list();
		superit() = i.superit(); 
		return *this; 
	}
	
	//-----------------------------------------------------------------------------------------
	// Navigation
	//-----------------------------------------------------------------------------------------
public:
	bool operator!() const 
	{ 
		return superl().end() == superit(); 
	    }

	bool operator==(const Iterator &i) const 
	{ 
		return _list == i._list && _iterator == i._iterator; 
	}
	
	Iterator &operator++() 
	{ 
		++superit(); 
		return *this; 
	}
	
	Iterator operator++(int) 
	{ 
		Iterator i(list(), superit()); 
		superit()++; 
		return i; 
	}

	Iterator &operator--()
	{
		--superit(); 
		return *this; 
	}
	
	Iterator operator--(int) 
	{ 
		Iterator i(list(), superit()); 
		superit()--; 
		return i; 
	}
	
	Iterator skip(int i)
	{
		if (i > 0)
			for (; i && !!*this; --i)
				++*this;
		else
			for (; i && !!*this; ++i)
				--*this;
		return *this;
	}

	Iterator operator+(int k) { return Iterator(*this).skip(k); }
	Iterator &operator+=(int k) { return skip(k); }

	Iterator operator-(int k) { return Iterator(*this).skip(-k); }
	Iterator &operator-=(int k) { return skip(-k); }

	//-----------------------------------------------------------------------------------------
	// Search
	//-----------------------------------------------------------------------------------------
public:
	Iterator operator[](int index)
	{
		Iterator i = *this;
		while (index--)
			++i;
		return i;				
	}

	template <class K>
	Iterator find(const K &k) const
	{
		Iterator j = *this;
		for (; !!j; ++j)
			if (static_cast<const K &>(*j) == k)
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

	//-----------------------------------------------------------------------------------------
	// Access
	//-----------------------------------------------------------------------------------------
public:
	T &operator*() { return *superit(); }
	const T &operator*() const { return *superit(); }

	T *operator->() { return &**this; } 
	T *operator->() const { return &**this; } 
	
	//-----------------------------------------------------------------------------------------
	// Mutation
	//-----------------------------------------------------------------------------------------
public:
	void prepend(const T &data)
	{
		SuperIt i = superit();
		superl().insert(i, data);
	}
	
	void append(const T &data)
	{
		SuperIt i = superit();
		superl().insert(++i, data);
	}
	
	This &operator<<(const T &data) { append(data); return *this; }
	
	void remove(int k = 1)
	{
		Iterator i = *this + k;
		_iterator = superl().erase(superit(), i.superit());
	}

	//-----------------------------------------------------------------------------------------
	// Diagnostics
	//-----------------------------------------------------------------------------------------
public:
	void _dump()
	{
		//printf("%lx %lx %lx : %lx\n", (long) node->prev, (long) node, (long) node->next, (long) node->data);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class ConstIterator
{
	friend List<T>;
	typedef ConstIterator This;

	//-----------------------------------------------------------------------------------------
	// Internals
	//-----------------------------------------------------------------------------------------
protected:
	const List<T> *_list;
	const List<T> &list() const { return *_list; }

	typedef typename List<T>::Super SuperL;
	const SuperL &superl() const { return _list->super(); }

	typedef typename SuperL::const_iterator SuperIt;
	SuperIt _iterator;
	SuperIt &superit() { return _iterator; }
	const SuperIt &superit() const { return _iterator; }

	//-----------------------------------------------------------------------------------------
	// Exceptions
	//-----------------------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::types::2::List::ConstIterator");
	CLASS_SUB_EXCEPTION_DEF(NotFoundError, "not found error");
	
	//-----------------------------------------------------------------------------------------
	// Construction
	//-----------------------------------------------------------------------------------------
protected:
	ConstIterator(const List<T> &list, const SuperIt &i) : _list(&list), _iterator(i) {}
	
public:
	ConstIterator(const List<T> &list) : _list(&list), 
		_iterator(list.super().begin()) {}
		
	ConstIterator(const ConstIterator &i) : _list(&i.list()), 
		_iterator(i.superit()) {}

	ConstIterator(const Iterator<T> &i) : _list(&i.list()), 
		_iterator(i.superit()) {}

	ConstIterator &operator=(const Iterator<T> &i) 
	{ 
		_list = &i.list();
		superit() = i.superit(); 
		return *this; 
	}
	
	ConstIterator &operator=(const ConstIterator &i) 
	{ 
		_list = &i.list();
		superit() = i.superit(); 
		return *this; 
	}
	
	//-----------------------------------------------------------------------------------------
	// Navigation
	//-----------------------------------------------------------------------------------------
public:
	bool operator!() const 
	{ 
		return superl().end() == superit(); 
	}

	bool operator==(const Iterator<T> &i) const 
	{ 
		return _list == i._list && _iterator == i._iterator; 
	}
	
	bool operator==(const ConstIterator &i) const 
	{ 
		return _list == i._list && _iterator == i._iterator; 
	}
	
	ConstIterator &operator++() 
	{ 
		++superit(); 
		return *this; 
	}
	
	ConstIterator operator++(int) 
	{
		ConstIterator i(list(), superit()); 
		superit()++; 
		return i; 
	}

	ConstIterator &operator--()
	{
		--superit(); 
		return *this; 
	}
	
	ConstIterator operator--(int) 
	{ 
		Iterator<T> i(list(), superit()); 
		superit()--; 
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

	ConstIterator operator+(int k) { return ConstIterator(*this).skip(k); }
	ConstIterator &operator+=(int k) { return skip(k); }

	ConstIterator operator-(int k) { return ConstIterator(*this).skip(-k); }
	ConstIterator &operator-=(int k) { return skip(-k); }
	
	//-----------------------------------------------------------------------------------------
	// Search
	//-----------------------------------------------------------------------------------------
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
			if (static_cast<const K &>(*j) == k)
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

	//-----------------------------------------------------------------------------------------
	// Access
	//-----------------------------------------------------------------------------------------
public:
	const T &operator*() const { return *superit(); }
	const T *operator->() const { return &**this; } 
	
	//-----------------------------------------------------------------------------------------
	// Diagnostics
	//-----------------------------------------------------------------------------------------
public:
	void _dump()
	{
		// printf("%lx %lx %lx : %lx\n", (long) node->prev, (long) node, (long) node->next, (long) node->data);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace _List_

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_list_
