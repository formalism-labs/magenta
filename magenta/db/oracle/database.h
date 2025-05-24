
#ifndef _magenta_db_oracle_database_
#define _magenta_db_oracle_database_

#include "magenta/db/oracle/general.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Manager
{
public:
	Manager();
	~Manager();
};

///////////////////////////////////////////////////////////////////////////////////////////////
#if 0
class Parameter
{
	string _name;
	int server_type;

public:
	Parameter(const string &name, int serverType) : 
		_name(name), server_type(serverType) {}

	const string &name() const { return _name; }
	int serverType() const { return server_type; }
};

//---------------------------------------------------------------------------------------------

class Paremeters
{
	typedef This Parameters;

	Session &_session;

public:
	Parameters(Session &session) : _session(session) {}

	This &operator<<(const Parameter &param);

	operator[](const sting &name)
};
#endif
///////////////////////////////////////////////////////////////////////////////////////////////

class Session
{
	static Manager _manager;
	ODatabase _database;

    friend Cursor;
	friend Object;

//protected:
public:
	operator ODatabase&() { return _database; }

public:
    Session(const string &spec);
	Session(const string &server, const string &username, const string &passwd);
	~Session();

	Cursor openCursor(const string &command);
	void execute(const string &command);

	void beginTrans();
	void commit();
	void rollback();

	CLASS_EXCEPTION_DEF("magenta::Session");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Field
{
	friend Fields;

protected:
	OField _field;

protected:
	Field(OField &field) : _field(field) {}

public:
	Field(Fields &fields, const string &name);
	Field(Fields &fields, int i);
	Field(Field &field) : _field(field._field) {}

public:
	string name() const;
	operator OField&() { return _field; }

public:
	CLASS_EXCEPTION_DEF("magenta::Field");
	CLASS_SUB_EXCEPTION_DEF(NameError, "inproper name");
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Fields
{
	friend Field;

protected:
	OFieldCollection _fields;

protected:
	Fields(OFieldCollection fields) : _fields(fields) {}

public:
	Fields(Fields &fields) : _fields(fields._fields) {}

	Field field(const string &name);
	Field operator[](const string &name) { return field(name); }

	Field field(int i);
	Field operator[](int i) { return field(i); }

	int count() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Type
///////////////////////////////////////////////////////////////////////////////////////////////

class Type
{
	string _name;

public:
	Type(const string &name) : _name(name) {}

	operator string() const { return _name; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_db_oracle_database_
