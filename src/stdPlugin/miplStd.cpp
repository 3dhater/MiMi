#define MI_EXPORTS
#include "miSDK.h"

#include "miplStd.h"
#include "miplPolygonObjectPlane.h"

static bool g_isCreated = false;

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
//MI_API void MI_C_DECL miplDestroyPlugin(miPlugin* plugin) {
//	if (plugin)
//	{
//		delete plugin;
//		g_isCreated = false;
//	}
//}

miplStd::miplStd() {
	m_newObjectPtr = 0;
}

miplStd::~miplStd() {
	printf("destoy plugin\n");
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
void miplStd::_saveNewObject() {
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

//miplDestroyPlugin_t miplStd::GetDestroyFunction(){
//	return miplDestroyPlugin;
//}

unsigned int g_CommandID_Plane = 0;
//unsigned int g_CommandID_Box = 0;
//unsigned int g_CommandID_Sphere = 0;
//unsigned int g_CommandID_Point = 0;
//unsigned int g_CommandID_Spot = 0;
//unsigned int g_CommandID_Directional = 0;
//unsigned int g_CommandID_FreeCamera = 0;

bool miplStd::Init(miSDK* sdk){
	m_sdk = sdk;
	//miSingleton<miSDK>::s_instance = sdk;

	g_CommandID_Plane = sdk->RegisterNewObject(this, L"Polygonal objects", L"Plane");
	/*g_CommandID_Box = sdk->RegisterNewObject(this, L"Polygonal objects", L"Box");
	g_CommandID_Sphere = sdk->RegisterNewObject(this, L"Polygonal objects", L"Sphere");
	g_CommandID_Point = sdk->RegisterNewObject(this, L"Light", L"Point");
	g_CommandID_Spot = sdk->RegisterNewObject(this, L"Light", L"Spot");
	g_CommandID_Directional = sdk->RegisterNewObject(this, L"Light", L"Directional");
	g_CommandID_FreeCamera = sdk->RegisterNewObject(this, L"Camera", L"Free camera");*/
	return true;
}

void miplStd::OnLMBDown(miSelectionFrust*) {
	if (m_newObjectPtr) m_newObjectPtr->OnCreationLMBDown();
}

void miplStd::OnLMBUp(miSelectionFrust* sf) {
	if (m_newObjectPtr) m_newObjectPtr->OnCreationLMBUp();

	OnCancel(sf);
}

void miplStd::OnCancel(miSelectionFrust*) {
	if (m_newObjectPtr) _destroyNewObject();
	m_sdk->SetCursorBehaviorModer(miCursorBehaviorMode::CommonMode);
	m_sdk->SetActivePlugin(nullptr);
}

void miplStd::OnCursorMove(miSelectionFrust*) {
	if (m_newObjectPtr) m_newObjectPtr->OnCreationMouseMove();
}
void miplStd::OnUpdate(miSelectionFrust* sf) {
}

void miplStd::OnPopupCommand(unsigned int id) {
	if (id == g_CommandID_Plane) 
	{
		m_sdk->SetCursorBehaviorModer(miCursorBehaviorMode::ClickAndDrag);
		m_sdk->SetActivePlugin(this);

		assert(!m_newObjectPtr);

		//miplPolygonObjectPlane* newObject = new miplPolygonObjectPlane(m_sdk, this);
		miplPolygonObjectPlane* newObject = (miplPolygonObjectPlane*)miMalloc(sizeof(miplPolygonObjectPlane));
		new(newObject)miplPolygonObjectPlane(m_sdk, this);

		m_newObjectPtr = newObject;
	}
}