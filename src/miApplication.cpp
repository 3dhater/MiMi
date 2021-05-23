#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "miShortcutManager.h"
#include "miSDK.h"
#include "miSDKImpl.h"
#include "miGraphics2D.h"
#include "miSelectionFrustImpl.h"
#include "miRootObject.h"
#include "yy_color.h"
#include "yy_gui.h"
#include "yy_model.h"

#include <map>
#include <ctime>

miApplication * g_app = 0;
Mat4 g_emptyMatrix;


namespace mimath
{
	miVec2 v2f_to_miVec2(const v2f& v) { return miVec2(v.x, v.y); }
	miVec3 v3f_to_miVec3(const v3f& v) { return miVec3(v.x, v.y, v.z); }
	miVec4 v4f_to_miVec4(const v4f& v) { return miVec4(v.x, v.y, v.z, v.w); }
	miMatrix Mat4_to_miMatrix(const Mat4& m) { 
		miMatrix mi_m;
		auto mi_m_ptr = mi_m.getPtr();
		auto m_ptr = m.getPtrConst();
		mi_m_ptr[0] = m_ptr[0];
		mi_m_ptr[1] = m_ptr[1];
		mi_m_ptr[2] = m_ptr[2];
		mi_m_ptr[3] = m_ptr[3];
		mi_m_ptr[4] = m_ptr[4];
		mi_m_ptr[5] = m_ptr[5];
		mi_m_ptr[6] = m_ptr[6];
		mi_m_ptr[7] = m_ptr[7];
		mi_m_ptr[8] = m_ptr[8];
		mi_m_ptr[9] = m_ptr[9];
		mi_m_ptr[10] = m_ptr[10];
		mi_m_ptr[11] = m_ptr[11];
		mi_m_ptr[12] = m_ptr[12];
		mi_m_ptr[13] = m_ptr[13];
		mi_m_ptr[14] = m_ptr[14];
		mi_m_ptr[15] = m_ptr[15];
		return mi_m; 
	}
	v2f miVec2_to_v2f(const miVec2& v) { return v2f(v.x, v.y); }
	v3f miVec3_to_v3f(const miVec3& v) { return v3f(v.x, v.y, v.z); }
	v4f miVec4_to_v4f(const miVec4& v) { return v4f(v.x, v.y, v.z, v.w); }
	Mat4 miMatrix_to_Mat4(const miMatrix& m1) {
		Mat4 m2;
		auto p1 = m1.getPtrConst();
		auto p2 = m2.getPtrConst();
		p2[0] = p1[0];
		p2[1] = p1[1];
		p2[2] = p1[2];
		p2[3] = p1[3];
		p2[4] = p1[4];
		p2[5] = p1[5];
		p2[6] = p1[6];
		p2[7] = p1[7];
		p2[8] = p1[8];
		p2[9] = p1[9];
		p2[10] = p1[10];
		p2[11] = p1[11];
		p2[12] = p1[12];
		p2[13] = p1[13];
		p2[14] = p1[14];
		p2[15] = p1[15];
		return m2;
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
	
	case miCommandID_ViewportDrawMaterial: g_app->CommandViewportSetDrawMode(g_app->m_popupViewport, miViewport::DrawMode::Draw_Material); break;
	case miCommandID_ViewportDrawMaterialWireframe: g_app->CommandViewportSetDrawMode(g_app->m_popupViewport, miViewport::DrawMode::Draw_MaterialWireframe); break;
	case miCommandID_ViewportDrawWireframe: g_app->CommandViewportSetDrawMode(g_app->m_popupViewport, miViewport::DrawMode::Draw_Wireframe); break;
	}
	if (commandID >= miCommandID_for_plugins)
	{
		miPluginCommandIDMapNode mapNode;
		if (g_app->m_pluginCommandID.Get(commandID - miCommandID_for_plugins, mapNode)) {
			mapNode.m_plugin->OnPopupCommand(mapNode.m_commandID);
		}
	}
}

void window_onActivate(yyWindow* window) {
	switch (g_app->m_keyboardModifier) {
	case miKeyboardModifier::None:
	case miKeyboardModifier::Ctrl:
	case miKeyboardModifier::Shift:
	case miKeyboardModifier::ShiftCtrl:
	case miKeyboardModifier::END:
	default:
		break;
	case miKeyboardModifier::Alt:
	case miKeyboardModifier::ShiftAlt:
	case miKeyboardModifier::ShiftCtrlAlt:
	case miKeyboardModifier::CtrlAlt:
	{
		g_app->m_keyboardModifier = miKeyboardModifier::None;
	}
	break;
	}
	g_app->m_isViewportInFocus = false;
}

int main(int argc, char* argv[]) {
	
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
	m_isGUIInputFocus = false;
	m_currentViewportDraw = 0;
	m_editMode = miEditMode::Object;
	m_pluginActive = 0;
	m_cursorBehaviorMode = miCursorBehaviorMode::CommonMode;
	m_isSelectByRectangle = false;
	m_isClickAndDrag = false;
	m_selectionFrust = new miSelectionFrustImpl;
	
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

	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		m_cursors[i] = 0;
	}
}

miApplication::~miApplication() {
	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		if (m_cursors[i])delete m_cursors[i];
	}

	if (m_rootObject)
	{
		DestroyAllSceneObjects(m_rootObject);
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

void miApplication::RemoveObjectFromScene(miSceneObject* o) {
	assert(o);
	o->SetParent(0);
	auto node = o->GetChildren()->m_head;
	if (node)
	{
		auto last = node->m_left;
		while (true) {
			node->m_data->SetParent(m_rootObject);

			if (node == last)
				break;

			node = node->m_right;
		}
	}
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

		if (newPlugin->Init(m_sdk) != MI_SDK_VERSION)
		{
			newPlugin->~miPlugin();
			miFree(newPlugin);
			yyLogWriteInfo("FAIL (bad version)\n");
			continue;
		}

		yyLogWriteInfoW(L"DONE (%s)\n", newPlugin->GetName());

		m_plugins.push_back(newPlugin);
	}
	m_plugins.shrink_to_fit();
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

	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		m_cursors[i] = new yyCursor((yyCursorType)i);

		switch ((yyCursorType)i)
		{
		default:
		case yyCursorType::Arrow: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/arrow.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Cross: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/prec.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Hand: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/link.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Help: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/helpsel.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::IBeam: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/select.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::No: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/unavail.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Size: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/move.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::SizeNESW: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/nesw.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::SizeNS: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/ns.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::SizeNWSE: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/nwse.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::SizeWE: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/ew.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::UpArrow: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/up.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Wait: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/working.ani", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		}
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
					{
						goto vidOk;
					}
					else
					{
						yyLogWriteWarning("Can't load video driver : %s\n", path.generic_string().c_str());
					}
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

	m_color_viewportBorder = ColorYellow;

	m_color_windowClearColor.setAsByteRed(118);
	m_color_windowClearColor.setAsByteGreen(118);
	m_color_windowClearColor.setAsByteBlue(118);
	m_gpu->SetClearColor(m_color_windowClearColor.m_data[0], m_color_windowClearColor.m_data[1], m_color_windowClearColor.m_data[2], 1.f);
	m_window->SetTitle(m_gpu->GetVideoDriverName());
	
	_initPlugins();
	_initPopups();
	_initGrid();

	m_GUIManager = new miGUIManager;

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
				mimath::v2f_to_miVec2(m_inputContext->m_cursorCoords),
				mimath::v4f_to_miVec4(m_viewportUnderCursor->m_currentRect),
				mimath::Mat4_to_miMatrix(m_viewportUnderCursor->m_activeCamera->m_viewProjectionInvertMatrix)
			);
			_get_objects_under_cursor();
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

			_update_transforms(m_rootObject);

			DrawViewports();

			yyGUIDrawAll();

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
	}

	if (m_pluginActive)
		m_pluginActive->OnCancel(m_selectionFrust, m_isCursorInGUI);
}

void miApplication::_update_transforms(miSceneObject* o) {
	o->UpdateTransform();

	auto node = o->GetChildren()->m_head;
	if (node)
	{
		auto last = node->m_left;
		while (true) {
			_update_transforms(node->m_data);

			if (node == last)
				break;

			node = node->m_right;
		}
	}
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
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_dmMaterial)) this->CommandViewportSetDrawMode(m_activeViewportLayout->m_activeViewport, miViewport::DrawMode::Draw_Material);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_dmMaterialWireframe)) this->CommandViewportSetDrawMode(m_activeViewportLayout->m_activeViewport, miViewport::DrawMode::Draw_MaterialWireframe);
	if (m_shortcutManager->IsShortcutActive(miShortcutCommandType::viewport_dmWireframe)) this->CommandViewportSetDrawMode(m_activeViewportLayout->m_activeViewport, miViewport::DrawMode::Draw_Wireframe);
}

void miApplication::_get_objects_under_cursor_(miSceneObject* o) {
	miVec4 ip;
	f32 d = 0.f;
	if (o->IsRayIntersect(&m_rayCursor, &ip, &d))
	{
		o->m_cursorIntersectionPointDistance = d;
		o->m_cursorIntersectionPoint = ip;
		m_objectsUnderCursor.push_back(o);

		//wprintf(L"%s", o->GetName().data());
	}

	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_get_objects_under_cursor_(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}

void miApplication::_get_objects_under_cursor() {
	m_objectsUnderCursor.clear();
	_get_objects_under_cursor_(m_rootObject);

	struct _pred{
		bool operator() (miSceneObject* a, miSceneObject* b) const{
			return a->GetDistanceToCursorIP() > b->GetDistanceToCursorIP();
		}
	};
	m_objectsUnderCursor.sort_insertion(_pred());
}

void miApplication::_deselect_all(miSceneObject* o) {
	YY_DEBUG_PRINT_FUNC;
	o->DeselectAll(m_editMode);
	auto node = o->GetChildren()->m_head;
	if (node){
		auto last = node->m_left;
		while (true) {
			_deselect_all(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}

void miApplication::_select_all(miSceneObject* o) {
	YY_DEBUG_PRINT_FUNC;
	o->SelectAll(m_editMode);
	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_select_all(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}

void miApplication::DeselectAll() {
	YY_DEBUG_PRINT_FUNC;
	_deselect_all(m_rootObject);
	update_selected_objects_array();
}

void miApplication::SelectAll() {
	_select_all(m_rootObject);
	update_selected_objects_array();
}

void miApplication::_select_multiple() {
	if (m_keyboardModifier != miKeyboardModifier::Alt && m_keyboardModifier != miKeyboardModifier::Ctrl)
		DeselectAll();

	for (u32 i = 0; i < m_activeViewportLayout->m_activeViewport->m_visibleObjects.m_size; ++i)
	{
		m_activeViewportLayout->m_activeViewport->m_visibleObjects.m_data[i]->Select(m_editMode, m_keyboardModifier, m_selectionFrust);
	}
	update_selected_objects_array();
}

void miApplication::_select_single_call(miSceneObject* o) {
	if(o->IsSelected())
		o->SelectSingle(m_editMode, m_keyboardModifier, m_selectionFrust);
	
	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_select_single_call(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}

void miApplication::_select_single() {
	if (m_keyboardModifier != miKeyboardModifier::Alt && m_keyboardModifier != miKeyboardModifier::Ctrl)
		DeselectAll();

	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		_select_single_call(m_rootObject);
		break;
	case miEditMode::Object:
	default:
		if (m_objectsUnderCursor.m_size)
		{
			m_objectsUnderCursor.m_data[0]->SelectSingle(m_editMode, m_keyboardModifier, m_selectionFrust);
			update_selected_objects_array();
		}
		break;
	}
}

void miApplication::_update_selected_objects_array(miSceneObject* o) {
	if (o->IsSelected())
		m_selectedObjects.push_back(o);

	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_update_selected_objects_array(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}
void miApplication::update_selected_objects_array() {
	m_selectedObjects.clear();
	_update_selected_objects_array(m_rootObject);
	printf("selected objects: %u\n", m_selectedObjects.m_size);
	m_GUIManager->m_textInput_rename->DeleteAll();
	if (m_selectedObjects.m_size == 1)
	{
		m_GUIManager->m_textInput_rename->SetText(L"%s", m_selectedObjects.m_data[0]->m_name.data());
	}
	else if (m_selectedObjects.m_size > 1)
	{
		m_GUIManager->m_textInput_rename->SetText(L"Many objects");
	}
}

void miApplication::UpdateViewports() {
	if (m_isViewportInFocus)
	{
		if (m_inputContext->m_isLMBUp)
		{
			if (m_isSelectByRectangle)
			{
				Aabb aabb;
				aabb.add(v3f(m_inputContext->m_cursorCoords.x, m_inputContext->m_cursorCoords.y, 0.f));
				aabb.add(v3f(m_cursorLMBClickPosition.x, m_cursorLMBClickPosition.y, 0.f));

				m_selectionFrust->CreateWithFrame(
					miVec4(aabb.m_min.x, aabb.m_min.y, aabb.m_max.x, aabb.m_max.y),
					mimath::v4f_to_miVec4(m_activeViewportLayout->m_activeViewport->m_currentRect),
					mimath::Mat4_to_miMatrix(m_activeViewportLayout->m_activeViewport->m_activeCamera->m_viewProjectionInvertMatrix));

				_select_multiple();
			}
			else
			{
				const f32 _size = 2.f;
				m_selectionFrust->CreateWithFrame(
					miVec4(
						m_inputContext->m_cursorCoords.x - _size, 
						m_inputContext->m_cursorCoords.y - _size, 
						m_inputContext->m_cursorCoords.x + _size,
						m_inputContext->m_cursorCoords.y + _size),
					mimath::v4f_to_miVec4(m_activeViewportLayout->m_activeViewport->m_currentRect),
					mimath::Mat4_to_miMatrix(m_activeViewportLayout->m_activeViewport->m_activeCamera->m_viewProjectionInvertMatrix));

				_select_single();
			}
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

	if (!m_isCursorInGUI)
	{
		if (m_inputContext->m_isLMBDown)
		{
			m_cursorLMBClickPosition = m_inputContext->m_cursorCoords;
			miRay r;
			m_sdk->GetRayFromScreen(&r, mimath::v2f_to_miVec2(m_inputContext->m_cursorCoords),
				mimath::v4f_to_miVec4(m_activeViewportLayout->m_activeViewport->m_currentRect),
				mimath::Mat4_to_miMatrix(m_activeViewportLayout->m_activeViewport->m_activeCamera->m_viewProjectionInvertMatrix));
			
			g_rays.push_back(r);
		}

		for (u16 i = 0, sz = m_activeViewportLayout->m_viewports.size(); i < sz; ++i)
		{
			auto viewport = m_activeViewportLayout->m_viewports[i];

			viewport->m_isCursorInRect =
				math::pointInRect(m_inputContext->m_cursorCoords.x, m_inputContext->m_cursorCoords.y,
					viewport->m_currentRect);

			if (viewport->m_isCursorInRect)
			{
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

	if (m_inputContext->m_isLMBDown)
	{
		m_cursorLMBClickPosition3D = m_activeViewportLayout->m_activeViewport->GetCursorRayHitPosition(m_inputContext->m_cursorCoords);
		//printf("%f %f %f\n", m_cursorLMBClickPosition3D.x, m_cursorLMBClickPosition3D.y, m_cursorLMBClickPosition3D.z);
	}

	static bool is_pan_move = false;
	if (m_inputContext->m_isMMBHold && m_isViewportInFocus)
	{
		switch (m_keyboardModifier)
		{
		default:
			is_pan_move = true;
			yySetCursor(yyCursorType::Arrow, m_cursors[(u32)yyCursorType::Size]);
			m_cursors[(u32)yyCursorType::Size]->Activate();
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
			yySetCursor(yyCursorType::Arrow, m_cursors[(u32)yyCursorType::Arrow]);
			m_cursors[(u32)yyCursorType::Arrow]->Activate();
		}
	}
	if (m_isCursorMove && m_isViewportInFocus)
	{
		m_cursorPosition3D = m_activeViewportLayout->m_activeViewport->GetCursorRayHitPosition(m_inputContext->m_cursorCoords);

		if( m_cursorBehaviorMode == miCursorBehaviorMode::CommonMode && m_inputContext->m_isLMBHold)
			m_isSelectByRectangle = true;

		if (m_cursorBehaviorMode == miCursorBehaviorMode::ClickAndDrag && m_inputContext->m_isLMBHold)
		{
			if (!m_isClickAndDrag)
			{
				//printf("%f %f %f\n", m_cursorLMBClickPosition3D.x, m_cursorLMBClickPosition3D.y, m_cursorLMBClickPosition3D.z);
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
				m_activeViewportLayout->m_activeViewport->m_activeCamera->Rotate();
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

	//if(m_isCursorInGUI) printf("InGUI");

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
		m_gpu->SetScissorRect(v4f(0.f, 0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y), m_window);
		m_gpu->SetViewport(0.f, 0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y, m_window);
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
		viewport->OnDraw();
	}

	m_gpu->SetScissorRect(v4f(0.f,0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y), m_window);
	m_gpu->SetViewport(0.f, 0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y, m_window);
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
	m_2d->UpdateClip();
}

void miApplication::CommandViewportToggleGrid(miViewport* vp) {
	vp->SetDrawGrid(vp->m_drawGrid ? false : true);
}

void miApplication::CommandViewportSetDrawMode(miViewport* vp, miViewport::DrawMode dm) {
	vp->SetDrawMode(dm);
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
	ColorRed,	ColorAliceBlue,	ColorAqua,	ColorAquamarine,	ColorAzure,	ColorBeige,
	ColorBisque,	ColorBlanchedAlmond,	ColorBlue,	ColorBlueViolet,	ColorBrown,	ColorBurlyWood,
	ColorCadetBlue,	ColorChartreuse,	ColorCoral,	ColorCornflowerBlue,	ColorCornsilk,	ColorCrimson,
	ColorCyan,	ColorDarkBlue,	ColorDarkCyan,	ColorDarkGoldenRod,	ColorDarkKhaki,	ColorDarkMagenta,
	ColorDarkOliveGreen,	ColorDarkOrange,	ColorDarkOrchid,	ColorDarkRed,	ColorDarkSalmon,
	ColorDarkSeaGreen,	ColorDarkSlateBlue,	ColorDarkTurquoise,	ColorDarkViolet,	ColorDeepPink,
	ColorDeepSkyBlue,	ColorDodgerBlue,	ColorFireBrick,	ColorForestGreen,	ColorFuchsia,
	ColorGainsboro,	ColorGold,	ColorGoldenRod,	ColorGreen,	ColorGreenYellow,	ColorHoneyDew,
	ColorHotPink,	ColorIndianRed,	ColorIndigo,	ColorIvory,	ColorKhaki,	ColorLavender,
	ColorLavenderBlush,	ColorLawnGreen,	ColorLemonChiffon,	ColorLightBlue,	ColorLightCoral,
	ColorLightCyan,	ColorLightGoldenRodYellow,	ColorLightGreen,	ColorLightPink,	ColorLightSalmon,
	ColorLightSeaGreen,	ColorLightSkyBlue,	ColorLightSteelBlue,	ColorLightYellow,	ColorLime,
	ColorLimeGreen,	ColorLinen,	ColorMagenta,	ColorMaroon,	ColorMediumAquaMarine,	ColorMediumBlue,
	ColorMediumOrchid,	ColorMediumPurple,	ColorMediumSeaGreen,	ColorMediumSlateBlue,	ColorMediumSpringGreen,
	ColorMediumTurquoise,	ColorMediumVioletRed,	ColorMidnightBlue,	ColorMintCream,	ColorMistyRose,
	ColorMoccasin,	ColorNavy,	ColorOldLace,	ColorOlive,	ColorOliveDrab,	ColorOrange,	ColorOrangeRed,
	ColorOrchid,	ColorPaleGoldenRod,	ColorPaleGreen,	ColorPaleTurquoise,	ColorPaleVioletRed,	ColorPapayaWhip,
	ColorPeachPuff,	ColorPeru,	ColorPink,	ColorPlum,	ColorPowderBlue,		ColorPurple,		ColorRebeccaPurple,
		ColorRed,		ColorRosyBrown,		ColorRoyalBlue,		ColorSaddleBrown,		ColorSalmon,		ColorSandyBrown,
		ColorSeaGreen,		ColorSeaShell,		ColorSienna,		ColorSkyBlue,		ColorSlateBlue,		ColorSpringGreen,
		ColorSteelBlue,		ColorTan,		ColorTeal,		ColorThistle,		ColorTomato,		ColorTurquoise,
		ColorViolet,		ColorYellow,		ColorYellowGreen,
};

void miApplication::AddObjectToScene(miSceneObject* o, const wchar_t* name) {
	assert(o);
	assert(name);
	auto newName = this->GetFreeName(name);
	yyLogWriteInfoW(L"Add object to scene: %s\n", newName.data());
	o->SetName(newName.data());
	o->SetParent(m_rootObject);
	
	miVec4 ec(1.f);

	std::srand(std::time(0));
	int result = std::rand() % (sizeof(g_colors) / sizeof(g_colors[0]));

	yyLogWriteInfo("Color index: %i\n", result);

	ec.x = g_colors[result].m_data[0];
	ec.y = g_colors[result].m_data[1];
	ec.z = g_colors[result].m_data[2];

	o->SetEdgeColor(ec);
}

void miApplication::_buildSceneAabb(miSceneObject* o) {
	m_sceneAabb.add(*o->GetAABBTransformed());
	auto node = o->GetChildren()->m_head;
	if (node)
	{
		auto last = node->m_left;
		while (true) {
			_buildSceneAabb(node->m_data);

			if (node == last)
				break;

			node = node->m_right;
		}
	}
}
void miApplication::UpdateSceneAabb() {
	m_sceneAabb.reset();
	if(m_rootObject)
		_buildSceneAabb(m_rootObject);
}