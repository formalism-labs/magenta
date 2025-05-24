
#include "collection.h"

#include "magenta/types/datetime/date.h"
#include "magenta/types/datetime/ole-datetime.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Collection
///////////////////////////////////////////////////////////////////////////////////////////////

int Collection::count() const
{
	return _collection.GetSize();
}

//---------------------------------------------------------------------------------------------

void Collection::append(OValue value)
{
	_collection.Append(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Collection::Value
///////////////////////////////////////////////////////////////////////////////////////////////

Collection::Value::Value(const Collection &collection, int index)  : 
	_value(collection._collection), _i(index) 
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Collection::Value::getValue(double &n) const
{
	_value.GetValue(_i, &n);
}

void Collection::Value::getValue(long &n) const
{
	_value.GetValue(_i, &n);
}

void Collection::Value::getValue(int &n) const
{
	_value.GetValue(_i, &n);
}

void Collection::Value::getValue(string &s) const
{
	OValue v = _value.GetValue(_i);
	if (!v.IsNull())
	{
		const char *p = v;
		if (!!p)
			s = p;
	}
}

void Collection::Value::getValue(OClob &lob) const
{
	OValue v = _value.GetValue(_i);
	if (!v.IsNull())
		lob = OClob(v);
}

void Collection::Value::getValue(OBlob &lob) const
{
	OValue v = _value.GetValue(_i);
	if (!v.IsNull())
		lob = OBlob(v);
}

void Collection::Value::getValue(Date &d) const
{
//		OleDateTime dt = read(VT_DATE);
//		return Date(dt.GetDay(), dt.GetMonth(), dt.GetYear());
}

void Collection::Value::getValue(OObject  &o) const
{
	_value.GetValue(_i, &o);
}

void Collection::Value::getValue(ORef &r) const
{
	_value.GetValue(_i, &r);
}

void Collection::Value::getValue(OCollection &c) const
{
	THROW_EX(Unsupported);
}

//---------------------------------------------------------------------------------------------

void Collection::Value::setValue(int n)
{
	_value.SetValue(_i, n);
}

void Collection::Value::setValue(long n)
{
	_value.SetValue(_i, n);
}

void Collection::Value::setValue(double n)
{
	_value.SetValue(_i, n);
}

void Collection::Value::setValue(const string &s)
{
	_value.SetValue(_i, s.c_str());
}

void Collection::Value::setValue(const Date &d)
{
	THROW_EX(Unimplemented);
//		OleDateTime dt(d.year(), d.month(), d.day(), 0, 0, 0);
//		_field->PutValue(&_variant_t(dt));
}

void Collection::Value::setValue(OClob &lob)
{
	_value.SetValue(_i, lob);
}

void Collection::Value::setValue(OBlob &lob)
{
	_value.SetValue(_i, lob);
}

void Collection::Value::setValue(OObject &o)
{
	_value.SetValue(_i, o);
}

void Collection::Value::setValue(ORef &r)
{
	_value.SetValue(_i, r);
}

void Collection::Value::setValue(const OCollection &c)
{
	THROW_EX(Unsupported);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

