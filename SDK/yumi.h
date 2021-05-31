#ifndef _YUMI_H_
#define _YUMI_H_

#ifndef _YUYU_H_

#if defined _WIN32 || defined __CYGWIN__
#define YY_PLATFORM_WINDOWS
#else
#error Only for Windows
#endif
#ifdef YY_PLATFORM_WINDOWS
#ifdef _MSC_VER
#define YY_C_DECL _cdecl
#ifdef YY_EXPORTS
#define YY_API _declspec(dllexport)
#else
#define YY_API _declspec(dllimport)
#endif
#else
#define YY_C_DECL
#define YY_API
#endif
#else
#define YY_C_DECL
#define YY_API
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define YY_DEBUG
#endif

#ifdef YY_DEBUG
#define YY_DEBUGBREAK __debugbreak()
#else
#define YY_DEBUGBREAK
#endif

#ifdef YY_PLATFORM_WINDOWS
#define YY_FILE __FILE__
#define YY_FUNCTION __FUNCTION__
#define YY_LINE __LINE__

#define YY_ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#define YY_DL_LOADLIBRARY
#define YY_DL_FREELIBRARY
#define YY_DL_GETPROCADDRESS

#ifdef _MSC_VER
#define YY_FORCE_INLINE __forceinline
#else
#define YY_FORCE_INLINE inline
#endif
#endif

#ifdef YY_DEBUG
#define YY_DEBUG_PRINT_FUNC yyLogWriteInfo("%s\n", YY_FUNCTION)
#else
#define YY_DEBUG_PRINT_FUNC
#endif

typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#define YY_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((u32)(u8)(ch0)|((u32)(u8)(ch1)<<8)|\
	((u32)(u8)(ch2)<<16)|((u32)(u8)(ch3)<<24))

#define BIT(x)0x1<<(x)

#include "math/vec.h"
#include "math/mat.h"
#include "math/quat.h"
#include "math/math.h"
#include "math/ray.h"
#include "math/aabb.h"
#include "math/triangle.h"

#endif

#endif