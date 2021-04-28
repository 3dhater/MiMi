#ifndef _MISDK_H_
#define _MISDK_H_

#if defined _WIN32 || defined __CYGWIN__
#define MI_PLATFORM_WINDOWS
#else
#error Only for Windows
#endif

#ifdef MI_PLATFORM_WINDOWS
#ifdef _MSC_VER
#define MI_C_DECL _cdecl
#ifdef MI_EXPORTS
#define MI_API _declspec(dllexport)
#else
#define MI_API _declspec(dllimport)
#endif
#else
#define MI_C_DECL
#define MI_API
#endif
#else
#define MI_C_DECL
#define MI_API
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define MI_DEBUG
#endif

class miSceneObject;
class miVisualObject;
class miPlugin;
struct miMatrix;

#include "miMath.h"
#include "miModel.h"
#include "miPlugin.h"

class miSDK
{
public:
	miSDK() {}
	virtual ~miSDK() {}

	// call DestroyVisualObject for destoy
	virtual miVisualObject* CreateVisualObject() = 0;
	virtual void DestroyVisualObject(miVisualObject*) = 0;

	virtual void* AllocateMemory(unsigned int size) = 0;
	virtual void  FreeMemory(void*) = 0;
};

#endif