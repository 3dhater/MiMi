#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

// only for miDefaultAllocator for this mesh
// obviously
void miAppendMesh(miMesh* thisMesh, miMesh* other)
{
	miDefaultAllocator<miPolygon> pa(0);
	miDefaultAllocator<miEdge> ea(0);
	miDefaultAllocator<miVertex> va(0);

	// 1. Create maps-dictionaries
	// 2. Create verts/polys/edges
	// 3. Change addresses using maps

	// old address, new address
	std::unordered_map<size_t, size_t> pmap;
	std::unordered_map<size_t, size_t> emap;
	std::unordered_map<size_t, size_t> vmap;

	{
		auto current_polygon = other->m_first_polygon;
		auto last_polygon = current_polygon->m_left;
		while (true) {

			if (current_polygon == last_polygon)
				break;
			current_polygon = current_polygon->m_right;
		}
	}

	{
		auto current_vertex = other->m_first_vertex;
		auto last_vertex = current_vertex->m_left;
		while (true) {

			if (current_vertex == last_vertex)
				break;
			current_vertex = current_vertex->m_right;
		}
	}

	{
		auto current_edge = other->m_first_edge;
		auto last_edge = current_edge->m_left;
		while (true) {

			if (current_edge == last_edge)
				break;
			current_edge = current_edge->m_right;
		}
	}
}

miEditableObject::miEditableObject(miSDK* sdk, miPlugin*) {
	m_sdk = sdk;
	m_visualObject_polygon = m_sdk->CreateVisualObject(this, miVisualObjectType::Polygon);
	m_visualObject_vertex = m_sdk->CreateVisualObject(this, miVisualObjectType::Vertex);
	m_visualObject_edge = m_sdk->CreateVisualObject(this, miVisualObjectType::Edge);
	m_flags = 0;
	//m_meshBuilder = 0;
	m_mesh = miCreate<miMesh>();
}

miEditableObject::~miEditableObject() {
	if (m_visualObject_polygon) miDestroy(m_visualObject_polygon);
	if (m_visualObject_vertex) miDestroy(m_visualObject_vertex);
	if (m_visualObject_edge) miDestroy(m_visualObject_edge);
}

void miEditableObject::OnDraw(miViewportDrawMode dm, miEditMode em, float dt) {
	if (dm == miViewportDrawMode::Material
		|| dm == miViewportDrawMode::MaterialWireframe)
	{
		if (m_visualObject_polygon) m_visualObject_polygon->Draw();
	}

	if (dm == miViewportDrawMode::Wireframe
		|| dm == miViewportDrawMode::MaterialWireframe)
	{
		if (m_visualObject_edge) m_visualObject_edge->Draw();
	}

	if (dm == miViewportDrawMode::Wireframe
		|| em == miEditMode::Edge)
	{
		if (m_visualObject_edge) m_visualObject_edge->Draw();
	}

	if (em == miEditMode::Vertex && m_isSelected)
	{
		if (m_visualObject_vertex) m_visualObject_vertex->Draw();
	}
}

void miEditableObject::OnUpdate(float dt) {
}
void miEditableObject::OnCreation(miPluginGUI*) {
}
void miEditableObject::OnCreationLMBDown() {
}
void miEditableObject::OnCreationLMBUp() {
}
void miEditableObject::OnCreationMouseMove() {
}
void miEditableObject::OnCreationEnd() {
}


miPlugin* miEditableObject::GetPlugin() { 
	return 0; 
}

int miEditableObject::GetVisualObjectCount() { 
	return 3;
}

int miEditableObject::GetMeshCount() {
	return 1;
}
miMesh* miEditableObject::GetMesh(int) {
	return m_mesh;
}

miVisualObject* miEditableObject::GetVisualObject(int i){ 
	switch (i)
	{
	default:
	case 0: return m_visualObject_polygon; break;
	case 1: return m_visualObject_edge; break;
	case 2: return m_visualObject_vertex; break;
	}
	return m_visualObject_polygon;
}

void miEditableObject::DeleteSelectedObjects(miEditMode em) {
	switch (em)
	{
	case miEditMode::Vertex:
		break;
	case miEditMode::Edge:
		break;
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
	default:
		break;
	}
}

void miEditableObject::OnConvertToEditableObject() {

}

void miEditableObject::SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	miSceneObject::SelectSingle(em, km, sf);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

void miEditableObject::Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	miSceneObject::Select(em, km, sf);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
		if (m_isSelected)
		{
			//printf("select verts\n");

		}
		break;
	case miEditMode::Edge:
		break;
	case miEditMode::Polygon:
		break;
	}
}

void miEditableObject::SelectAll(miEditMode em) {
	miSceneObject::SelectAll(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

void miEditableObject::DeselectAll(miEditMode em) {
	miSceneObject::DeselectAll(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

void miEditableObject::InvertSelection(miEditMode em) {
	miSceneObject::InvertSelection(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

