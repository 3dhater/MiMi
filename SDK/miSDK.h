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
#include "yumi.h"

enum class miViewportDrawMode : u32 {
	Material,
	Wireframe,
	MaterialWireframe
};

enum class miGizmoMode : u32 {
	NoTransform,
	MoveX,
	MoveY,
	MoveZ,
	MoveXZ,
	MoveXY,
	MoveZY,
	MoveScreen,
	ScaleX,
	ScaleY,
	ScaleZ,
	ScaleXZ,
	ScaleXY,
	ScaleZY,
	ScaleScreen,
	RotateX,
	RotateY,
	RotateZ,
	RotateScreen,
};

enum class miViewportCameraType : u32 {
	Perspective,
	Left,
	Right,
	Top,
	Bottom,
	Front,
	Back
};

enum class miDirection : u32 {
	North,
	NorthEast,
	East,
	SouthEast,
	South,
	SouthWest,
	West,
	NorthWest
};
inline
const char* miGetDirectionName(miDirection d) {
	switch (d)
	{
	case miDirection::North:
		return "North";
	case miDirection::NorthEast:
		return "NorthEast";
	case miDirection::East:
		return "East";
	case miDirection::SouthEast:
		return "SouthEast";
	case miDirection::South:
		return "South";
	case miDirection::SouthWest:
		return "SouthWest";
	case miDirection::West:
		return "West";
	case miDirection::NorthWest:
		return "NorthWest";
	}
	return "";
}

enum class miEditMode : u32 {
	Vertex,
	Edge,
	Polygon,
	Object,
};

enum class miTransformMode : u32 {
	NoTransform,
	Move,
	Scale,
	Rotate,
};

enum class miCursorBehaviorMode : u32 {
	CommonMode, // select by rect
	ClickAndDrag, // like target weld or creation new object like plane
	HideCursor, // hide cursor, save coords, and setCursorPo( this coords )
};

enum class miKeyboardModifier : u32 {
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
#include "miMesh.h"
#include "miModel.h"
#include "miPlugin.h"
#include "miSelectionFrust.h"
#include "miVisualObject.h"
#include "miSceneObject.h"



typedef void(*miCallback_onClickPopup)(u32 id);
typedef void(miPlugin::*miCallback_onUpdate)();

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
	virtual void AddText(const v2f& position, const wchar_t* text, const wchar_t* (*onSelectObject)(miSceneObject*)) = 0;
	
	//   must return new ptr
	virtual void AddRangeSliderInt(const v4f& rect, int minimum, int maximum, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*,int)) = 0;
	virtual void AddRangeSliderFloat(const v4f& rect, float minimum, float maximum, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float)) = 0;
	virtual void AddRangeSliderIntNoLimit(const v4f& rect, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int)) = 0;
	virtual void AddRangeSliderFloatNoLimit(const v4f& rect, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float)) = 0;
	
	virtual void AddCheckBox(const v2f& position, const wchar_t* text, void (*onClick)(bool isChecked), bool isChecked) = 0;
};

class miSDKImporterHelper
{
public:
	miSDKImporterHelper() {
		Create();
	}
	~miSDKImporterHelper() {
		if (m_meshBuilder)
			delete m_meshBuilder;
	}

	miMeshBuilder<miDefaultAllocator<miPolygon>, miDefaultAllocator<miEdge>, miDefaultAllocator<miVertex>> * m_meshBuilder;
	miPolygonCreator m_polygonCreator;

	// call only after creating new object.
	//  because miMeshBuilder actually is that mesh for edit/draw/raytest...maybe need to rename it
	//   scene object must save pointer.
	/*
		importeHelper.m_meshBuilder->End();
		g_sdk->CreateSceneObjectFromHelper(&importeHelper, curr_word.data());
		importeHelper.Drop();
	*/
	void Drop() {
		m_meshBuilder = 0;
	}

	// call this after Drop() only if you continue create objects
	//  like in OBJ importer, file can contain many objects
	/*
		importeHelper.m_meshBuilder->End();
		g_sdk->CreateSceneObjectFromHelper(&importeHelper, prev_word.data());
		importeHelper.Drop();
		importeHelper.Create();
		importeHelper.m_meshBuilder->Begin();
	*/
	//  ...or just use Create(); without Drop(); 
	void Create() {
		m_meshBuilder = new miMeshBuilder<miDefaultAllocator<miPolygon>, miDefaultAllocator<miEdge>, miDefaultAllocator<miVertex>>(0,0,0);
	}
};

class miSDK
{
public:
	miSDK() {}
	virtual ~miSDK() {}

	virtual miPluginGUI* CreatePluginGUI(miPluginGUIType) = 0;

	virtual miVisualObject* CreateVisualObject(miSceneObject* parent, miVisualObjectType type) = 0;

	virtual miViewportCameraType GetActiveViewportCameraType() = 0;

	virtual miKeyboardModifier GetKeyboardModifier() = 0;
	virtual miCursorBehaviorMode GetCursorBehaviorModer() = 0;
	virtual void SetCursorBehaviorModer(miCursorBehaviorMode) = 0; // auto CommonMode when Escape
	virtual v2f GetCursorPosition2D() = 0;
	virtual v3f GetCursorPosition3D() = 0;
	virtual v3f GetCursorPosition3DFirstClick() = 0;

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

	virtual void GetRayFromScreen(yyRay* ray, const v2f& coords, const v4f& viewportRect, const Mat4& VPInvert) = 0;

	// application will call plugin->onUpdate();
	virtual void SetActivePlugin(miPlugin*) = 0;

	virtual void AddObjectToScene(miSceneObject*, const wchar_t* name) = 0;
	virtual void RemoveObjectFromScene(miSceneObject*) = 0; // only remove, not delete
	virtual void CreateSceneObjectFromHelper(miSDKImporterHelper*,const wchar_t* name) = 0;

	virtual size_t FileSize(const char* fileName) = 0;
};

#endif