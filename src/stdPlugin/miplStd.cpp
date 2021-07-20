#define MI_EXPORTS
#include "miSDK.h"

#include "miplStd.h"
#include "miplPolygonObjectPlane.h"

static bool g_isCreated = false;

void miplStd_initGuiForPlane(miPluginGUI*);
void miplStd_initGuiForImportOBJ(miPluginGUI*);

miSDK* g_sdk = 0;

extern "C" {
	MI_API miPlugin* MI_C_DECL miplCreatePlugin() {
		if (g_isCreated)
			return 0;
		g_isCreated = true;
		//return new miplStd;
		miplStd* p = (miplStd*)miMalloc(sizeof(miplStd));
		new(p)miplStd();
		return p;
	}
}

miplStd::miplStd() {
	m_newObjectPtr = 0;
	m_gui_for_plane = 0;
	m_gui_for_importOBJ = 0;
}

miplStd::~miplStd() {
//	printf("destoy plugin\n");
	if (m_newObjectPtr) _destroyNewObject();
}
void miplStd::_destroyNewObject() {
	if (m_newObjectPtr)
	{
		m_newObjectPtr->~miSceneObject();
		miFree(m_newObjectPtr);
		m_newObjectPtr = 0;
	}
}
void miplStd::_dropNewObject() {
	m_newObjectPtr = 0;
}

const wchar_t* miplStd::GetName() {
	return L"Standart plugin";
}

const wchar_t* miplStd::GetDescription() {
	return L"Standart plugin";
}

const wchar_t* miplStd::GetAuthor() {
	return L"mimi developer";
}

bool miplStd::IsDebug() {
#ifdef MI_DEBUG
	return true;
#else
	return false;
#endif
}

const unsigned int g_ObjectID_Plane = 0;

const unsigned int g_ImporterID_OBJ = 0;

int miplStd::CheckVersion() {
	return MI_SDK_VERSION;
}

void miplStd::Init(miSDK* sdk){
	m_sdk = sdk;
	g_sdk = sdk;
	//miSingleton<miSDK>::s_instance = sdk;

	m_gui_for_plane = sdk->CreatePluginGUI(miPluginGUIType::ObjectParams);
	miplStd_initGuiForPlane(m_gui_for_plane);

	sdk->RegisterNewObject(this, g_ObjectID_Plane, L"Polygonal objects", L"Plane");

	/*sdk->RegisterNewObject(this, L"Polygonal objects", L"Box");
	sdk->RegisterNewObject(this, L"Polygonal objects", L"Sphere");
	sdk->RegisterNewObject(this, L"Light", L"Point");
	sdk->RegisterNewObject(this, L"Light", L"Spot");
	sdk->RegisterNewObject(this, L"Light", L"Directional");
	sdk->RegisterNewObject(this, L"Camera", L"Free camera");*/
	
	m_gui_for_importOBJ = sdk->CreatePluginGUI(miPluginGUIType::ImportExportParams);
	miplStd_initGuiForImportOBJ(m_gui_for_importOBJ);
	sdk->RegisterImporter(this, g_ImporterID_OBJ, L"OBJ", L"obj", m_gui_for_importOBJ);
}

void miplStd::OnLMBDown(miSelectionFrust*, bool isCursorInGUI) {
	if (!isCursorInGUI)
	{
		m_isLMBDown = true;

		if (m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::ClickAndDrag) {
			if (m_newObjectPtr) m_newObjectPtr->OnCreationLMBDown();
		}
	}
}

void miplStd::OnLMBUp(miSelectionFrust* sf, bool isCursorInGUI) {
	if (m_isLMBDown)
	{
		if (m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::ClickAndDrag) {
			if (m_isLMBDown)
			{
				if (m_newObjectPtr) m_newObjectPtr->OnCreationLMBUp();

				OnCancel(sf, isCursorInGUI);
			}
		}

		m_isLMBDown = false;
	}
}

void miplStd::OnCancel(miSelectionFrust*, bool isCursorInGUI) {
	if (m_newObjectPtr)
	{
		m_sdk->RemoveObjectFromScene(m_newObjectPtr);
		_destroyNewObject();
	}
	m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	m_sdk->SetActivePlugin(nullptr);
}

void miplStd::OnCursorMove(miSelectionFrust*, bool isCursorInGUI) {
	if (m_newObjectPtr) m_newObjectPtr->OnCreationMouseMove();
}
void miplStd::OnUpdate(miSelectionFrust* sf, bool isCursorInGUI) {
}

void miplStd::OnCreateObject(unsigned int objectId){

	if (objectId == g_ObjectID_Plane)
	{
		m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::ClickAndDrag);
		m_sdk->SetActivePlugin(this);

		if (m_newObjectPtr) miDestroy(m_newObjectPtr);

		miplPolygonObjectPlane* newObject = (miplPolygonObjectPlane*)miMalloc(sizeof(miplPolygonObjectPlane));
		new(newObject)miplPolygonObjectPlane(m_sdk, this);
		newObject->OnCreation(m_gui_for_plane);
		

		m_newObjectPtr = newObject;
	}
}

void miplStd::OnImport(const wchar_t* fileName, unsigned int id) {
	switch (id)
	{
	case g_ImporterID_OBJ:
		miplStd_ImportOBJ(fileName);
		break;
	default:
		break;
	}
}

void miplStd::OnShiftGizmo(miGizmoMode gm, miEditMode em, miSceneObject* o) 
{
	if (o->GetPlugin() != this)
		return;

	switch (em)
	{
	case miEditMode::Vertex:
		break;
	case miEditMode::Edge:
		break;
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
		break;
	default:
		break;
	}
}