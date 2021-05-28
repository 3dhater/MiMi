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

#define MI_SDK_VERSION 1


#include "umHalf.h"

enum class miViewportCameraType : unsigned int {
	Perspective,
	Left,
	Right,
	Top,
	Bottom,
	Front,
	Back
};

enum class miEditMode : unsigned int {
	Vertex,
	Edge,
	Polygon,
	Object,
};

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



typedef void(*miCallback_onClickPopup)(unsigned int id);
typedef void(miPlugin::*miCallback_onUpdate)();

struct v2f;
struct v3f;
struct v4f;
class Mat4;
namespace mimath
{
	const float PI = static_cast<float>(3.14159265358979323846);
	const float PIHalf = static_cast<float>(3.14159265358979323846 * 0.5);
	const float PIPlusHalf = static_cast<float>(3.14159265358979323846 + PIHalf);
	const float PIPI = 6.2831853f;
	inline float degToRad(float degrees) { return degrees * (PI / 180.f); }
	inline float radToDeg(float radians) { return radians * (180.f / PI); }

	miVec2 v2f_to_miVec2(const v2f& v);
	miVec3 v3f_to_miVec3(const v3f& v);
	miVec4 v4f_to_miVec4(const v4f& v);
	miMatrix Mat4_to_miMatrix(const Mat4& m);
	v2f miVec2_to_v2f(const miVec2& v);
	v3f miVec3_to_v3f(const miVec3& v);
	v4f miVec4_to_v4f(const miVec4& v);
	Mat4 miMatrix_to_Mat4(const miMatrix& m);
}

enum class miPluginGUIType {
	ObjectParams,
	ImportExportParams,
};
class miPluginGUI
{
public:
	miPluginGUI() {}
	virtual ~miPluginGUI() {}

	// onSelectObject - will call when 1 object wil be selected
	//   must return new or old text
	virtual void AddText(const miVec2& position, const wchar_t* text, const wchar_t* (*onSelectObject)(miSceneObject*)) = 0;
	
	//   must return new ptr
	virtual void AddRangeSliderInt(const miVec4& rect, int minimum, int maximum, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*,int)) = 0;
	virtual void AddRangeSliderFloat(const miVec4& rect, float minimum, float maximum, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float)) = 0;
	virtual void AddRangeSliderIntNoLimit(const miVec4& rect, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int)) = 0;
	virtual void AddRangeSliderFloatNoLimit(const miVec4& rect, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float)) = 0;
	
	virtual void AddCheckBox(const miVec2& position, const wchar_t* text, void (*onClick)(bool isChecked), bool isChecked) = 0;
};

class miSDKImporterHelper
{
public:
	miSDKImporterHelper() {
		m_meshBuilder = new miMeshBuilder<miDefaultAllocator<miPolygon>, miDefaultAllocator<miEdge>, miDefaultAllocator<miVertex>>(0,0,0);
	}
	~miSDKImporterHelper() {
		if (m_meshBuilder)
			delete m_meshBuilder;
	}

	miMeshBuilder<miDefaultAllocator<miPolygon>, miDefaultAllocator<miEdge>, miDefaultAllocator<miVertex>> * m_meshBuilder;
	miPolygonCreator m_polygonCreator;

};

class miSDK
{
public:
	miSDK() {}
	virtual ~miSDK() {}

	virtual miPluginGUI* CreatePluginGUI(miPluginGUIType) = 0;

	virtual miVisualObject* CreateVisualObject(miSceneObject* parent) = 0;

	virtual miViewportCameraType GetActiveViewportCameraType() = 0;

	virtual miKeyboardModifier GetKeyboardModifier() = 0;
	virtual miCursorBehaviorMode GetCursorBehaviorModer() = 0;
	virtual void SetCursorBehaviorModer(miCursorBehaviorMode) = 0; // auto CommonMode when Escape
	virtual miVec2 GetCursorPosition2D() = 0;
	virtual miVec3 GetCursorPosition3D() = 0;
	virtual miVec3 GetCursorPosition3DFirstClick() = 0;

	virtual miEditMode GetEditMode() = 0;
	virtual void SetEditMode(miEditMode) = 0;
	
	virtual void UpdateSceneAabb() = 0;

	// register new object for popup when you press `Add` button
	virtual void RegisterNewObject(miPlugin*, unsigned int id, const wchar_t* category, const wchar_t* objectName) = 0;
	
	// title - L"Wavefront OBJ"
	// extensions - L"obj"
	// gui - some GUI elements for import 
	//  for multiple 'extensions' it must be like this "obj dae fbx"
	virtual void RegisterImporter(miPlugin*, unsigned int id, const wchar_t* title, const wchar_t* extensions, miPluginGUI* gui) = 0;

	virtual void GetRayFromScreen(miRay* ray, const miVec2& coords, const miVec4& viewportRect, const miMatrix& VPInvert) = 0;

	// application will call plugin->onUpdate();
	virtual void SetActivePlugin(miPlugin*) = 0;

	virtual void AddObjectToScene(miSceneObject*, const wchar_t* name) = 0;
	virtual void RemoveObjectFromScene(miSceneObject*) = 0; // only remove, not delete
	virtual void CreateSceneObjectFromHelper(miSDKImporterHelper*,const wchar_t* name) = 0;

	virtual size_t FileSize(const char* fileName) = 0;
};

#endif