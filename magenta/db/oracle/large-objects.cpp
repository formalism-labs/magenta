
#include "large-objects.h"

#include "magenta/types/datetime/date.h"
#include "magenta/types/datetime/ole-datetime.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Large Objects
///////////////////////////////////////////////////////////////////////////////////////////////

PersistentDynablock ILargeObject::read() const
{
	PersistentDynablock block(size());
	short rc;
	char *p = block.data();
	unsigned long n = 0, N = block.size();
	unsigned long m;
	do
	{
		m = read(&rc, (unsigned char *) p + n, N, 0);
		n += m;
		N -= m;
	}
	while (rc == OLOB_NEED_DATA);
	block.length() = n;
	return block;
}

//---------------------------------------------------------------------------------------------

void ILargeObject::write(Block block)
{
	try
	{
		short rc;
		const char *p = block.data();
		unsigned long n = 0, N = block.size();
		unsigned long m;
		do
		{
			m = write(&rc, (unsigned char *) p + n, N, OLOB_ONE_PIECE, 0);
			n += m;
			N -= m;
		}
		while (rc == OLOB_NEED_DATA);
	}
	catch (OException &x)
	{
		long n = x.GetErrorNumber();
		const char *s = x.GetErrorText();
		THROW_EX(Exception)("%s (%d)", s, n);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

Text &Text::operator=(Block block)
{
	ILargeObject::write(block);
	return *this;
}

//---------------------------------------------------------------------------------------------

bool Text::isNull() const
{
	return _lob.IsNull();
}

//---------------------------------------------------------------------------------------------

unsigned long Text::size() const
{
	return _lob.GetSize();
}

//---------------------------------------------------------------------------------------------

unsigned long Text::read(short *status, unsigned char *buffer,  
	unsigned long buffersize, unsigned long offset) const
{
	try
	{
		return _lob.Read(status, buffer, buffersize, offset);
	}
	catch (...)
	{
		throw;
	}
}

//---------------------------------------------------------------------------------------------

unsigned long Text::write(short *status, unsigned char *buffer, unsigned long buffersize, 
	unsigned char piecetype, unsigned long offset)
{
	try
	{
		return _lob.Write(status, buffer, buffersize, piecetype, offset);
	}
	catch (OException &x)
	{
		long n = x.GetErrorNumber();
		const char *s = x.GetErrorText();
		THROW_EX(Exception)("%s (%d)", s, n);
	}
}

//---------------------------------------------------------------------------------------------

void Text::setEmpty()
{
	OValue v;
	v.SetEmpty();
	_lob = v;
}

///////////////////////////////////////////////////////////////////////////////////////////////

Binary &Binary::operator=(Block block)
{
	ILargeObject::write(block);
	return *this;
}

//---------------------------------------------------------------------------------------------

bool Binary::isNull() const
{
	return _lob.IsNull();
}

//---------------------------------------------------------------------------------------------

unsigned long Binary::size() const
{
	return _lob.GetSize();
}

//---------------------------------------------------------------------------------------------

unsigned long Binary::read(short *status, unsigned char *buffer,  
	unsigned long buffersize, unsigned long offset) const
{
	return _lob.Read(status, buffer, buffersize, offset);
}

//---------------------------------------------------------------------------------------------

unsigned long Binary::write(short *status, unsigned char *buffer, unsigned long buffersize, 
	unsigned char piecetype, unsigned long offset)
{
	return _lob.Write(status, buffer, buffersize, piecetype, offset);
}

//---------------------------------------------------------------------------------------------

void Binary::setEmpty()
{
	OValue v;
	v.SetEmpty();
	_lob = v;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
