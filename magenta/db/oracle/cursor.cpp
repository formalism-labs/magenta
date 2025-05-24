
#include "cursor.h"

#include "magenta/types/datetime/date.h"
#include "magenta/types/datetime/ole-datetime.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Cursor
///////////////////////////////////////////////////////////////////////////////////////////////

Cursor::Cursor(Cursor &cursor) : _session(cursor._session)
{
	_dynaset = cursor._dynaset.Clone();
}

//---------------------------------------------------------------------------------------------

Cursor::Cursor(Session &session, const string &command) :
	_session(session)
{
	try
	{
		oresult rc = _dynaset.Open(session, command.c_str());
		if (rc != OERROR_NONE)
			THROW_EX(Exception);
	}
	catch (...) { THROW_EX(Exception); }
}

//---------------------------------------------------------------------------------------------

Cursor::~Cursor()
{
	_dynaset.Close();
}

///////////////////////////////////////////////////////////////////////////////////////////////

int Cursor::fieldCount() const
{
	return _dynaset.GetFieldCount();
}

//---------------------------------------------------------------------------------------------

long Cursor::recordCount() const
{
	return _dynaset.GetRecordCount();
}

//---------------------------------------------------------------------------------------------

bool Cursor::isEmpty() const
{
	return isBOF() && isEOF();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Cursor::moveFirst()
{
	oresult rc = _dynaset.MoveFirst();
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

//---------------------------------------------------------------------------------------------

void Cursor::moveLast()
{
	oresult rc = _dynaset.MoveLast();
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

//---------------------------------------------------------------------------------------------

void Cursor::movePrevious()
{
	oresult rc = _dynaset.MovePrev();
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

//---------------------------------------------------------------------------------------------

void Cursor::moveNext()
{
	oresult rc = _dynaset.MoveNext();
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

//---------------------------------------------------------------------------------------------

void Cursor::move(long records)
{
	oresult rc = _dynaset.MoveRel(records);
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool Cursor::isBOF() const
{
	return !!_dynaset.IsBOF();
}

//---------------------------------------------------------------------------------------------

bool Cursor::isEOF() const
{
	return !!_dynaset.IsEOF();
}

//---------------------------------------------------------------------------------------------

bool Cursor::isOff() const
{
	return isBOF() || isEOF();
}

///////////////////////////////////////////////////////////////////////////////////////////////

Cursor &Cursor::operator++()
{
	moveNext();
	return *this;
}

//---------------------------------------------------------------------------------------------

Cursor &Cursor::operator--()
{
	movePrevious();
	return *this;
}

//---------------------------------------------------------------------------------------------

bool Cursor::operator!() const
{
	return isOff();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Cursor::append()
{
	oresult rc = _dynaset.AddNewRecord();
	if (rc != OERROR_NONE)
	{
		const char *s = _dynaset.GetSession().GetErrorText();
		THROW_EX(Exception);
	}
}

//---------------------------------------------------------------------------------------------

void Cursor::remove()
{
	oresult rc = _dynaset.DeleteRecord();
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

//---------------------------------------------------------------------------------------------

void Cursor::edit()
{
	oresult rc = _dynaset.StartEdit();
	if (rc != OERROR_NONE)
	{
		const char *s = _dynaset.GetSession().GetErrorText();
		THROW_EX(Exception);
	}
}

//---------------------------------------------------------------------------------------------

void Cursor::update()
{
	oresult rc = _dynaset.Update();
	if (rc != OERROR_NONE)
		THROW_EX(Exception);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Cursor::Field
///////////////////////////////////////////////////////////////////////////////////////////////

Cursor::Field::Field(Cursor &cursor, const string &name) : 
	_cursor(cursor),
	__NAMESPACE::Field(cursor._dynaset.GetField(name.c_str()))
{
}

//---------------------------------------------------------------------------------------------

Cursor::Field::Field(Cursor &cursor, int i) : 
	_cursor(cursor),
	__NAMESPACE::Field(cursor._dynaset.GetField(i))
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Cursor::Field::getValue(long &n) const
{
	n = _field;
}

void Cursor::Field::getValue(int &n) const
{
	n = _field;
}

void Cursor::Field::getValue(double &n) const
{
	n = _field;
}

void Cursor::Field::getValue(string &s) const
{
	s = (const char *) _field;
}

void Cursor::Field::getValue(Date &d) const
{
	THROW_EX(Unimplemented);
}

void Cursor::Field::getValue(OClob &lob) const
{
	lob = OClob(_field);
	if (lob.IsNull())
	{
		initLOB();
		lob = OClob(_field);
	}
}

void Cursor::Field::getValue(OBlob &lob) const
{
	lob = OBlob(_field);
	if (lob.IsNull())
	{
		initLOB();
		lob = OBlob(_field);
	}
}

void Cursor::Field::getValue(OObject &o) const
{
	o = (OObject) _field;
}

void Cursor::Field::getValue(ORef &r) const
{
	r = (ORef) _field;
}

void Cursor::Field::getValue(OCollection &c) const
{
	c = (OCollection) _field;
}

//---------------------------------------------------------------------------------------------

void Cursor::Field::setValue(int n)
{
	_field.SetValue(n);
}

void Cursor::Field::setValue(long n)
{
	_field.SetValue(n);
}

void Cursor::Field::setValue(double n)
{
	_field.SetValue(n);
}

void Cursor::Field::setValue(const string &s)
{
	_field.SetValue(s.c_str());
}

void Cursor::Field::setValue(const Date &d)
{
	THROW_EX(Unimplemented);
}

void Cursor::Field::setValue(OClob &lob)
{
	_field.SetValue(lob);
}

void Cursor::Field::setValue(OBlob &lob)
{
	_field.SetValue(lob);
}

void Cursor::Field::setValue(OObject &o)
{
	_field.SetValue(o);
}

void Cursor::Field::setValue(ORef &r)
{
	_field.SetValue(r);
}

void Cursor::Field::setValue(const OCollection &c)
{
	_field.SetValue(c);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Cursor::Field::initLOB() const
{
	bool edit_mode = _cursor._dynaset.GetEditMode() == ODYNASET_EDIT_EDITING;
	if (!edit_mode)
		_cursor.edit();
	OValue v;
	v.SetEmpty();
	oresult rc = _cursor._dynaset.SetFieldValue(name().c_str(), v);
	_cursor.update();
	if (edit_mode)
		_cursor.edit();
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
