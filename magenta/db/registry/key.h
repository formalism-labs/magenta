
#ifndef _magenta_db_registry_key_
#define _magenta_db_registry_key_

#include "magenta/exceptions/defs.h"
#include "magenta/db/registry/classes.h"

#include <windows.h>
#include <string>

namespace magenta
{

namespace Registry
{

using std::string;


///////////////////////////////////////////////////////////////////////////////////////////////

class Key
{
	HKEY parent_handle;
	HKEY _handle;
	string _name;
	bool _predefined;

	static void assert_(LONG rc);

protected:
	Key(HKEY handle);

public:
	Key(const Key &key, const string &keyName);
	Key(const Key &key);
	~Key();

	operator HKEY() const;
	const string &name() const { return _name; }

	Value operator()();
	Value operator()(const string &name);

	//-----------------------------------------------------------------------------------------
public:
	CLASS_EXCEPTION_DEF("magenta::db::registry::key");

	//-----------------------------------------------------------------------------------------
public:
	struct IMapFunction
	{
		virtual void operator()(KeysIterator &i) = 0;
	};

protected:
	void preOrderMap(IMapFunction &f, KeysIterator &i);
	void postOrderMap(IMapFunction &f, KeysIterator &i);

public:
	void preOrderMap(IMapFunction &f);
	void postOrderMap(IMapFunction &f);

	//-----------------------------------------------------------------------------------------
public:
	void remove();
	bool removeValue(const string &name);
	bool removeSubKey(const string &name, bool force = false);
	void removeSubKey(IMapFunction &f, KeysIterator &i);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class KeysIterator
{
	Key &parent_key;
	int _index;
	bool _valid;
	Key *_key;
	string key_name;

	void getKey();

public:
	KeysIterator(Key &key);
	~KeysIterator() { delete _key; }

	bool operator!() const { return !_valid; }
	void refresh();

	KeysIterator &operator++();
	KeysIterator &operator--();

	Key &parentKey() { return parent_key; }
	const string &keyName() const { return key_name; }

	Key &operator*();
	Key *operator->();
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ValuesIterator
{
	Key &_key;
	int _index;
	bool _valid;
	Value *_value;
	string value_name;

	void getValue();

public:
	ValuesIterator(Key &key);
	~ValuesIterator();

	bool operator!() const { return !_valid; }
	void refresh();

	ValuesIterator &operator++();
	ValuesIterator &operator--();

	Key &key() { return _key; }
	const string &valueName() const { return value_name; }

	Value &operator*();
	Value *operator->();
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ClassesRootKey : public Key
{
public:
	ClassesRootKey() : Key(HKEY_CLASSES_ROOT) {}
};

//---------------------------------------------------------------------------------------------

class CurrentUserKey : public Key
{
public:
	CurrentUserKey() : Key(HKEY_CURRENT_USER) {}
};

//---------------------------------------------------------------------------------------------

class LocalMachineKey : public Key
{
public:
	LocalMachineKey() : Key(HKEY_LOCAL_MACHINE) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Registry
} // namespace magenta

#endif // _magenta_db_registry_key_
