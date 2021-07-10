#ifndef _MISDK_H_
#define _MISDK_H_

#include <cassert>

// in future this software must be created for other platforms/OS
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

// help structure
template<typename T1, typename T2>
struct miPair
{
	miPair() {}
	miPair(const T1& _t1, const T2& _t2):m_first(_t1), m_second(_t2) {}
	T1 m_first;
	T2 m_second;
};

// Viewport can draw triangles or lines or all together
enum class miViewportDrawMode : u32 {
	// draw filled triangles with material
	Material,

	// draw only line-model
	Wireframe,

	// draw all
	MaterialWireframe
};

// When Transform Mode != NoTransform app will draw gizmo
// When user will move cursor on gizmo and will press LMB
//  application will remember what part of the gizme he pressed
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

// Each viewport have something like camera + when you create object like a plane
//  you need to get point in 3D space in right plane (XY, XZ, ZY)
enum class miViewportCameraType : u32 {
	Perspective,
	Left,
	Right,
	Top,
	Bottom,
	Front,
	Back
};

// Help structure
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
	// select by rect
	// just default mode
	CommonMode,

	// like target weld or creation new object like plane
	ClickAndDrag, 

	// It will hide cursor, save coords, and setCursorPo( this coords )
	HideCursor,

	// It will change cursor. Will be disabled on escape/RMB/miSceneObject selection
	// set callbacks sdk->SetSelectObjectCallbacks(...); before using
	SelectObject, 

	// set callbacks sdk->SetSelectVertexCallbacks(...); before using
	SelectVertex, 

	// for some actions.
	// For example some actions like Weld: you can select vertices at any time when Weld is active
	// must be disabled on escape/RMB/toggle edit mode/toggle transform mode
	Other
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
#include "miArray.h"
#include "miList.h"
#include "miString.h"
#include "miBST.h"
#include "miMaterial.h"
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

	enum
	{
		Flag_ForVertexEditMode = BIT(0),
		Flag_ForEdgeEditMode = BIT(1),
		Flag_ForPolygonEditMode = BIT(2),
	};

	// onSelectObject - will call when 1 object wil be selected
	//   must return new or old text
	virtual void AddText(const v2f& position, const wchar_t* text, const wchar_t* (*onSelectObject)(miSceneObject*), u32 flags) = 0;
	
	//   must return new ptr
	virtual void AddRangeSliderInt(const v4f& positionSize, int minimum, int maximum, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*,int*), u32 flags) = 0;
	virtual void AddRangeSliderFloat(const v4f& positionSize, float minimum, float maximum, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float*), u32 flags, f32 speed = 1.f) = 0;
	virtual void AddRangeSliderIntNoLimit(const v4f& positionSize, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int*), u32 flags) = 0;
	virtual void AddRangeSliderFloatNoLimit(const v4f& positionSize, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float*), u32 flags, f32 speed = 1.f) = 0;
	
	virtual void AddCheckBox(const v2f& position, const wchar_t* text, void(*onClick)(bool isChecked), bool isChecked, u32 flags) = 0;
	
	virtual void AddButton(const v4f& positionSize, const wchar_t* text, s32 id, void(*onClick)(s32), u32 flags) = 0;
	virtual void AddButtonAsCheckbox(const v4f& positionSize, const wchar_t* text, s32 id, void (*onClick)(s32,bool), 
		void(*onCheck)(s32), void(*onUncheck)(s32), s32 buttonGroupIndex, u32 flags) = 0;
	virtual void UncheckButtonGroup(s32 buttonGroupIndex) = 0;
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
	virtual miCursorBehaviorMode GetCursorBehaviorMode() = 0;
	virtual void SetCursorBehaviorMode(miCursorBehaviorMode) = 0; // auto CommonMode when Escape
	virtual v2f GetCursorPosition2D() = 0;
	virtual v3f GetCursorPosition3D() = 0;
	virtual v3f GetCursorPosition3DFirstClick() = 0;

	virtual miEditMode GetEditMode() = 0;
	virtual void SetEditMode(miEditMode) = 0;
	virtual void SetTransformMode(miTransformMode) = 0;
	
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

	// only for miDefaultAllocator for this mesh
	// obviously
	virtual void AppendMesh(miMesh* mesh_with_miDefaultAllocator, miMesh* other) = 0;
	// for version with pool allocator
	// just for creating a copy of some other model
	virtual void AppendMesh(miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>>* mesh_with_miPoolAllocator, miMesh* other) = 0;

	virtual void AddVertexToSelection(miVertex*, miSceneObject*) = 0;
	virtual void AddEdgeToSelection(miEdge*, miSceneObject*) = 0;
	virtual void AddPolygonToSelection(const miPair<miPolygon*, f32>&, miSceneObject*) = 0;

	// onSelect - will call right after selection
	virtual void SetSelectObjectCallbacks(void (*onSelect)(miEditMode)) = 0;

	virtual void SetPickObjectCallbacks(bool(*onIsGoodObject)(miSceneObject*), void(*onSelect)(miSceneObject*), void(*onCancel)()) = 0;
	virtual void SetPickVertexCallbacks(bool(*onIsGoodVertex)(miSceneObject*, miVertex*), void(*onSelectFirst)(miSceneObject* o, miVertex*), void(*onSelectSecond)(miSceneObject* o, miVertex*, miVertex*), void(*onCancel)()) = 0;
};

#endif