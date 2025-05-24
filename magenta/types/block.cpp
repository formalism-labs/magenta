
///////////////////////////////////////////////////////////////////////////////////////////////
// Blocks
///////////////////////////////////////////////////////////////////////////////////////////////

#include "block.h"

#include <algorithm>
#include <memory.h>

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// IBlock
///////////////////////////////////////////////////////////////////////////////////////////////

bool IBlock::operator==(IBlock &blk) const
{
	Length n1 = length(), n2 = blk.length();
	return n1 == n2 && !!memcmp(data(), blk.data(), n1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool IBlock::operator<(IBlock &blk) const
{
	Length n1 = length(), n2 = blk.length();
	int m = memcmp(data(), blk.data(), std::min(n1, n2));
	return !m ? n1 < n2 : m < 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool IBlock::operator<=(IBlock &blk) const
{
	Length n1 = length(), n2 = blk.length();
	int m = memcmp(data(), blk.data(), std::min(n1, n2));
	return !m ? n1 <= n2 : m < 0;
}

//---------------------------------------------------------------------------------------------
/*
const IBlock::Data IBlock::operator[](Length i) const
{
	return &data()[i]; 
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////
// IMutableBlock
///////////////////////////////////////////////////////////////////////////////////////////////
	
void IMutableBlock::copy(ConstData p, Length n)
{
    length() = std::min((Length) size(), n ? n : (Length) length());
    memcpy(data(), p, length());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void IMutableBlock::copy(const IBlock &blk)
{
    length() = std::min(size(), blk.length());
    memcpy(data(), blk.data(), length());
}

//---------------------------------------------------------------------------------------------

void IMutableBlock::append(ConstData p, Length n)
{
	Length m = std::min(n, std::max((Length) 0L, size() - length()));
    memcpy(data() + (Length) length(), p, m);
	length() += m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void IMutableBlock::append(const IBlock &blk)
{
    ConstData p = blk.data();
    Length m = std::min((Length) blk.length(), 
		std::max((Length) 0L, size() - length()));
    memcpy(data() + (Length) length(), p, m);
    length() += m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void IMutableBlock::append(Element e)
{
	append(&e, sizeof(e));
}

//---------------------------------------------------------------------------------------------

void IMutableBlock::set(Data data_, Length len_, Length size_)
{
	set(Block(data_, len_, size_));
}

//---------------------------------------------------------------------------------------------
/*
IBlock::Data IMutableBlock::operator[](Length i)
{
	return &data()[i]; 
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////
// ConstBlock
///////////////////////////////////////////////////////////////////////////////////////////////

void IConstBlock::set(ConstData data_, Length len_)
{
	set(ConstBlock(data_, len_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ConstBlock::set(const IBlock &blk)
{
	_data = blk.data();
	_length = blk.length();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Block
///////////////////////////////////////////////////////////////////////////////////////////////

//Block::Block(Dynablock &blk) : 
//	_data(blk.data()), _length(blk.length()), _size(blk.size()) 
//{
//}

void Block::set(const IMutableBlock &blk)
{
	// it is ok to cast because "const" is to protect the block, not its content
	_data = Data(blk.data());
	_length = blk.length();
	_size = blk.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ConstDynablock
///////////////////////////////////////////////////////////////////////////////////////////////

ConstDynablock::ConstDynablock(ConstData data, Length len) : 
	ConstBlock(newBlock(ConstBlock(data, len)))
{
}

//---------------------------------------------------------------------------------------------

ConstDynablock::ConstDynablock(const IBlock &blk) : ConstBlock(newBlock(blk))
{
}

//---------------------------------------------------------------------------------------------

ConstDynablock::ConstDynablock(const ConstDynablock &blk) : ConstBlock(newBlock(blk))
{
}

//---------------------------------------------------------------------------------------------

ConstDynablock::~ConstDynablock()
{
	delete (void *) data();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ConstDynablock::set(const IBlock &blk)
{
	delete (void *) data();
	ConstBlock::set(newBlock(blk));
}

//---------------------------------------------------------------------------------------------

ConstBlock ConstDynablock::newBlock(const IBlock &blk)
{
	Block new_blk(new Element[blk.length()], blk.length());
	new_blk.append(blk);
	return new_blk;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// PersistentDynablock
///////////////////////////////////////////////////////////////////////////////////////////////

Block PersistentDynablock::newBlock(const IBlock &blk, Length size_)
{
	Block new_blk(new Element[blk.length()], 0, std::max(size_, blk.length()));
	new_blk.append(blk);
	return new_blk;
}

//---------------------------------------------------------------------------------------------

PersistentDynablock::PersistentDynablock(ConstData p, Length size) : 
	Block(newBlock(ConstBlock(p, size)))
{
}

//---------------------------------------------------------------------------------------------

PersistentDynablock::PersistentDynablock(const IBlock &blk, Length size) : 
	Block(newBlock(blk, size))
{
}

//---------------------------------------------------------------------------------------------

PersistentDynablock::PersistentDynablock(const PersistentDynablock &b) :
	Block(newBlock(b))
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

void PersistentDynablock::set(const IMutableBlock &blk)
{
	if (blk.length() < size())
		copy(blk);
	else
	{
		delete data();
		Block::set(newBlock(blk));
	}
}

//---------------------------------------------------------------------------------------------

void PersistentDynablock::dynaset(const IMutableBlock &blk)
{
	if (!!data())
		delete data();
	Block::set(blk);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Dynablock
///////////////////////////////////////////////////////////////////////////////////////////////

Dynablock::Dynablock(const PersistentDynablock &blk)
{
	dynaset(blk);
}

//---------------------------------------------------------------------------------------------

Dynablock::~Dynablock()
{
	delete _data;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

