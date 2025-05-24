
///////////////////////////////////////////////////////////////////////////////////////////////
// Blocks
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_types_block_
#define _magenta_types_block_

#include <string>

#include "magenta/types/classes.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class IBlock
{
public:
	typedef char Element;
	typedef Element *Data;
	typedef const Element *ConstData;
	typedef unsigned int Length;

	virtual ConstData data() const = 0;

	// Logical size
	virtual Length &length() = 0;
	Length length() const { Length &n = ((IBlock *) this)->length(); return n; }

	// Physical size
	virtual Length &size() { return length(); }
	Length size() const { Length &n = ((IBlock *) this)->size(); return n; }

	Length &maxSize() { return size(); }
	Length maxSize() const { return size(); }

	operator ConstData() const { return data(); }
	Element operator[](Length i) const;

	bool isEmpty() const { return !length(); }
	bool operator!() const { return isEmpty(); }

	bool operator==(IBlock &blk) const;
	bool operator<(IBlock &blk) const;
	bool operator<=(IBlock &blk) const;
	bool operator!=(IBlock &blk) const { return !operator==(blk); }
	bool operator>(IBlock &blk)  const { return !operator<=(blk); }
	bool operator>=(IBlock &blk) const { return !operator<(blk); }
};

//---------------------------------------------------------------------------------------------

class IConstBlock : public IBlock
{
public:
	virtual ConstData data() const = 0;

	virtual void set(ConstData data_, Length len_);
	virtual void set(const IBlock &blk) = 0;
	IConstBlock &operator=(const IConstBlock &blk) { set(blk); return *this; }
};

//---------------------------------------------------------------------------------------------

class IMutableBlock : public IBlock
{
public:
	virtual Data data() = 0;
	virtual ConstData data() const { return const_cast<IMutableBlock*>(this)->data(); }

	virtual void set(Data data_, Length len_, Length size_ = 0);
	virtual void set(const IMutableBlock &blk) = 0;
	IBlock &operator=(const IMutableBlock &blk) { set(blk); return *this; }

	virtual void copy(const IBlock &blk);
	virtual void copy(ConstData p, Length n);

	virtual void append(const IBlock &blk);
	virtual void append(ConstData _data, Length n);
	virtual void append(Element c);

	operator Data() { return data(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ConstBlock : public IConstBlock
{
protected:
	ConstData _data;
	Length _length;

public:
	ConstBlock() : _data(0), _length(0) {}
	ConstBlock(const ConstData data, Length len) : _data(data), _length(len) {}
	ConstBlock(const IBlock &blk) : _data(blk.data()), _length(blk.length()) {}

	virtual ~ConstBlock() {}
	
	virtual ConstData data() const { return _data; }
	virtual Length &length() { return _length; }
	virtual Length &size() { return _length; }
	
	virtual void set(const IBlock &blk);
};

//---------------------------------------------------------------------------------------------

class Block : public IMutableBlock
{
protected:
	Data _data;
	Length _length, _size;

public:
	Block() : _data(0), _length(0), _size(0) {}
	Block(Data data, Length len, Length size = 0) : _data(data), _length(len), _size(size) {}
	Block(const IBlock &blk) : _data(Data(blk.data())), _length(blk.length()), _size(blk.size()) {}
	virtual ~Block() {}

	virtual ConstData data() const { return _data; }
	virtual Data data() { return _data; }
	virtual Length &length() { return _length; }
	virtual Length &size() { return _size; }

	void set(const IMutableBlock &blk);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ConstDynablock : public ConstBlock
{
	static ConstBlock newBlock(const IBlock &blk);

public:
	ConstDynablock(ConstData data, Length len);
	ConstDynablock(const IBlock &blk);
	ConstDynablock(const ConstDynablock &blk);
	virtual ~ConstDynablock();

	virtual void set(const IBlock &blk);
};

//---------------------------------------------------------------------------------------------

class PersistentDynablock : public Block
{
	Block newBlock(const IBlock &blk, Length size = 0);

protected:
	void dynaset(const IMutableBlock &blk);

public:
	PersistentDynablock() {}
	PersistentDynablock(Length size) : Block(new Element[size], 0, size) {}
	PersistentDynablock(ConstData data, Length size);
	PersistentDynablock(const IBlock &blk, Length size = 0);
	PersistentDynablock(const PersistentDynablock &b);
	virtual ~PersistentDynablock() {}

	void set(const IMutableBlock &blk);
};

//---------------------------------------------------------------------------------------------

class Dynablock : public PersistentDynablock
{
public:
	Dynablock() {}
	Dynablock(Length size) : PersistentDynablock(size) {}
	Dynablock(ConstData data, Length size) : PersistentDynablock(data, size) {}
	Dynablock(const IBlock &blk, Length size = 0) : PersistentDynablock(blk, size) {}
	Dynablock(const PersistentDynablock &blk);
	virtual ~Dynablock();
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_block_
