
#include "database.h"
#include "cursor.h"

#include "magenta/types/datetime/date.h"
#include "magenta/types/datetime/ole-datetime.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Manager
///////////////////////////////////////////////////////////////////////////////////////////////

Manager::Manager()
{
	OStartup(OSTARTUP_APARTMENTTHREADED);
}

//---------------------------------------------------------------------------------------------

Manager::~Manager()
{
	OShutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Session
///////////////////////////////////////////////////////////////////////////////////////////////

Manager Session::_manager;

///////////////////////////////////////////////////////////////////////////////////////////////

Session::Session(const string &spec)
{
	oresult rc = _database.Open(spec.c_str(), "system", "manager");
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

//---------------------------------------------------------------------------------------------

Session::Session(const string &server, const string &username, const string &passwd)
{
	oresult rc = _database.Open(server.c_str(), username.c_str(), passwd.c_str());
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

//---------------------------------------------------------------------------------------------

Session::~Session()
{
	_database.Close();
}

///////////////////////////////////////////////////////////////////////////////////////////////

Cursor Session::openCursor(const string &command)
{
	return Cursor(*this, command);
}

//---------------------------------------------------------------------------------------------

void Session::execute(const string &command)
{
	try
	{
		oresult rc = _database.ExecuteSQL(command.c_str());
		if (rc != OSUCCESS)
			THROW_EX(Exception)("execute() error");
	}
	catch (...) { THROW_EX(Exception)("execute() error"); }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Session::beginTrans()
{
	try
	{
		oresult rc = _database.BeginTrans();
		if (rc != OSUCCESS)
			THROW_EX(Exception)("beginTrans error");
	}
	catch (...) { THROW_EX(Exception)("beginTrans error"); }
}

//---------------------------------------------------------------------------------------------

void Session::commit()
{
	try
	{
		oresult rc = _database.CommitTrans();
		if (rc != OSUCCESS)
			THROW_EX(Exception)("error in command");
	}
	catch (...) { THROW_EX(Exception)("commit error"); }
}

//---------------------------------------------------------------------------------------------

void Session::rollback()
{
	try
	{
		oresult rc = _database.Rollback();
		if (rc != OSUCCESS)
			THROW_EX(Exception)("error in command");
	}
	catch (...) { THROW_EX(Exception)("rollback error"); }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Field
///////////////////////////////////////////////////////////////////////////////////////////////

Field::Field(Fields &fields, const string &name)
{
	_field = fields[name];
}

//---------------------------------------------------------------------------------------------

Field::Field(Fields &fields, int i)
{
	_field = fields[i];
}

///////////////////////////////////////////////////////////////////////////////////////////////

string Field::name() const
{
	return _field.GetName();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Fields
///////////////////////////////////////////////////////////////////////////////////////////////

Field Fields::field(const string &name)
{
	return Field(_fields.GetField(name.c_str()));
}

//---------------------------------------------------------------------------------------------

Field Fields::field(int i)
{
	return Field(_fields.GetField(i));
}

///////////////////////////////////////////////////////////////////////////////////////////////

int Fields::count() const
{
	return _fields.GetCount();
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
