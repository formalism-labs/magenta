
#include "magenta/sync/atomic.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

UINT32 AtomicUINT32::operator++()
{
#ifdef __linux__
	return __sync_add_and_fetch(&_n, 1);

#elif defined(_WIN32) && defined(_MSC_VER)
	return InterlockedIncrement(&_n); 
#endif
}

//---------------------------------------------------------------------------------------------

UINT32 AtomicUINT32::operator++(int)
{
#ifdef __linux__
	return __sync_fetch_and_add(&_n, 1);

#elif defined(_WIN32) && defined(_MSC_VER)
	UINT32 n = _n;
	InterlockedIncrement(&_n);
	return n;
#endif
}

//---------------------------------------------------------------------------------------------

UINT32 AtomicUINT32::operator--()
{
#ifdef __linux__
	return __sync_sub_and_fetch(&_n, 1);

#elif defined(_WIN32) && defined(_MSC_VER)
	return InterlockedDecrement(&_n); 
#endif
}

//---------------------------------------------------------------------------------------------

UINT32 AtomicUINT32::operator--(int)
{
#ifdef __linux__
	return __sync_fetch_and_sub(&_n, 1);

#elif defined(_WIN32) && defined(_MSC_VER)
	UINT32 n = _n;
	InterlockedDecrement(&_n);
	return n;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
