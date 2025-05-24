
#include "wait-object.h"

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////
// Wait Object
///////////////////////////////////////////////////////////////////////////////////////////////

bool WaitObject::_wait(DWORD ms)
{
	DWORD rc = WaitForSingleObject(handle(), ms);
	switch (rc)
	{
	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
		return false;

	case WAIT_OBJECT_0:
		onReady();
		return true;

	case WAIT_FAILED:
	default:
		THROW_OS(Exception)("error while waiting for event");
	}
}

//---------------------------------------------------------------------------------------------

bool WaitObject::wait()
{
	return _wait(INFINITE);
}

//---------------------------------------------------------------------------------------------

bool WaitObject::wait(Milliseconds interval)
{
	return _wait(interval.value());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Wait Objects
///////////////////////////////////////////////////////////////////////////////////////////////

WaitObjects::WaitObjects(int n, WaitObject *objects[])
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

void WaitObjects::waitAll()
{
}

//---------------------------------------------------------------------------------------------

WaitObject *WaitObjects::waitAny()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// WaitFor
///////////////////////////////////////////////////////////////////////////////////////////////

WaitFor::WaitFor(HANDLE *objs, int num) : _n(num), _handles(num), _indices(num)
{
	for (int i = 0; i < num; ++i)
	{
		_handles[i] = objs[i];
		_indices[i] = i;
	}
}
	
//---------------------------------------------------------------------------------------------

int WaitFor::wait(bool forAll, DWORD timeout)
{
	while (_n > 0)
	{
		DWORD rc = WaitForMultipleObjects(_n, _handles.data(), forAll, timeout);
		if (rc == WAIT_TIMEOUT || rc == WAIT_FAILED)
			return rc;

		bool adandoned = false;
		int k, index;
		if (rc >= WAIT_OBJECT_0 && rc < WAIT_OBJECT_0 + _n)
		{
			k = rc - WAIT_OBJECT_0;
			index = _indices[k];
		}
		if (rc >= WAIT_ABANDONED_0 && rc < WAIT_ABANDONED_0  + _n)
		{
			k = rc - WAIT_ABANDONED_0;
			adandoned = true;
		}

		if (k < --_n)
		{
			_handles[k] = _handles[_n];
			_indices[k] = _indices[_n];
		}

		if (!adandoned)
			return index;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
