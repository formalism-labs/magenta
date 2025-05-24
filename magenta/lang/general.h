
#ifndef _magenta_lang_general_
#define _magenta_lang_general_

#include <memory.h>

#include <functional>
#include <tuple>
#include <limits>

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

#define foreach(i, exp) for (auto i = (exp); !!i; ++i)

///////////////////////////////////////////////////////////////////////////////////////////////
// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////

#define TO_STRING_(x) #x
#define TO_STRING(x) TO_STRING_(x) 

///////////////////////////////////////////////////////////////////////////////////////////////
// Numbers
///////////////////////////////////////////////////////////////////////////////////////////////

#define _1e3 1000L
#define _1e6 1000000L
#define _1e9 1000000000L

typedef unsigned int uint;

///////////////////////////////////////////////////////////////////////////////////////////////
// Null
///////////////////////////////////////////////////////////////////////////////////////////////

struct Null {};

const Null null = Null();

///////////////////////////////////////////////////////////////////////////////////////////////
// Object Sizes
///////////////////////////////////////////////////////////////////////////////////////////////

#define lengthof(v) (sizeof(v)/sizeof(*v))

//---------------------------------------------------------------------------------------------

template <class T>
int bitsInWord()
{
	return std::numeric_limits<T>::digits;
	//return log(T(~T(0)))/log(2)+1;
}

//---------------------------------------------------------------------------------------------

template <class T>
int bitsCount()
{
	int n = 0;
	for (int i = 1; i; i <<= 1, ++n) ;
	return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Psuedo-formal Types
///////////////////////////////////////////////////////////////////////////////////////////////

#define enum__(T) \
	struct T \
	{ \
		enum _##T {

#define __enum(T) \
		} _x; \
	\
		typedef _##T Enum; \
		explicit T() {} \
		T(_##T x) : _x(x) {} \
		explicit T(int k) : _x(_##T(k)) {} \
	\
		T &operator=(_##T x) { _x = x; return *this; } \
		bool operator==(_##T x) const { return _x == x; } \
		bool operator==(const T &e) const { return _x == e._x; } \
		operator _##T() const { return _x; } \
	} 

#if 0
#define enum__(T) \
	struct T \
	{ \
		typedef T This; \
		enum X; \
	\
		explicit T() {} \
		T(X x) : _x(x) {} \
		explicit T(int k) : _x(X(k)) {} \
	\
		enum X {

#define __enum \
		} _x; \
		This &operator=(X x) { _x = x; return *this; } \
		bool operator==(X x) const { return _x == x; } \
		bool operator==(const This &e) const { return _x == e._x; } \
		operator X() const { return _x; } \
		operator X&() { return _x; } \
	}
#endif

//---------------------------------------------------------------------------------------------

#define SCALAR_TYPE(T, Name) \
	class Name \
	{ \
		T _n; \
	\
	protected: \
		Name(T n) : _n(n) {} \
	\
	public: \
		Name() : _n(0) {} \
		Name(const Name &n) : _n(n._n) {} \
	\
		bool operator==(const Name &x) const { return _n == x._n; } \
	\
		T &value() { return _n; } \
		const T &value() const { return _n; } \
	\
		Name &operator=(const Name &n) { _n = n._n; return *this; } \
	}

//---------------------------------------------------------------------------------------------

#define VALUE_TYPE(V, T, v0) \
	class T \
	{ \
		V _val; \
	\
	public: \
		explicit T(V v = v0) : _val(v) {} \
	\
		V value() const { return _val; } \
	\
		T &operator=(const T &t) { _val = t._val; return *this; } \
	}

///////////////////////////////////////////////////////////////////////////////////////////////
// Delayed Construction
///////////////////////////////////////////////////////////////////////////////////////////////

#define NULL_CTOR_SPEC(T_) \
	class T_##_null_ctor \
	{ \
	protected: \
		T_##_null_ctor() {} \
	\
		friend T_;

#define NULL_CTOR_END }

#define FREE_NULL_CTOR_SPEC(T_) \
	class T_##_null_ctor {}

#define NULL_CTOR_DEF(T_) explicit T_(T_##_null_ctor)

#define DELAY_CTOR(T_, member) member(T_##_null_ctor())

#define DELAYED_CTOR(T_, member, args) \
	member.~T_(); \
	new (&member) T_ args

///////////////////////////////////////////////////////////////////////////////////////////////

class Memset0
{
public:
	Memset0(char *p, size_t n)
	{
		memset(p, 0, n);
	}
};

#define Memset0_CTOR Memset0((char *) this, sizeof(*this))

///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#define __thread __declspec(thread)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
// Lambda expressions
///////////////////////////////////////////////////////////////////////////////////////////////

// https://github.com/kennytm/utils/blob/master/traits.hpp

template <typename T>
struct lambda : public lambda<decltype(&T::operator())> {};

template <typename R, typename... Args>
struct lambda<R(Args...)>
{
	typedef R rtype;
	typedef R type(Args...);

	enum { arity = sizeof...(Args) };

	template <size_t i>
	struct arg
	{
		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
	};
};

template <typename R, typename... Args>
struct lambda<R(*)(Args...)> : public lambda<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct lambda<R(C::*)(Args...)> : public lambda<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct lambda<R(C::*)(Args...) const> : public lambda<R(Args...)> {};

// template <typename C, typename R, typename... Args>
// struct lambda<R(C::*)(Args...) volatile> : public lambda<R(Args...)> {};

// template <typename C, typename R, typename... Args>
// struct lambda<R(C::*)(Args...) const volatile> : public lambda<R(Args...)> {};

template <typename FunctionType>
struct lambda<std::function<FunctionType>> : public lambda<FunctionType> {};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_lang_general_
