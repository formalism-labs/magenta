
#ifndef _magenta_db_oracle_cursor_
#define _magenta_db_oracle_cursor_

#include "magenta/db/oracle/general.h"
#include "magenta/db/oracle/database.h"
#include "magenta/db/oracle/values.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class Cursor : public CValuesMap
{
    friend Session;

    Session &_session;
	ODynaset _dynaset;

protected:
	Cursor(ODynaset &dynaset);

public:
    Cursor(Cursor &cursor);
    Cursor(Session &session, const string &command);
    ~Cursor();

    //-----------------------------------------------------------------------------------------
    // Record position
    //-----------------------------------------------------------------------------------------
public:
	long recordCount() const;
	bool isEmpty() const;

	void moveFirst();
	void moveLast();
	void movePrevious();
	void moveNext();
	void move(long records);

	bool isBOF() const;
	bool isEOF() const;
	bool isOff() const;

	Cursor &operator++();
	Cursor &operator--();
	bool operator!() const;

    //-----------------------------------------------------------------------------------------
    // Content Operations
    //-----------------------------------------------------------------------------------------
public:
	void append();
	void edit();
	void update();
	void remove();

    //-----------------------------------------------------------------------------------------
    // Exceptions
    //-----------------------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::Cursor");
	CLASS_SUB_EXCEPTION_DEF(OpenError, "open error");
	CLASS_SUB_EXCEPTION_DEF(CloseError, "close error");
	CLASS_SUB_EXCEPTION_DEF(MoveError, "move error");

    //-----------------------------------------------------------------------------------------
    // Field
    //-----------------------------------------------------------------------------------------
public:
	class Field : public __NAMESPACE::Field, public IValue
	{
		Cursor &_cursor;

	public:
		Field(Cursor &cursor, const string &name);
		Field(Cursor &cursor, int i);

		DEFINE_IValue_Assignment_Operators;

	protected:
		void getValue(int &n) const;
		void getValue(long&n) const;
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
		void initLOB() const;

		CLASS_EXCEPTION_DEF("magenta::Field");
	};

	friend Field;

	Field field(const string &name) { return Field(*this, name); }
	Field operator[](const string &name) { return field(name); }
	Field field(int i) { return Field(*this, i); }
	Field operator[](int i) { return field(i); }

	int fieldCount() const;

protected:
	IValue *getValue(int i) { return new Field(*this, i); }
	IValue *getValue(const string &name) { return new Field(*this, name); }

public:
	int valuesCount() const { return fieldCount(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_db_oracle_cursor_
