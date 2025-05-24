
#ifndef _magenta_types_string_
#define _magenta_types_string_

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <stdexcept>
#include <string>

namespace magenta
{

using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

#define VA_ARGS_MEMORY_THRESHOLD (1024 * 1024)

#define ALLOC_VA_ARGS_BUFFER(F, N, P) \
	char P##_s[N]; \
	va_list P##_args; \
	va_start(P##_args, F); \
	char *P = alloc_va_args_buffer(F, P##_s, N, P##_args); \
	va_args_buffer_deallocator P##_free(P, P##_s); \
	va_end(P##_args)

#define ALLOC_VA_ARGS_BUFFER_1(F, N, P, A) \
	char P##_s[N]; \
	char *P = alloc_va_args_buffer(F, P##_s, N, A); \
	va_args_buffer_deallocator P##_free(P, P##_s)

//---------------------------------------------------------------------------------------------

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

//---------------------------------------------------------------------------------------------

inline char *alloc_va_args_buffer(const char *fmt, char *buf, size_t buf_size, va_list args)
{
	char *p = buf;
	size_t size = buf_size;
	for (;;)
	{
		int chars = vsnprintf(p, size, fmt, args);
		p[size - 1] = '\0';
		if (chars >= 0 && (size_t) chars < size)
			return buf;
		size *= 2;
		p = (char *) (size == 2 * buf_size ? malloc(size) : realloc(p, size));
		if (!p || size > VA_ARGS_MEMORY_THRESHOLD)
		{
			if (p != buf)
				free(p);
			throw std::runtime_error("out of memory");
		}
	}
}

//---------------------------------------------------------------------------------------------

struct va_args_buffer_deallocator
{
	char *p, *buf;
	va_args_buffer_deallocator(char *p, char *buf) : p(p), buf(buf) {}
	~va_args_buffer_deallocator()
	{
		if (p != buf)
			free(p);
	}
};

//---------------------------------------------------------------------------------------------

inline string stringf(const char *fmt, ...)
{
	ALLOC_VA_ARGS_BUFFER(fmt, 1024, p);
	return string(p);
}

//---------------------------------------------------------------------------------------------

inline string vstringf(const char *fmt, va_list args)
{
	ALLOC_VA_ARGS_BUFFER_1(fmt, 1024, p, args);
	return string(p);
}

//---------------------------------------------------------------------------------------------

inline bool operator!(const string &s) { return s.empty(); }

inline const char *operator+(const string &s) { return s.c_str(); }

inline bool start_with(const string &s, const string &part) { return s.find(part) == 0; }

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_types_string_
