
#ifndef _magenta_db_oracle_values_
#define _magenta_db_oracle_values_

#include "magenta/db/oracle/general.h"
#include "magenta/db/oracle/large-objects.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Value
///////////////////////////////////////////////////////////////////////////////////////////////

class IConstValue
{
protected:
	virtual void getValue(int &n) const = 0;
	virtual void getValue(long &n) const = 0;
	virtual void getValue(double &n) const = 0;
	virtual void getValue(string &s) const = 0;
	virtual void getValue(Date &d) const = 0;
	virtual void getValue(OClob &lob) const = 0;
	virtual void getValue(OBlob &lob) const = 0;
	virtual void getValue(OObject &o) const = 0;
	virtual void getValue(ORef &r) const = 0;
	virtual void getValue(OCollection &c) const = 0;
};

//---------------------------------------------------------------------------------------------

#define DEFINE_IValue_Assignment_Operators \
	void operator=(bool b) { _setValue(b); } \
	void operator=(int n) { _setValue(n); } \
	void operator=(long n) { _setValue(n); } \
	void operator=(double n) { _setValue(n); } \
	void operator=(const string &s) { _setValue(s); } \
	void operator=(const String &s) { _setValue(s); } \
	void operator=(const Date &d) { _setValue(d); } \
	void operator=(Text &lob) { _setValue(lob); } \
	void operator=(Binary &lob) { _setValue(lob); } \
	void operator=(Object &o) { _setValue(o); } \
	void operator=(Ref &r) { _setValue(r); } \
	void operator=(const Collection &c) { _setValue(c); }

class IValue : public IConstValue
{
	friend Value;

protected:
	virtual void setValue(int n) = 0;
	virtual void setValue(long n) = 0;
	virtual void setValue(double n) = 0;
	virtual void setValue(const string &s) = 0;
	virtual void setValue(const Date &d) = 0;
	virtual void setValue(OClob &lob) = 0;
	virtual void setValue(OBlob &lob) = 0;
	virtual void setValue(OObject &o) = 0;
	virtual void setValue(ORef &r) = 0;
	virtual void setValue(const OCollection &c) = 0;

	void _setValue(bool b);
	void _setValue(int n);
	void _setValue(long n);
	void _setValue(double n);
	void _setValue(const string &s);
	void _setValue(const Date &d);
	void _setValue(Text &test);
	void _setValue(Binary &bin);
	void _setValue(Object &o);
	void _setValue(Ref &r);
	void _setValue(const Collection &c);

public:
	operator bool() const;
	operator double() const;
	operator long() const;
	operator int() const;
	operator string() const;
	operator String() const;
	operator Date() const;
//	operator PersistentDynablock() const;
	operator Text() const;
	operator Binary() const;
	operator Object() const;
	operator Ref() const;
	operator Collection() const;

	DEFINE_IValue_Assignment_Operators;

	CLASS_EXCEPTION_DEF("magenta::db::IValue");
	CLASS_SUB_EXCEPTION_DEF(ReadError, "read error");
	CLASS_SUB_EXCEPTION_DEF(WriteError, "write error");
};

//---------------------------------------------------------------------------------------------

class Value : public IValue
{
	IValue &_value;

protected:
	void getValue(int &n) const { _value.getValue(n); }
	void getValue(long &n) const { _value.getValue(n); }
	void getValue(double &n) const { _value.getValue(n); }
	void getValue(string &s) const { _value.getValue(s); }
	void getValue(Date &d) const { _value.getValue(d); }
	void getValue(OClob &lob) const { _value.getValue(lob); }
	void getValue(OBlob &lob) const { _value.getValue(lob); }
	void getValue(OObject &o) const { _value.getValue(o); }
	void getValue(ORef &r) const { _value.getValue(r); }
	void getValue(OCollection &c) const { _value.getValue(c); }

	void setValue(int n) { _value.setValue(n); }
	void setValue(long n) { _value.setValue(n); }
	void setValue(double n) { _value.setValue(n); }
	void setValue(const string &s) { _value.setValue(s); }
	void setValue(const Date &d) { _value.setValue(d); }
	void setValue(OClob &lob) { _value.setValue(lob); }
	void setValue(OBlob &lob) { _value.setValue(lob); }
	void setValue(OObject &o) { _value.setValue(o); }
	void setValue(ORef &r) { _value.setValue(r); }
	void setValue(const OCollection &c) { _value.setValue(c); }

public:
	Value(IValue &value) : _value(value) {}

	DEFINE_IValue_Assignment_Operators;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Values
///////////////////////////////////////////////////////////////////////////////////////////////

class CValues
{
protected:
	typedef list<IValue*> Values;
	Values _values;

public:
	virtual ~CValues();

	virtual int valuesCount() const = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CValuesList : public CValues
{
	friend ValuesList;
	friend ValuesMap;

protected:
	virtual IValue *getValue(int i) = 0;

public:
	IValue &operator[](int i);
};

//---------------------------------------------------------------------------------------------

class ValuesList : public CValuesList
{
	CValuesList &_values;

protected:
	IValue *getValue(int i) { return _values.getValue(i); }
	int valuesCount() const { return _values.valuesCount(); }

public:
	ValuesList(CValuesList &values) : _values(values) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CValuesMap : public CValuesList
{
	friend ValuesMap;

protected:
	virtual IValue *getValue(const string &name) = 0;

public:
	IValue &operator[](const string &name);
};

//---------------------------------------------------------------------------------------------

class ValuesMap : public CValuesMap
{
	CValuesMap &_values;

protected:
	IValue *getValue(int i) { return ((CValuesList &)_values).getValue(i); }
	IValue *getValue(const string &name) { return _values.getValue(name); }

	int valuesCount() const { return _values.valuesCount(); }

public:
	ValuesMap(CValuesMap &values) : _values(values) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_db_oracle_values_
