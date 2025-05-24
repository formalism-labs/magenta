
#ifndef _magenta_db_registry_value_
#define _magenta_db_registry_value_

#include "magenta/db/registry/classes.h"

#include "magenta/exceptions/defs.h"
#include "magenta/types/defs.h"
#include "magenta/lang/defs.h"

#include <string>

#include <windows.h>

namespace magenta
{

namespace Registry
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class Value
{
	class ReadValue;
	class WriteValue;

	friend ReadValue;
	friend WriteValue;

public:
	enum__(Type)
		DWord,
		String,
		Binary
	__enum;

private:
	Key &_key;
	string _name;
	unsigned long _type;

public:
	const string &name() { return _name; }

private:
	static void assert_(LONG rc);

	//-----------------------------------------------------------------------------------------

	class DWord
	{
		unsigned long _value;

	public:
		DWord(const void *p) : _value(*(const unsigned long *) p) {}
		DWord(unsigned long n) : _value(n) {}
		operator unsigned long() const { return _value; }
		operator Block();
	};

	//-----------------------------------------------------------------------------------------

	class HexString : public string
	{
	public:
		HexString(const IBlock &block);
	};

	//-----------------------------------------------------------------------------------------

	class ReadValue
	{
		unsigned char _data[1024];
		unsigned long _size;
		unsigned long _type;

	public:
		ReadValue(const Value &value);

		operator unsigned long() const;
		operator string() const;
		operator PersistentDynablock() const;
	};

	//-----------------------------------------------------------------------------------------

	class WriteValue
	{
		const Value &_value;

		void write(const IBlock &block);

	public:
		WriteValue(const Value &value, unsigned long n);
		WriteValue(const Value &value, const string &s);
		WriteValue(const Value &value, const IBlock &block);
	};

	//-----------------------------------------------------------------------------------------

public:
	Value(Key &key, const string &name);
	Value(Key &key, const string &name, Type type);
	Value(Key &key, const string &name, unsigned long value);
	Value(Key &key, const string &name, const string &value);
	Value(Key &key, const string &name, const IBlock &value);

	operator unsigned long() const;
	operator string() const;
	operator PersistentDynablock() const;

	Value &operator=(unsigned long value);
	Value &operator=(const string &s);
	Value &operator=(const IBlock &block);

	void remove();

	CLASS_EXCEPTION_DEF("magenta::db::registry::value");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Registry
} // namespace magenta

#endif // _magenta_db_registry_value_
