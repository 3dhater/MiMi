#define MILIB_EXPORTS

#include "miSDK.h"
#include "miLib.h"

#ifdef MILIB_PLATFORM_WINDOWS
#include <Windows.h>
#endif

extern "C"
{
	MILIB_API void* MILIB_C_DECL miMalloc(size_t s)
	{
#ifdef MILIB_PLATFORM_WINDOWS
		return HeapAlloc(GetProcessHeap(), 0, (SIZE_T)s);
#else
#error Only for Windows
#endif
	}

	MILIB_API void MILIB_C_DECL miFree(void* ptr)
	{
#ifdef MILIB_PLATFORM_WINDOWS
		HeapFree(GetProcessHeap(), 0, ptr);
#else
#error Only for Windows
#endif
	}
}