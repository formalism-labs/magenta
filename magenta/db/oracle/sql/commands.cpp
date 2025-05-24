
#pragma warning(disable : 4786)

#include "commands.h"
//#include "magenta/db/oracle/defs.h"

namespace magenta
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

ObjectName::operator string() const
{
	return _schema + "." + _name;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CommaList
///////////////////////////////////////////////////////////////////////////////////////////////

CommaList::CommaList(const Strings &strings)
{
	for (Strings::ConstIterator i = strings.begin(); !!i; ++i)
		*this += *i;
}

//---------------------------------------------------------------------------------------------

CommaList &CommaList::operator+=(const string &s)
{
	if (empty())
		*this = s;
	else
		(string &) *this += ", " + s;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// SQL Object
///////////////////////////////////////////////////////////////////////////////////////////////

void SQLObject::value(const SQLConstant &val)
{
	_values += val;
}

//---------------------------------------------------------------------------------------------

SQLObject::operator string() const
{
	return _type.name() + "("  + _values + ")";
}

///////////////////////////////////////////////////////////////////////////////////////////////
// SQL Ref
///////////////////////////////////////////////////////////////////////////////////////////////

SQLRef::operator string() const
{
	char s[256];
	char sp[] = "(select ref(p) from %s p where id = %d)";
	char sq[] = "(select ref(q) from %s q where id = %d)";
	string name = type().name();
	int rc = sprintf(s, name == "p" ? sq : sp, name.c_str(), id());
	return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// SQL Constant
///////////////////////////////////////////////////////////////////////////////////////////////

SQLConstant::SQLConstant(double n)
{
	char s[50];
	int rc = sprintf(s, "%f", n);
	_const = s;
}

SQLConstant::SQLConstant(long n)
{
	char s[50];
	int rc = sprintf(s, "%ld", n);
	_const = s;
}

SQLConstant::SQLConstant(int n)
{
	char s[50];
	int rc = sprintf(s, "%ld", n);
	_const = s;
}

//---------------------------------------------------------------------------------------------

SQLConstant::SQLConstant(const string &s)
{
	_const = string("'") + s.c_str() + "'";
}

SQLConstant::SQLConstant(const String &s)
{
	_const = string("'") + s.c_str() + "'";
}

//---------------------------------------------------------------------------------------------

SQLConstant::SQLConstant(const Date &date)
{
	char s[50];
	int rc = sprintf(s, "to_date('%02d/%02d/%04d', 'DD/MM/YYYY')", date.day(), 
		date.month(), date.year());
	_const = s;
}

//---------------------------------------------------------------------------------------------

SQLConstant::SQLConstant(const SQLExpression &exp)
{
	_const = exp;
}

//---------------------------------------------------------------------------------------------

SQLConstant::SQLConstant(const SQLRef &ref)
{
	_const = string(ref);
}

//---------------------------------------------------------------------------------------------

SQLConstant::SQLConstant(const SQLObject &object)
{
	_const = string(object);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Nested Table Expression
///////////////////////////////////////////////////////////////////////////////////////////////

NestedTableExpression::NestedTableExpression(const ObjectName &parentTable, 
	const string &nestedTable, const SQLConstant &id)
{
	_exp = "table(select " + string(nestedTable) + " from " + string(parentTable) +
		" where id=" + string(id) + ")";
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ReturningClause
///////////////////////////////////////////////////////////////////////////////////////////////

void ReturningClause::returning(const SQLExpression &expr, const string &into)
{
	_returning += string(_returning.empty() ? "" : ", ") + expr;
	_into += string(_into.empty() ? "" : ", ") + ":" + into;
}

//---------------------------------------------------------------------------------------------

string ReturningClause::sql() const
{
	return string(" returning ") + _returning + " into " + _into;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Commands
///////////////////////////////////////////////////////////////////////////////////////////////

namespace Commands
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Select
///////////////////////////////////////////////////////////////////////////////////////////////

void Select::column(const string &name)
{
	_columns += name;
	_prepared = false;
}

//---------------------------------------------------------------------------------------------

void Select::table(const string &name, const string &alias)
{
	if (!alias.empty())
	{
		if (!!_aliases.find(name))
			return;

		_aliases.push_back(alias);
		_tables.push_back(name + " " + alias);
		_prepared = false;
		return;
	}

	Tables::Iterator i = _tables.find(name);
	if (!i)
		_tables.push_back(name);
	_prepared = false;
}

//---------------------------------------------------------------------------------------------

void Select::column(const string &table_, const string &name, const string &alias)
{
	column(table_ + "." + name + " " + alias);
	table(table_);
}

//---------------------------------------------------------------------------------------------

void Select::prepare() const
{
	string columns = !_columns ? string("*") : string(_columns);
	_sql = string("select ") + columns + " from " + CommaList(_tables);
	if (!_criteria.empty())
		_sql += " where " + _criteria;
	if (!_order.empty())
		_sql += " order by " + _order;
	if (!_grouping.empty())
	{
		_sql += " group by " + _grouping;
		if (!group_criteria.empty())
			_sql += " having " + _grouping;
	}
	_prepared = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Update
///////////////////////////////////////////////////////////////////////////////////////////////

Update::Update(const string &table)
{
	_sql = "update " + table;
}

//---------------------------------------------------------------------------------------------

void Update::prepare() const
{
	_sql += " set " + _columns;
	if (!_criteria.empty())
		_sql += " where " + _criteria;
	if (!_returning.empty())
		_sql += ReturningClause::sql();
	_prepared = true;
}

//---------------------------------------------------------------------------------------------

void Update::column(const string &name, const SQLConstant &const_)
{
	_columns += name + " = " + string(const_);
	_prepared = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Insert
///////////////////////////////////////////////////////////////////////////////////////////////

Insert::Insert(const string &table)
{
	_sql = "insert into " + table;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void InsertValues::prepare() const
{
	_sql += " (" + _columns + ") values (" + _values + ")";
	if (!_returning.empty())
		_sql += ReturningClause::sql();
	_prepared = true;
}

//---------------------------------------------------------------------------------------------

void InsertValues::column(const string &name, const SQLConstant &const_)
{
	_columns += name;
	_values += const_;
	_prepared = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////

InsertRows::InsertRows(const string &table, const Select &select) : 
	Insert(table), _select(select.sql())
{
}

//---------------------------------------------------------------------------------------------

void InsertRows::prepare() const
{
	_sql += " (" + _columns + ") " + _select;
	if (!_returning.empty())
		_sql += ReturningClause::sql();
	_prepared = true;
}

//---------------------------------------------------------------------------------------------

void InsertRows::column(const string &name)
{
	_columns += name;
	_prepared = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Delete
///////////////////////////////////////////////////////////////////////////////////////////////

Delete::Delete(const string &table, const string &criteria)
{
	_sql = "delete from " + table + " where " + criteria + "";
}

//---------------------------------------------------------------------------------------------

void Delete::prepare() const
{
	if (!_returning.empty())
		_sql += ReturningClause::sql();
	_prepared = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CreateTypeCommand
///////////////////////////////////////////////////////////////////////////////////////////////

CreateTypeCommand::CreateTypeCommand(const ObjectName &name) :
	type_name(name)
{
}

//---------------------------------------------------------------------------------------------

void CreateTypeCommand::prepare() const
{
	_sql += "(\n";
	bool first = true;
	for (Attributes::ConstIterator i = _attributes; !!i; ++i)
	{
		if (first)
			first = false;
		else
			_sql += ",\n";
		_sql += "\t" + string(*i);
	}
	_sql += string("\n);") + (requiresSlashTerminator() ? "\n/" : "");

	_prepared = true;
}

//---------------------------------------------------------------------------------------------

void CreateTypeCommand::attribute(const string &name, const string &type, const string &options)
{
	_prepared = false;
	_attributes.push_back(Attribute(name, type, options));
}

//---------------------------------------------------------------------------------------------

CreateTypeCommand::Attribute::operator string() const
{
	string s = _name + " " + _type;
	if (!_options.empty())
		s += " " + _options;
	return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CreateType
///////////////////////////////////////////////////////////////////////////////////////////////

CreateType::CreateType(const ObjectName &name) : CreateTypeCommand(name)
{
	_sql = "create type " + type_name + " as object\n";
}

//---------------------------------------------------------------------------------------------

string CreateType::incompleteForm() const
{
	return "create type " + type_name + ";\n/";
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CreateTypeAsTable
///////////////////////////////////////////////////////////////////////////////////////////////

CreateTypeAsTable::CreateTypeAsTable(const ObjectName &name, 
	const ObjectName &type)
{
	_sql = "create type " + string(name) + " as table of " + string(type) +";\n/";
	_prepared = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CreateTable
///////////////////////////////////////////////////////////////////////////////////////////////

CreateTable::CreateTable(const ObjectName &name) : CreateTypeCommand(name)
{
	_sql = "create table " + type_name + "\n";
}

//---------------------------------------------------------------------------------------------

void CreateTable::column(const string &name, const string &type, const string &options)
{
	attribute(name, type, options);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CreateObjectTable
///////////////////////////////////////////////////////////////////////////////////////////////

CreateObjectTable::CreateObjectTable(const ObjectName &name, const ObjectName &type)
{
	_name = string(name);
	_type = string(type);
	_prepared = false;
}

//---------------------------------------------------------------------------------------------

void CreateObjectTable::primaryKey(const string &name)
{
	primary_key = name;
	_prepared = false;
}

//---------------------------------------------------------------------------------------------

void CreateObjectTable::nestedTable(const string &name, const string &store)
{
	nested_tables.push_back("nested table " + name + " store as " + store);
	_prepared = false;
}

//---------------------------------------------------------------------------------------------

void CreateObjectTable::prepare() const
{
	_sql = "create table " + _name + " of " + _type;
	if (!primary_key.empty())
		_sql += "\n\t(primary key (" + primary_key + "))";
	for (NestedTables::ConstIterator i = nested_tables; !!i; ++i)
		_sql += "\n\t" + *i;
	_sql += ";";
	_prepared = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CreateView
///////////////////////////////////////////////////////////////////////////////////////////////

CreateView::CreateView(const ObjectName &name, Select &query) :
	_name(name), _query(query)
{
}

//---------------------------------------------------------------------------------------------

void CreateView::prepare() const
{
	_sql = "create view " + _name;
	if (!_columns.empty())
		_sql += " (" + _columns + ")";
	_sql += " as " + _query;
	_prepared = true;
}

//---------------------------------------------------------------------------------------------

void CreateView::column(const string &name)
{
	if (!_columns.empty())
		_columns += ", ";
	_columns += name;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CreateSequence
///////////////////////////////////////////////////////////////////////////////////////////////

CreateSequence::CreateSequence(const ObjectName &name)
{
	_sql = "create sequence " + string(name) + ";";
	_prepared = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CreateUser
///////////////////////////////////////////////////////////////////////////////////////////////

CreateUser::CreateUser(const string &user, const string &password, 
	const string &defaultTablespace, const string &tempTablespace)
{
	_user = user;
	_sql += "create user " + user + "\n";
	_sql += "\tprofile default\n";
	_sql += "\tidentified by " + password + "\n";
	_sql += "\tdefault tablespace " + defaultTablespace + "\n";
	_sql += "\ttemporary tablespace " + tempTablespace + "\n";
	_sql += "\taccount unlock;";
	_prepared = true;
}

//---------------------------------------------------------------------------------------------

void CreateUser::privilege(const string &privilege)
{
	_sql += "\n" "grant " + privilege + " to " + _user + ";";
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Commands

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
