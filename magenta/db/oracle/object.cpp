
#include "object.h"

#include "magenta/types/datetime/date.h"
#include "magenta/types/datetime/ole-datetime.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Object
///////////////////////////////////////////////////////////////////////////////////////////////

int Object::attributeCount() const
{
	return _object.GetCount();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Object::Attribute
///////////////////////////////////////////////////////////////////////////////////////////////

void Object::Attribute::getValue(double &n) const
{
	if (_index == -1)
		_object.GetAttrValue(_name.c_str(), &n);
	else
		_object.GetAttrValue(_index, &n);
}

void Object::Attribute::getValue(long &n) const
{
	if (_index == -1)
		_object.GetAttrValue(_name.c_str(), &n);
	else
		_object.GetAttrValue(_index, &n);
}

void Object::Attribute::getValue(int &n) const
{
	if (_index == -1)
		_object.GetAttrValue(_name.c_str(), &n);
	else
		_object.GetAttrValue(_index, &n);
}

void Object::Attribute::getValue(string &s) const
{
	const char *p;
	if (_index == -1)
		p = (const char *) _object.GetAttrValue(_name.c_str());
	else
		p = (const char *) _object.GetAttrValue(_index);
	if (p)
		s = p;
}

void Object::Attribute::getValue(Date &d) const
{
	THROW_EX(Unimplemented);
//		OleDateTime dt = read(VT_DATE);
//		return Date(dt.GetDay(), dt.GetMonth(), dt.GetYear());
}

void Object::Attribute::getValue(OClob &lob) const
{
	if (_index == -1)
		_object.GetAttrValue(_name.c_str(), &lob);
	else
		_object.GetAttrValue(_index, &lob);
}

void Object::Attribute::getValue(OBlob &lob) const
{
	if (_index == -1)
		_object.GetAttrValue(_name.c_str(), &lob);
	else
		_object.GetAttrValue(_index, &lob);
}

void Object::Attribute::getValue(OObject &o) const
{
	if (_index == -1)
		_object.GetAttrValue(_name.c_str(), &o);
	else
		_object.GetAttrValue(_index, &o);
}

void Object::Attribute::getValue(ORef &r) const
{
	if (_index == -1)
		_object.GetAttrValue(_name.c_str(), &r);
	else
		_object.GetAttrValue(_index, &r);
}

void Object::Attribute::getValue(OCollection &c) const
{
	if (_index == -1)
		_object.GetAttrValue(_name.c_str(), &c);
	else
		_object.GetAttrValue(_index, &c);
}

//---------------------------------------------------------------------------------------------

void Object::Attribute::setValue(int n)
{
	if (_index == -1)
		_object.SetAttrValue(_name.c_str(), n);
	else
		_object.SetAttrValue(_index, n);
}

void Object::Attribute::setValue(long n)
{
	if (_index == -1)
		_object.SetAttrValue(_name.c_str(), n);
	else
		_object.SetAttrValue(_index, n);
}

void Object::Attribute::setValue(double n)
{
	if (_index == -1)
		_object.SetAttrValue(_name.c_str(), n);
	else
		_object.SetAttrValue(_index, n);
}

void Object::Attribute::setValue(const string &s)
{
	if (_index == -1)
		_object.SetAttrValue(_name.c_str(), s.c_str());
	else
		_object.SetAttrValue(_index, s.c_str());
}

void Object::Attribute::setValue(const Date &d)
{
	THROW_EX(Unimplemented);
//		OleDateTime dt(d.year(), d.month(), d.day(), 0, 0, 0);
//		_field->PutValue(&_variant_t(dt));
}

void Object::Attribute::setValue(OClob &lob)
{
	if (_index == -1)
		_object.SetAttrValue(_name.c_str(), lob);
	else
		_object.SetAttrValue(_index, lob);
}

void Object::Attribute::setValue(OBlob &lob)
{
	if (_index == -1)
		_object.SetAttrValue(_name.c_str(), lob);
	else
		_object.SetAttrValue(_index, lob);
}

void Object::Attribute::setValue(OObject &o)
{
		if (_index == -1)
		_object.SetAttrValue(_name.c_str(), o);
	else
		_object.SetAttrValue(_index, o);
}

void Object::Attribute::setValue(ORef &r)
{
	if (_index == -1)
		_object.SetAttrValue(_name.c_str(), r);
	else
		_object.SetAttrValue(_index, r);
}

void Object::Attribute::setValue(const OCollection &c)
{
	if (_index == -1)
		_object.SetAttrValue(_name.c_str(), c);
	else
		_object.SetAttrValue(_index, c);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
