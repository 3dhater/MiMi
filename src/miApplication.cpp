#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "miShortcutManager.h"
#include "miSDK.h"
#include "miSDKImpl.h"
#include "miGraphics2D.h"
#include "miSelectionFrustImpl.h"
#include "miRootObject.h"
#include "miPluginGUIImpl.h"
#include "miEditableObject.h"
#include "yy_color.h"
#include "yy_gui.h"
#include "yy_model.h"

#include <map>
#include <ctime>
#include <random>

miApplication * g_app = 0;
Mat4 g_emptyMatrix;

u32 miApplicationPlugin::m_objectType_editableObject = 1;
miApplicationPlugin::miApplicationPlugin() {
}
miApplicationPlugin::~miApplicationPlugin() {}
void miApplicationPlugin::Init(miSDK* sdk) {
}
void miApplicationPlugin::OnShiftGizmo(miGizmoMode gm, miEditMode em, miSceneObject* o) {
	if (o->GetPlugin() != this)
		return;

	switch (em)
	{
	case miEditMode::Vertex:
		break;
	case miEditMode::Edge: {
		if(o->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
			printf("Edge extrude\n");
	}break;
	case miEditMode::Polygon:{
		if (o->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
			printf("Polygon extrude\n");
	}break;
	case miEditMode::Object: {
		if (o->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
			printf("Clone object\n");
	}break;
	default:
		break;
	}
}

void log_writeToFile(const char* message) {
	auto l = strlen(message);
	if (l < 1) return;
	FILE * f = fopen("log.txt", "a");
	fwrite(message, 1, l, f);
	fclose(f);
}

void log_onError(const char* message) {
	fprintf(stderr, message);
	log_writeToFile(message);
}

void log_onInfo(const char* message) {
	fprintf(stdout, message);
	log_writeToFile(message);
}

void log_onWarning(const char* message) {
	fprintf(stderr, message);
	log_writeToFile(message);
}

void window_onCLose(yyWindow* window) {
	yyQuit();
}

void window_callbackOnSize(yyWindow* window) {
}

void window_callbackOnCommand(s32 commandID) {
	switch (commandID)
	{
	default:
		break;
	case miCommandID_CameraReset: g_app->CommandCameraReset(g_app->m_popupViewport); break;
	case miCommandID_CameraMoveToSelection: g_app->CommandCameraMoveToSelection(g_app->m_popupViewport); break;
	case miCommandID_ViewportViewPerspective: g_app->CommandViewportChangeView(g_app->m_popupViewport,miViewportCameraType::Perspective); break;
	case miCommandID_ViewportViewLeft: g_app->CommandViewportChangeView(g_app->m_popupViewport, miViewportCameraType::Left); break;
	case miCommandID_ViewportViewRight: g_app->CommandViewportChangeView(g_app->m_popupViewport, miViewportCameraType::Right); break;
	case miCommandID_ViewportViewTop: g_app->CommandViewportChangeView(g_app->m_popupViewport, miViewportCameraType::Top); break;
	case miCommandID_ViewportViewBottom: g_app->CommandViewportChangeView(g_app->m_popupViewport, miViewportCameraType::Bottom); break;
	case miCommandID_ViewportViewBack: g_app->CommandViewportChangeView(g_app->m_popupViewport, miViewportCameraType::Back); break;
	case miCommandID_ViewportViewFront: g_app->CommandViewportChangeView(g_app->m_popupViewport, miViewportCameraType::Front); break;
	
	case miCommandID_ViewportToggleFullView: g_app->CommandViewportToggleFullView(g_app->m_popupViewport); break;
	case miCommandID_ViewportToggleGrid: g_app->CommandViewportToggleGrid(g_app->m_popupViewport); break;
	
	case miCommandID_ViewportDrawMaterial: g_app->CommandViewportSetDrawMode(g_app->m_popupViewport, miViewportDrawMode::Material); break;
	case miCommandID_ViewportDrawMaterialWireframe: g_app->CommandViewportSetDrawMode(g_app->m_popupViewport, miViewportDrawMode::MaterialWireframe); break;
	case miCommandID_ViewportDrawWireframe: g_app->CommandViewportSetDrawMode(g_app->m_popupViewport, miViewportDrawMode::Wireframe); break;
	
	case miCommandID_ViewportToggleDrawMaterial: g_app->CommandViewportToggleDrawMaterial(g_app->m_popupViewport); break;
	case miCommandID_ViewportToggleDrawWireframe: g_app->CommandViewportToggleDrawWireframe(g_app->m_popupViewport); break;
	case miCommandID_ViewportToggleDrawAABB: g_app->CommandViewportToggleAABB(g_app->m_popupViewport); break;
	
	case miCommandID_ConvertToEditableObject: 
		g_app->ConvertSelectedObjectsToEditableObjects(); 
		g_app->SetObjectParametersMode(miObjectParametersMode::ObjectParameters);
		break;
	
	case miCommandID_DeleteSelectedObjects: g_app->DeleteSelected(); break;
	}
	if (commandID >= miCommandID_for_plugins)
	{
		for (u16 i = 0, sz = g_app->m_sdk->m_importers.size(); i < sz; ++i)
		{
			auto imp = g_app->m_sdk->m_importers[i];
			if (commandID == imp->m_popupIndex)
			{
				g_app->OnImport(imp);
				return;
			}
		}
		/*miPluginCommandIDMapNode mapNode;
		if (g_app->m_pluginCommandID.Get(commandID - miCommandID_for_plugins, mapNode)) {
			mapNode.m_plugin->OnCreateObject(mapNode.m_objectID);
		}*/
		for (u16 i = 0, sz = g_app->m_sdk->m_objectCategories.size(); i < sz; ++i)
		{
			auto cat = g_app->m_sdk->m_objectCategories.m_data[i];
			for (u16 o = 0, osz = cat->m_objects.size(); o < osz; ++o)
			{
				auto cat_obj = cat->m_objects[o];
				if (commandID == cat_obj->m_popupIndex)
				{
					cat_obj->m_plugin->OnCreateObject(cat_obj->m_objectID);
					return;
				}
			}
		}
	}
}

void window_onActivate(yyWindow* window) {
	g_app->m_isViewportInFocus = false;
}

int main(int argc, char* argv[]) {

	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	yyPtr<miApplication> app = yyCreate<miApplication>();

	{
		const char * videoDriverType = "vdd3d11.dll"; // for example read name from .ini
		yyStringA videoDriverTypeStr = videoDriverType;
		for (int i = 0; i < argc; ++i)
		{
			if (strcmp(argv[i], "-vid") == 0)
			{
				++i;
				if (i < argc)
				{
					videoDriverTypeStr = argv[i];
				}
			}
		}

		if (!app.m_data->Init(videoDriverTypeStr.c_str())) {
			YY_PRINT_FAILED;
			return 1;
		}
	}

	app.m_data->MainLoop();

	return 0;
}


miApplication::miApplication() {
	m_pluginForApp = 0;
	m_currentPluginGUI = 0;
	m_pluginGuiForEditableObject = 0;
	m_isLocalTransform = false;
	m_isEdgeMouseHover = false;
	m_objectParametersMode = miObjectParametersMode::CommonParameters;
	m_gizmoMode = miGizmoMode::NoTransform;
	//m_isGizmoInput = false;
	m_isGizmoMouseHover = false;
	m_onImport_importer = 0;
	m_gizmo = 0;
	m_isGUIInputFocus = false;
	m_currentViewportDraw = 0;
	m_editMode = miEditMode::Object;
	m_transformMode = miTransformMode::NoTransform;
	m_pluginActive = 0;
	m_cursorBehaviorMode = miCursorBehaviorMode::CommonMode;
	m_isSelectByRectangle = false;
	m_isClickAndDrag = false;
	m_selectionFrust = new miSelectionFrustImpl;
	
	m_mouseHoverVertex = 0;
	m_mouseHoverVertexObject = 0;

	m_rootObject = (miRootObject*)miMalloc(sizeof(miRootObject));
	new(m_rootObject)miRootObject(0, 0);

	m_miCommandID_for_plugins_count = 0;
	m_popupViewport = 0;
	m_inputContext = 0;
	m_engineContext = 0;
	m_window = 0;
	g_app = this;
	m_dt = 0.f;
	m_gpu = 0;
	m_isVertexEdgePolygonSelected = false;
	m_GUIManager = 0;
	m_activeViewportLayout = 0;
	m_previousViewportLayout = 0;
	m_keyboardModifier = miKeyboardModifier::None;
	m_isViewportInFocus = false;
	m_gridModel_perspective1 = 0;
	m_gridModel_perspective2 = 0;
	m_gridModel_top1 = 0;
	m_gridModel_top2 = 0;
	m_gridModel_top1_10 = 0;
	m_gridModel_top2_10 = 0;
	m_gridModel_top1_100 = 0;
	m_gridModel_top2_100 = 0;
	m_gridModel_front1 = 0;
	m_gridModel_front2 = 0;
	m_gridModel_front1_10 = 0;
	m_gridModel_front2_10 = 0;
	m_gridModel_front1_100 = 0;
	m_gridModel_front2_100 = 0;
	m_gridModel_left1 = 0;
	m_gridModel_left2 = 0;
	m_gridModel_left1_10 = 0;
	m_gridModel_left2_10 = 0;
	m_gridModel_left1_100 = 0;
	m_gridModel_left2_100 = 0;

	for (s32 i = 0; i < miViewportLayout_Count; ++i)
	{
		m_viewportLayouts[i] = 0;
	}

	m_shortcutManager = new miShortcutManager;

	m_2d = 0;
	m_sdk = new miSDKImpl;

	for (u32 i = 0; i < (u32)miCursorType::_count; ++i)
	{
		m_cursors[i] = 0;
	}
}

miApplication::~miApplication() {
	if (m_gizmo) delete m_gizmo;
	for (u32 i = 0; i < (u32)miCursorType::_count; ++i)
	{
		if (m_cursors[i])delete m_cursors[i];
	}

	for(u32 i = 0; i < m_materials.m_size; ++i){
		auto & m = m_materials.m_data[i];
		for (u32 o = 0; o < miMaterialMaxMaps; ++o) {
			if (m->m_first->m_maps[o].m_GPUTexture)
			{
				auto r = (yyResource*)m->m_first->m_maps[o].m_GPUTexture;
				if (r->IsLoaded())
				{
					r->Unload();
					if (!r->IsLoaded())
					{
						yyDeleteTexture(r, true);
						m->m_first->m_maps[o].m_GPUTexture = 0;
					}
				}
			}
		}
		miDestroy(m->m_first);
		delete m;
	}

	if (m_rootObject)
	{
		DestroyAllSceneObjects(m_rootObject);
	}
	for (s32 i = 0, sz = m_pluginGuiAll.size(); i < sz; ++i)
	{
		miDestroy(m_pluginGuiAll[i]);
	}
	for (s32 i = 0; i < m_plugins.size(); ++i)
	{
		m_plugins[i]->~miPlugin();
		miFree(m_plugins[i]);
	}

	if (m_selectionFrust) delete m_selectionFrust;
	if (m_2d) delete m_2d;
	if (m_sdk) delete m_sdk;
	for (s32 i = 0; i < miViewportLayout_Count; ++i)
	{
		if(m_viewportLayouts[i])
			delete m_viewportLayouts[i];
	}
	if (m_gridModel_left1) yyMegaAllocator::Destroy(m_gridModel_left1);
	if (m_gridModel_left2) yyMegaAllocator::Destroy(m_gridModel_left2);
	if (m_gridModel_left1_10) yyMegaAllocator::Destroy(m_gridModel_left1_10);
	if (m_gridModel_left2_10) yyMegaAllocator::Destroy(m_gridModel_left2_10);
	if (m_gridModel_left1_100) yyMegaAllocator::Destroy(m_gridModel_left1_100);
	if (m_gridModel_left2_100) yyMegaAllocator::Destroy(m_gridModel_left2_100);
	if (m_gridModel_front1) yyMegaAllocator::Destroy(m_gridModel_front1);
	if (m_gridModel_front2) yyMegaAllocator::Destroy(m_gridModel_front2);
	if (m_gridModel_front1_10) yyMegaAllocator::Destroy(m_gridModel_front1_10);
	if (m_gridModel_front2_10) yyMegaAllocator::Destroy(m_gridModel_front2_10);
	if (m_gridModel_front1_100) yyMegaAllocator::Destroy(m_gridModel_front1_100);
	if (m_gridModel_front2_100) yyMegaAllocator::Destroy(m_gridModel_front2_100);
	if (m_gridModel_top1) yyMegaAllocator::Destroy(m_gridModel_top1);
	if (m_gridModel_top2) yyMegaAllocator::Destroy(m_gridModel_top2);
	if (m_gridModel_top1_10) yyMegaAllocator::Destroy(m_gridModel_top1_10);
	if (m_gridModel_top2_10) yyMegaAllocator::Destroy(m_gridModel_top2_10);
	if (m_gridModel_top1_100) yyMegaAllocator::Destroy(m_gridModel_top1_100);
	if (m_gridModel_top2_100) yyMegaAllocator::Destroy(m_gridModel_top2_100);
	if (m_gridModel_perspective1) yyMegaAllocator::Destroy(m_gridModel_perspective1);
	if (m_gridModel_perspective2) yyMegaAllocator::Destroy(m_gridModel_perspective2);
	if (m_shortcutManager) delete m_shortcutManager;
	if (m_GUIManager) delete m_GUIManager;
	if (m_window) yyDestroy(m_window);
	if (m_engineContext) yyDestroy(m_engineContext);
	if (m_inputContext) yyDestroy(m_inputContext);
}

miMaterial* miApplication::CreateMaterial() {
	miMaterial* m = 0;

	for (u32 i = 0; i < m_materials.m_size; ++i) {
		auto  _m = m_materials.m_data[i];
		if (_m->m_second == 0)
		{
			m = _m->m_first;
			_m->m_second = 1;
			break;
		}
	}

	if (!m)
	{
		m = miCreate<miMaterial>();
		auto p = new miPair<miMaterial*, u8>(m, 1);
		m_materials.push_back(p);
	}

	static s32 c = 0;
	m->m_name = "Material";
	m->m_name += c++;
	m_GUIManager->OnNewMaterial(m);
	return m;
}

void miApplication::_updateObjectsOnSceneArray(miSceneObject* o) {
	m_objectsOnScene.push_back(o);
	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_updateObjectsOnSceneArray(node->m_data);
			if (node == last) break;
			node = node->m_right;
		}
	}
}
void miApplication::_updateObjectsOnSceneArray() {
	m_objectsOnScene.clear();
	auto node = m_rootObject->GetChildren()->m_head;
	if (node){
		auto last = node->m_left;
		while (true) {
			_updateObjectsOnSceneArray(node->m_data);
			if (node == last) break;
			node = node->m_right;
		}
	}
}

void miApplication::SetCursorBehaviorMode(miCursorBehaviorMode bm) {
	m_cursorBehaviorMode = bm;
	switch (m_cursorBehaviorMode)
	{
	case miCursorBehaviorMode::CommonMode:
	default:
		yySetCursorClip(0, 0, 0);
		yySetCursorDisableAutoChange(false);
		yyShowCursor(true);
		yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Arrow]);
		m_cursors[(u32)miCursorType::Arrow]->Activate();
		break;
	case miCursorBehaviorMode::ClickAndDrag:
		yySetCursorClip(0, 0, 0);
		yySetCursorDisableAutoChange(false);
		yyShowCursor(true);
		yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Cross]);
		m_cursors[(u32)miCursorType::Cross]->Activate();
		break;
	case miCursorBehaviorMode::HideCursor:
		yySetCursorDisableAutoChange(true);
		yyShowCursor(false);
		break;
	case miCursorBehaviorMode::SelectObject:
		break;
	case miCursorBehaviorMode::SelectVertex:
		break;
	case miCursorBehaviorMode::Other:
		break;
	}
	//printf("SetCursorBehaviorMode %i\n", (s32)bm);
}

// need to move children somewhere !!!!
void miApplication::RemoveObjectFromScene(miSceneObject* o) {
	assert(o);
	o->SetParent(0);
	auto node = o->GetChildren()->m_head;
	if (node)
	{
		auto last = node->m_left;
		while (true) {
			node->m_data->SetParent(m_rootObject); // or maybe to other object/ to grandparent?

			if (node == last)
				break;

			node = node->m_right;
		}
	}
	this->_updateObjectsOnSceneArray();
	UpdateSceneAabb();
}

void miApplication::DestroyAllSceneObjects(miSceneObject* o) {
	auto node = o->GetChildren()->m_head;
	if (node)
	{
		auto last = node->m_left;
		while (true){
			DestroyAllSceneObjects(node->m_data);

			if (node == last)
				break;

			node = node->m_right;
		}
	}
	miDestroy(o);
}

void miApplication::_initViewports() {
	for (s32 i = 0; i < miViewportLayout_Count; ++i)
	{
		m_viewportLayouts[i] = new miViewportLayout;
		switch (i)
		{
		default:
			YY_PRINT_FAILED;
			break;
		case miViewportLayout_Full:
			m_viewportLayouts[i]->Add(v4f(0.f, 0.f, 1.f, 1.f), miViewportCameraType::Perspective);
			break;
		case miViewportLayout_Standart: {
			const f32 midX = 0.4;
			m_viewportLayouts[i]->Add(v4f(0.f, 0.f, midX, 0.5f), miViewportCameraType::Top);
			m_viewportLayouts[i]->Add(v4f(midX, 0.f, 1.0f, 0.5f), miViewportCameraType::Left);
			m_viewportLayouts[i]->Add(v4f(0.f, 0.5f, midX, 1.0f), miViewportCameraType::Front);
			m_viewportLayouts[i]->Add(v4f(midX, 0.5f, 1.0f, 1.0f), miViewportCameraType::Perspective);
		}break;
		}
	}

	m_activeViewportLayout = m_viewportLayouts[miViewportLayout_Standart];
	m_activeViewportLayout->ShowGUI();

	m_viewportUnderCursor = m_activeViewportLayout->m_activeViewport;
}

void miApplication::_initPopups() {

	m_popup_ViewportCamera.AddItem(L"Camera Reset", miCommandID_CameraReset, m_shortcutManager->GetText(miShortcutCommandType::viewport_cameraReset));
	m_popup_ViewportCamera.AddItem(L"Camera Move to selection", miCommandID_CameraMoveToSelection, m_shortcutManager->GetText(miShortcutCommandType::viewport_cameraMoveToSelection));

	m_popup_ViewportParameters.AddItem(L"Perspective", miCommandID_ViewportViewPerspective, m_shortcutManager->GetText(miShortcutCommandType::viewport_viewPerspective));
	m_popup_ViewportParameters.AddItem(L"Top", miCommandID_ViewportViewTop, m_shortcutManager->GetText(miShortcutCommandType::viewport_viewTop));
	m_popup_ViewportParameters.AddItem(L"Bottom", miCommandID_ViewportViewBottom, m_shortcutManager->GetText(miShortcutCommandType::viewport_viewBottom));
	m_popup_ViewportParameters.AddItem(L"Left", miCommandID_ViewportViewLeft, m_shortcutManager->GetText(miShortcutCommandType::viewport_viewLeft));
	m_popup_ViewportParameters.AddItem(L"Right", miCommandID_ViewportViewRight, m_shortcutManager->GetText(miShortcutCommandType::viewport_viewRight));
	m_popup_ViewportParameters.AddItem(L"Front", miCommandID_ViewportViewFront, m_shortcutManager->GetText(miShortcutCommandType::viewport_viewFront));
	m_popup_ViewportParameters.AddItem(L"Back", miCommandID_ViewportViewBack, m_shortcutManager->GetText(miShortcutCommandType::viewport_viewBack));
	m_popup_ViewportParameters.AddSeparator();
	m_popup_ViewportParameters.AddItem(L"Toggle full view", miCommandID_ViewportToggleFullView, m_shortcutManager->GetText(miShortcutCommandType::viewport_toggleFullView));
	m_popup_ViewportParameters.AddItem(L"Toggle grid", miCommandID_ViewportToggleGrid, m_shortcutManager->GetText(miShortcutCommandType::viewport_toggleGrid));
	m_popup_ViewportParameters.AddSeparator();
	m_popup_ViewportParameters.AddItem(L"Material", miCommandID_ViewportDrawMaterial, m_shortcutManager->GetText(miShortcutCommandType::viewport_dmMaterial));
	m_popup_ViewportParameters.AddItem(L"Material+Wireframe", miCommandID_ViewportDrawMaterialWireframe, m_shortcutManager->GetText(miShortcutCommandType::viewport_dmMaterialWireframe));
	m_popup_ViewportParameters.AddItem(L"Wireframe", miCommandID_ViewportDrawWireframe, m_shortcutManager->GetText(miShortcutCommandType::viewport_dmWireframe));
	m_popup_ViewportParameters.AddItem(L"Toggle draw material", miCommandID_ViewportToggleDrawMaterial, m_shortcutManager->GetText(miShortcutCommandType::viewport_toggleDMMaterial));
	m_popup_ViewportParameters.AddItem(L"Toggle draw wireframe", miCommandID_ViewportToggleDrawWireframe, m_shortcutManager->GetText(miShortcutCommandType::viewport_toggleDMWireframe));
	m_popup_ViewportParameters.AddSeparator();
	m_popup_ViewportParameters.AddItem(L"Toggle draw AABB", miCommandID_ViewportToggleDrawAABB, m_shortcutManager->GetText(miShortcutCommandType::viewport_toggleDrawAABB));
	
	//m_popup_NewObject
	for (u16 i = 0, sz = m_sdk->m_objectCategories.size(); i < sz; ++i)
	{
		auto cat = m_sdk->m_objectCategories[i];
		
		miPopupInfo* subMenu = 0;
		
		if (cat->m_objects.size())
		{
			subMenu = m_popup_NewObject.m_menu->CreateSubMenu(cat->m_categoryName.data());
		}
		else
		{
			m_popup_NewObject.AddItem(cat->m_categoryName.data(), 0, 0);
		}

		for (u16 k = 0, ksz = cat->m_objects.size(); k < ksz; ++k)
		{
			auto object = cat->m_objects[k];
			subMenu->AddItem(object->m_objectName.data(), object->m_popupIndex, 0);
		}
	}

	for (u16 i = 0, sz = m_sdk->m_importers.size(); i < sz; ++i)
	{
		auto imp = m_sdk->m_importers[i];
		
		m_popup_Importers.AddItem(imp->m_title.data(), imp->m_popupIndex, 0);
	}

}

void miApplication::_initPlugins() {
	for (auto & entry : yy_fs::directory_iterator(L"plugins/"))
	{
		auto path = entry.path();
		if (!path.has_extension())
			continue;

		auto ex = path.extension();
		if (ex != ".plg")
			continue;

		auto lib_str = path.generic_string();

		auto module = yyLoadLibrary(lib_str.c_str());
		if (!module)
			continue;

		yyLogWriteInfo("Load plugin: %s...\n", lib_str.data());

		miplCreatePlugin_t miplCreatePlugin = (miplCreatePlugin_t)yyGetProcAddress(module, miplCreatePlugin_funcName);
		if (!miplCreatePlugin)
		{
			yyLogWriteInfo("FAIL (function %s not found)\n", miplCreatePlugin_funcName);
			continue;
		}
		

		auto newPlugin = miplCreatePlugin();
		
		bool isDebug = false;
#ifdef YY_DEBUG
		isDebug = true;
#endif
		if (newPlugin->IsDebug() && isDebug != true)
		{
			newPlugin->~miPlugin();
			miFree(newPlugin);
			yyLogWriteInfo("FAIL (debug version)\n");
			continue;
		}

		if (newPlugin->CheckVersion() != MI_SDK_VERSION)
		{
			newPlugin->~miPlugin();
			miFree(newPlugin);
			yyLogWriteInfo("FAIL (bad version)\n");
			continue;
		}
		newPlugin->Init(m_sdk);

		yyLogWriteInfoW(L"DONE (%s)\n", newPlugin->GetName());

		m_plugins.push_back(newPlugin);
	}

	m_pluginForApp = (miApplicationPlugin*)miMalloc(sizeof(miApplicationPlugin));
	new(m_pluginForApp)miApplicationPlugin();
	m_plugins.push_back(m_pluginForApp);

	m_plugins.shrink_to_fit();

	_initEditableObjectGUI();
}


bool miApplication::Init(const char* videoDriver) {
	{
		FILE * f = fopen("log.txt", "w");
		fwrite(" ", 1, 1, f);
		fclose(f);
	}


	m_inputContext = yyCreate<yyInputContext>();
	m_engineContext = yyCreate<yyEngineContext>();
	m_engineContext->init(m_inputContext);

	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onWarning);

	for (u32 i = 0; i < (u32)miCursorType::_count; ++i)
	{
		if (i < (u32)yyCursorType::Custom)
			m_cursors[i] = new yyCursor((yyCursorType)i);
		else 
			m_cursors[i] = new yyCursor(yyCursorType::Arrow);

		switch ((miCursorType)i)
		{
		case miCursorType::Arrow: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/arrow.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::Cross: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/prec.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::Hand: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/link.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::Help: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/helpsel.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::IBeam: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/select.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::No: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/unavail.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::Size: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/move.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::SizeNESW: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/nesw.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::SizeNS: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/ns.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::SizeNWSE: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/nwse.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::SizeWE: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/ew.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::UpArrow: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/up.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::Wait: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/working.ani", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::SelectObject: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/prec.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case miCursorType::SelectVertex: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/prec.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		default:break;
		}

		if(i < (u32)yyCursorType::Custom)
			yySetCursor((yyCursorType)i, m_cursors[i]);
	}

	m_window = yyCreate<yyWindow>();
	u32 windowStyle = 0;
	if (!m_window->init(800, 600, windowStyle))
	{
		YY_PRINT_FAILED;
		return false;
	}
	
	yySetMainWindow(m_window);
	m_window->m_onCommand = window_callbackOnCommand;
	m_window->m_onClose = window_onCLose;
	m_window->m_onSize = window_callbackOnSize;
	m_window->m_onMaximize = window_callbackOnSize;
	m_window->m_onRestore = window_callbackOnSize;
	m_window->m_onActivate = window_onActivate;
	ShowWindow(m_window->m_hWnd, SW_MAXIMIZE);

	if (!yyInitVideoDriver(videoDriver, m_window))
	{
		yyLogWriteWarning("Can't load video driver : %s\n", videoDriver);
		for (auto & entry : yy_fs::directory_iterator(yy_fs::current_path()))
		{
			auto path = entry.path();
			if (path.has_extension())
			{
				auto ex = path.extension();
				if (ex == ".dll" && yyIsValidVideoDriver(path.generic_string().c_str()))
				{
					yyLogWriteWarning("Trying to load video driver : %s\n", path.generic_string().c_str());

					if (yyInitVideoDriver(path.generic_string().c_str(), m_window))
						goto vidOk;
					else
						yyLogWriteWarning("Can't load video driver : %s\n", path.generic_string().c_str());
				}
			}
		}
		YY_PRINT_FAILED;
		return false;
	}

vidOk:

	m_gpu = yyGetVideoDriverAPI();
	m_gpu->GetDepthRange(&m_gpuDepthRange);
	m_gpu->UseVSync(true);
	yySetDefaultTexture(yyGetTextureFromCache("../res/gui/white.dds"));

	m_blackTexture = yyGetTextureFromCache("../res/gui/black.dds");
	m_transparentTexture = yyGetTextureFromCache("../res/gui/tr.dds");

	m_color_viewportBorder = ColorYellow;

	m_color_windowClearColor.set(0.41f);
	m_gpu->SetClearColor(m_color_windowClearColor.m_data[0], m_color_windowClearColor.m_data[1], m_color_windowClearColor.m_data[2], 1.f);
	m_window->SetTitle(m_gpu->GetVideoDriverName());

	m_color_viewportColor.set(0.35f);
	
	m_gizmo = new miGizmo;
	m_GUIManager = new miGUIManager;

	_initPlugins();
	_initPopups();
	_initGrid();

	
	m_2d = new miGraphics2D;
	m_2d->Init(m_window);

	_initViewports();
	m_2d->UpdateClip();

	yyGUIRebuild();


	return true;
}

yyWindow* miApplication::GetWindowMain() {
	return m_window;
}

miViewportCamera* miApplication::GetActiveCamera() {
	return m_activeViewportLayout->m_activeViewport->m_activeCamera;
}

void miApplication::MainLoop() {
	f32 fps_timer = 0.f;
	u32 fps = 0;
	u32 fps_counter = 0;

	yyEvent currentEvent;

	while (yyRun(&m_dt))
	{
	//	yySetCursor(yyCursorType::Arrow, m_cursors[(u32)yyCursorType::Arrow]);
		_updateKeyboardModifier();
		m_isCursorMove = (m_inputContext->m_mouseDelta.x != 0.f) || (m_inputContext->m_mouseDelta.y != 0.f);
	
		m_GUIManager->m_default_value_float = 0.f;

		m_gizmo->OnStartFrame();

		m_isCursorInWindow = false;

		if (math::pointInRect(m_inputContext->m_cursorCoords.x, m_inputContext->m_cursorCoords.y,
			v4f(0.f, 0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y)))
		{
			m_isCursorInWindow = true;
		}

		++fps_counter;
		fps_timer += m_dt;

		if (fps_timer > 1.f)
		{
			fps_timer = 0.f;
			fps = fps_counter;
			fps_counter = 0;
		}
		
		m_isCursorInGUI = yyGUIUpdate(m_dt);
		if (!m_isCursorInGUI)
		{
			if (m_inputContext->m_cursorCoords.x >= 0 && m_inputContext->m_cursorCoords.x <= miViewportLeftIndent)
				m_isCursorInGUI = true;
			else if (m_inputContext->m_cursorCoords.x >= m_window->m_currentSize.x - miViewportRightIndent && m_inputContext->m_cursorCoords.x <= m_window->m_currentSize.x)
				m_isCursorInGUI = true;
			else if (m_inputContext->m_cursorCoords.y >= 0 && m_inputContext->m_cursorCoords.y <= miViewportTopIndent)
				m_isCursorInGUI = true;
			else if (m_inputContext->m_cursorCoords.y >= m_window->m_currentSize.y - miViewportBottomIndent && m_inputContext->m_cursorCoords.y <= m_window->m_currentSize.y)
				m_isCursorInGUI = true;
		}
		m_isGUIInputFocus = yyGUIIsInputFocus();

		if (
			//m_isCursorMove && 
			m_isCursorInWindow && !m_isCursorInGUI && !m_isGUIInputFocus)
		{
			m_sdk->GetRayFromScreen(
				&m_rayCursor, 
				m_inputContext->m_cursorCoords,
				m_viewportUnderCursor->m_currentRect,
				m_viewportUnderCursor->m_activeCamera->m_viewProjectionInvertMatrix
			);

			if (m_gizmoMode == miGizmoMode::NoTransform
				&& !m_inputContext->m_isMMBHold // camera move/rotate
				&& m_editMode == miEditMode::Object
				&& m_isCursorMove
				)
			{
				_get_objects_under_cursor();
			}

			if (m_cursorBehaviorMode == miCursorBehaviorMode::SelectObject
				&& m_selectedObjects.m_size)
			{
				bool setCursor = false;
				if (m_objectsUnderCursor.m_size)
				{
					auto selectedObject = m_selectedObjects.m_data[0];
					if (m_objectsUnderCursor.m_data[0] != selectedObject)
					{
						if (m_sdk->m_pickObject_onIsGoodObject)
						{
							if (m_sdk->m_pickObject_onIsGoodObject(m_objectsUnderCursor.m_data[0]))
							{
								if (m_sdk->m_pickObject_onSelect && m_inputContext->m_isLMBDown)
								{
									m_sdk->m_pickObject_onSelect(m_objectsUnderCursor.m_data[0]);
									m_objectsUnderCursor.clear();
								}
								setCursor = true;
							}
						}
					}
				}
				if (setCursor)
				{
					yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::SelectObject]);
					m_cursors[(u32)miCursorType::SelectObject]->Activate();
				}
				else
				{
					yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Arrow]);
					m_cursors[(u32)miCursorType::Arrow]->Activate();
				}
			}

			if (m_cursorBehaviorMode == miCursorBehaviorMode::SelectVertex
				&& m_selectedObjects.m_size)
			{
				bool setCursor = false;
				if (m_mouseHoverVertex)
				{
					static miVertex* pickedVertex1 = 0;

					if (m_sdk->m_pickVertex_onIsGoodVertex)
					{
						if (m_sdk->m_pickVertex_onIsGoodVertex(m_mouseHoverVertexObject, m_mouseHoverVertex))
						{
							if (m_inputContext->m_isLMBDown) 
							{
								pickedVertex1 = m_mouseHoverVertex;
								if (m_sdk->m_pickVertex_onSelectFirst)
									m_sdk->m_pickVertex_onSelectFirst(m_mouseHoverVertexObject, m_mouseHoverVertex);
							}
							if (pickedVertex1 && m_inputContext->m_isLMBUp)
							{
								if (m_sdk->m_pickVertex_onSelectSecond)
									m_sdk->m_pickVertex_onSelectSecond(m_mouseHoverVertexObject, pickedVertex1, m_mouseHoverVertex);
								pickedVertex1 = 0;
							}

							setCursor = true;
						}
					}
				}
				if (setCursor)
				{
					yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::SelectVertex]);
					m_cursors[(u32)miCursorType::SelectVertex]->Activate();
				}
				else
				{
					yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Arrow]);
					m_cursors[(u32)miCursorType::Arrow]->Activate();
				}
			}

			
		}

		while (yyPollEvent(currentEvent))
		{
			switch (currentEvent.m_type)
			{
			default:
			case yyEventType::Engine:
				break;
			case yyEventType::System:
				break;
			case yyEventType::Window: {
				if (currentEvent.m_event_window.m_event == yyEvent_Window::size_changed) {
					yyGetVideoDriverAPI()->UpdateMainRenderTarget(m_window->m_currentSize,
						v2f((f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y));
					yyGUIRebuild();
					_callViewportOnSize();
					m_2d->UpdateClip();
				}
			}break;
			case yyEventType::User: {
				switch (currentEvent.m_event_user.m_id)
				{
				case miEventId_ShowMainMenu:
					m_GUIManager->ShowMenu(*((s32*)currentEvent.m_event_user.m_data));
					break;
				default:
					break;
				}
			}break;
			}
		}

		switch (*m_engineContext->m_state)
		{
		default:
			break;
		case yySystemState::Run:
		{
			auto old_active_viewport = m_activeViewportLayout->m_activeViewport;
			
			if (!m_isGUIInputFocus)
				UpdateViewports();
		
			if (m_pluginActive  /*&& (m_activeViewportLayout->m_activeViewport == old_active_viewport)*/)
			{
				bool isCancel = m_inputContext->IsKeyHit(yyKey::K_ESCAPE);
				if (!isCancel) isCancel = m_inputContext->m_isRMBUp;

				
				if (m_inputContext->m_isLMBDown)
					m_pluginActive->OnLMBDown(m_selectionFrust, m_isCursorInGUI);

				if (m_inputContext->m_isLMBUp)
					m_pluginActive->OnLMBUp(m_selectionFrust, m_isCursorInGUI);

				if (m_isCursorMove && m_pluginActive)
					m_pluginActive->OnCursorMove(m_selectionFrust, m_isCursorInGUI);

				if (isCancel && m_pluginActive)
					m_pluginActive->OnCancel(m_selectionFrust, m_isCursorInGUI);

				if(m_pluginActive)
					m_pluginActive->OnUpdate(m_selectionFrust, m_isCursorInGUI);
			}


			m_gpu->BeginDraw();
			m_gpu->ClearAll();

			if (m_inputContext->IsKeyHit(yyKey::K_DELETE) && !m_isGUIInputFocus)
			{
				if (m_cursorBehaviorMode == miCursorBehaviorMode::CommonMode)
				{
					DeleteSelected();
				}
			}

			DrawViewports();

			yyGUIDrawAll(m_dt);

			m_gpu->EndDraw();

			m_2d->BeginDraw();
			if (m_isSelectByRectangle)
			{
				m_2d->DrawSelectionBox(m_cursorLMBClickPosition, m_inputContext->m_cursorCoords);
			}
			if (m_isClickAndDrag)
			{
				m_2d->DrawClickAndDrag(m_cursorLMBClickPosition, m_inputContext->m_cursorCoords);
			}

			m_2d->EndDraw();

			m_gpu->SwapBuffers();
			
			if(!m_isGUIInputFocus)
				ProcessShortcuts();
		}
		}
		m_gizmo->OnEndFrame();
	}

	if (m_pluginActive)
		m_pluginActive->OnCancel(m_selectionFrust, m_isCursorInGUI);
}

void miApplication::_updateKeyboardModifier() {
	s32 ctrl_shift_alt = 0;
	if (m_inputContext->IsKeyHold(yyKey::K_LALT) || m_inputContext->IsKeyHold(yyKey::K_RALT))
		ctrl_shift_alt |= 1;
	if (m_inputContext->IsKeyHold(yyKey::K_LSHIFT) || m_inputContext->IsKeyHold(yyKey::K_RSHIFT))
		ctrl_shift_alt |= 2;
	if (m_inputContext->IsKeyHold(yyKey::K_LCTRL) || m_inputContext->IsKeyHold(yyKey::K_RCTRL))
		ctrl_shift_alt |= 4;

	switch (ctrl_shift_alt)
	{
	default:
	case 0:  m_keyboardModifier = miKeyboardModifier::None;          break;
	case 1:  m_keyboardModifier = miKeyboardModifier::Alt;           break;
	case 2:  m_keyboardModifier = miKeyboardModifier::Shift;         break;
	case 3:  m_keyboardModifier = miKeyboardModifier::ShiftAlt;      break;
	case 4:  m_keyboardModifier = miKeyboardModifier::Ctrl;          break;
	case 5:  m_keyboardModifier = miKeyboardModifier::CtrlAlt;       break;
	case 6:  m_keyboardModifier = miKeyboardModifier::ShiftCtrl;     break;
	case 7:  m_keyboardModifier = miKeyboardModifier::ShiftCtrlAlt;  break;
	}
}

void miApplication::CallPluginGUIOnCancel() {
	if (m_sdk->m_pickObject_onCancel)
		m_sdk->m_pickObject_onCancel();
}

void miApplication::ProcessShortcuts() {
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_cameraReset)) this->CommandCameraReset(m_activeViewportLayout->m_activeViewport);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_cameraMoveToSelection)) this->CommandCameraMoveToSelection(m_activeViewportLayout->m_activeViewport);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_viewPerspective)) this->CommandViewportChangeView(m_activeViewportLayout->m_activeViewport, miViewportCameraType::Perspective);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_viewTop)) this->CommandViewportChangeView(m_activeViewportLayout->m_activeViewport, miViewportCameraType::Top);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_viewBottom)) this->CommandViewportChangeView(m_activeViewportLayout->m_activeViewport, miViewportCameraType::Bottom);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_viewLeft)) this->CommandViewportChangeView(m_activeViewportLayout->m_activeViewport, miViewportCameraType::Left);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_viewRight)) this->CommandViewportChangeView(m_activeViewportLayout->m_activeViewport, miViewportCameraType::Right);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_viewFront)) this->CommandViewportChangeView(m_activeViewportLayout->m_activeViewport, miViewportCameraType::Front);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_viewBack)) this->CommandViewportChangeView(m_activeViewportLayout->m_activeViewport, miViewportCameraType::Back);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_toggleGrid)) this->CommandViewportToggleGrid(m_activeViewportLayout->m_activeViewport);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_toggleFullView)) this->CommandViewportToggleFullView(m_activeViewportLayout->m_activeViewport);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_dmMaterial)) this->CommandViewportSetDrawMode(m_activeViewportLayout->m_activeViewport, miViewportDrawMode::Material);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_dmMaterialWireframe)) this->CommandViewportSetDrawMode(m_activeViewportLayout->m_activeViewport, miViewportDrawMode::MaterialWireframe);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_dmWireframe)) this->CommandViewportSetDrawMode(m_activeViewportLayout->m_activeViewport, miViewportDrawMode::Wireframe);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_toggleDMMaterial)) this->CommandViewportToggleDrawMaterial(m_activeViewportLayout->m_activeViewport);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_toggleDMWireframe)) this->CommandViewportToggleDrawWireframe(m_activeViewportLayout->m_activeViewport);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::transfromMode_NoTransform)) this->CommandTransformModeSet(miTransformMode::NoTransform);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::transfromMode_Move)) this->CommandTransformModeSet(miTransformMode::Move);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::transfromMode_Scale)) this->CommandTransformModeSet(miTransformMode::Scale);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::transfromMode_Rotate)) this->CommandTransformModeSet(miTransformMode::Rotate);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::select_selectAll)) this->SelectAll();
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::select_deselectAll)) this->DeselectAll();
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::select_invertSelection)) this->InvertSelection();
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::editMode_Vertex)) this->ToggleEditMode(miEditMode::Vertex);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::editMode_Edge)) this->ToggleEditMode(miEditMode::Edge);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::editMode_Polygon)) this->ToggleEditMode(miEditMode::Polygon);
}


void miApplication::_get_objects_under_cursor() {
	m_objectsUnderCursor.clear();
	for (u32 i = 0; i < m_objectsOnScene.m_size; ++i)
	{
		auto o = m_objectsOnScene.m_data[i];
		v4f ip;
		f32 d = 0.f;
		if (o->IsRayIntersect(&m_rayCursor, &ip, &d))
		{
			o->m_cursorIntersectionPointDistance = d;
			o->m_cursorIntersectionPoint = ip;
			m_objectsUnderCursor.push_back(o);
			//wprintf(L"%s", o->GetName().data());
		}
	}

	struct _pred{
		bool operator() (miSceneObject* a, miSceneObject* b) const{
			return a->GetDistanceToCursorIP() > b->GetDistanceToCursorIP();
		}
	};
	m_objectsUnderCursor.sort_insertion(_pred());
}

bool miApplication::_isEdgeMouseHover() {
	for (u32 i = 0; i < m_objectsOnScene.m_size; ++i)
	{
		auto o = m_objectsOnScene.m_data[i];
		if (o->IsSelected())
		{
			if (o->IsEdgeMouseHover(m_selectionFrust))
			{
				m_isEdgeMouseHover = true;
				return true;
			}
		}
	}
	return false;
}
bool miApplication::_isVertexMouseHover() {
	for (u32 i = 0; i < m_objectsOnScene.m_size; ++i)
	{
		auto o = m_objectsOnScene.m_data[i];
		if (o->IsSelected())
		{
			m_mouseHoverVertexObject = o;
			m_mouseHoverVertex = o->IsVertexMouseHover(m_selectionFrust);
			if (m_mouseHoverVertex)
				return true;
		}
	}
	return false;
}
void miApplication::_isObjectMouseHover() {
	m_mouseHoverVertex = 0;
	m_isEdgeMouseHover = false;
	switch (m_editMode)
	{
	case miEditMode::Vertex:
		_isVertexMouseHover();
		break;
	case miEditMode::Edge:
		_isEdgeMouseHover();
		break;
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
	default:
		break;
	}
}

void miApplication::UpdateViewports() {
	if (m_isCursorInViewport)
	{
		if (m_inputContext->m_isRMBUp && m_gizmoMode == miGizmoMode::NoTransform)
		{
			miPopup* p = _getPopupInViewport();
			ShowPopupAtCursor(p);
			delete p;
		}
		if (m_gizmoMode == miGizmoMode::NoTransform)
		{
			if (m_isCursorMove)
			{
				const f32 _size = 6.f;
				m_selectionFrust->CreateWithFrame(
					v4f(
						m_inputContext->m_cursorCoords.x - _size,
						m_inputContext->m_cursorCoords.y - _size,
						m_inputContext->m_cursorCoords.x + _size,
						m_inputContext->m_cursorCoords.y + _size),
					m_activeViewportLayout->m_activeViewport->m_currentRect,
					m_activeViewportLayout->m_activeViewport->m_activeCamera->m_viewProjectionInvertMatrix);
				if (m_editMode == miEditMode::Vertex)
				{
					_isObjectMouseHover();
					if (m_cursorBehaviorMode != miCursorBehaviorMode::SelectVertex)
					{
						if (m_mouseHoverVertex)
						{
							yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Cross]);
							m_cursors[(u32)miCursorType::Cross]->Activate();
						}
						else
						{
							yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Arrow]);
							m_cursors[(u32)miCursorType::Arrow]->Activate();
						}
					}
				}
				else if (m_editMode == miEditMode::Edge)
				{
					_isObjectMouseHover();
					if (m_isEdgeMouseHover)
					{
						yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Cross]);
						m_cursors[(u32)miCursorType::Cross]->Activate();
					}
					else
					{
						yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Arrow]);
						m_cursors[(u32)miCursorType::Arrow]->Activate();
					}
				}
			}
		}
	}

	if (m_isViewportInFocus && (_isDoNotSelect() == false))
	{
		if (m_inputContext->m_isLMBUp)
		{			
			if (m_gizmoMode == miGizmoMode::NoTransform)
			{
				_onSelect();
			}
			else
			{
				m_gizmo->OnRelease();
				_transformObjectsApply();
				UpdateSceneAabb();
				UpdateSelectionAabb();
			}
		}

		if (m_gizmoMode != miGizmoMode::NoTransform)
		{
			if (m_inputContext->m_isRMBUp || m_inputContext->IsKeyHit(yyKey::K_ESCAPE))
			{
				m_isViewportInFocus = false;
				m_gizmo->OnEscape();
				_transformObjectsReset();
			}
			else if (m_isCursorMove)
			{
				_transformObjects();
			}
		}
	}

	if (m_gizmoMode == miGizmoMode::NoTransform && m_cursorBehaviorMode == miCursorBehaviorMode::CommonMode)
	{
		if (m_inputContext->IsKeyHit(yyKey::K_ESCAPE))
		{
			DeselectAll();
		}
	}

	if (m_cursorBehaviorMode == miCursorBehaviorMode::SelectObject
		|| m_cursorBehaviorMode == miCursorBehaviorMode::SelectVertex
		|| m_cursorBehaviorMode == miCursorBehaviorMode::Other)
	{
		if (m_inputContext->IsKeyHit(yyKey::K_ESCAPE) || m_inputContext->m_isRMBUp)
		{
			SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
			CallPluginGUIOnCancel();
		}
	}


	if (m_isSelectByRectangle)
	{
		if (m_inputContext->m_isLMBUp
			|| m_inputContext->m_isRMBUp
			|| m_inputContext->m_isRMBDown
			|| m_inputContext->m_isMMBDown
			|| m_inputContext->m_isX1MBDown
			|| m_inputContext->m_isX2MBDown
			|| m_inputContext->IsKeyHit(yyKey::K_ESCAPE))
		{
			m_isSelectByRectangle = false;
			m_isViewportInFocus = false;
		}
	}

	if (m_isClickAndDrag)
	{
		if (m_inputContext->m_isLMBUp
			|| m_inputContext->m_isRMBUp
			|| m_inputContext->m_isRMBDown
			|| m_inputContext->m_isMMBDown
			|| m_inputContext->m_isX1MBDown
			|| m_inputContext->m_isX2MBDown
			|| m_inputContext->IsKeyHit(yyKey::K_ESCAPE))
		{
			m_isClickAndDrag = false;
			m_isViewportInFocus = false;

			if (m_inputContext->m_isLMBUp)
			{
			}
		}
	}

	m_isCursorInViewport = false;
	//yyRay ray;
	if (!m_isCursorInGUI)
	{
		if (m_inputContext->m_isLMBDown)
		{
			m_cursorLMBClickPosition = m_inputContext->m_cursorCoords;
			/*yyRay r;
			m_sdk->GetRayFromScreen(&r, m_inputContext->m_cursorCoords,
				m_activeViewportLayout->m_activeViewport->m_currentRect,
				m_activeViewportLayout->m_activeViewport->m_activeCamera->m_viewProjectionInvertMatrix);*/
			
		//	g_rays.push_back(r);
		}

		for (u16 i = 0, sz = m_activeViewportLayout->m_viewports.size(); i < sz; ++i)
		{
			auto viewport = m_activeViewportLayout->m_viewports[i];

			viewport->m_isCursorInRect =
				math::pointInRect(m_inputContext->m_cursorCoords.x, m_inputContext->m_cursorCoords.y,
					viewport->m_currentRect);

			if (viewport->m_isCursorInRect)
			{
				m_sdk->GetRayFromScreen(&m_screenRayCurrent, m_inputContext->m_cursorCoords,
					m_activeViewportLayout->m_activeViewport->m_currentRect,
					m_activeViewportLayout->m_activeViewport->m_activeCamera->m_viewProjectionInvertMatrix);
			//	m_cursorPosition3DNear = ray.m_origin;
			//	m_cursorPosition3DFar = ray.m_end;

				m_isCursorInViewport = true;
				m_viewportUnderCursor = viewport;

				

				if(m_inputContext->m_wheelDelta)
					viewport->m_activeCamera->Zoom();

				if (m_inputContext->m_isLMBDown
					|| m_inputContext->m_isMMBDown
					|| m_inputContext->m_isRMBDown
					|| m_inputContext->m_isX1MBDown
					|| m_inputContext->m_isX2MBDown)
				{
					if (m_activeViewportLayout->m_activeViewport != viewport)
					{
						m_activeViewportLayout->m_activeViewport = viewport;
						m_2d->UpdateClip();
					}
				}
			}
		}
	}

	if (m_inputContext->m_isLMBDown && !m_isCursorInGUI)
	{
		m_cursorLMBClickPosition3D = m_activeViewportLayout->m_activeViewport->GetCursorRayHitPosition(m_inputContext->m_cursorCoords);
		//m_cursorLMBClickPosition3DNear = ray.m_origin;
		//m_cursorLMBClickPosition3DFar = ray.m_end;
		m_screenRayOnClick = m_screenRayCurrent;

		m_cursorPosition3D = m_cursorLMBClickPosition3D;
		//printf("%f %f %f\n", m_cursorLMBClickPosition3D.x, m_cursorLMBClickPosition3D.y, m_cursorLMBClickPosition3D.z);
	}

	static bool is_pan_move = false;
	if (m_inputContext->m_isMMBHold && m_isViewportInFocus)
	{
		switch (m_keyboardModifier)
		{
		default:
			is_pan_move = true;
			yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Size]);
			m_cursors[(u32)miCursorType::Size]->Activate();
			break;
		case miKeyboardModifier::Alt:
			break;
		case miKeyboardModifier::CtrlAlt:
			break;
		case miKeyboardModifier::ShiftCtrlAlt:
			break;
		}
	}
	else
	{
		if (is_pan_move)
		{
			is_pan_move = false;
			//yySetCursor(yyCursorType::Arrow, m_cursors[(u32)miCursorType::Arrow]);
			//m_cursors[(u32)miCursorType::Arrow]->Activate();
			SetCursorBehaviorMode(m_cursorBehaviorMode);
		}
	}

	if (m_cursorBehaviorMode == miCursorBehaviorMode::HideCursor)
	{
		if(m_isCursorMove)
			yySetCursorPosition(m_cursorLMBClickPosition.x, m_cursorLMBClickPosition.y, m_window);

		if (m_inputContext->m_isLMBUp || m_inputContext->m_isRMBUp || m_inputContext->IsKeyHit(yyKey::K_ESCAPE))
			this->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	}

	if (m_isCursorMove && m_isViewportInFocus)
	{
		m_cursorPosition3D = m_activeViewportLayout->m_activeViewport->GetCursorRayHitPosition(m_inputContext->m_cursorCoords);
		
		if(m_gizmoMode == miGizmoMode::NoTransform)
		{
			if ((m_cursorBehaviorMode == miCursorBehaviorMode::CommonMode) ||
				(m_cursorBehaviorMode == miCursorBehaviorMode::Other))
			{
				if(m_inputContext->m_isLMBHold)
					m_isSelectByRectangle = true;
			}
			
			if (
				(m_cursorBehaviorMode == miCursorBehaviorMode::ClickAndDrag && m_inputContext->m_isLMBHold)
				|| (m_cursorBehaviorMode == miCursorBehaviorMode::SelectVertex && m_inputContext->m_isLMBHold))
			{
				if (!m_isClickAndDrag)
					m_isClickAndDrag = true;
			}
		}

		if (m_inputContext->m_isMMBHold)
		{
			switch (m_keyboardModifier)
			{
			default:
				m_activeViewportLayout->m_activeViewport->m_activeCamera->PanMove();
				break;
			case miKeyboardModifier::Alt:
				m_activeViewportLayout->m_activeViewport->m_activeCamera->Rotate(m_inputContext->m_mouseDelta.x, m_inputContext->m_mouseDelta.y);
				break;
			case miKeyboardModifier::CtrlAlt:
				m_activeViewportLayout->m_activeViewport->m_activeCamera->ChangeFOV();
				break;
			case miKeyboardModifier::ShiftCtrlAlt:
				m_activeViewportLayout->m_activeViewport->m_activeCamera->RotateZ();
				break;
			}
		}
	}

	if(m_inputContext->IsKeyHit(yyKey::K_NUM_4))
		m_activeViewportLayout->m_activeViewport->m_activeCamera->Rotate(-5.f, 0.f);
	if (m_inputContext->IsKeyHit(yyKey::K_NUM_6))
		m_activeViewportLayout->m_activeViewport->m_activeCamera->Rotate(5.f, 0.f);
	if (m_inputContext->IsKeyHit(yyKey::K_NUM_2))
		m_activeViewportLayout->m_activeViewport->m_activeCamera->Rotate(0.f, -5.f);
	if (m_inputContext->IsKeyHit(yyKey::K_NUM_8))
		m_activeViewportLayout->m_activeViewport->m_activeCamera->Rotate(0.f, 5.f);
	if (m_inputContext->IsKeyHit(yyKey::K_NUM_5))
		m_activeViewportLayout->m_activeViewport->m_activeCamera->m_forceOrtho = 
		m_activeViewportLayout->m_activeViewport->m_activeCamera->m_forceOrtho ? false : true;

	//if(m_isCursorInGUI) printf("InGUI");
	//m_isGizmoInput = false;

	if (m_isCursorInWindow && !m_isCursorInGUI)
	{
		if (m_inputContext->m_isMMBDown || m_inputContext->m_isLMBDown)
		{
			m_isViewportInFocus = true;
		}
	}

	if (m_inputContext->m_isLMBUp || m_inputContext->m_isMMBUp)
	{
		m_isViewportInFocus = false;
	}
}

void miApplication::DrawViewports() {
	for (u16 i = 0, sz = m_activeViewportLayout->m_viewports.size(); i < sz; ++i)
	{
		m_gpu->SetScissorRect(v4f(0.f, 0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y), m_window, 0);
		m_gpu->SetViewport(0.f, 0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y, m_window, 0);
		//yySetEyePosition(m_activeViewportLayout->m_activeViewport->m_activeCamera->m_positionCamera);

		auto viewport = m_activeViewportLayout->m_viewports[i];
		if (viewport == m_activeViewportLayout->m_activeViewport)
		{
			v4f rect = viewport->m_currentRect;
			rect.x -= miViewportBorderSize;
			rect.y -= miViewportBorderSize;
			rect.z += miViewportBorderSize;
			rect.w += miViewportBorderSize;
			m_gpu->UseDepth(false);
			m_gpu->DrawRectangle(rect, m_color_viewportBorder, m_color_viewportBorder);
		}

		switch (m_editMode)
		{
		case miEditMode::Object:
			if(g_app->m_selectedObjects.m_size)
				m_gizmo->Update(viewport);
			break;
		default:
			if(m_isVertexEdgePolygonSelected)
				m_gizmo->Update(viewport);
			break;
		}
		
		if (m_isCursorInViewport && viewport == m_viewportUnderCursor)
		{
			if (m_isGizmoMouseHover)
			{
				if (m_inputContext->m_isLMBDown)
				{
					m_gizmo->OnClick();
				}
			}
		}

		//if (m_gizmo->Update(viewport))
			//m_isGizmoInput = true;
		viewport->OnDraw();
	}

	m_gpu->SetScissorRect(v4f(0.f,0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y), m_window, 0);
	m_gpu->SetViewport(0.f, 0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y, m_window, 0);
}

void miApplication::_callViewportOnSize() {
	for (s32 k = 0; k < miViewportLayout_Count; ++k)
	{
		auto l = m_viewportLayouts[k];
		for (u16 i = 0, sz = l->m_viewports.size(); i < sz; ++i)
		{
			l->m_viewports[i]->OnWindowSize();
		}
	}
}

void miApplication::ShowPopupAtCursor(miPopup* popup) {
	popup->Show((s32)m_inputContext->m_cursorCoords.x, (s32)m_inputContext->m_cursorCoords.y);
}

void miApplication::CommandCameraReset(miViewport* vp) {
	vp->m_activeCamera->Reset();
	_callViewportOnSize();
}

void miApplication::CommandCameraMoveToSelection(miViewport* vp) {
	vp->m_activeCamera->MoveToSelection();
	_callViewportOnSize();
}

void miApplication::CommandViewportChangeView(miViewport* vp, miViewportCameraType ct) {
	vp->SetCameraType(ct);
	vp->m_activeCamera->Reset();
}

void miApplication::CommandViewportToggleFullView(miViewport* vp) {
	if (m_activeViewportLayout == m_viewportLayouts[miViewportLayout_Full])
	{
		m_activeViewportLayout->HideGUI();
		m_activeViewportLayout = m_previousViewportLayout;
		m_activeViewportLayout->m_activeViewport->Copy(m_viewportLayouts[miViewportLayout_Full]->m_activeViewport);
		m_activeViewportLayout->ShowGUI();
	}
	else
	{
		m_previousViewportLayout = m_activeViewportLayout;
		m_activeViewportLayout->HideGUI();
		m_activeViewportLayout = m_viewportLayouts[miViewportLayout_Full];
		m_activeViewportLayout->m_activeViewport->Copy(m_previousViewportLayout->m_activeViewport);
		m_activeViewportLayout->ShowGUI();
	}
	m_activeViewportLayout->m_activeViewport->UpdateAspect();
	m_2d->UpdateClip();
}

void miApplication::CommandViewportToggleGrid(miViewport* vp) {
	vp->SetDrawGrid(vp->m_drawGrid ? false : true);
}

void miApplication::CommandViewportSetDrawMode(miViewport* vp, miViewportDrawMode dm) {
	vp->SetDrawMode(dm);
}

void miApplication::CommandViewportToggleDrawMaterial(miViewport* vp) {
	vp->ToggleDrawModeMaterial();
}
void miApplication::CommandViewportToggleDrawWireframe(miViewport* vp) {
	vp->ToggleDrawModeWireframe();
}
void miApplication::CommandViewportToggleAABB(miViewport* vp) {
	vp->ToggleDrawAABB();
}

void miApplication::CommandTransformModeSet(miTransformMode m) {
	CallPluginGUIOnCancel();
	this->SetTransformMode(m, false);
	m_GUIManager->UpdateTransformModeButtons();
}
void miApplication::DeleteObject(miSceneObject* obj) {
	RemoveObjectFromScene(obj);
	obj->~miSceneObject();
	miFree(obj);
}
miEditableObject* miApplication::ConvertObjectToEditableObject(miSceneObject* obj) {
	miEditableObject* newEditableObject = 0;

	if (obj->GetPlugin() == m_pluginForApp)
	{
		auto p = (miApplicationPlugin*)m_pluginForApp;
		if (obj->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
		{
			newEditableObject = (miEditableObject*)obj;
			return newEditableObject;
		}
	}

	auto flags = obj->GetFlags();
	auto meshCount = obj->GetMeshCount();
	if (flags & miSceneObjectFlag_CanConvertToEditableObject && meshCount)
	{
		newEditableObject = (miEditableObject*)miMalloc(sizeof(miEditableObject));
		new(newEditableObject)miEditableObject(m_sdk, 0);
		newEditableObject->CopyBase(obj);
		newEditableObject->m_typeForPlugin = miApplicationPlugin::m_objectType_editableObject;

		if (newEditableObject->m_flags & miSceneObjectFlag_CanConvertToEditableObject)
			newEditableObject->m_flags ^= miSceneObjectFlag_CanConvertToEditableObject;

		newEditableObject->m_gui = m_pluginGuiForEditableObject;

		// must real parent or m_rootObject
		newEditableObject->SetParent(obj->m_parent);
		obj->SetParent(0);
		for (int o = 0; o < meshCount; ++o)
		{
			auto mesh = obj->GetMesh(o);

			m_sdk->AppendMesh(newEditableObject->m_mesh, mesh);
		}
		newEditableObject->m_visualObject_polygon->CreateNewGPUModels(newEditableObject->m_mesh);
		newEditableObject->m_visualObject_vertex->CreateNewGPUModels(newEditableObject->m_mesh);
		newEditableObject->m_visualObject_edge->CreateNewGPUModels(newEditableObject->m_mesh);
		newEditableObject->UpdateTransform();
		newEditableObject->UpdateAabb();

		if (obj->m_children.m_head)
		{
			auto curr_child = obj->m_children.m_head;
			auto last_child = obj->m_children.m_head->m_left;
			while (true)
			{
				curr_child->m_data->SetParent(newEditableObject);

				if (curr_child == last_child)
					break;
				curr_child = curr_child->m_right;
			}
			obj->m_children.clear();
		}

		DeleteObject(obj);
	}
	return newEditableObject;
}
void miApplication::ConvertSelectedObjectsToEditableObjects() {
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto newEditableObject = ConvertObjectToEditableObject(m_selectedObjects.m_data[i]);
		if (newEditableObject)
			m_selectedObjects.m_data[i] = newEditableObject;
	}
	UpdateSelectedObjectsArray();
}

bool miApplication::NameIsFree(const miString& name, miSceneObject* o) {
	auto & n = o->GetName();
	if (n == name)
		return false;

	auto children = o->GetChildren();
	auto node = children->m_head;
	if (node)
	{
		auto last = node->m_left;
		while (true)
		{
			if (NameIsFree(name, node->m_data))
			{
				return true;
			}
			else
			{
				return false;
			}


			if (node == last)
				break;

			node = node->m_right;
		}
	}
	return true;
}
miString miApplication::GetFreeName(const wchar_t* name) {
	miString newName = name;

	static int i = 0;

	while (true) {
		if (NameIsFree(newName, m_rootObject))
		{
			break;
		}
		else
		{
			newName = name;
			newName += i++;
		}
	}

	return newName;
}

const yyColor g_colors[] = {
	ColorRed,	ColorAliceBlue,	ColorAqua,	ColorAquamarine,	ColorAzure,	
	ColorBeige,	ColorBisque,	ColorBlanchedAlmond,	ColorBlue,	ColorBlueViolet,	
	
	ColorBrown, ColorBurlyWood,	ColorCadetBlue,	ColorChartreuse,	ColorCoral,	
	ColorCornflowerBlue, ColorCornsilk,	ColorCrimson,	ColorCyan,	ColorDarkBlue,	
	
	ColorDarkCyan, ColorDarkGoldenRod,	ColorDarkKhaki,	ColorDarkMagenta,	ColorDarkOliveGreen,	
	ColorDarkOrange, ColorDarkOrchid,	ColorDarkRed,	ColorDarkSalmon,	ColorDarkSeaGreen,	
	
	ColorDarkSlateBlue, ColorDarkTurquoise,	 ColorDarkViolet,	ColorDeepPink,	ColorDeepSkyBlue,	
	ColorDodgerBlue, ColorFireBrick,	ColorForestGreen,	ColorFuchsia, ColorGainsboro,	
	
	//                                                                *
	ColorGold, ColorGoldenRod,	ColorGreen,	ColorGreenYellow,	ColorLightBlue,
	ColorHotPink, ColorIndianRed,	ColorIndigo,	ColorIvory,	ColorKhaki,	
	
	ColorLavender, ColorLavenderBlush,	ColorLawnGreen,	ColorLemonChiffon,	ColorLightBlue,
	ColorLightCoral, ColorLightCyan,	ColorLightGoldenRodYellow,	ColorLightGreen,	ColorLightPink,

	ColorLightSalmon, ColorLightSeaGreen,	ColorLightSkyBlue,	ColorLightSteelBlue,	ColorLightYellow,
	ColorLime, ColorLimeGreen,	ColorLinen,	ColorMagenta,	ColorMaroon,	
	
	ColorMediumAquaMarine, ColorMediumBlue, ColorMediumOrchid,	ColorMediumPurple,	ColorMediumSeaGreen,
	ColorMediumSlateBlue, ColorMediumSpringGreen, ColorMediumTurquoise,	ColorMediumVioletRed,	ColorMidnightBlue,

	ColorMintCream, ColorMistyRose, ColorMoccasin,	ColorNavy,	ColorOldLace,	
	ColorOlive,	ColorOliveDrab,	ColorOrange,	ColorOrangeRed, ColorOrchid,	
	
	ColorPaleGoldenRod, ColorPaleGreen,	ColorPaleTurquoise,	ColorPaleVioletRed,	ColorPapayaWhip,	
	ColorPeachPuff, ColorPeru,	ColorPink,	ColorPlum,	ColorPowderBlue,		
	
	ColorPurple, ColorRebeccaPurple, ColorRosyBrown,		ColorRoyalBlue,		ColorSaddleBrown,		
	ColorSalmon,ColorSandyBrown, ColorSeaGreen,		ColorSeaShell,		ColorSienna,
	
	ColorSkyBlue,ColorSlateBlue,		ColorSpringGreen,		ColorSteelBlue,		ColorTan,
	ColorTeal,ColorThistle,		ColorTomato,		ColorTurquoise,		ColorViolet,	

	ColorYellow,ColorYellowGreen,
};

void miApplication::AddObjectToScene(miSceneObject* o, const wchar_t* name) {
	assert(o);
	assert(name);
	auto newName = this->GetFreeName(name);
	yyLogWriteInfoW(L"Add object to scene: %s\n", newName.data());
	o->SetName(newName.data());
	o->SetParent(m_rootObject);
	
	v4f ec(1.f);

	//std::srand(std::time(0));
	//int result = std::rand() % (sizeof(g_colors) / sizeof(g_colors[0]));
	static std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, sizeof(g_colors) / sizeof(g_colors[0]));
	s32 result = distribution(generator);
	yyLogWriteInfo("Color index: %i\n", result);

	ec.x = g_colors[result].m_data[0];
	ec.y = g_colors[result].m_data[1];
	ec.z = g_colors[result].m_data[2];

	o->SetEdgeColor(ec);
	_updateObjectsOnSceneArray();
}

void miApplication::UpdateSceneAabb() {
	m_sceneAabb.reset();
	if (m_rootObject)
	{
		for (u32 i = 0; i < m_objectsOnScene.m_size; ++i)
		{
			auto o = m_objectsOnScene.m_data[i];
			m_sceneAabb.add(*o->GetAABBTransformed());
		}
	}
}

void miApplication::OnImport_openDialog() {
	yyStringA title;
	yyStringA extensions;

	title = m_onImport_importer->m_title.data();
	for (auto & i : m_onImport_importer->m_extensions)
	{
		extensions += i.data();
		extensions += " ";
	}

	auto path = yyOpenFileDialog("Import model", "Import", extensions.data(), title.data());
	if (path)
	{
		m_onImport_importer->m_plugin->OnImport((const wchar_t*)path->data(), m_onImport_importer->m_importerID);
		yyDestroy(path);
	}
}
void miApplication::OnImport(miImporter* importer) {
	m_onImport_importer = importer;
	if (importer->m_gui)
	{
		m_GUIManager->ShowImportMenu(importer->m_gui);
	}
	else
	{
		OnImport_openDialog();
	}
}

void miApplication::_rebuildEdgeModels() {
	for (u32 i = 0; i < m_objectsOnScene.m_size; ++i)
	{
		auto obj = m_objectsOnScene.m_data[i];
		auto voc = obj->GetVisualObjectCount();
		for (s32 o = 0; o < voc; ++o)
		{
			auto vo = obj->GetVisualObject(o);
			if (vo->GetType() == miVisualObjectType::Edge)
				vo->CreateNewGPUModels(0);
		}
	}
}
void miApplication::_rebuildPolygonModels() {
	for (u32 i = 0; i < m_objectsOnScene.m_size; ++i)
	{
		auto obj = m_objectsOnScene.m_data[i];
		auto voc = obj->GetVisualObjectCount();
		for (s32 o = 0; o < voc; ++o)
		{
			auto vo = obj->GetVisualObject(o);
			if (vo->GetType() == miVisualObjectType::Polygon)
				vo->CreateNewGPUModels(0);
		}
	}
}

void miApplication::ToggleEditMode(miEditMode m) {
	CallPluginGUIOnCancel();
	miEditMode mode = miEditMode::Object;
	if (m_editMode != m)
	{
		mode = m;
	}
	SetEditMode(mode);
}

miEditMode miApplication::GetEditMode() {
	return m_editMode;
}

void miApplication::SetEditMode(miEditMode m) {
	m_editMode = m;
	_rebuildEdgeModels();
	_rebuildPolygonModels();

	// update arrays with visual object nodes for update
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto obj = m_selectedObjects.m_data[i];
		obj->OnSetEditMode(m_editMode);
	}

	if (m_currentPluginGUI && m_objectParametersMode == miObjectParametersMode::ObjectParameters)
	{
		//if (m_selectedObjects.m_size == 1)
			m_currentPluginGUI->Show(true);
	}

	switch (m)
	{
	case miEditMode::Vertex:
		m_GUIManager->m_button_editModeVertex->m_isChecked = true;
		m_GUIManager->m_button_editModeEdge->m_isChecked = false;
		m_GUIManager->m_button_editModePolygon->m_isChecked = false;
		break;
	case miEditMode::Edge:
		m_GUIManager->m_button_editModeVertex->m_isChecked = false;
		m_GUIManager->m_button_editModeEdge->m_isChecked = true;
		m_GUIManager->m_button_editModePolygon->m_isChecked = false;
		break;
	case miEditMode::Polygon:
		m_GUIManager->m_button_editModeVertex->m_isChecked = false;
		m_GUIManager->m_button_editModeEdge->m_isChecked = false;
		m_GUIManager->m_button_editModePolygon->m_isChecked = true;
		break;
	case miEditMode::Object:
		m_GUIManager->m_button_editModeVertex->m_isChecked = false;
		m_GUIManager->m_button_editModeEdge->m_isChecked = false;
		m_GUIManager->m_button_editModePolygon->m_isChecked = false;
		break;
	default:
		break;
	}
	_updateIsVertexEdgePolygonSelected();
	this->UpdateSelectionAabb();
}
void miApplication::SetTransformMode(miTransformMode m, bool local) {
	m_isLocalTransform = local;
	if (m_gizmoMode == miGizmoMode::NoTransform)
		m_transformMode = m;
}

void miApplication::DrawAabb(const Aabb& aabb, const v4f& _color, const v3f& positionOffset) {
	auto & p1 = aabb.m_min;
	auto & p2 = aabb.m_max;

	yyColor color;
	color.m_data[0] = _color.x;
	color.m_data[1] = _color.y;
	color.m_data[2] = _color.z;
	color.m_data[3] = 1.f;

	v4f v1 = p1;
	v4f v2 = p2;

	v4f v3(p1.x, p1.y, p2.z, 1.f);
	v4f v4(p2.x, p1.y, p1.z, 1.f);
	v4f v5(p1.x, p2.y, p1.z, 1.f);
	v4f v6(p1.x, p2.y, p2.z, 1.f);
	v4f v7(p2.x, p1.y, p2.z, 1.f);
	v4f v8(p2.x, p2.y, p1.z, 1.f);

	m_gpu->DrawLine3D(v1 + positionOffset, v4 + positionOffset, color);
	m_gpu->DrawLine3D(v5 + positionOffset, v8 + positionOffset, color);
	m_gpu->DrawLine3D(v1 + positionOffset, v5 + positionOffset, color);
	m_gpu->DrawLine3D(v4 + positionOffset, v8 + positionOffset, color);
	m_gpu->DrawLine3D(v3 + positionOffset, v7 + positionOffset, color);
	m_gpu->DrawLine3D(v6 + positionOffset, v2 + positionOffset, color);
	m_gpu->DrawLine3D(v3 + positionOffset, v6 + positionOffset, color);
	m_gpu->DrawLine3D(v7 + positionOffset, v2 + positionOffset, color);
	m_gpu->DrawLine3D(v2 + positionOffset, v8 + positionOffset, color);
	m_gpu->DrawLine3D(v4 + positionOffset, v7 + positionOffset, color);
	m_gpu->DrawLine3D(v5 + positionOffset, v6 + positionOffset, color);
	m_gpu->DrawLine3D(v1 + positionOffset, v3 + positionOffset, color);
}

void miApplication::_setGizmoMode(miGizmoMode gm) {
	m_gizmoMode = gm;
}

void miApplication::SetObjectParametersMode(miObjectParametersMode opm) {
	m_objectParametersMode = opm;

	auto showPluginGui = [=](bool show) {
		if (m_currentPluginGUI)
		{
			if (m_selectedObjects.m_size == 1)
				m_currentPluginGUI->Show(show);
		}
	};

	switch (m_objectParametersMode)
	{
	case miObjectParametersMode::CommonParameters:
		m_GUIManager->m_button_objectCommonParams->m_isChecked = true;
		m_GUIManager->m_button_objectObjectParams->m_isChecked = false;
		m_GUIManager->m_button_materials->m_isChecked = false;

		m_GUIManager->m_gui_drawGroup_commonParams->SetDraw(true);
		m_GUIManager->m_gui_drawGroup_commonParams->SetInput(true);
		showPluginGui(false);

		m_GUIManager->m_gui_drawGroup_materials->SetDraw(false);
		m_GUIManager->m_gui_drawGroup_materials->SetInput(false);
		break;
	case miObjectParametersMode::ObjectParameters:
		m_GUIManager->m_button_objectCommonParams->m_isChecked = false;
		m_GUIManager->m_button_objectObjectParams->m_isChecked = true;
		m_GUIManager->m_button_materials->m_isChecked = false;

		m_GUIManager->m_gui_drawGroup_commonParams->SetDraw(false);
		m_GUIManager->m_gui_drawGroup_commonParams->SetInput(false);
		showPluginGui(true);

		m_GUIManager->m_gui_drawGroup_materials->SetDraw(false);
		m_GUIManager->m_gui_drawGroup_materials->SetInput(false);
		break;
	case miObjectParametersMode::Materials:
		m_GUIManager->m_button_materials->m_isChecked = true;
		m_GUIManager->m_button_objectCommonParams->m_isChecked = false;
		m_GUIManager->m_button_objectObjectParams->m_isChecked = false;

		m_GUIManager->m_gui_drawGroup_materials->SetDraw(true);
		m_GUIManager->m_gui_drawGroup_materials->SetInput(true);
		showPluginGui(false);

		m_GUIManager->m_gui_drawGroup_commonParams->SetDraw(false);
		m_GUIManager->m_gui_drawGroup_commonParams->SetInput(false);
		break;
	default:
		YY_PRINT_FAILED;
		break;
	}
}

void miApplication::DeleteSelected() {
	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
	default:
		for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
		{
			m_selectedObjects.m_data[i]->DeleteSelectedObjects(m_editMode);
		}
		break;
	case miEditMode::Object:
		for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
		{
			DeleteObject(m_selectedObjects.m_data[i]);
		}
		m_selectedObjects.clear();
		this->_updateObjectsOnSceneArray();
		DeselectAll();
		break;
	}
	this->_updateObjectsOnSceneArray();
	this->_transformObjectsApply();
	this->UpdateSceneAabb();
	this->UpdateSelectionAabb();
	this->_updateIsVertexEdgePolygonSelected();
	m_GUIManager->SetCommonParamsRangePosition();
}

miPopup* miApplication::_getPopupInViewport() {
	bool is_CanConvertToEditableObject = false;
	miPopup* p = new miPopup;
	if (m_selectedObjects.m_size)
	{
		if(m_cursorBehaviorMode == miCursorBehaviorMode::CommonMode)
			p->AddItem(L"Delete", miCommandID_DeleteSelectedObjects, 0);

		for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
		{
			auto obj = m_selectedObjects.m_data[i];
			auto flags = obj->GetFlags();

			if (m_editMode == miEditMode::Object)
			{
				if (flags & miSceneObjectFlag_CanConvertToEditableObject
					&& !is_CanConvertToEditableObject)
				{
					is_CanConvertToEditableObject = true;
					p->AddItem(L"Convert to editable object", miCommandID_ConvertToEditableObject, 0);
				}
			}
		}
	}
	return p;
}