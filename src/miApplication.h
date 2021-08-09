#ifndef _MI_APP_H_
#define _MI_APP_H_

#include "yy.h"
#include "yy_window.h"
#include "yy_input.h"
#include "yy_material.h"
#include "math\math.h"

#include "../SDK/miSDK.h"
#include "../SDK/miVisualObject.h"
#include "../SDK/miSceneObject.h"

class miEditableObject;
class miPluginGUIImpl;
class miGUIManager;
class miShortcutManager;
class miSDKImpl;
class miGraphics2D;
class miSelectionFrustImpl;
class miRootObject;
struct miViewportCamera;
struct miImporter;

#include "miGizmo.h"
#include "miViewport.h"
#include "miPopup.h"

#define miViewportBorderSize 1.f
#define miViewportLeftIndent 25.f
#define miViewportRightIndent 225.f
#define miViewportTopIndent 25.f
#define miViewportBottomIndent 50.f
#define miRightSideButtonSize 25.f

#define miCommandID_CameraReset 1
#define miCommandID_CameraMoveToSelection 2
#define miCommandID_ViewportViewPerspective 3
#define miCommandID_ViewportViewFront 4
#define miCommandID_ViewportViewBack 5
#define miCommandID_ViewportViewTop 6
#define miCommandID_ViewportViewBottom 7
#define miCommandID_ViewportViewLeft 8
#define miCommandID_ViewportViewRight 9
#define miCommandID_ViewportToggleFullView 10
#define miCommandID_ViewportToggleGrid 11
#define miCommandID_ViewportDrawMaterial 12
#define miCommandID_ViewportDrawMaterialWireframe 13
#define miCommandID_ViewportDrawWireframe 14
#define miCommandID_ViewportToggleDrawMaterial 15
#define miCommandID_ViewportToggleDrawWireframe 16
#define miCommandID_ConvertToEditableObject 17
#define miCommandID_DeleteSelectedObjects 18
#define miCommandID_ViewportToggleDrawAABB 19
#define miCommandID_for_plugins 100

#define miEventId_ShowMainMenu 1

class miApplicationPlugin : public miPlugin
{
public:
	miApplicationPlugin();
	virtual ~miApplicationPlugin();
	virtual void Init(miSDK* sdk) override;

	virtual void OnShiftGizmo(miGizmoMode, miEditMode, miSceneObject*) override;

	static u32 m_objectType_editableObject;
};

// right side of GUI
enum class miObjectParametersMode
{
	CommonParameters,
	ObjectParameters,
	Materials,
	UV
};


enum class miCursorType : u32
{
	Arrow,
	Cross,
	Hand,
	Help,
	IBeam,
	No,
	Size,
	SizeNESW,
	SizeNS,
	SizeNWSE,
	SizeWE,
	UpArrow,
	Wait,

	SelectObject,
	SelectVertex,
	Rotate,
	_count
};

enum class miEditorType
{
	_3D,
	UV
};

class miApplication
{
	yyInputContext * m_inputContext;
	yyEngineContext* m_engineContext;
	yyWindow*        m_window;
	yyVideoDriverAPI* m_gpu;

	yyCursor* m_cursors[(u32)miCursorType::_count];

	miEditorType m_editorType;

	// draw selection rectangle and other things
	miGraphics2D* m_2d;

	miObjectParametersMode m_objectParametersMode;

	miSelectionFrust* m_selectionFrust;

	miGUIManager* m_GUIManager;
	miShortcutManager* m_shortcutManager;

	yyResource* m_gridModel_perspective1;
	yyResource* m_gridModel_perspective2;
	yyResource* m_gridModel_top1;
	yyResource* m_gridModel_top2;
	yyResource* m_gridModel_top1_10;
	yyResource* m_gridModel_top2_10;
	yyResource* m_gridModel_top1_100;
	yyResource* m_gridModel_top2_100;
	yyResource* m_gridModel_front1;
	yyResource* m_gridModel_front2;
	yyResource* m_gridModel_front1_10;
	yyResource* m_gridModel_front2_10;
	yyResource* m_gridModel_front1_100;
	yyResource* m_gridModel_front2_100;
	yyResource* m_gridModel_left1;
	yyResource* m_gridModel_left2;
	yyResource* m_gridModel_left1_10;
	yyResource* m_gridModel_left2_10;
	yyResource* m_gridModel_left1_100;
	yyResource* m_gridModel_left2_100;
	yyMaterial m_gridModelMaterial;
	void _initGrid();

	yyResource* m_transparentTexture;
	yyResource* m_blackTexture;


	yyColor m_color_windowClearColor;
	yyColor m_color_viewportColor;
	yyColor m_color_viewportBorder;

	bool m_isCursorInWindow;
	bool m_isCursorInGUI;
	bool m_isCursorInViewport;
	bool m_isCursorInUVEditor;
	bool m_isCursorMove;
	bool m_isGUIInputFocus;

	bool _isVertexMouseHover();
	bool _isEdgeMouseHover();
	void _isObjectMouseHover();
	bool m_isEdgeMouseHover;

	// надо определить первый клик в зоне вьюпорта. если был то true. потом двигать камеру и объекты
	// только если m_isViewportInFocus == true;
	bool m_isViewportInFocus;
	miKeyboardModifier  m_keyboardModifier;
	void _updateKeyboardModifier();
	void _callViewportOnSize();

	Aabb m_sceneAabb;
	Aabb m_selectionAabb;
	v4f m_selectionAabb_center;
	v4f m_selectionAabb_extent;

	//miViewportCamera* m_currentViewportDrawCamera;
	miViewport* m_currentViewportDraw;
	miViewport* m_viewportUnderCursor;
	miViewport* m_viewportInMouseFocus;
	miViewportLayout* m_activeViewportLayout;
	miViewportLayout* m_previousViewportLayout; // save here when Alt + W
	miViewportLayout* m_viewportLayouts[miViewportLayout_Count];
	miViewportLayout* m_UVViewport;
	void _initViewports();

	void _initPlugins();

	void _initPopups();

	// for every popup command ID from plugins
	u32 m_miCommandID_for_plugins_count;
	//miBinarySearchTree<miPluginCommandIDMapNode> m_pluginCommandID;

	v2f m_gpuDepthRange;

	miRootObject* m_rootObject;
	void AddObjectToScene(miSceneObject* o, const wchar_t* name);
	void NameIsFree(const miString& name, miSceneObject*, u8*);
	void DestroyAllSceneObjects(miSceneObject* o);
	void RemoveObjectFromScene(miSceneObject* o);

	miEditMode m_editMode;
	miTransformMode m_transformMode;
	miGizmoMode m_gizmoMode;
	miGizmoUVMode m_gizmoModeUV;
	void _setGizmoMode(miGizmoMode);
	void _transformObjects();
	void _transformObjects_move(miSceneObject* o);
	void _transformObjects_scale(miSceneObject* o);
	void _transformObjects_rotate(miSceneObject* o);
	void _transformObjectsReset();
	void _transformObjectsApply();
	
	bool m_isVertexEdgePolygonSelected;
	void _updateIsVertexEdgePolygonSelected();

	void _onSelect();
	void _select_multiple();
	void _select_single();
	
	yyRay m_rayCursor;
	yyArraySimple<miSceneObject*> m_objectsUnderCursor;
	//void _get_objects_under_cursor_(miSceneObject*);
	void _get_objects_under_cursor();
	void _update_selected_objects_array(miSceneObject*);

	miPluginGUIImpl * m_currentPluginGUI;
	miPluginGUIImpl * m_pluginGuiForEditableObject;
	yyArray<miPluginGUI*> m_pluginGuiAll;

	void _initEditableObjectGUI();

	miPopup* _getPopupInViewport();

	yyArraySimple<miSceneObject*> m_objectsOnScene;
	// call it when acreate/import/duplicate/delete...other_operations object
	void _updateObjectsOnSceneArray(miSceneObject*);
	void _updateObjectsOnSceneArray();

	void _rebuildEdgeModels();
	void _rebuildPolygonModels();

	bool _isDoNotSelect();

	yyResource* m_UVPlaneModel;
	yyResource* m_UVPlaneTexture;

public:
	miApplication();
	~miApplication();

	bool IsMouseFocusInUVEditor();
	void SelectAll();
	void DeselectAll();
	void InvertSelection();
	void DeleteSelected();

	void SetEditorType(miEditorType);

	miString GetFreeName(const wchar_t* name);

	yyArraySimple<miSceneObject*> m_selectedObjects;
	void UpdateSelectedObjectsArray();
	
	void _callVisualObjectOnSelect();

	std::vector<yyRay> g_rays;

	miCursorBehaviorMode m_cursorBehaviorMode;
	bool m_isSelectByRectangle;
	bool m_isClickAndDrag;
	v2f m_cursorLMBClickPosition; // save coords onLMB
	v4f m_cursorLMBClickPosition3D; // intersection point
	v4f m_cursorPosition3D;         // intersection point
	//v4f m_cursorLMBClickPosition3DNear; // point on camera near plane
	//v4f m_cursorPosition3DNear;         // 
	//v4f m_cursorLMBClickPosition3DFar; // point on camera far plane
	//v4f m_cursorPosition3DFar;         // 
	yyRay m_screenRayOnClick;
	yyRay m_screenRayCurrent;

	bool m_isLocalTransform;

	f32 m_dt;

	//bool m_isGizmoInput;
	bool m_isGizmoMouseHover;

	bool Init(const char* videoDriver);
	void MainLoop();
	void UpdateViewports();
	void ProcessShortcuts();
	void DrawViewports();
	yyWindow* GetWindowMain();

	miViewportCamera* GetActiveCamera();

	miViewport* m_popupViewport; // set this when click on button whith popup menu
	miPopup m_popup_ViewportCamera;
	miPopup m_popup_ViewportParameters;
	miPopup m_popup_NewObject;
	miPopup m_popup_Importers;
	//miPopup m_popup_InViewport;
	void ShowPopupAtCursor(miPopup* popup);

	miGizmo* m_gizmo;

	miEditMode GetEditMode();

	void CommandCameraReset(miViewport* vp);
	void CommandCameraMoveToSelection(miViewport* vp);
	void CommandViewportChangeView(miViewport* vp, miViewportCameraType);
	void CommandViewportToggleFullView(miViewport* vp);
	void CommandViewportToggleGrid(miViewport* vp);
	void CommandViewportToggleDrawMaterial(miViewport* vp);
	void CommandViewportToggleDrawWireframe(miViewport* vp);
	void CommandViewportToggleAABB(miViewport* vp);
	void CommandViewportSetDrawMode(miViewport* vp, miViewportDrawMode);
	void CommandTransformModeSet(miTransformMode m);
	
	miVertex * m_mouseHoverVertex;
	miSceneObject* m_mouseHoverVertexObject;

	void ConvertSelectedObjectsToEditableObjects();
	miEditableObject* ConvertObjectToEditableObject(miSceneObject*);
	void DeleteObject(miSceneObject*);

	void UpdateSceneAabb();
	void UpdateSelectionAabb();

	void SetCursorBehaviorMode(miCursorBehaviorMode bm);

	miImporter* m_onImport_importer;
	void OnImport(miImporter* importer);
	void OnImport_openDialog();

	void ToggleEditMode(miEditMode);
	void SetEditMode(miEditMode);
	void SetTransformMode(miTransformMode, bool local);
	void SetObjectParametersMode(miObjectParametersMode opm);

	void DrawAabb(const Aabb& aabb, const v4f& _color, const v3f& offset);

	miPlugin* m_pluginActive;
	miPlugin* m_pluginForApp;
	yyArraySmall<miPlugin*> m_plugins;

	void OnSelect();

	yyArraySimple<miPair<miMaterial*, u8>*> m_materials;
	miMaterial* CreateMaterial();

	miSDKImpl* m_sdk;
	void CallPluginGUIOnCancel();

	Aabb m_UVAabb;
	Aabb m_UVAabbOnClick;
	v4f m_UVAabbCenterOnClick;
	v4f m_UVAabbMoveOffset;
	f32 m_UVAngle;
	void TransformUV();
	void TransformUVCancel();
	void OnGizmoUVClick();

	friend class miEditableObject;
	friend class miGizmo;
	friend class miGUIManager;
	friend class miPluginGUIImpl;
	friend class miGraphics2D;
	friend class miSDKImpl;
	friend struct miViewportCamera;
	friend struct miViewport;
	friend class miShortcutManager;
	friend class miVisualObjectImpl;
	friend class miSelectionFrustImpl;
	friend void window_onActivate(yyWindow* window);
	friend void window_callbackOnCommand(s32 commandID);
	friend void log_writeToFile(const char* message);
};

#endif