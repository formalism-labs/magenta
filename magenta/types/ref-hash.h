
#ifndef _magenta_types_ref_hash_
#define _magenta_types_ref_hash_

#pragma warning(disable : 4355 4786)

#include "magenta/types/classes.h"
#include "magenta/types/ref.h"
#include "magenta/types/hash.h"

#include "magenta/exceptions/defs.h"

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////////////////////

namespace magenta
{

using std::string;
using std::unordered_map;
using std::pair;

///////////////////////////////////////////////////////////////////////////////////////////////
// RefHash
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Key = string, class F = StringHashfun<Key> >
class RefHash : public Hash<Ref<T>, Key, F>
{
public:
	typedef _RefHash_::Iterator<T, Key> Iterator;
	typedef _RefHash_::ConstIterator<T, Key> ConstIterator;

public:
	CLASS_EXCEPTION_DEF("magenta::RefHash");

public:
	RefHash() {}

	Iterator find(const Key &key)
	{
		return Iterator(*this, STLHash::find(key));
	}

	ConstIterator find(const Key &key) const
	{
		return ConstIterator(*this, STLHash::find(key));
	}

	Ref<T> operator[](const Key &key)
	{
		Iterator i = find(key);
		if (!i)
			THROW_EX(NotFound);
		return *i;
	}

	ConstRef<T> operator[](const Key &key) const
	{
		ConstIterator i = find(key);
		if (!i)
			THROW_EX(NotFound);
		return *i;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

namespace _RefHash_
{

//---------------------------------------------------------------------------------------------

template <class T, class Key>
class Iterator
{
	typedef Iterator This;
	typedef RefHash<T, Key> _Hash;
	typedef typename RefHash<T, Key>::iterator STLIterator;

	_Hash &_hash;
	STLIterator _iterator;

protected:
	Iterator(_Hash &hash, STLIterator &i) : _hash(hash), _iterator(i) {}
	
public:
	Iterator(_Hash &hash) : _hash(hash), _iterator(hash.begin()) {}
	Iterator(Iterator &i) : _hash(i._hash), _iterator(i._iterator) {}

	This &operator++() { ++_iterator; return *this; }
	This operator++(int) { Iterator i = _iterator; ++_iterator; return i; }

	This &operator--() { --_iterator; return *this; }
	This operator--(int) { Iterator i = _iterator; --_iterator; return i; }

	bool operator!() const { return _iterator == _hash.end(); }

	const Key &key() const { return (*_iterator).first; }
	Ref<T> &value() { return (*_iterator).second; }
	ConstRef<T> value() const { return (*_iterator).second; }

	T *operator->() { return &*value(); }
	const T *operator->() const { return &*value(); }

	Ref<T> &operator*() { return value(); }
	ConstRef<T> operator*() const { return value(); }
};

//---------------------------------------------------------------------------------------------

template <class T, class Key>
class ConstIterator
{
	typedef ConstIterator This;
	typedef RefHash<T, Key> _Hash;
	typedef typename RefHash<T, Key>::const_iterator STLIterator;

	const _Hash &_hash;
	STLIterator _iterator;

protected:
	ConstIterator(const _Hash &hash, STLIterator &i) : _hash(hash), _iterator(i) {}

public:
	ConstIterator(const _Hash &hash) : _hash(hash), _iterator(hash.begin()) {}
	ConstIterator(const Iterator<T, Key> &i) : _hash(i._hash), _iterator(i._iterator) {}
	ConstIterator(const ConstIterator &i) : _hash(i._hash), _iterator(i._iterator) {}

	This &operator++() { ++_iterator; return *this; }
	This operator++(int) { Iterator i = _iterator; ++_iterator; return i; }

	This &operator--() { --_iterator; return *this; }
	This operator--(int) { Iterator i = _iterator; --_iterator; return i; }

	bool operator!() const { return _iterator == _hash.end(); }

	const Key &key() const { return (*_iterator).first; }
	ConstRef<T> value() const { return (*_iterator).second; }

	const T *operator->() const { return &*value(); }

	ConstRef<T> operator*() const { return value(); }
};

//---------------------------------------------------------------------------------------------

} // namespace _RefHash_

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_hash_
