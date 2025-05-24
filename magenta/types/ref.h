
///////////////////////////////////////////////////////////////////////////////////////////////
// Smart Pointer
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_types_ref_
#define _magenta_types_ref_

#pragma warning(push)
#pragma warning(disable : 4786 4348)

#include "magenta/types/classes.h"

#include "magenta/exceptions/defs.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Pointer
{
protected:
	T *p;

public:
	Pointer(T *q = 0) : p(q) {}
	operator T*() { return p; }
	operator const T*() const { return p; }

	typedef const T *Const;
};

//---------------------------------------------------------------------------------------------

template <class T>
class ConstPointer
{
protected:
	const T *p;

public:
	ConstPointer(const T *q = 0) : p(q) {}
	operator const T*() const { return p; }

	typedef ConstPointer<T> Const;
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Core
{
protected:
	friend GeneralRef<T, true>;
	friend GeneralRef<T, false>;
	friend Ref<T>;
	friend ConstRef<T>;
	friend Ptr<T>;
	friend ConstPtr<T>;
	
	T *_data;
	unsigned long _ref;
	
	Core() : _data(0), _ref(0) {}
	Core(const T &data) : _data((T *) &data), _ref(1) {}
	virtual ~Core() {}
	
	int operator++() { return ++_ref; }
	virtual unsigned long operator--()
	{
		if (!--_ref) 
			_data = 0;
		return _ref; 
	}
	
	Core *dereference() { _ref++; return this; }
	T *data() { return _data; }
	const T *data() const { return (const T *) _data; }

//	Core *operator&() { _ref++; return this; }
//	T *operator*() { return _data; }
//	const T *operator*() const { return (const T *) _data; }

	bool operator!() const { return !_ref; }
};

//---------------------------------------------------------------------------------------------

template <class T>
class Dynacore : public Core<T>
{
	typedef Core<T> Super;

	friend GeneralRef<T, true>;
	friend GeneralRef<T, false>;
	friend Ref<T>;
	friend ConstRef<T>;
	friend Ptr<T>;
	friend ConstPtr<T>;
	
	Dynacore() {}
	Dynacore(const T &data) : Core<T>(data) {}
	~Dynacore() { delete Core<T>::_data; }

	unsigned long operator--()
	{ 
		if (!--Super::_ref)
		{
			delete Super::_data; 
			Super::_data = 0; 
		}
		return Super::_ref; 
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T, bool AllowNulls>
class GeneralRef
{
	friend GeneralRef<T, !AllowNulls>;
	friend Ref<T>;
	friend ConstRef<T>;
	friend Ptr<T>;
	friend ConstPtr<T>;
	
protected:
    typedef Core<T> Core;
    typedef Dynacore<T> Dynacore;
    
	GeneralRef(Core *core = 0) : _core(core) {}
	GeneralRef(T *data) : _core(new Dynacore(*data)) {}
	GeneralRef(const T *data) : _core(new Dynacore(*data)) {}
//	GeneralRef(T &data) : _core(new Core(data)) {}
//	GeneralRef(const T &data) : _core(new Core(*data)) {}
	GeneralRef(const Ref<T> &ref) : _core(ref.corePtr()) {}
	GeneralRef(const ConstRef<T> &ref) : _core(ref.corePtr()) {}
	GeneralRef(const Ptr<T> &ref) : _core(ref.corePtr()) {}
	GeneralRef(const ConstPtr<T> &ref) : _core(ref.corePtr()) {}

	virtual ~GeneralRef() { cleanup(); }

    mutable Core *_core;
    Core &core() const 
	{ 
		if (!_core)
			THROW_EX(Exception)("access of null reference");
		return *_core; 
	}

    Core *corePtr() const 
	{ 
		if (!_core)
			return 0;
		return core().dereference(); 
	}

	void cleanup()
	{
		if (_core && !--*_core)
			delete _core;
		_core = 0;
	}

	void assign(T *data)
	{
		cleanup();
		if (!AllowNulls && !data)
			THROW_EX(Exception)("null pointer assignment ");
		_core = data ? new Dynacore(*data) : 0;
	}
/*
	void assign(T &data)
	{
		cleanup();
		if (!AllowNulls && !data)
			THROW_EX(Exception)("null pointer assignment ");
		_core = new Core(*data);
	}
*/
	void assign(const GeneralRef &ref)
	{
		if(&ref == this)
			return;
		cleanup();
		Core *core = ref.corePtr();
		if (!AllowNulls && !core)
			THROW_EX(Exception)("null pointer assignment ");
		_core = core;;
	}

	unsigned long refs() const { return core()._ref; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Ref : public GeneralRef<T>
{
	typedef GeneralRef<T> Super;

	friend ConstRef<T>;
	friend Ptr<T>;
	friend ConstPtr<T>;

public:
	typedef ConstRef<T> Const;

	CLASS_EXCEPTION_DEF("magenta::types::2::Ref");

public:
	Ref() {}
	Ref(T *data) : Super(data) {}
//	Ref(T &data) : Super(data) {}
	Ref(const Ref &ref) : Super(ref) {}
	Ref(const Ptr<T> &ptr) : Super(ptr) {}

	Ref &operator=(T *data) { assign(data); return *this; }
//	Ref &operator=(T &data) { assign(data); return *this; }
	Ref &operator=(const Ref &ref) { Super::assign(ref); return *this; }

	operator T&() { return *Super::core().data(); }
	operator const T&() const { return *Super::core().data(); }
	operator T*() { return Super::core().data(); }
	operator const T*() const { return Super::core().data(); }

	T *operator->() { return Super::core().data(); }
	const T *operator->() const { return Super::core().data(); }
	T &operator*() { return *Super::core().data(); }
	const T &operator*() const { return *Super::core().data(); }
};

//---------------------------------------------------------------------------------------------

template <class T>
class ConstRef : public GeneralRef<T>
{
	friend Ptr<T>;
	friend ConstPtr<T>;

	typedef GeneralRef<T> Super;

public:
	CLASS_EXCEPTION_DEF("magenta::types::ConstRef");

public:
	ConstRef() {}
	ConstRef(const T *data) : Super(data) {}
//	ConstRef(const T &data) : Super(data) {}
	ConstRef(const Ref<T> &ref) : Super(ref) {}
	ConstRef(const ConstRef &ref) : Super(ref) {}
	ConstRef(const Ptr<T> &ref) : Super(ref) {}
	ConstRef(const ConstPtr<T> &ref) : Super(ref) {}

	ConstRef &operator=(const T *data) { assign(data); return *this; }
//	ConstRef &operator=(const T &data) { assign(data); return *this; }
	ConstRef &operator=(const Ref<T> &ref) { assign(ref); return *this; }
	ConstRef &operator=(const ConstRef &ref) { assign(ref); return *this; }

//	operator const T &() const { return *core().data(); }
	operator const T *() const { return Super::core().data(); }
	const T *operator->() const { return Super::core().data(); }
	const T &operator*() const { return *Super::core().data(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Ptr : public GeneralRef<T, true>
{
	friend ConstPtr<T>;
	friend Ref<T>;
	friend ConstRef<T>;

	typedef GeneralRef<T, true> Super;
	typedef Ptr<T> This;

public:
	typedef ConstPtr<T> Const;

	CLASS_EXCEPTION_DEF("magenta::types::2::Ptr");

public:
	Ptr() {}
	Ptr(T *data) : Super(data) {}
//	Ptr(T &data) : Super(data) {}
	Ptr(const Ref<T> &ref) : Super(ref) {}
	Ptr(const Ptr &ptr) : Super(ptr) {}

	This &operator=(T *data) { assign(data); return *this; }
//	This &operator=(T &data) { assign(data); return *this; }
	This &operator=(const Ref<T> &ref) { assign(ref); return *this; }
	This &operator=(const Ptr &ptr) { assign(ptr); return *this; }

	operator T&() { return *Super::core().data(); }
	operator const T&() const { return *Super::core().data(); }
	operator T*() { return Super::core().data(); }
	operator const T*() const { return Super::core().data(); }

	T *operator->() { return Super::core().data(); }
	const T *operator->() const { return Super::core().data(); }
	T &operator*() { return *Super::core().data(); }
	const T &operator*() const { return *Super::core().data(); }

	bool operator!() const { return !Super::_core; }
};

//---------------------------------------------------------------------------------------------

template <class T>
class ConstPtr : GeneralRef<T, true>
{
	friend ConstRef<T>;
	
	typedef GeneralRef<T, true> Super;
	typedef ConstPtr<T> This;

public:
	CLASS_EXCEPTION_DEF("magenta::types::ConstPtr");

public:
	ConstPtr() {}
	ConstPtr(const T *data) : Super(data) {}
//	ConstPtr(const T &data) : Super(data) {}
	ConstPtr(const Ptr<T> &ptr) : Super(ptr) {}
	ConstPtr(const ConstPtr &ptr) : Super(ptr) {}
	ConstPtr(const Ref<T> &ref) : Super(ref) {}
	ConstPtr(const ConstRef<T> &ref) : Super(ref) {}

	This &operator=(const T *data) { assign(data); return *this; }
	This &operator=(const Ptr<T> &ref) { assign(ref); return *this; }
	This &operator=(const ConstPtr &ref) { assign(ref); return *this; }
	This &operator=(const Ref<T> &ref) { assign(ref); return *this; }
	This &operator=(const ConstRef<T> &ref) { assign(ref); return *this; }

	operator const T &() const { return *Super::core().data(); }
	operator const T *() const { return Super::core().data(); }
	const T *operator->() const { return Super::core().data(); }
	const T &operator*() const { return *Super::core().data(); }

	bool operator!() const { return !Super::_core; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#pragma warning(pop)

#endif
