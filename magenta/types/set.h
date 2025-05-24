
#ifndef _magenta_types_set_
#define _magenta_types_set_

#include <stdexcept>
#include <algorithm> 
#include <functional> 
#include <set>
#include <string>

namespace magenta
{

using std::set;
using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T = std::string>
class Set : public std::set<T>
{
	typedef Set<T> This;

public:
	This &operator<<(const T &t) { insert(t); return *this; }
	bool in(const T &t) const { return find(t) != this->end(); }
	bool operator[](const T &t) const { return in(t); }
	bool operator!() const { return this->empty(); }

	class Iterator : public std::set<T>::const_iterator
	{
		const Set<T> &_set;

	public:
		Iterator(const Set &set) : std::set<T>::const_iterator(set.begin()), _set(set) {}
		Iterator(const Iterator &i) : std::set<T>::const_iterator(i), _set(i.set) {}

		bool operator!() const { return *this == this->_set.end(); }
	};
};


typedef Set<string> Sext;

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_set_
