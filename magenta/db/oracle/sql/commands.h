
#ifndef _magenta_db_oracle_sql_commands_
#define _magenta_db_oracle_sql_commands_

#include "magenta/db/oracle/sql/classes.h"

#include "magenta/types/defs.h"
#include "magenta/types/datetime/date.h"

#include <string>

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class CommaList : public string	
{
public:
	typedef List<string> Strings;

	CommaList() {}
	CommaList(const string &s) : string(s) {}
	CommaList(const Strings &strings);

	CommaList &operator+=(const string &s);

	bool operator!() const { return empty(); }
};

//---------------------------------------------------------------------------------------------

class ObjectName
{
	string _schema, _name;

public:
	ObjectName(const string &schema, const string &name) : _schema(schema),
		_name(name) {}

	const string &schema() const { return _schema; }
	const string &name() const { return _name; }

	operator string() const;
};

//---------------------------------------------------------------------------------------------

class SQLExpression : public string
{
public:
	SQLExpression(const string &s) : string(s) {}
};

//---------------------------------------------------------------------------------------------

class SQLType
{
	ObjectName _name;

public:
	SQLType(const ObjectName &name) : _name(name) {}

	string name() const { return _name; }
};

//---------------------------------------------------------------------------------------------

class SQLRef
{
	SQLType _type;
	int _id;

public:
	SQLRef(const SQLType &type, int objectId) : 
		_type(type), _id(objectId) {}

	const SQLType &type() const { return _type; }
	int id() const { return _id; }

	operator string() const;
};

//---------------------------------------------------------------------------------------------

class SQLObject
{
	SQLType _type;
	CommaList _values;

public:
	SQLObject(const SQLType &type) : _type(type) {}

	void value(const SQLConstant &val);

	operator string() const;
};

//---------------------------------------------------------------------------------------------

class SQLConstant
{
	string _const;

public:
	SQLConstant(double n);
	SQLConstant(long n);
	SQLConstant(int n);
	SQLConstant(const string &s);
	SQLConstant(const String &s);
	SQLConstant(const Date &date);
	SQLConstant(const SQLExpression &exp);
	SQLConstant(const SQLObject &object);
	SQLConstant(const SQLRef &ref);

	operator string() const { return _const; }
};

//---------------------------------------------------------------------------------------------

class NestedTableExpression
{
	string _exp;

public:
	NestedTableExpression(const ObjectName &parentTable, 
		const string &nestedTable, const SQLConstant &id);

	operator string() const { return _exp; }
};

//---------------------------------------------------------------------------------------------

class ReturningClause
{
protected:
	string _returning, _into;

	string sql() const;
	
public:
	void returning(const SQLExpression &expr, const string &into);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Command
{
protected:
	mutable string _sql;

	mutable bool _prepared;
	virtual void prepare() const { _prepared = true; }

	Command() : _prepared(false) {}

public:
	Command(const string &sql) : _sql(sql), _prepared(false) {}

	string sql() const
	{
		if (!_prepared)
			prepare();
		return _sql; 
	}

	operator string() const { return sql(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

namespace Commands
{


///////////////////////////////////////////////////////////////////////////////////////////////

class Select : public Command
{
	CommaList _columns;
	typedef List<string> Tables;
	Tables _tables, _aliases;
	string _criteria, _order, _grouping, group_criteria;

	void prepare() const;

public:
	Select() {}

	void column(const string &name);
	void table(const string &name, const string &alias = "");
	void column(const string &table, const string &name, const string &alias);
	void where(const string &criteria) { _criteria = criteria; _prepared = false; }
	void orderBy(const string &order) { _order = order; _prepared = false; }
	void group(const string &grouping) { _grouping = grouping; _prepared = false; }
	void having(const string &criteria) { group_criteria = criteria; _prepared = false; }

	string criteria() const { return _criteria; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Update : public Command, public ReturningClause
{
	CommaList _columns;
	string _criteria;

	void prepare() const;

public:
	Update(const string &table);

	void column(const string &name, const SQLConstant &exp);
	void where(const string &criteria) { _criteria = criteria; _prepared = false; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Insert : public Command, public ReturningClause
{
protected:
	CommaList _columns;

	virtual void prepare() const = 0;

public:
	Insert(const string &table);
};

//---------------------------------------------------------------------------------------------

class InsertValues : public Insert
{
	CommaList _values;

	void prepare() const;

public:
	InsertValues(const string &table) : Insert(table) {}

	void column(const string &name, const SQLConstant &exp);
};

//---------------------------------------------------------------------------------------------

class InsertRows : public Insert
{
	string _select;
	
	void prepare() const;

public:
	InsertRows(const string &table, const Select &select);

	void column(const string &name);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class Delete : public Command, public ReturningClause
{
	void prepare() const;

public:
	Delete(const string &table, const string &criteria);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CreateTypeCommand : public Command
{
protected:
	class Attribute
	{
		friend CreateTypeCommand;

		string _name, _type, _options;

	public:
		Attribute(const string &name, const string &type, const string &options = "") :
			_name(name), _type(type), _options(options) {}

		operator string() const;
	};

	string type_name;

	typedef Vector<Attribute> Attributes;
	Attributes _attributes;

	void prepare() const;
	virtual bool requiresSlashTerminator() const { return true; }

public:
	CreateTypeCommand(const ObjectName &name);

	void attribute(const string &name, const string &type, const string &options = "");
};

//---------------------------------------------------------------------------------------------

class CreateType : public CreateTypeCommand
{
public:
	CreateType(const ObjectName &name);

	string incompleteForm() const;
};

//---------------------------------------------------------------------------------------------

class CreateTypeAsTable : public Command
{
public:
	CreateTypeAsTable(const ObjectName &name, const ObjectName &type);
};

//---------------------------------------------------------------------------------------------

class CreateTable : public CreateTypeCommand
{
protected:
	bool requiresSlashTerminator() const { return false; }
	
public:
	CreateTable(const ObjectName &name);

	void column(const string &name, const string &type, const string &options = "");
};

//---------------------------------------------------------------------------------------------

class CreateObjectTable : public Command
{
	string _name, _type, primary_key;

	typedef List<string> NestedTables;
	NestedTables nested_tables;

	void prepare() const;

public:
	CreateObjectTable(const ObjectName &name, const ObjectName &type);

	void primaryKey(const string &name);
	void nestedTable(const string &name, const string &store);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CreateView : public Command
{
	string _name, _columns, _query;

	void prepare() const;

public:
	CreateView(const ObjectName &name, Select &query);

	void column(const string &name);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CreateSequence : public Command
{
public:
	CreateSequence(const ObjectName &name);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class CreateUser : public Command
{
	string _user;

public:
	CreateUser(const string &user, const string &password, 
		const string &defaultTablespace = "users", 
		const string &tempTablespace = "temp");

	void privilege(const string &privilege);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Commands

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_db_oracle_sql_commands_
