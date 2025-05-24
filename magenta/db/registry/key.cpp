
#include "key.h"
#include "value.h"

namespace magenta
{

namespace Registry
{
	
///////////////////////////////////////////////////////////////////////////////////////////////

Key::Key(const Key &parentKey, const string &keyName) : _predefined(false)
{
    parent_handle = parentKey;
    _name = keyName;
    switch (RegOpenKeyEx(parent_handle, _name.c_str(), 0, KEY_ALL_ACCESS, &_handle))
	{
    case ERROR_SUCCESS:
		return;

    case ERROR_ACCESS_DENIED:
		assert_(RegOpenKeyEx(parent_handle, _name.c_str(), 0, KEY_READ, &_handle));
		break;

    default:
        THROW_EX(Exception);
    }
}

//---------------------------------------------------------------------------------------------

Key::Key(const Key &key) : 
	_predefined(false),
	parent_handle(key.parent_handle),
	_name(key._name)
{
    switch (RegOpenKeyEx(key._handle, 0, 0, KEY_ALL_ACCESS, &_handle))
	{
    case ERROR_SUCCESS:
		return;

    case ERROR_ACCESS_DENIED:
		assert_(RegOpenKeyEx(key._handle, 0, 0, KEY_READ, &_handle));
		break;

    default:
        THROW_EX(Exception);
    }
}

//---------------------------------------------------------------------------------------------

Key::Key(HKEY handle) : _handle(handle), _predefined(true)
{
}

//---------------------------------------------------------------------------------------------

Key::~Key()
{
    RegCloseKey(_handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Key::assert_(LONG rc)
{
	if (rc != ERROR_SUCCESS)
		THROW_EX(Exception);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Key::operator HKEY() const
{
	return _handle;
}

///////////////////////////////////////////////////////////////////////////////////////////////

Value Key::operator()()
{
	return Value(*this, "");
}

//---------------------------------------------------------------------------------------------

Value Key::operator()(const string &name)
{
	return Value(*this, name);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Key::preOrderMap(IMapFunction &f, KeysIterator &i)
{
	for (; !!i; ++i)
	{
		f(i);
		preOrderMap(f, KeysIterator(*i));
	}
}

//---------------------------------------------------------------------------------------------

void Key::preOrderMap(IMapFunction &f)
{
	preOrderMap(f, KeysIterator(*this));
}

//---------------------------------------------------------------------------------------------

void Key::postOrderMap(IMapFunction &f, KeysIterator &i)
{
	for (; !!i; ++i)
	{
		postOrderMap(f, KeysIterator(*i));
		f(i);
	}
}

//---------------------------------------------------------------------------------------------

void Key::postOrderMap(IMapFunction &f)
{
	postOrderMap(f, KeysIterator(*this));
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Key::remove()
{
	assert_(RegDeleteKey(parent_handle, _name.c_str()));
	_handle = 0;
}

//---------------------------------------------------------------------------------------------

bool Key::removeValue(const string &name)
{
	return RegDeleteValue(_handle, name.c_str()) == ERROR_SUCCESS;
}

//---------------------------------------------------------------------------------------------

bool Key::removeSubKey(const string &name, bool force)
{
	struct F : IMapFunction
	{
		void operator()(KeysIterator &i)
		{
			i.parentKey().removeSubKey(i.keyName(), true);
		}
	} f;

	LONG rc = RegDeleteKey(_handle, name.c_str());
	if (rc != ERROR_SUCCESS && force)
	{
		removeSubKey(f, KeysIterator(Key(*this, name)));
		rc = RegDeleteKey(_handle, name.c_str());
	}
	return rc == ERROR_SUCCESS;
}

//---------------------------------------------------------------------------------------------

void Key::removeSubKey(IMapFunction &f, KeysIterator &i)
{
	while (!!i)
	{
		removeSubKey(f, KeysIterator(*i));
		f(i);
		i.refresh();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// KeysIterator
///////////////////////////////////////////////////////////////////////////////////////////////

KeysIterator::KeysIterator(Key &key) : 
	parent_key(key), _index(0), _key(0)
{
	getKey();
}


///////////////////////////////////////////////////////////////////////////////////////////////

void KeysIterator::getKey()
{
	char name[256];
	LONG rc = RegEnumKey(parent_key, _index, name, sizeof(name));
	_valid = rc == ERROR_SUCCESS;
	if (!_valid && rc != ERROR_NO_MORE_ITEMS)
		THROW_EX(Exception);

	key_name.assign(_valid ? name : "");
}

//---------------------------------------------------------------------------------------------

void KeysIterator::refresh()
{
	delete _key;
	_key = 0;
	getKey();
}

///////////////////////////////////////////////////////////////////////////////////////////////

KeysIterator &KeysIterator::operator++()
{
	if (_valid)
	{
		++_index;
		refresh();
	}
	return *this;
}

//---------------------------------------------------------------------------------------------

KeysIterator &KeysIterator::operator--()
{
	if (_index > 0) 
	{
		--_index;
		refresh();
	}
	return *this;
}


///////////////////////////////////////////////////////////////////////////////////////////////

Key &KeysIterator::operator*()
{
	if (!_key)
		_key = new Key(parent_key, key_name);
	return *_key;
}

//---------------------------------------------------------------------------------------------

Key *KeysIterator::operator->()
{
	return &**this;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ValuesIterator
///////////////////////////////////////////////////////////////////////////////////////////////

ValuesIterator::ValuesIterator(Key &key) : 
	_key(key), _index(0), _value(0)
{
	getValue();
}

//---------------------------------------------------------------------------------------------

ValuesIterator::~ValuesIterator()
{
	delete _value;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ValuesIterator::getValue()
{
	char name[256];
	DWORD name_size = sizeof(name);
	LONG rc = RegEnumValue(_key, _index, name, &name_size, 0, REG_NONE, 0, 0);
	_valid = rc == ERROR_SUCCESS;
	if (!_valid && rc != ERROR_NO_MORE_ITEMS)
		THROW_EX(Exception);

	value_name.assign(_valid ? name : "");
}

//---------------------------------------------------------------------------------------------

void ValuesIterator::refresh()
{
	delete _value;
	_value = 0;
	getValue();
}

///////////////////////////////////////////////////////////////////////////////////////////////

ValuesIterator &ValuesIterator::operator++()
{
	if (_valid)
	{
		++_index;
		refresh();
	}
	return *this;
}

//---------------------------------------------------------------------------------------------

ValuesIterator &ValuesIterator::operator--()
{
	if (_index > 0) 
	{
		--_index;
		refresh();
	}
	return *this;
}


///////////////////////////////////////////////////////////////////////////////////////////////

Value &ValuesIterator::operator*()
{
	if (!_value)
		_value = new Value(_key, value_name);
	return *_value;
}

//---------------------------------------------------------------------------------------------

Value *ValuesIterator::operator->()
{
	return &**this;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Registry
} // namespace magenta

