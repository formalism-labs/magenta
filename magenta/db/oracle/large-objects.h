
#ifndef _magenta_db_oracle_large_objects_
#define _magenta_db_oracle_large_objects_

#include "magenta/db/oracle/general.h"
#include "magenta/db/oracle/values.h"

#include "magenta/types/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class ILargeObject
{
protected:
	virtual unsigned long size() const = 0;
	virtual unsigned long read(short *status, unsigned char *buffer, 
		unsigned long buffersize, unsigned long offset = 0) const = 0;
	virtual unsigned long write(short *status, unsigned char *buffer, unsigned long buffersize, 
		unsigned char piecetype = OLOB_ONE_PIECE, unsigned long offset = 0) = 0;

public:
	virtual bool isNull() const = 0;
	bool operator!() const { return isNull(); }

	PersistentDynablock read() const;
	operator PersistentDynablock() const { return read(); }

	void write(Block block);
	LargeObject &operator=(Block block);

	CLASS_EXCEPTION_DEF("magenta::LargeObject");
};

//---------------------------------------------------------------------------------------------

class Text : public ILargeObject
{
	friend IValue;

	OClob _lob;

protected:
	Text(const OClob &lob) : _lob(lob) {}

public:
	Text &operator=(Block block);

	operator OClob&() { return _lob; }

	bool isNull() const;

	static string emptyLOBConst() { return "empty_clob()"; }
	void setEmpty();

protected:
	unsigned long size() const;
	unsigned long read(short *status, unsigned char *buffer, 
		unsigned long buffersize, unsigned long offset = 0) const;
	unsigned long write(short *status, unsigned char *buffer, unsigned long buffersize, 
		unsigned char piecetype = OLOB_ONE_PIECE, unsigned long offset = 0);
};

//---------------------------------------------------------------------------------------------

class Binary : public ILargeObject
{
	friend IValue;
	
	OBlob _lob;

protected:
	Binary(const OBlob &lob) : _lob(lob) {}

public:
	Binary &operator=(Block block);

	operator OBlob&() { return _lob; }

	bool isNull() const;

	static string emptyLOBConst() { return "empty_blob()"; }
	void setEmpty();

protected:
	unsigned long size() const;
	unsigned long read(short *status, unsigned char *buffer, 
		unsigned long buffersize, unsigned long offset = 0) const;
	unsigned long write(short *status, unsigned char *buffer, unsigned long buffersize, 
		unsigned char piecetype = OLOB_ONE_PIECE, unsigned long offset = 0);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_db_oracle_large_objects_
