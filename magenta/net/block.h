
#ifndef _magenta_net_block_
#define _magenta_net_block_

#include "magenta/exceptions/defs.h"

#include "magenta/net/classes.h"

namespace magenta
{

/////////////////////////////////////////////////////////////////////////////////////////////// 

class Block
{
	UINT8 *_data;
	UINT32 _length, _size;

public:
	Block(UINT8 *p, UINT32 len) : _data(p), _length(len), _size(len) {}
	Block(UINT8 *p, UINT32 len, UINT32 size) : _data(p), _length(len), _size(size) {}

	Block(const std::string &str) : _data((UINT8 *) str.c_str())
	{
		_length = str.length();
		_size = _length;
	}

	explicit Block(const char *str) : _data((UINT8 *) str)
	{
		_length = strlen(str);
		_size = _length + 1;
	}

	UINT8 *data() const { return _data; }
	UINT32 length() const { return _length; }
	UINT32 size() const { return _size; }

	std::string toString() const { return std::string((char *)_data, _length); }

	void setLength(UINT32 n)
	{
		if (n > _size)
			n = _size;
		_length = n;
	}

	UINT8 at(UINT32 i) const { return _data[i]; }
	UINT8 &at(UINT32 i) { return _data[i]; }

	UINT8 operator[](UINT32 i) const { return at(i); }
	UINT8 &operator[](UINT32 i) { return at(i); }

	int compare(const Block &b) const
	{
		UINT32 i;
		for (i = 0; i < _length && i < b._length; ++i)
		{
			int c = (int) at(i) - (int) b[i];
			if (c != 0)
				return c > 0 ? 1 : -1;
		}
		return (i < b._length ? 0 : 1) - (i < _length ? 0 : 1);
	}

	bool operator==(const Block &b) const { return compare(b) == 0; }
	bool operator!=(const Block &b) const { return compare(b) != 0; }
	bool operator>(const Block &b)  const { return compare(b) > 0; }
	bool operator>=(const Block &b) const { return compare(b) >= 0; }
	bool operator<(const Block &b)  const { return compare(b) < 0; }
	bool operator<=(const Block &b) const { return compare(b) <= 0; }
};

//---------------------------------------------------------------------------------------------

template <UINT32 Size>
class StaticBlock : public Block
{
	UINT8 _buffer[Size];

public:
	StaticBlock() : Block(_buffer, 0, Size) {}

	StaticBlock(UINT8 *p, UINT32 len) : Block(_buffer, len, Size)
	{
		memcpy(_buffer, p, len);
	}

	StaticBlock(std::string &s)
	{
		memcpy((UINT8 *) _buffer, s.c_str(), min(Size, s.Size() + 1));
	}

	text string() const
	{
		return text(reinterpret_cast<const char *>(_buffer), length());
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_net_block_
