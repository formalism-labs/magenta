
#ifndef _magenta_db_oracle_collection_
#define _magenta_db_oracle_collection_

#include "magenta/db/oracle/general.h"
#include "magenta/db/oracle/large-objects.h"
#include "magenta/db/oracle/cursor.h"
#include "magenta/db/oracle/object.h"
#include "magenta/db/oracle/values.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Collection
///////////////////////////////////////////////////////////////////////////////////////////////

class Collection : public CValuesList
{
	friend IValue;
	friend Cursor::Field;
	friend Object::Attribute;

	const OCollection _collection;

	operator const OCollection&() const { return _collection; }

public:
	Collection(const Collection &collection) : _collection(collection._collection) {}
	Collection(const OCollection &collection) : _collection(collection) {}

	class Value : public IValue
	{
		const OCollection &_value;
		int _i;

	public:
		Value(const Collection &collection, int index);

		DEFINE_IValue_Assignment_Operators;

	protected:
		void getValue(int &n) const;
		void getValue(long &n) const;
		void getValue(double &n) const;
		void getValue(string &s) const;
		void getValue(Date &d) const;
		void getValue(OClob &lob) const;
		void getValue(OBlob &lob) const;
		void getValue(OObject &o) const;
		void getValue(ORef &r) const;
		void getValue(OCollection &c) const;

		void setValue(int n);
		void setValue(long n);
		void setValue(double n);
		void setValue(const string &s);
		void setValue(const Date &d);
		void setValue(OClob &lob);
		void setValue(OBlob &lob);
		void setValue(OObject &o);
		void setValue(ORef &r);
		void setValue(const OCollection &c);

	public:
		CLASS_EXCEPTION_DEF("magenta::db::Collection::Value");
		CLASS_SUB_EXCEPTION_DEF(Unsupported, "unsupported operation");
	};

	friend Value;

	Value operator[](int i) const { return Value(*this, i); }
	int count() const;

protected:
	IValue *getValue(int i) { return new Value(*this, i); }

public:
	int valuesCount() const { return count(); }
	
	void append(OValue value);
	Collection &operator<<(OValue value) { append(value); return *this; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_db_oracle_collection_
