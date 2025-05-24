
///////////////////////////////////////////////////////////////////////////////////////////////
// Resources
///////////////////////////////////////////////////////////////////////////////////////////////

#include "resources.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// Resource
///////////////////////////////////////////////////////////////////////////////////////////////

Resource::Resource(const Module &module, const Id &id) :
	_module(module), _id(id)
{
	_handle = FindResource(_module, id.name(), id.type());
	if (!_handle)
		THROW_EX(Exception)("cannot find resource");
}

///////////////////////////////////////////////////////////////////////////////////////////////

ConstBlock Resource::data() const
{
	HGLOBAL mem_h = LoadResource(_module, _handle);
	if (!mem_h)
		THROW_EX(Exception)("cannot load resource");
	void *resource = LockResource(mem_h);
	DWORD size = SizeofResource(_module, _handle);
	return ConstBlock((char *) resource, size);
}

//---------------------------------------------------------------------------------------------

void Resource::writeToFile(const File &file)
{
	NewFile new_file(file);
	new_file.write(data());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Resource File
///////////////////////////////////////////////////////////////////////////////////////////////

void ResourceFile::add(const Resource::Id &id, const IBlock &data)
{
	UpdateSession update_session(*this);
	UpdateResource(update_session, id.type(), id.name(), id.language(),
		(LPVOID) (const void *) data, data.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ResourceFile::Resource::update(const IBlock &data)
{
	UpdateSession update_session(_file);
	assert_(UpdateResource(update_session, _id.type(), _id.name(), _id.language(), 
		(LPVOID) (const void *) data, data.size()));
}

//---------------------------------------------------------------------------------------------

void ResourceFile::Resource::remove()
{
	UpdateSession update_session(_file);
	assert_(UpdateResource(update_session, _id.type(), _id.name(), _id.language(), 0, 0));
}

//---------------------------------------------------------------------------------------------

void ResourceFile::Resource::assert_(BOOL rc)
{
	if (!rc)
		THROW_EX(Exception);
}

///////////////////////////////////////////////////////////////////////////////////////////////

ResourceFile::UpdateSession::UpdateSession(const ResourceFile &file)
{
	_handle = BeginUpdateResource(+file.name(), false);
	if (!_handle)
		THROW_EX(Exception)("cannot update resource file");
}

//---------------------------------------------------------------------------------------------

ResourceFile::UpdateSession::~UpdateSession()
{
	EndUpdateResource(_handle, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
