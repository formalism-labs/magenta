
#ifndef _magenta_types_ref_vector_
#define _magenta_types_ref_vector_

#include "magenta/types/classes.h"
#include "magenta/types/ref.h"

#include "magenta/exceptions/defs.h"

#include <vector>

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class RefT>
class RefVector : public std::vector<RefT>
{
	typedef RefVector<T> This;
	friend _RefVector_::Iterator<T, RefT>;
	friend _RefVector_::ConstIterator<T, RefT>;

public:
	typedef _RefVector_::Iterator<T, RefT> Iterator;
	typedef _RefVector_::ConstIterator<T, RefT> ConstIterator;

	typedef T Item;

protected:
	typedef std::vector<RefT> Super;

	Super &super() { return *this; }
	const Super &super() const { return *this; }

public:
	T &at(size_t i) { return super()[i]; }
	const T &at(size_t i) const { return super()[i]; }

	T &operator[](size_t i) { return at(i); }
	const T &operator[](size_t i) const { return at(i); }

	RefT ref(size_t i) { return super()[i]; }
	typename RefT::Const ref(size_t i) const { return super()[i]; }


	//-------------------------------------------------------------------------
	// Navigation
	//-------------------------------------------------------------------------
protected:
	T &array(int i) { return (*this)[i]; }
	const T &array(int i) const { return (*this)[i]; }

public:
	Iterator begin() { return Iterator(*this, super().begin()); }
	ConstIterator begin() const { return ConstIterator(*this, super().begin()); }
	
	Iterator end() { return Iterator(*this, super().end()); }
	ConstIterator end() const { return ConstIterator(*this, super().end()); }

	Iterator first() { return Iterator(*this, super().begin()); }
	ConstIterator first() const { return ConstIterator(*this, super().begin()); }

	Iterator last() { return Iterator(*this, super().end() - 1); }
	ConstIterator last() const { return ConstIterator(*this, super().end() - 1); }

	//-------------------------------------------------------------------------
	// Search
	//-------------------------------------------------------------------------
public:
	template <class K>
	Iterator find(const K &k) const	{ return begin().find(k); }

	template <class K>
	Iterator operator()(const K &k) const { return begin().find(k); }

	//-------------------------------------------------------------------------
	// Mutation
	//-------------------------------------------------------------------------
public:
	void append(const RefT &t) { this->push_back(t); }
	RefVector &operator<<(const RefT &t) { append(t); return *this; }

	template <class Compare>
	void sort(const Compare &compare)
	{
		std::sort(super().begin(), super().end(), compare);
	}
	
	void swap(int i, int j)
	{
		RefT x = at(j);
		at(j) = at(i);
		at(i) = x;
	}

	//-------------------------------------------------------------------------
	// Construction
	//-------------------------------------------------------------------------
public:
	RefVector() {}
	RefVector(int n) : vector<RefT>(n) {}
	RefVector(const RefVector &v) : vector<RefT>(v.super()) {}

	virtual ~RefVector() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

namespace _RefVector_
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Iterators
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class RefT>
class Iterator
{
	friend RefVector<T>;
	friend ConstIterator<T>;

	//----------------------------------------------------------------------
	// Internals
	//----------------------------------------------------------------------
protected:
	typedef typename RefVector<T, RefT>::Super SuperV;
	typedef typename SuperV::iterator SuperIt;

	RefVector<T, RefT> *_vector;
	RefVector<T, RefT> &vector() { return *_vector; }
	const RefVector<T, RefT> &vector() const { return *_vector; }

	SuperV &superv() { return _vector->super(); }
	const SuperV &superv() const { return _vector->super(); }

	SuperIt _iterator;
	SuperIt &superit() { return _iterator; }
	const SuperIt &superit() const { return _iterator; }

public:
	operator SuperIt() { return superit(); }

	//----------------------------------------------------------------------
	// Exceptions
	//----------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::types::vector::iterator error");
	CLASS_SUB_EXCEPTION_DEF(NotFoundError, "item not found in collection");
	CLASS_SUB_EXCEPTION_DEF(NullIteratorError, "null iterator dereference");
	
	//----------------------------------------------------------------------
	// Construction
	//----------------------------------------------------------------------
protected:
	Iterator(RefVector<T, RefT> &vector, SuperIt i) : 
		_vector((RefVector<T, RefT> *) &vector), _iterator(i) {}
	
public:
	Iterator(RefVector<T, RefT> &vector) : _vector(&vector), 
		_iterator(vector.super().begin()) {}
		
	Iterator(Iterator &i) : _vector(i._vector), 
		_iterator(i.superit()) {}

	Iterator(Iterator &&i) : _vector(i._vector), 
		_iterator(i.superit()) {}

	Iterator &operator=(Iterator i) 
	{ 
		_vector = &i.vector();
		superit() = i.superit(); 
		return *this; 
	}
	
	//----------------------------------------------------------------------
	// Navigation
	//----------------------------------------------------------------------
public:
	bool operator!() const 
	{ 
		return superv().end() == superit(); 
	}

	bool operator==(const Iterator &i) const 
	{ 
		return _vector == i._vector && _iterator == i._iterator; 
	}
	
	size_t index() const
	{
		return superit() - vector().begin();
	}

	Iterator &operator++() 
	{ 
		++superit(); 
		return *this; 
	}
	
	Iterator operator++(int) 
	{ 
		Iterator i(vector(), superit()); 
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
		Iterator i(vector(), superit()); 
		superit()--; 
		return i; 
	}
	
	Iterator operator+(int n)
	{
		return Iterator(vector(), superit() + n);
	}

	Iterator operator-(int n)
	{
		return Iterator(vector(), superit() - n);
	}

	//----------------------------------------------------------------------
	// Search
	//----------------------------------------------------------------------
public:
	Iterator operator[](int index)
	{
		return superit() + index;				
	}

	template <class K>
	Iterator find(const K &k)
	{
		Iterator j = *this;
		for (; !!j; ++j)
			if (static_cast<const K &>(*j) == k)
				break;
		return j;
	}

	template <class K>
	ConstIterator<T> find(const K &k) const
	{
		ConstIterator<T> j = *this;
		for (; !!j; ++j)
			if (static_cast<const K &>(*j) == k)
				break;
		return j;
	}

	template <class K>
	Iterator operator()(const K &k)
	{
		Iterator i = find(k);
		if (!i)
			THROW_EX(NotFoundError);
		return i;
	}

	template <class K>
	ConstIterator<T> operator()(const K &k) const
	{
		ConstIterator<T> i = find(k);
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
		return *superit(); 
	}

	typename RefT::Const operator*() const 
	{ 
		if (!*this)
			THROW_EX(NullIteratorError);
		return *superit(); 
	}

	//----------------------------------------------------------------------
	// Mutation
	//----------------------------------------------------------------------
public:
	void remove(int k = 1)
	{
		Iterator i = *this + k;
		superv().erase(superit(), i.superit());
		_iterator = i.superit();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class RefT>
class ConstIterator
{
	friend RefVector<T>;

	typedef typename RefT::Const ConstRefT;

	//----------------------------------------------------------------------
	// Internals
	//----------------------------------------------------------------------
protected:
	typedef typename RefVector<T, RefT>::SuperV SuperV;
	typedef typename SuperV::const_iterator SuperIt;

	const RefVector<T, RefT> *_vector;
	const RefVector<T, RefT> &vector() const { return *_vector; }

	const SuperV &superv() const { return _vector->super(); }

	SuperIt _iterator;
	SuperIt &superit() { return _iterator; }
	const SuperIt &superit() const { return _iterator; }

public:
	operator SuperIt() { return superit(); }

	//----------------------------------------------------------------------
	// Exceptions
	//----------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::types::Vector::ConstIterator error");
	CLASS_SUB_EXCEPTION_DEF(NotFoundError, "item not found in collection");
	CLASS_SUB_EXCEPTION_DEF(NullIteratorError, "null iterator dereference");
	
	//----------------------------------------------------------------------
	// Construction
	//----------------------------------------------------------------------
protected:
	ConstIterator(const RefVector<T, RefT> &vector, SuperIt i) : 
		_vector((const RefVector<T, RefT> *) &vector), _iterator(i) {}
	
public:
	ConstIterator(const RefVector<T, RefT> &vector) : _vector(&vector), 
		_iterator(vector.super().begin()) {}
		
	ConstIterator(const ConstIterator &i) : _vector(i._vector), _iterator(i.superit()) {}

	ConstIterator(const Iterator<T, RefT> &i) : _vector(i._vector), _iterator(i.superit()) {}

	ConstIterator &operator=(ConstIterator &i) 
	{ 
		_vector = &i.vector();
		superit() = i.superit(); 
		return *this; 
	}

	ConstIterator &operator=(Iterator<T, RefT> &i) 
	{ 
		_vector = &i.vector();
		superit() = i.superit(); 
		return *this; 
	}
	
	//----------------------------------------------------------------------
	// Navigation
	//----------------------------------------------------------------------
public:
	bool operator!() const 
	{ 
		return superv().end() == superit(); 
	}

	bool operator==(const Iterator<T> &i) const 
	{ 
		return _vector == i._vector && _iterator == i._iterator; 
	}
	
	ConstIterator &operator++() 
	{ 
		++superit(); 
		return *this; 
	}
	
	ConstIterator operator++(int) 
	{ 
		ConstIterator i(vector(), superit()); 
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
		ConstIterator i(vector(), superit()); 
		superit()--; 
		return i; 
	}
	
	ConstIterator operator+(int n)
	{
		return ConstIterator(vector(), superit() + n);
	}

	ConstIterator operator-(int n)
	{
		return ConstIterator(vector(), superit() - n);
	}

	//----------------------------------------------------------------------
	// Search
	//----------------------------------------------------------------------
public:
	ConstIterator operator[](int index)
	{
		return superit() + index;				
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

	//----------------------------------------------------------------------
	// Access
	//----------------------------------------------------------------------
public:
	const T *operator->() const { return **this; } 
	
	ConstRefT operator*() const 
	{ 
		if (!*this)
			THROW_EX(NullIteratorError);
		return *superit();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace _RefVector_

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class PtrVector : public RefVector<T, Ptr<T>>
{
public:
	PtrVector() {}
	PtrVector(int n) : RefVector<T, Ptr<T>>(n) {}
	PtrVector(PtrVector &v) : RefVector<T, Ptr<T>>(v.superv()) {}
};
	
///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_list_
