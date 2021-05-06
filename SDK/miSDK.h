#ifndef _MISDK_H_
#define _MISDK_H_

#include <cassert>

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


#include "miLib.h"
#include "miMemory.h"
#include "miSingleton.h"
#include "miList.h"
#include "miString.h"
#include "miBST.h"
#include "miMath.h"
#include "miMesh.h"
#include "miModel.h"
#include "miPlugin.h"
#include "miSelectionFrust.h"
#include "miVisualObject.h"
#include "miSceneObject.h"

enum class miCursorBehaviorMode : unsigned int {
	CommonMode, // select by rect
	ClickAndDrag, // like target weld or creation new object like plane
};

enum class miKeyboardModifier : unsigned int {
	None,
	Ctrl,
	Alt,
	Shift,
	ShiftAlt,
	ShiftCtrl,
	ShiftCtrlAlt,
	CtrlAlt,
	END
};

typedef void(*miCallback_onClickPopup)(unsigned int id);
typedef void(miPlugin::*miCallback_onUpdate)();

struct v2f;
struct v3f;
struct v4f;
class Mat4;
namespace math
{
	miVec2 v2f_to_miVec2(const v2f& v);
	miVec3 v3f_to_miVec3(const v3f& v);
	miVec4 v4f_to_miVec4(const v4f& v);
	miMatrix Mat4_to_miMatrix(const Mat4& m);
	v2f miVec2_to_v2f(const miVec2& v);
	v3f miVec3_to_v3f(const miVec3& v);
	v4f miVec4_to_v4f(const miVec4& v);
	Mat4 miMatrix_to_Mat4(const miMatrix& m);
}

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

	virtual miKeyboardModifier GetKeyboardModifier() = 0;
	virtual miCursorBehaviorMode GetCursorBehaviorModer() = 0;
	virtual void SetCursorBehaviorModer(miCursorBehaviorMode) = 0; // auto CommonMode when Escape
	virtual miVec2 GetCursorPosition2D() = 0;
	virtual miVec3 GetCursorPosition3D() = 0;

	// register new object for popup when you press `Add` button
	// return - popup command id 
	/*  g_CommandID_Plane = sdk->RegisterNewObject(this, L"Polygonal objects", L"Plane");
		g_CommandID_Box = sdk->RegisterNewObject(this, L"Polygonal objects", L"Box");
		g_CommandID_Sphere = sdk->RegisterNewObject(this, L"Polygonal objects", L"Sphere");
		g_CommandID_Point = sdk->RegisterNewObject(this, L"Light", L"Point");
		g_CommandID_Spot = sdk->RegisterNewObject(this, L"Light", L"Spot");
		g_CommandID_Directional = sdk->RegisterNewObject(this, L"Light", L"Directional");
		g_CommandID_FreeCamera = sdk->RegisterNewObject(this, L"Camera", L"Free camera");
	*/
	virtual unsigned int  RegisterNewObject(miPlugin*, const wchar_t* category, const wchar_t* objectName) = 0;

	virtual void GetRayFromScreen(miRay* ray, const miVec2& coords, const miVec4& viewportRect, const miMatrix& VPInvert) = 0;

	// application will call plugin->onUpdate();
	virtual void SetActivePlugin(miPlugin*) = 0;

	virtual void AddObjectToScene(miSceneObject*, const wchar_t* name) = 0;
	virtual void RemoveObjectFromScene(miSceneObject*) = 0; // only remove, not delete
};

#endif