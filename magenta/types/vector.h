
#ifndef _magenta_types_vector_
#define _magenta_types_vector_

#include "magenta/types/classes.h"

#include "magenta/exceptions/defs.h"
#include "magenta/lang/defs.h"

#include <vector>
#include <initializer_list>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////

namespace magenta
{

using std::vector;

template <class>
struct mem_type;

template <class C, class T>
struct mem_type<T C::*>
{
	typedef T type;
};

template <class T>
struct lambda_func_type 
{
	typedef typename mem_type<decltype(&T::operator())>::type type;
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Vector : public std::vector<T>
{
	typedef Vector<T> This;

	friend _Vector_::Iterator<T>;
	friend _Vector_::ConstIterator<T>;

public:
	typedef _Vector_::Iterator<T> Iterator;
	typedef _Vector_::ConstIterator<T> ConstIterator;

	typedef T Item;

protected:
	typedef std::vector<T> Super;

	Super &super() { return *this; }
	const Super &super() const { return *this; }

public:
	T &at(size_t i) { return super()[i]; }
	const T &at(size_t i) const { return super()[i]; }

	bool operator!() const { return size() == 0; }
	
	size_t size() const { return Super::size(); }
	void resize(size_t n) { Super::resize(n); }

	//-------------------------------------------------------------------------
	// Navigation
	//-------------------------------------------------------------------------
protected:
	T &array(size_t i) { return (*this)[i]; }
	const T &array(size_t i) const { return (*this)[i]; }

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
	Iterator find(const K &k) { return begin().find(k); }

//	template <class K>
//	Iterator operator()(const K &k) { return find(k); }

	template <class K>
	ConstIterator find(const K &k) const { return begin().find(k); }

//	template <class K>
//	ConstIterator operator()(const K &k) const { return find(k); }

	//-------------------------------------------------------------------------
	// Ranges
	//-------------------------------------------------------------------------
public:
	Vector<T> operator()(size_t i, size_t j) const { return (*this)[range(i, j)]; }

	T &operator[](size_t i)
	{
		if (i < 0)
			i += size();
		return super()[i];
	}
	const T &operator[](size_t i) const
	{
		if (i < 0)
			i += size();
		return super()[i];
	}

	Vector<T> operator[](const range &r) const
	{
		auto rr = r.real(size());
		if (!rr)
			return Vector<T>();
		return Vector<T>(begin() + rr.min, begin() + rr.max + 1); 
	}

	Vector<T> operator[](const span &s) const { return Vector<T>(begin() + s.at, begin() + (s.at + s.size)); }

	//-------------------------------------------------------------------------
	// Mutation
	//-------------------------------------------------------------------------
public:
	void append(const T &t) { super().push_back(t); }
	Vector &operator<<(const T &t) { append(t); return *this; }

	void append(const This &v)
	{
		foreach (i, v.begin())
			append(*i);
	}

	Vector &operator<<(const This &v) { append(v); return *this; }

	template <class Compare>
	void sort(const Compare &compare)
	{
		std::sort(super().begin(), super().end(), compare);
	}
	
	void swap(size_t i, size_t j)
	{
		T x = at(j);
		at(j) = at(i);
		at(i) = x;
	}

	//----------------------------------------------------------------------
	// Map/reduce
	//----------------------------------------------------------------------
public:
	template <typename F>
	Vector<typename lambda<F>::rtype> map(F f)
	{
		Vector<typename lambda<F>::rtype> v;
		for (ConstIterator i = begin(); !!i; ++i)
			v << f(*i);
		return v;
	}

	template <typename F>
	typename lambda<F>::rtype reduce(F f, typename lambda<F>::rtype s0 = {})
	{
		auto s = s0;
		for (ConstIterator i = begin(); !!i; ++i)
			s = f(s, *i);
		return s;
	}

	//-------------------------------------------------------------------------
	// I/O
	//-------------------------------------------------------------------------
public:
	void read(std::istream &in);
	void write(std::ostream &out) const { print(out); }

	void print(std::ostream &out = std::cout) const
	{
		out << "[";
		for (ConstIterator i = begin(); !!i; ++i)
			out << (i == begin() ? "" : "\t") << *i;
		out << "]";
	}

	void println(std::ostream &out = std::cout) const
	{
		print(out);
		out << std::endl;
	}

	friend std::ostream &operator<<(std::ostream &out, const This &v)
	{
		v.write(out);
		return out;
	}

	//-------------------------------------------------------------------------
	// Construction
	//-------------------------------------------------------------------------
public:
	Vector() {}
	Vector(size_t n) : vector<T>(n) {}
	Vector(size_t n, const T &init ) { super().assign(n, init); }
	Vector(size_t n, const T *w) : vector<T>(n) { assign(n, w); }
	Vector(const ConstIterator &first, const ConstIterator &last) : 
		vector<T>(first.superit(), last.superit()) {}
	Vector(std::initializer_list<T> li) : vector<T>(li) {}

	Vector(const Vector &v) : vector<T>(v.super()) {}
	Vector(const vector<T> &v) : vector<T>(v) {}

	virtual ~Vector() {}

	void assign(size_t n, const T *w)
	{
		resize(n);
		for (size_t i = 0; i < n; ++i)
			at(i) = w[i];
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

namespace _Vector_
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Iterators
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Iterator
{
	friend Vector<T>;
	friend ConstIterator<T>;

	//----------------------------------------------------------------------
	// Internals
	//----------------------------------------------------------------------
protected:
	typedef typename Vector<T>::Super SuperV;
	typedef typename SuperV::iterator SuperIt;

	Vector<T> *_vector;
	Vector<T> &vector() { return *_vector; }
	const Vector<T> &vector() const { return *_vector; }

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
	Iterator(Vector<T> &vector, SuperIt i) : 
		_vector((Vector<T> *) &vector), _iterator(i) {}
	
public:
	Iterator(Vector<T> &vector) : _vector(&vector), 
		_iterator(vector.super().begin()) {}
		
	Iterator(Iterator &i) : _vector(i._vector), 
		_iterator(i.superit()) {}

	Iterator &operator=(Iterator i) 
	{ 
		_vector = (Vector<T> *) &i.vector();
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
	
	bool operator==(const ConstIterator<T> &i) const 
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
	
	Iterator operator+(size_t n)
	{
		return Iterator(vector(), superit() + n);
	}

	Iterator operator-(size_t n)
	{
		return Iterator(vector(), superit() - n);
	}

	//----------------------------------------------------------------------
	// Search
	//----------------------------------------------------------------------
public:
	Iterator operator[](size_t index)
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
	T &operator*() { return *superit(); }
	const T &operator*() const { return *superit(); }

	T *operator->() { return &**this; } 
	const T *operator->() const { return &**this; } 

	//----------------------------------------------------------------------
	// Mutation
	//----------------------------------------------------------------------
public:
	void remove(size_t k = 1)
	{
		Iterator i = *this + k;
		superv().erase(superit(), i.superit());
		_iterator = i.superit();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class ConstIterator
{
	friend Vector<T>;

	//----------------------------------------------------------------------
	// Internals
	//----------------------------------------------------------------------
protected:
	typedef std::vector<T> SuperV;
	typedef typename SuperV::const_iterator SuperIt;

	const Vector<T> *_vector;
	const Vector<T> &vector() const { return *_vector; }

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
	ConstIterator(const Vector<T> &vector, SuperIt i) : 
		_vector((const Vector<T> *) &vector), _iterator(i) {}
	
public:
	ConstIterator(const Vector<T> &vector) : _vector(&vector), 
		_iterator(vector.super().begin()) {}
		
	ConstIterator(const ConstIterator &i) : _vector(i._vector), _iterator(i.superit()) {}

	ConstIterator(const Iterator<T> &i) : _vector(i._vector), _iterator(i.superit()) {}

	ConstIterator &operator=(Iterator<T> i) 
	{ 
		_vector = (Vector<T> *) &i.vector();
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

	bool operator==(const ConstIterator<T> &i) const 
	{ 
		return _vector == i._vector && _iterator == i._iterator; 
	}

	size_t index() const
	{
		return superit() - vector().begin();
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
	
	ConstIterator operator+(size_t n)
	{
		return ConstIterator(vector(), superit() + n);
	}

	ConstIterator operator-(size_t n)
	{
		return ConstIterator(vector(), superit() - n);
	}

	//----------------------------------------------------------------------
	// Search
	//----------------------------------------------------------------------
public:
	ConstIterator operator[](size_t index)
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
	const T &operator*() const { return *superit(); }
	const T *operator->() const { return &**this; } 
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace _Vector_

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_list_
