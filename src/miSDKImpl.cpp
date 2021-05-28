#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miVisualObjectImpl.h"
#include "miPluginGUIImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;



miSDKImpl::miSDKImpl() {
}

miSDKImpl::~miSDKImpl() {
	for (u16 i = 0, sz = m_objectCategories.size(); i < sz; ++i)
	{
		delete m_objectCategories[i];
	}
	for (u16 i = 0, sz = m_importers.size(); i < sz; ++i)
	{
		delete m_importers[i];
	}
}

miPluginGUI* miSDKImpl::CreatePluginGUI(miPluginGUIType t) {
	miPluginGUIImpl * new_gui = miCreate<miPluginGUIImpl>();
	new_gui->_init(t);
	return new_gui;
}

miVisualObject* miSDKImpl::CreateVisualObject(miSceneObject* parent) {
	auto o = miCreate<miVisualObjectImpl>();
	o->m_parentSceneObject = parent;
	return o;
}

miViewportCameraType miSDKImpl::GetActiveViewportCameraType() { return g_app->m_activeViewportLayout->m_activeViewport->m_cameraType; }

miEditMode miSDKImpl::GetEditMode() {return g_app->m_editMode;}
void miSDKImpl::SetEditMode(miEditMode em) {g_app->m_editMode = em;}

miVec2 miSDKImpl::GetCursorPosition2D() {
	return mimath::v2f_to_miVec2(g_app->m_inputContext->m_cursorCoords);
}
miVec3 miSDKImpl::GetCursorPosition3D() {
	return mimath::v3f_to_miVec3(v3f(g_app->m_cursorPosition3D.x, g_app->m_cursorPosition3D.y, g_app->m_cursorPosition3D.z));
}
miVec3 miSDKImpl::GetCursorPosition3DFirstClick() {
	return mimath::v3f_to_miVec3(v3f(g_app->m_cursorLMBClickPosition3D.x, g_app->m_cursorLMBClickPosition3D.y, g_app->m_cursorLMBClickPosition3D.z));
}
void miSDKImpl::SetCursorBehaviorModer(miCursorBehaviorMode m) {
	g_app->ChangeCursorBehaviorMode(m);
	g_app->UpdateSceneAabb();
}
miCursorBehaviorMode miSDKImpl::GetCursorBehaviorModer() {
	return g_app->m_cursorBehaviorMode;
}
miKeyboardModifier miSDKImpl::GetKeyboardModifier() {
	return g_app->m_keyboardModifier;
}
void miSDKImpl::UpdateSceneAabb() {
	g_app->UpdateSceneAabb();
}

void miSDKImpl::SetActivePlugin(miPlugin* p) {
	g_app->m_pluginActive = p;
}

miObjectCategory* miSDKImpl::_getObjectCategory(const wchar_t* category) {
	miObjectCategory* cat = 0;
	yyStringW str = category;
	for (u16 i = 0, sz = m_objectCategories.size(); i < sz; ++i)
	{
		if (str == m_objectCategories[i]->m_categoryName)
		{
			cat = m_objectCategories[i];
			break;
		}
	}
	if (!cat)
	{
		cat = new miObjectCategory;
		cat->m_categoryName = category;
		m_objectCategories.push_back(cat);
	}
	return cat;
}

void miSDKImpl::RegisterNewObject(
	miPlugin* plugin, 
	unsigned int objectID,
	const wchar_t* category, 
	const wchar_t* objectName) 
{
	assert(plugin);
	assert(category);
	assert(objectName);

	auto cat = this->_getObjectCategory(category);

	unsigned int id = g_app->m_miCommandID_for_plugins_count;
	++g_app->m_miCommandID_for_plugins_count;

	cat->AddObject(plugin, objectName, id + miCommandID_for_plugins, objectID);

	//g_app->m_pluginCommandID.Add(id, miPluginCommandIDMapNode(plugin, id, objectID));

	//return id;
}

void miSDKImpl::RegisterImporter(
	miPlugin* plugin, 
	unsigned int importerID, 
	const wchar_t* title, 
	const wchar_t* extensions, 
	miPluginGUI* gui
)
{
	assert(plugin);
	assert(title);
	assert(extensions);

	unsigned int id = g_app->m_miCommandID_for_plugins_count;
	++g_app->m_miCommandID_for_plugins_count;

	miImporter* imp = new miImporter;
	imp->m_gui = gui;
	imp->m_title = title;
	imp->m_plugin = plugin;
	imp->m_importerID = importerID;
	imp->m_popupIndex = id + miCommandID_for_plugins;

	util::stringGetWords(&imp->m_extensions, yyStringW(extensions));
	m_importers.push_back(imp);
}

void miSDKImpl::GetRayFromScreen(
	miRay* ray, 
	const miVec2& coords, 
	const miVec4& viewportRect, 
	const miMatrix& VPInvert) 
{
	assert(ray);
	v2f point;
	point.x = coords.x - viewportRect.x;
	point.y = coords.y - viewportRect.y;
	float pt_x = ((float)point.x / (viewportRect.z - viewportRect.x)) * 2.f - 1.f;
	float pt_y = -((float)point.y / (viewportRect.w - viewportRect.y)) * 2.f + 1.f;
	
	Mat4 VPI;
	auto VPIptr = VPI.getPtr();
	auto VPInvertptr = VPInvert.getPtrConst();
	for (int i = 0; i < 16; ++i) {
		VPIptr[i] = VPInvertptr[i];
	}
	auto O = math::mul(v4f(pt_x, pt_y, g_app->m_gpuDepthRange.x, 1.f), VPI);
	auto E = math::mul(v4f(pt_x, pt_y, g_app->m_gpuDepthRange.y, 1.f), VPI);

	ray->m_origin.set(O.x, O.y, O.z, O.w);
	ray->m_end.set(E.x, E.y, E.z, E.w);

	ray->m_origin.w = 1.0f / ray->m_origin.w;
	ray->m_origin.x *= ray->m_origin.w;
	ray->m_origin.y *= ray->m_origin.w;
	ray->m_origin.z *= ray->m_origin.w;

	ray->m_end.w = 1.0f / ray->m_end.w;
	ray->m_end.x *= ray->m_end.w;
	ray->m_end.y *= ray->m_end.w;
	ray->m_end.z *= ray->m_end.w;

	ray->update();
}

void miSDKImpl::AddObjectToScene(miSceneObject* o, const wchar_t* n){
	g_app->AddObjectToScene(o, n);
}

void miSDKImpl::RemoveObjectFromScene(miSceneObject* o) {
	g_app->RemoveObjectFromScene(o);
}

size_t miSDKImpl::FileSize(const char* fileName) {
	return yy_fs::file_size(fileName);
}

void miSDKImpl::CreateSceneObjectFromHelper(miSDKImporterHelper* ih, const wchar_t* name) {
	wprintf(L"Hello %s\n", name);
	miEditableObject* newObject = (miEditableObject*)miMalloc(sizeof(miEditableObject));
	new(newObject)miEditableObject(this, 0);
	 
	newObject->m_visualObject->CreateNewGPUModels(&ih->m_meshBuilder->m_mesh);
	newObject->UpdateAabb();
	AddObjectToScene(newObject, name);
}