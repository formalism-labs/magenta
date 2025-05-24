
#pragma once

namespace magenta
{

template <class T> class Range;
class span;

//---------------------------------------------------------------------------------------------

class IBlock;
class IConstBlock;
class ConstBlock;
class IMutableBlock;
class Block;
class ConstDynablock;
class PersistentDynablock;
class Dynablock;

//---------------------------------------------------------------------------------------------

template <class T, bool AllowNulls = false> class GeneralRef;
template <class T> class Ref;
template <class T> class ConstRef;
template <class T> class Ptr;
template <class T> class ConstPtr;

//---------------------------------------------------------------------------------------------

template <class Key> struct StringHashfun;
template <class T, class Key = std::string, class F = StringHashfun<Key>> class Hash;

namespace _Hash_
{

	template <class T, class Key> class Iterator;
	template <class T, class Key> class ConstIterator;
}

template <class T, class Key = std::string, class F = StringHashfun<Key>> class RefHash;
namespace _RefHash_
{

	template <class T, class Key> class Iterator;
	template <class T, class Key> class ConstIterator;
}

//---------------------------------------------------------------------------------------------

template <class T, class Compare> class Heap;
template <class T, class C> struct RefCompare;
template <class T, class Compare> class RefHeap;

//---------------------------------------------------------------------------------------------

template <class T, class Compare> class PriorityQueue;
template <class T, class Compare> class RefPriorityQueue;

//--------------------------------------------------------------------------------------------

template <class T> class List;
namespace _List_
{
	template <class T> class Iterator;
	template <class T> class ConstIterator;
}

template <class T, class RefT = Ref<T>> class RefList;
namespace _RefList_
{
	template <class T, class RefT = Ref<T>> class Iterator;
	template <class T, class RefT = Ref<T>> class ConstIterator;
}

template <class T> class PtrList;

//---------------------------------------------------------------------------------------------

template <class T> class Vector;
namespace _Vector_
{
	template <class T> class Iterator;
	template <class T> class ConstIterator;
}

template <class T, class RefT = Ref<T>> class RefVector;
namespace _RefVector_
{
	template <class T, class RefT = Ref<T>> class Iterator;
	template <class T, class RefT = Ref<T>> class ConstIterator;
}

template <class T> class PtrVector;

} // namespace magenta
