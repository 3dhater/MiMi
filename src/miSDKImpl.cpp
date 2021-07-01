#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miVisualObjectImpl.h"
#include "miPluginGUIImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;



miSDKImpl::miSDKImpl() {
	m_selectObject_onIsGoodObject = 0;
	m_selectObject_onSelect = 0;
	m_selectObject_onCancel = 0;
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
	g_app->m_pluginGuiAll.push_back(new_gui);
	return new_gui;
}

miVisualObject* miSDKImpl::CreateVisualObject(miSceneObject* parent, miVisualObjectType type) {
	auto o = miCreate<miVisualObjectImpl>();
	o->m_type = type;
	o->m_parentSceneObject = parent;
	return o;
}

miViewportCameraType miSDKImpl::GetActiveViewportCameraType() { return g_app->m_activeViewportLayout->m_activeViewport->m_cameraType; }

miEditMode miSDKImpl::GetEditMode() {return g_app->m_editMode;}
void miSDKImpl::SetEditMode(miEditMode em) {g_app->m_editMode = em;}

v2f miSDKImpl::GetCursorPosition2D() {
	return g_app->m_inputContext->m_cursorCoords;
}
v3f miSDKImpl::GetCursorPosition3D() {
	return v3f(g_app->m_cursorPosition3D.x, g_app->m_cursorPosition3D.y, g_app->m_cursorPosition3D.z);
}
v3f miSDKImpl::GetCursorPosition3DFirstClick() {
	return v3f(g_app->m_cursorLMBClickPosition3D.x, g_app->m_cursorLMBClickPosition3D.y, g_app->m_cursorLMBClickPosition3D.z);
}

void miSDKImpl::SetCursorBehaviorMode(miCursorBehaviorMode m) {
	g_app->SetCursorBehaviorMode(m);
	g_app->UpdateSceneAabb();
}
miCursorBehaviorMode miSDKImpl::GetCursorBehaviorMode() {
	return g_app->m_cursorBehaviorMode;
}
void miSDKImpl::SetSelectObjectCallbacks(bool(*onIsGoodObject)(miSceneObject*), void(*onSelect)(miSceneObject*), void(*onCancel)()) {
	m_selectObject_onIsGoodObject = onIsGoodObject;
	m_selectObject_onSelect = onSelect;
	m_selectObject_onCancel = onCancel;
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
	yyRay* ray, 
	const v2f& coords, 
	const v4f& viewportRect, 
	const Mat4& VPInvert) 
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

	this->AppendMesh(newObject->m_mesh, &ih->m_meshBuilder->m_mesh);

	//newObject->m_meshBuilder = ih->m_meshBuilder;
	newObject->m_visualObject_polygon->CreateNewGPUModels(newObject->m_mesh);
	newObject->m_visualObject_vertex->CreateNewGPUModels(newObject->m_mesh);
	newObject->m_visualObject_edge->CreateNewGPUModels(newObject->m_mesh);
	newObject->UpdateTransform();
	newObject->UpdateAabb();
	AddObjectToScene(newObject, name);
}

void miSDKImpl::AppendMesh(miMesh* mesh_with_miDefaultAllocator, miMesh* other) {
	miDefaultAllocator<miPolygon> pa(0);
	miDefaultAllocator<miEdge> ea(0);
	miDefaultAllocator<miVertex> va(0);

	// 1. Create maps-dictionaries
	// 2. Create verts/polys/edges and put addresses in maps
	// 3. Change addresses using maps
	// 4. Set links in this mesh

	// old address, new address
	//std::unordered_map<uint64_t, uint64_t> pmap;
	//std::unordered_map<uint64_t, uint64_t> emap;
	//std::unordered_map<uint64_t, uint64_t> vmap;
	miBinarySearchTree<uint64_t> pmap;
	miBinarySearchTree<uint64_t> emap;
	miBinarySearchTree<uint64_t> vmap;

	miPolygon * P = 0;
	miEdge * E = 0;
	miVertex * V = 0;
	{
		auto current_polygon = other->m_first_polygon;
		auto last_polygon = current_polygon->m_left;
		while (true) {
			miPolygon* new_P = pa.Allocate();
			if (!P)
			{
				P = new_P;
				P->m_right = P;
				P->m_left = P;
			}
			else
			{
				auto last = P->m_left;
				last->m_right = new_P;
				new_P->m_left = last;
				new_P->m_right = P;
				P->m_left = new_P;
			}

			new_P->CopyData(current_polygon);

			pmap.Add((uint64_t)current_polygon, (uint64_t)new_P);

			if (current_polygon == last_polygon)
				break;
			current_polygon = current_polygon->m_right;
		}
	}

	{
		auto current_vertex = other->m_first_vertex;
		auto last_vertex = current_vertex->m_left;
		while (true) {

			miVertex* new_V = va.Allocate();
			if (!V)
			{
				V = new_V;
				V->m_right = V;
				V->m_left = V;
			}
			else
			{
				auto last = V->m_left;
				last->m_right = new_V;
				new_V->m_left = last;
				new_V->m_right = V;
				V->m_left = new_V;
			}

			new_V->CopyData(current_vertex);

			vmap.Add((uint64_t)current_vertex, (uint64_t)new_V);

			if (current_vertex == last_vertex)
				break;
			current_vertex = current_vertex->m_right;
		}
	}

	{
		auto current_edge = other->m_first_edge;
		auto last_edge = current_edge->m_left;
		while (true) {

			miEdge* new_E = ea.Allocate();
			if (!E)
			{
				E = new_E;
				E->m_right = E;
				E->m_left = E;
			}
			else
			{
				auto last = E->m_left;
				last->m_right = new_E;
				new_E->m_left = last;
				new_E->m_right = E;
				E->m_left = new_E;
			}

			new_E->CopyData(current_edge);

			emap.Add((uint64_t)current_edge, (uint64_t)new_E);

			if (current_edge == last_edge)
				break;
			current_edge = current_edge->m_right;
		}
	}

	// step 3
	{
		auto current_polygon = other->m_first_polygon;
		auto last_polygon = current_polygon->m_left;
		while (true) {
			uint64_t new_p_address = 0;
			if (pmap.Get((uint64_t)current_polygon, new_p_address))
			{
				miPolygon* new_P = (miPolygon*)new_p_address;

				auto current_edge = current_polygon->m_edges.m_head;
				auto last_edge = current_edge->m_left;
				while (true) {

					uint64_t new_e_address = 0;
					if (emap.Get((uint64_t)current_edge->m_data, new_e_address))
						new_P->m_edges.push_back((miEdge*)new_e_address);
					
					if (current_edge == last_edge)
						break;
					current_edge = current_edge->m_right;
				}
				
				auto current_vertex = current_polygon->m_verts.m_head;
				auto last_vertex = current_vertex->m_left;
				while (true) {

					uint64_t new_v_address = 0;
					if (vmap.Get((uint64_t)current_vertex->m_data, new_v_address))
						new_P->m_verts.push_back((miVertex*)new_v_address);

					if (current_vertex == last_vertex)
						break;
					current_vertex = current_vertex->m_right;
				}
			}

			if (current_polygon == last_polygon)
				break;
			current_polygon = current_polygon->m_right;
		}
	}

	{
		auto current_vertex = other->m_first_vertex;
		auto last_vertex = current_vertex->m_left;
		while (true) {
			uint64_t new_v_address = 0;
			if (vmap.Get((uint64_t)current_vertex, new_v_address))
			{
				miVertex* new_V = (miVertex*)new_v_address;
				auto current_edge = current_vertex->m_edges.m_head;
				auto last_edge = current_edge->m_left;
				while (true) {
					uint64_t new_e_address = 0;
					if (emap.Get((uint64_t)current_edge->m_data, new_e_address))
						new_V->m_edges.push_back((miEdge*)new_e_address);
					if (current_edge == last_edge) break;
					current_edge = current_edge->m_right;
				}
				auto current_polygon = current_vertex->m_polygons.m_head;
				auto last_polygon = current_polygon->m_left;
				while (true) {
					uint64_t new_p_address = 0;
					if (pmap.Get((uint64_t)current_polygon->m_data, new_p_address))
						new_V->m_polygons.push_back((miPolygon*)new_p_address);
					if (current_polygon == last_polygon) break;
					current_polygon = current_polygon->m_right;
				}
			}

			if (current_vertex == last_vertex)
				break;
			current_vertex = current_vertex->m_right;
		}
	}

	{
		auto current_edge = other->m_first_edge;
		auto last_edge = current_edge->m_left;
		while (true) {
			uint64_t new_e_address = 0;
			if (emap.Get((uint64_t)current_edge, new_e_address))
			{
				miEdge* new_E = (miEdge*)new_e_address;
				
				uint64_t new_v1_address = 0;
				uint64_t new_v2_address = 0;
				if (vmap.Get((uint64_t)current_edge->m_vertex1, new_v1_address))
					new_E->m_vertex1 = (miVertex*)new_v1_address;

				if (vmap.Get((uint64_t)current_edge->m_vertex2, new_v2_address))
					new_E->m_vertex2 = (miVertex*)new_v2_address;

				uint64_t new_p1_address = 0;
				uint64_t new_p2_address = 0;
				if (current_edge->m_polygon1)
				{
					if (pmap.Get((uint64_t)current_edge->m_polygon1, new_p1_address))
						new_E->m_polygon1 = (miPolygon*)new_p1_address;
				}

				if (current_edge->m_polygon2)
				{
					if (pmap.Get((uint64_t)current_edge->m_polygon2, new_p2_address))
						new_E->m_polygon2 = (miPolygon*)new_p2_address;
				}
			}


			if (current_edge == last_edge)
				break;
			current_edge = current_edge->m_right;
		}
	}

	// step 4
	if (!mesh_with_miDefaultAllocator->m_first_polygon)
	{
		mesh_with_miDefaultAllocator->m_first_polygon = P;
	}
	else
	{
		// old-end connect to new-begin
		mesh_with_miDefaultAllocator->m_first_polygon->m_left->m_right = P;
		P->m_left = mesh_with_miDefaultAllocator->m_first_polygon->m_left;
		// new-end connect to old-begin
		P->m_left->m_right = mesh_with_miDefaultAllocator->m_first_polygon;
		mesh_with_miDefaultAllocator->m_first_polygon->m_left = P;
	}

	if (!mesh_with_miDefaultAllocator->m_first_vertex)
	{
		mesh_with_miDefaultAllocator->m_first_vertex = V;
	}
	else
	{
		mesh_with_miDefaultAllocator->m_first_vertex->m_left->m_right = V;
		V->m_left = mesh_with_miDefaultAllocator->m_first_vertex->m_left;
		V->m_left->m_right = mesh_with_miDefaultAllocator->m_first_vertex;
		mesh_with_miDefaultAllocator->m_first_vertex->m_left = V;
	}

	if (!mesh_with_miDefaultAllocator->m_first_edge)
	{
		mesh_with_miDefaultAllocator->m_first_edge = E;
	}
	else
	{
		mesh_with_miDefaultAllocator->m_first_edge->m_left->m_right = E;
		E->m_left = mesh_with_miDefaultAllocator->m_first_edge->m_left;
		E->m_left->m_right = mesh_with_miDefaultAllocator->m_first_edge;
		mesh_with_miDefaultAllocator->m_first_edge->m_left = E;
	}
}

void miSDKImpl::AddVertexToSelection(miVertex* vertex, miSceneObject* o) {
	m_vertsForSelect.push_back(miPair<miVertex*, miSceneObject*>(vertex, o));
}

void miSDKImpl::AddEdgeToSelection(miEdge* e, miSceneObject* o) {
	m_edgesForSelect.push_back(miPair<miEdge*, miSceneObject*>(e, o));
}

void miSDKImpl::AddPolygonToSelection(const miPair<miPolygon*,f32>& p, miSceneObject* o) {
	m_polygonsForSelect.push_back(miPair<miPair<miPolygon*, f32>, miSceneObject*>(p, o));
}