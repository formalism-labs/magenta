
///////////////////////////////////////////////////////////////////////////////////////////////
// UUID
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_uuid_
#define _magenta_system_uuid_

#include "magenta/types/defs.h"
#include <rpc.h>

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class IUuid
{
	virtual UUID &uuid() = 0;
	const UUID &uuid() const { return ((IUuid *) this)->uuid(); }

	int compare(const IUuid &id) const;

public:
	bool operator==(const IUuid &id) const;
	bool operator>(const IUuid &id) const;
	bool operator<(const IUuid &id) const;
	bool operator!() const;

	operator String() const;
	unsigned short hash() const;

	class Error {};

	static void assert_status_ok(RPC_STATUS &rc);
};

//----------------------------------------------------------------------------------------------

class Uuid : public IUuid
{
	UUID _uuid;
	UUID &uuid() { return _uuid; }

public:
	Uuid();
	Uuid(const IString &s);
};

//----------------------------------------------------------------------------------------------

class NullUuid : public IUuid
{
	UUID _uuid;
	UUID &uuid() { return _uuid; }

public:
	NullUuid();
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_system_uuid_
