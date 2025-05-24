
#ifndef _magenta_math_discrete_unordered_selector_
#define _magenta_math_discrete_unordered_selector_

#include "magenta/types/defs.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////

struct N_K
{
	N_K(int n, int k) : n(n), k(k) {}
	int n;
	int k;
};

//---------------------------------------------------------------------------------------------

class UnorderedSelector
{
	friend class UnorderedSelectors;

	typedef UnorderedSelector This;

protected:
	int _n, _k;
	bool simple() const { return _n == _k; }
	int max(int i) const { return _n - _k + i + 1; }
public:
	int N() const { return _n; }
	int K() const { return _k; }

protected:
	Vector<int> _v;
public:
	int selection(int i) const { return _v[i]; }
	const Vector<int> &selection() const { return _v; }

	int operator()(int i) const{ return selection(i); }
	const Vector<int> &operator()() const { return selection(); }

protected:
	bool _done;
public:
	bool done() const { return _done; }
	bool operator!() const { return done(); }

public:
	UnorderedSelector(int n, int k);

	void reset();

	bool next();
	This &operator++() { next(); return *this; }
};

//---------------------------------------------------------------------------------------------

class UnorderedSelectors
{
	typedef UnorderedSelectors This;

protected:
	typedef RefVector<UnorderedSelector> Selectors;
	Selectors _selectors;

	UnorderedSelector &selector(int i) { return _selectors[i]; }
	const UnorderedSelector &selector(int i) const { return _selectors[i]; }

protected:
	bool _done;
public:
	bool operator!() const { return _done; }
	
public:
	UnorderedSelectors(const Vector<N_K> &n_k);

	const Vector<int> &selection(int i) const { return selector(i).selection(); }
	int selection(int i, int j) const { return selector(i).selection(j); }

	int operator()(int i, int j) const{ return selection(i, j); }
	const Vector<int> &operator()(int i) const { return selection(i); }

	bool next();
	This &operator++() { next(); return *this; }

	int numSelectors() const { return (int) _selectors.size(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_math_discrete_unordered_selector_
