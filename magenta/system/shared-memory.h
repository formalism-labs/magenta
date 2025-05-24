
///////////////////////////////////////////////////////////////////////////////////////////////
// Shared Memory
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_system_shared_memory_
#define _magenta_system_shared_memory_

#include "magenta/system/classes.h"

#include "magenta/types/defs.h"

#include <string>

#ifndef _WI32
#include "magenta/system/windows.h"
#endif

#pragma warning(push)
#pragma warning(disable : 4250)

namespace magenta
{

namespace SharedMemory
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

template <class Type>
class IMemory
{
public:
	virtual bool isNew() const = 0;

	class Error {};
	class NameErr : public Error {};
	class LockErr : public Error {};
};

//---------------------------------------------------------------------------------------------

template <class Type>
class Memory : virtual public IMemory<Type>
{
protected:
	typedef Memory<Type> This;

	HANDLE _handle;
	bool is_new;
	Type *_memory;

	virtual void *origin() const { return _memory; }

	void allocate(const string &name, int size, bool isWritable)
	{
		_handle = CreateFileMapping(HANDLE(0xFFFFFFFF), NULL, 
			(isWritable ? PAGE_READWRITE : PAGE_READONLY), 0L, size, name.c_str());
		if (!_handle)
			throw Error();
		is_new = GetLastError() != ERROR_ALREADY_EXISTS;
		_memory = (Type *) MapViewOfFile(_handle, 
			FILE_MAP_READ | (isWritable ? FILE_MAP_WRITE : 0), 0, 0, 0);
		if (!_memory)
			throw Error();
	}

	void free()
	{
		UnmapViewOfFile(LPVOID(origin()));
		CloseHandle(_handle);
	}

	Memory() {}

public:
	~Memory() { free(); }

    bool isNew() const { return is_new; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
	
template <class Type>
class IObject : virtual public IMemory<Type>
{
	typedef IObject<Type> This;

	virtual Type &object() = 0;
	const Type &object() const { return (const Type &) ((IThis *) this)->object(); }

public:
    Type &operator*() { return object(); }
    const Type &operator*() const { return object(); }

    operator Type&() { return object(); }
    operator const Type&() const { return object(); }

	Type *operator->() { return &object(); }
	const Type *operator->() const { return &object(); }
};

//---------------------------------------------------------------------------------------------

template <class Type>
class Object : public Memory<Type>, public IObject<Type>
{
    Type &object() { return *_memory; }

public:
	Object(const string &name, bool isWritable = true)
	{
		allocate(name, 1, isWritable);
		if (is_new)
			new (_memory) Type;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class Type>
class IArray : virtual public IMemory<Type>
{
	typedef IArray<Type> IThis;

	virtual Type *array() = 0;
	const Type *array() const { return (const Type *) ((IThis *) this)->array(); }

public:
	virtual Type &operator[](int i) { return array()[i]; }
	virtual const Type &operator[](int i) const { return array()[i]; }

    virtual Type &operator*() { return *array(); }
	virtual const Type &operator*() const { return *array(); }

	virtual int size() const = 0;

	class NotAllocatedErr : public IMemory<Type>::Error {};
};

//---------------------------------------------------------------------------------------------

template <class Type>
class Array : public Memory<Type>, public IArray<Type>
{
	void *origin() const { return (int *) memory - 1; }
	Type *array() { return memory; }

	void allocate(const string &name, int units, bool isWritable = true)
	{
		memory_t<Type>::allocate(name, sizeof(Type) * units + sizeof(int), isWritable);
		if (is_new)
			*(int *) memory = units;
		memory = (Type *) ((int *) memory + 1);
		if (is_new)
			new (memory) Type[units];
	}

public:
	Array(const string &name, bool isWritable = true)
	{
		allocate(name, sizeof(int), false);
		if (is_new)
			throw NotAllocatedErr();
		int units = *(int *) memory;
		free();
		allocate(name, units, isWritable);
	}

	Array(const string &name, int units, bool isWritable = true)
	{
		allocate(name, units, isWritable);
	}

	int size() const { return *(int *) origin(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace SharedMemory
} // namespace magenta

#pragma warning(pop)

#endif
