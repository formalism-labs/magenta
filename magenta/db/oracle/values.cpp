
#include "values.h"
#include "magenta/db/oracle/collection.h"
#include "magenta/db/oracle/object.h"

#include "magenta/types/datetime/date.h"
#include "magenta/types/datetime/ole-datetime.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// IValue
///////////////////////////////////////////////////////////////////////////////////////////////

IValue::operator bool() const
{
	try 
	{ 
		int n;
		getValue(n);
		return !!n; 
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator double() const
{
	try 
	{ 
		double n;
		getValue(n);
		return n; 
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator long() const
{
	try 
	{ 
		long n;
		getValue(n);
		return n; 
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator int() const
{
	try 
	{ 
		int n;
		getValue(n);
		return n; 
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator string() const
{
	try 
	{ 
		string s;
		getValue(s);
		return s; 
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator String() const
{
	try 
	{ 
		string s;
		getValue(s);
		return s; 
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator Date() const
{
	try 
	{
		string s;
		getValue(s);
		int d, m, y;
		int rc = sscanf(s.c_str(), "%d/%d/%d", &m, &d, &y);
		return Date(d, m, y);
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator Text() const
{
	try 
	{
		OClob lob;
		getValue(lob);
		return lob;
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator Binary() const
{
	try 
	{
		OBlob lob;
		getValue(lob);
		return lob;
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator Object() const
{
	try 
	{ 
		OObject o;
		getValue(o);
		return o;
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator Ref() const
{
	try 
	{ 
		ORef r;
		getValue(r);
		return r;
	} 
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

IValue::operator Collection() const
{
	try 
	{ 
		OCollection c;
		getValue(c);
		return c;
	}
	catch (OException &x) 
	{
		THROW_EX(ReadError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void IValue::_setValue(bool b)
{
	try 
	{
		setValue(b ? 1 : 0); 
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(int n)
{
	try 
	{ 
		setValue(n); 
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(long n)
{
	try 
	{ 
		setValue(n); 
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(double n)
{
	try 
	{ 
		setValue(n); 
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(const string &s)
{
	try 
	{ 
		setValue(s);
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(const Date &d)
{
	try 
	{
		char s[20];
		sprintf(s, "%d/%d/%d", d.month(), d.day(), d.year());
		setValue(string(s));
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(Text &lob)
{
	try 
	{ 
		setValue(lob); 
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(Binary &lob)
{
	try 
	{ 
		setValue(lob); 
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(Object &o)
{
	try 
	{ 
		setValue(o); 
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(Ref &r)
{
	try 
	{ 
		setValue(r);
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

//---------------------------------------------------------------------------------------------

void IValue::_setValue(const Collection &c)
{
	try 
	{  
		setValue(c);
	} 
	catch (OException &x) 
	{
		THROW_EX(WriteError)("%s (%d)", x.GetErrorText(), x.GetErrorNumber()); 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Values
///////////////////////////////////////////////////////////////////////////////////////////////

CValues::~CValues()
{
	for (Values::iterator i = _values.begin(); i != _values.end(); ++i)
		delete *i;	
}

//---------------------------------------------------------------------------------------------

IValue &CValuesList::operator[](int i)
{
	IValue *v = getValue(i);
	_values.push_back(v);
	return *v;
}

//---------------------------------------------------------------------------------------------

IValue &CValuesMap::operator[](const string &name)
{
	IValue *v = getValue(name);
	_values.push_back(v);
	return *v;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
