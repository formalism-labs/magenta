
///////////////////////////////////////////////////////////////////////////////////////////////
// UUID
///////////////////////////////////////////////////////////////////////////////////////////////

#include "uuid.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

void IUuid::assert_status_ok(RPC_STATUS &rc)
{
	if (rc != RPC_S_OK)
		throw Error();
}

//----------------------------------------------------------------------------------------------

int IUuid::compare(const IUuid &id) const
{
	RPC_STATUS rc;
	int result = UuidCompare((UUID *) &uuid(), (UUID *) &id.uuid(), &rc);
	assert_status_ok(rc);
	return result;
}

bool IUuid::operator==(const IUuid &id) const { return !compare(id); }
bool IUuid::operator>(const IUuid &id) const { return compare(id) == 1; }
bool IUuid::operator<(const IUuid &id) const { return compare(id) == 1; }

//----------------------------------------------------------------------------------------------

bool IUuid::operator!() const
{
	RPC_STATUS rc;
	int result = UuidIsNil((UUID *) &uuid(), &rc);
	assert_status_ok(rc);
	return !!result;
}

//----------------------------------------------------------------------------------------------

IUuid::operator String() const
{
	unsigned char *pc;
	RPC_STATUS rc = UuidToString((UUID *) &uuid(), &pc);
	assert_status_ok(rc);
	String s= pc;
	RpcStringFree(&pc);
	return s;
}

unsigned short IUuid::hash() const
{
	RPC_STATUS rc;
	int result = UuidHash((UUID *) &uuid(), &rc);
	assert_status_ok(rc);
	return result;
}

//----------------------------------------------------------------------------------------------

Uuid::Uuid()
{
	RPC_STATUS rc = UuidCreate(&_uuid);
	assert_status_ok(rc);
}

Uuid::Uuid(const IString &s)
{
	RPC_STATUS rc = UuidFromString((unsigned char *) (const char *) s, &_uuid);
	assert_status_ok(rc);
}

//----------------------------------------------------------------------------------------------

NullUuid::NullUuid()
{
	RPC_STATUS rc = UuidCreateNil(&_uuid);
	assert_status_ok(rc);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace green
