
#include "key.h"
#include "value.h"

namespace magenta
{

namespace Registry
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

Value::Value(Key &key, const string &name) : _key(key), _name(name)
{
    _type = REG_SZ;
	unsigned long n;
    assert_(RegQueryValueEx(_key, _name.c_str(), 0, &_type, 0, &n));
}

//---------------------------------------------------------------------------------------------

Value::Value(Key &key, const string &name, Value::Type type) :
	_key(key), _name(name)
{
	switch (_type)
	{
	case Type::DWord:
		_type = REG_DWORD;
		break;

	case Type::String:
		_type = REG_SZ;
		break;

	case Type::Binary:
		_type = REG_BINARY;
		break;

	default:
		THROW_EX(Exception);
	}
}

//---------------------------------------------------------------------------------------------

Value::Value(Key &key, const string &name, unsigned long value) :
	_key(key), _name(name), _type(REG_DWORD)
{
}

//---------------------------------------------------------------------------------------------

Value::Value(Key &key, const string &name, const string &value) :
	_key(key), _name(name), _type(REG_SZ)
{
}

//---------------------------------------------------------------------------------------------

Value::Value(Key &key, const string &name, const IBlock &value) :
	_key(key), _name(name), _type(REG_BINARY)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Value::assert_(LONG rc)
{
	if (rc != ERROR_SUCCESS)
		THROW_EX(Exception);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Value::operator unsigned long() const 
{
	return ReadValue(*this);
}

//---------------------------------------------------------------------------------------------

Value::operator string() const 
{
	return ReadValue(*this);
}

//---------------------------------------------------------------------------------------------

Value::operator PersistentDynablock() const 
{
	return ReadValue(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Value &Value::operator=(unsigned long value)
{
	WriteValue(*this, value);
	return *this;
}

//---------------------------------------------------------------------------------------------

Value &Value::operator=(const string &s)
{
	WriteValue(*this, s);
	return *this;
}

//---------------------------------------------------------------------------------------------

Value &Value::operator=(const IBlock &block)
{
	WriteValue(*this, block);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Value::remove()
{
	assert_(RegDeleteValue(_key, _name.c_str()));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Value :: ReadValue
///////////////////////////////////////////////////////////////////////////////////////////////

Value::ReadValue::ReadValue(const Value &value) : _size(sizeof(_data))
{
	Value::assert_(RegQueryValueEx(value._key, value._name.c_str(), 0, 
		&_type, _data, &_size));
}

///////////////////////////////////////////////////////////////////////////////////////////////

Value::ReadValue::operator unsigned long() const 
{
	switch (_type)
	{
	case REG_SZ:
		char *p;
		return strtoul((const char *) _data, &p, 10);

	case REG_BINARY:
	case REG_DWORD:
		return Value::DWord(_data);

	default:
		THROW_EX(Exception);
	}
}

//---------------------------------------------------------------------------------------------

Value::ReadValue::operator string() const 
{
	switch (_type)
	{
	case REG_SZ:
	case REG_EXPAND_SZ:
		return string((const char *) _data, 0, _size);

	case REG_BINARY:
		return HexString(Block((char *) _data, _size));

	case REG_DWORD:
		char s[256];
		return string(_ultoa(Value::DWord(_data), s, 16));

	default:
		THROW_EX(Exception);
	}
}

//---------------------------------------------------------------------------------------------

Value::ReadValue::operator PersistentDynablock() const 
{
	return PersistentDynablock((char *) _data, _size);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Value :: WriteValue
///////////////////////////////////////////////////////////////////////////////////////////////

Value::WriteValue::WriteValue(const Value &value, unsigned long n) : 
	_value(value)
{
	switch (value._type)
	{
	case REG_SZ:
	case REG_EXPAND_SZ:
		char s[256];
		write(Block(_ultoa(value, s, 10), strlen(s)));
		break;
		
	case REG_BINARY:
	case REG_DWORD:
		write(Value::DWord(n));
		break;	

	default:
		THROW_EX(Exception);
	}
}

//---------------------------------------------------------------------------------------------

Value::WriteValue::WriteValue(const Value &value, const string &s) : 
	_value(value)
{
	switch (value._type)
	{
	case REG_SZ:
	case REG_EXPAND_SZ:
	case REG_BINARY:
		write(ConstBlock(s.c_str(), s.length()));
		break;

	case REG_DWORD:
		char *p;
		write(Value::DWord(strtoul(s.c_str(), &p, 10)));
		break;

	default:
		THROW_EX(Exception);
	}
}

//---------------------------------------------------------------------------------------------

Value::WriteValue::WriteValue(const Value &value, const IBlock &block) : 
	_value(value)
{
	switch (value._type)
	{
	case REG_SZ:
	case REG_EXPAND_SZ:
	case REG_BINARY:
		write(block);
		break;

	case REG_DWORD:
		write(Value::DWord(block.data()));
		break;

	default:
		THROW_EX(Exception);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Value::WriteValue::write(const IBlock &block)
{
	Value::assert_(RegSetValueEx(_value._key, _value._name.c_str(), 
		0L, _value._type, (const BYTE *) block.data(), block.length()));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Value :: HexString
///////////////////////////////////////////////////////////////////////////////////////////////

Value::HexString::HexString(const IBlock &block)
{
//	resize(block.length() * 2);
	char s[3];
	const char *data = block.data();
	unsigned long size = block.length();
	for (unsigned int i = 0; i < size; ++i)
	{
		sprintf(s, "%2.2X", *data++);
		append(s);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Value :: DWord
///////////////////////////////////////////////////////////////////////////////////////////////

Value::DWord::operator Block() 
{ 
	return Block((char *) &_value, sizeof(_value)); 
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Registry
} // namespace magenta
