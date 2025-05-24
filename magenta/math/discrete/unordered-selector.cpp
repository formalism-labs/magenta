
#include "unordered-selector.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedSelector
///////////////////////////////////////////////////////////////////////////////////////////////

UnorderedSelector::UnorderedSelector(int n, int k) : _n(n), _k(k), _v(k)
{
	reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void UnorderedSelector::reset()
{
	_done = !_k;
	for (int i = 0; i < _k; ++i)
		_v[i] = i;
}

//---------------------------------------------------------------------------------------------

bool UnorderedSelector::next()
{
	if (_done)
		return false;

	int i;
	for (i = _k-1; i > -1; --i)
		if (++_v[i] < max(i))
			break;

	if (i < 0)
	{
		_done = true;
		return false;
	}

	for (++i; i < _k; ++i)
		_v[i] = _v[i-1]+1;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedSelectors
///////////////////////////////////////////////////////////////////////////////////////////////

UnorderedSelectors::UnorderedSelectors(const Vector<N_K> &n_k) : _done(false)
{
	for (size_t i = 0; i < n_k.size(); ++i)
		_selectors << new UnorderedSelector(n_k[i].n, n_k[i].k);
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool UnorderedSelectors::next()
{
	if (_done)
		return false;
	
	int sn = numSelectors();
	for (int i = sn-1; i >= 0; --i)
	{
		UnorderedSelector &s = selector(i);
		if (s.simple())
			continue;
		if (s.next())
		{
			for (++i; i < sn; ++i)
				selector(i).reset();
			return true;
		}
	}
	_done = true;
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
