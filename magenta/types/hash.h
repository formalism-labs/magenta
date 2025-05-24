
#ifndef _magenta_types_hash_
#define _magenta_types_hash_

#pragma warning(disable : 4355 4786 4348)

#include "magenta/types/classes.h"
#include "magenta/types/ref.h"

#include "magenta/exceptions/defs.h"

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////////////////////

namespace magenta
{

using std::string;
using std::unordered_map;
using std::pair;

///////////////////////////////////////////////////////////////////////////////////////////////
// Hash
///////////////////////////////////////////////////////////////////////////////////////////////

template <class Key>
struct StringHashfun 
{
	size_t operator()(const Key &k) const 
	{ 
		return std::__stl_hash_string(k.c_str()); 
	}
};

//---------------------------------------------------------------------------------------------

template <class T, class Key = string, class F = StringHashfun<Key> >
class Hash : public unordered_map<Key, T, F>
{
	typedef unordered_map<Key, T, F> STLHash;
	typedef pair<const Key, T> Pair;

public:
	typedef _Hash_::Iterator<T, Key> Iterator;
	typedef _Hash_::ConstIterator<T, Key> ConstIterator;

	CLASS_EXCEPTION_DEF("magenta::Hash");
	CLASS_SUB_EXCEPTION_DEF(NotFound, "item hot found in collection");

public:
	Hash() {}

	void insert(T data)
	{
		Key key(data);
		STLHash::insert(Pair(key, data));
	}

	void insert(const Key &key, T data)
	{
		STLHash::insert(Pair(key, data));
	}

	Iterator find(const Key &key)
	{
		return Iterator(*this, STLHash::find(key));
	}

	ConstIterator find(const Key &key) const
	{
		return ConstIterator(*this, STLHash::find(key));
	}

	T &operator[](const Key &key)
	{
		Iterator i = find(key);
		if (!i)
			THROW_EX(NotFound);
		return *i;
	}

	const T &operator[](const Key &key) const
	{
		ConstIterator i = find(key);
		if (!i)
			THROW_EX(NotFound);
		return *i;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

namespace _Hash_
{

//---------------------------------------------------------------------------------------------

template <class T, class Key>
class Iterator
{
public:
	typedef ConstIterator<T, Key> Const;

protected:
	typedef Iterator This;
	typedef Hash<T, Key> _Hash;
	typedef typename Hash<T, Key>::iterator STLIterator;

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
	T &value() { return (*_iterator).second; }
	const T &value() const { return (*_iterator).second; }

	T *operator->() { return &value(); }
	const T *operator->() const { return &value(); }

	T &operator*() { return value(); }
	const T &operator*() const { return value(); }
};

//---------------------------------------------------------------------------------------------

template <class T, class Key>
class ConstIterator
{
	typedef ConstIterator This;
	typedef Hash<T, Key> _Hash;
	typedef typename Hash<T, Key>::const_iterator STLIterator;

	const _Hash &_hash;
	STLIterator _iterator;

protected:
	ConstIterator(_Hash &hash, STLIterator &i) : _hash(hash), _iterator(i) {}

public:
	ConstIterator(_Hash &hash) : _hash(hash), _iterator(hash.begin()) {}
	ConstIterator(const Iterator<T, Key> &i) : _hash(i._hash), _iterator(i._iterator) {}
	ConstIterator(const ConstIterator &i) : _hash(i._hash), _iterator(i._iterator) {}

	This &operator++() { ++_iterator; return *this; }
	This operator++(int) { Iterator i = _iterator; ++_iterator; return i; }

	This &operator--() { --_iterator; return *this; }
	This operator--(int) { Iterator i = _iterator; --_iterator; return i; }

	bool operator!() const { return _iterator == _hash.end(); }

	const Key &key() const { return (*_iterator).first; }
	const T &value() const { return (*_iterator).second; }

	const T *operator->() const { return &*value(); }

	const T &operator*() const { return *value(); }
};

//---------------------------------------------------------------------------------------------

} // namespace _Hash_

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_hash_
