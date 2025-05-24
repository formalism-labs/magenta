
///////////////////////////////////////////////////////////////////////////////////////////////
// Resources
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_win32_resources_
#define _magenta_system_win32_resources_

#include <string>

#include "magenta/files/defs.h"
#include "magenta/system/win32/module.h"
#include "magenta/system/win32/language.h"
#include "magenta/types/defs.h"

#ifndef _WIN32
#include "magenta/system/windows.h"
#endif

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////
// Resource
///////////////////////////////////////////////////////////////////////////////////////////////

class Resource
{
	//-----------------------------------------------------------------------------------------
	// Id
	//-----------------------------------------------------------------------------------------
public:
	class Name
	{
		string str_name;
		LPCTSTR _value;

	public:
		Name(const string &name) : str_name(name), _value(str_name.c_str()) {}
		Name(int name) : str_name(), _value(MAKEINTRESOURCE(name)) {}
		Name(const Name &name) : str_name(name.str_name)
		{
			_value = str_name.empty() ? name._value : str_name.c_str();
		}

		operator LPCSTR() const { return _value; }
	};

	class Id
	{	
		Name _type, _name;
		Language _lang;

	public:
		Id(const Name &type, const Name &name, const Language &lang = Language()) : 
			_type(type), _name(name), _lang(lang) {}

		const Name &type() const { return _type; }
		const Name &name() const { return _name; }
		const Language &language() const { return _lang; }
	};

protected:
	const Module &_module;
	Id _id;
	HRSRC _handle;

public:
	CLASS_EXCEPTION_DEF("magenta::Resource");

public:
	Resource(const Module &module, const Id &id);

	ConstBlock data() const;
	operator ConstBlock() const { return data(); }

	void writeToFile(const File &file);
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Resource File
///////////////////////////////////////////////////////////////////////////////////////////////

class ResourceFile
{
	File _file;

public:
	ResourceFile(const File &file) : _file(file) {}

	//-----------------------------------------------------------------------------------------
	// UpdateSession
	//-----------------------------------------------------------------------------------------
public:
	class UpdateSession
	{
		HANDLE _handle;

	public:
		UpdateSession(const ResourceFile &file);
		~UpdateSession();

		operator HANDLE() const { return _handle; }
	};

	//-----------------------------------------------------------------------------------------
	// Resource
	//-----------------------------------------------------------------------------------------
public:
	class Resource
	{
	public:
		typedef magenta::Resource::Id Id;

	protected:
		ResourceFile &_file;
		Id _id;

		static void assert_(BOOL rc);

	public:
		Resource(ResourceFile &file, const Id &id) : _file(file), _id(id) {}

		void update(const IBlock &data);
		void remove();

		CLASS_EXCEPTION_DEF("magenta::ResourceFile::Resource");
	};

	const Path &name() const { return _file.path(); }

	void add(const Resource::Id &id, const IBlock &data);

	Resource operator[](const Resource::Id &id) { return Resource(*this, id); }

	Resource operator()(const string &type, const string &name)
	{
		return Resource(*this, Resource::Id(type, name));
	}

	CLASS_EXCEPTION_DEF("magenta::ResourceFile");
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_win32_resources_
