
#ifndef _magenta_sync_common_
#define _magenta_sync_common_

#include "magenta/exceptions/defs.h"
#include "magenta/time/defs.h"

#include "rvfc/Include/rvfcTime.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Take
{
	T &_obj;

public:
	Take(T &obj) : _obj(obj)
	{
		_obj.take();
	}

	Take(T &obj, const rvTicks &timeout) : _obj(obj)
	{
		if (_obj.ask(timeout))
			throw std::runtime_error("lock exceeded timeout");
	}

	~Take()
	{
		_obj.give();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_sync_common_
