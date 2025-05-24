
#ifndef _magenta_types_general_
#define _magenta_types_general_

#include <algorithm>
#include <limits>
#include <cinttypes>

#include "magenta/types/classes.h"
#include "magenta/lang/general.h"

namespace magenta
{

using std::numeric_limits;

///////////////////////////////////////////////////////////////////////////////////////////////

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned long DWord;
typedef std::uint64_t QWord;

//---------------------------------------------------------------------------------------------

template <class T = long long int> // T is signed
class Range
{
public:
	T min, max;

	Range(T min = 0, T max = 0) : min(min), max(max) {}
	Range(Null) : min(numeric_limits<T>::max()), max(numeric_limits<T>::max()) {}

	void set(T min_, T max_)
	{
		min = min_;
		max = max_;
	}

	Range<T> &operator=(const Range<T> &r) { set(r.min, r.max); return *this; }
	Range<T> &operator=(const Null &null) { setNull(); return *this; }
	void setNull() { min = max = numeric_limits<T>::max(); }

	bool operator!() const { return min == numeric_limits<T>::max() && max == numeric_limits<T>::max(); }
	bool isIn(T n) const { return !!*this && n >= min && n <= max; }
	bool isOut(T n) const { return !isIn(n); }

	T size() const { return max - min + 1; }

	Range<T> real(T size = 0) const
	{
		T from = min, to = max;
		if (size == 0)
		{
			if (from < 0 || to < 0)
				return null;
			size = from - to + 1;	
		}
		if (from < 0)
			from = size + from;
		if (to < 0)
			to = size + to;
		if (to < from)
			return null;
		return Range<T>(from, to);
	}

	magenta::span span(T size = 0) const;
};

typedef Range<long long int> range;

//---------------------------------------------------------------------------------------------

class span
{
public:
	size_t at;
	size_t size;

	span(size_t at = 0, size_t size = 0) : at(at), size(size) {}

	void set(size_t at_, size_t size_)
	{
		at = at_;
		size = size_;
	}

	span &operator=(const span &s)
	{
		set(s.at, s.size);
		return *this;
	}

	bool operator!() const { return !size; }
	bool isIn(int k) const { return k >= at && k < at + size; }
	bool isOut(int k) const { return !isIn(k); }

	range range() const { return magenta::range(at, at + size - 1); }
};

//---------------------------------------------------------------------------------------------

template <class T>
span Range<T>::span(T size) const
{
	auto rr = real(size);
	if (!rr)
		return magenta::span();
	return magenta::span(rr.min, rr.max - rr.min + 1);
}

//---------------------------------------------------------------------------------------------

template <class T>
std::pair<T,T> cons(const T &a, const T &b) 
{
	return std::pair<T,T>(a, b);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_general_
