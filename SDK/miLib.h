#ifndef _MI_LIB_H_
#define _MI_LIB_H_

#if defined _WIN32 || defined __CYGWIN__
#define MILIB_PLATFORM_WINDOWS
#else
#error Only for Windows
#endif

#ifdef MILIB_PLATFORM_WINDOWS
#ifdef _MSC_VER
#define MILIB_C_DECL _cdecl
#ifdef MILIB_EXPORTS
#define MILIB_API _declspec(dllexport)
#else
#define MILIB_API _declspec(dllimport)
#endif
#else
#define MILIB_C_DECL
#define MILIB_API
#endif
#else
#define MILIB_C_DECL
#define MILIB_API
#endif

extern "C"
{
	MILIB_API void* MILIB_C_DECL miMalloc(size_t);
	MILIB_API void MILIB_C_DECL miFree(void*);
}

template<typename _type>
_type * miCreate()
{
	_type * ptr = (_type*)miMalloc(sizeof(_type));
	if (ptr)
		new(ptr) _type();
	return ptr;
}

template<typename _type>
void miDestroy(_type * ptr)
{
	assert(ptr);
	ptr->~_type();
	miFree(ptr);
}

#endif