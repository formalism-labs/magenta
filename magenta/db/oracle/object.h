
#ifndef _magenta_db_oracle_object_
#define _magenta_db_oracle_object_

#include "magenta/db/oracle/general.h"
#include "magenta/db/oracle/cursor.h"
#include "magenta/db/oracle/large-objects.h"
#include "magenta/db/oracle/values.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Object
///////////////////////////////////////////////////////////////////////////////////////////////

class Object : public CValuesMap
{
	friend IValue;
	friend Cursor::Field;

protected:
	OObject _object;

public:
	operator OObject&() { return _object; }

//protected:
//	Object();
//	void operator=(const Object &o) { _object = o._object; }

public:
	Object(Session &session, const Type &type) : _object(session, string(type).c_str()) {}
	Object(const OObject &object) : _object(object) {}

public:
	string name() const;
	Type type() const;

	class Attribute : public IValue
	{
		OObject &_object;
		int _index;
		string _name;

	public:
		Attribute(Object &o, const string &name) : 
			_object(o._object), _name(name), _index(-1) {}
		Attribute(Object &o, int index) : 
			_object(o._object), _index(index) {}

		Type type() const;
		
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
		CLASS_EXCEPTION_DEF("magenta::db::object::attribute error");
		CLASS_SUB_EXCEPTION_DEF(ReadError, "open error");
		CLASS_SUB_EXCEPTION_DEF(WriteError, "close error");
	};
	
	friend Attribute;

	Attribute operator[](const string &name) { return Attribute(*this, name); }

	int attributeCount() const;

protected:
	IValue *getValue(int i) { return new Attribute(*this, i); }
	IValue *getValue(const string &name) { return new Attribute(*this, name); }

public:
	int valuesCount() const { return attributeCount(); }

	bool operator!() const { return _object.IsNull(); }

	operator OValue() { return OValue(_object); }

public:
	CLASS_EXCEPTION_DEF("magenta::Object");
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Ref
///////////////////////////////////////////////////////////////////////////////////////////////

class Ref
{
	friend IValue;
	friend Cursor::Field;
	friend Object::Attribute;

	ORef _ref;

//protected:
//	Ref();
//	void operator=(const Ref &r) { _ref = r._ref; }

public:
	Ref(ORef &ref) : _ref(ref) {}
//	Ref(Session &session, const string &name);

	operator ORef&() { return _ref; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_db_oracle_object_
