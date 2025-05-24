
#ifndef _magenta_types_heap_
#define _magenta_types_heap_

#include "magenta/types/classes.h"

#include "magenta/types/vector.h"
#include "magenta/types/ref-vector.h"

#include "magenta/exceptions/defs.h"

#include <algorithm>
//#include <queue>

namespace magenta
{

using std::vector;
using std::pop_heap;
using std::push_heap;
using std::make_heap;
using std::sort_heap;
using std::less;
using std::greater;
//using std::priority_queue;

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare = greater<T> >
class Heap : protected Vector<T>
{
	typedef Vector<T> Super;

protected:
	Vector<T>::iterator begin() { return super().begin(); }
	Vector<T>::iterator end() { return super().end(); }

public:
	Heap() {}

	Heap(const Vector<T> &v) : Vector<T>(v)
	{
		rebuild();
	}

	template <class RandomAccessIterator>
	Heap(RandomAccessIterator first, RandomAccessIterator last) : Vector<T>(first, last)
	{
		rebuild();
	}

	T &at(int i) { return Super::at(i); }
	const T &at(int i) const { return Super::at(i); }
	T &operator[](int i) { return at(i); }
	const T &operator[](int i) const { return at(i); }

	bool isEmpty() const { return empty(); }

	size_t size() const { return Super::size(); }

	void push(const T &e)
	{
		push_back(e);
		if (size() > 1)
			push_heap(begin(), end(), Compare());
	}

	Heap &operator<<(const T &e)
	{
		push(e);
		return *this;
	}

	T pop()
	{
		if (isEmpty())
			THROW_EX(EmptyErr);

		pop_heap(begin(), end(), Compare());
		T e = back();
		pop_back();
		return e;
	}

	T top() const 
	{ 
		if (isEmpty())
			THROW_EX(EmptyErr);
		return *Super::begin(); 
	}

	T remove(size_t idx, bool rebuildHeap = true)
	{
		size_t n = size()-1;
		if (idx > n)
			THROW_EX(IndexOutOfRangeErr);
		if (idx < n)
			swap(idx, n);
		T e = back();
		pop_back();
		if (rebuildHeap)
			rebuild();
		return e;
	}

	bool remove(const T &e)
	{
		bool flag = false;
		for (Iterator i = Super::begin(); !!i; ++i)
			if (*i == e)
			{
				remove((i--).index(), false);
				flag = true;
			}
		if (flag)
			rebuild();
		return flag;
	}

	void rebuild()
	{
		make_heap(begin(), end(), Compare());
	}

	void sort()
	{
		sort_heap(begin(), end(), Compare());
	}

	CLASS_EXCEPTION_DEF("magenta::Heap");
	CLASS_SUB_EXCEPTION_DEF(EmptyErr, "Heap is empty");
	CLASS_SUB_EXCEPTION_DEF(IndexOutOfRangeErr, "Index out of range");
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class C>
struct RefCompare
{
	bool operator()(const Ref<T> &r1, const Ref<T> &r2)
	{
		return C()(*r1, *r2);
	}
};

//---------------------------------------------------------------------------------------------

template <class T, class Compare = greater<T> >
class RefHeap : public Heap< Ref<T>, RefCompare<T, Compare> >
{
	typedef Heap< Ref<T>, RefCompare<T, Compare> > Super;

public:
	RefHeap() {}

	RefHeap(const RefVector<T> &v) : Super(v) {}

	template <class RandomAccessIterator>
	RefHeap(RandomAccessIterator first, RandomAccessIterator last) : Super(first, last) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare = greater<T> >
class PriorityQueue : protected Heap<T, Compare>
{
	typedef PriorityQueue This;
	typedef Heap<T, Compare> Super;

public:
	T top() const { return Super::top(); }
	T pop() {return Super::pop(); }

	void push(const T &e) { Super::push(e); }
	This &operator<<(const T &e) { push(e); return *this; }
	
	bool isEmpty() const { return empty(); }
};

//---------------------------------------------------------------------------------------------

template <class T, class Compare = greater<T> >
class RefPriorityQueue : protected RefHeap<T, Compare>
{
	typedef RefPriorityQueue This;
	typedef RefHeap<T, Compare> Super;

public:
	Ref<T> top() const { return Super::top(); }
	Ref<T> pop() {return Super::pop(); }

	void push(const Ref<T> &e) { Super::push(e); }
	This &operator<<(const Ref<T> &e) { push(e); return *this; }
	
	bool isEmpty() const { return empty(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_heap_
