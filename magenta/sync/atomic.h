
#ifndef _magenta_sync_atomic_
#define _magenta_sync_atomic_

#include "rvfc/Include/rvfc.h"

#if defined(_WIN32)
#include <windows.h>

#elif defined(__linux__)
#include <pthread.h>
#include <semaphore.h>
#endif

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class AtomicUINT32
{
#if defined(__linux__)
	volatile long __attribute__ ((aligned (32))) _n; 
#elif defined(_WIN32)
	volatile __declspec(align(32)) LONG _n; 
#endif

public:
	AtomicUINT32(UINT32 k = 0) : _n(k) {}

	operator UINT32() const { return _n; }

	UINT32 operator++();
	UINT32 operator++(int);
	UINT32 operator--();
	UINT32 operator--(int);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_sync_atomic_
