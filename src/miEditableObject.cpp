#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

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

bool miEditableObject::IsVertexMouseHover(miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;

	static yyArraySimple<miVertex*> verts_in_frust;
	verts_in_frust.clear();

	while (true) {

		if (sf->PointInFrust(math::mul(current_vertex->m_position, M) + *position))
		{
			verts_in_frust.push_back(current_vertex);
			break;
		}

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}
	return verts_in_frust.m_size != 0;
}

void miEditableObject::_selectVerts(miKeyboardModifier km, miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;

	static yyArraySimple<miVertex*> verts_in_frust;
	verts_in_frust.clear();

	while (true) {

		if (sf->PointInFrust(math::mul(current_vertex->m_position, M) + *position))
		{
			verts_in_frust.push_back(current_vertex);
		}

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}

	printf("Select verts %u\n", verts_in_frust.m_size);
	if (verts_in_frust.m_size)
	{
		struct _pred {
			bool operator() (miVertex* a, miVertex* b) const {
				auto camera = g_app->GetActiveCamera();
				return a->m_position.distance(camera->m_positionCamera) > b->m_position.distance(camera->m_positionCamera);
			}
		};
		verts_in_frust.sort_insertion(_pred());
		if (km == miKeyboardModifier::Alt)
		{
			if (verts_in_frust.m_data[0]->m_flags & miVertex::flag_isSelected)
				verts_in_frust.m_data[0]->m_flags ^= miVertex::flag_isSelected;
		}
		else
		{
			printf("Select\n");
			verts_in_frust.m_data[0]->m_flags |= miVertex::flag_isSelected;
		}
		m_visualObject_vertex->CreateNewGPUModels(m_mesh);
	}
}

void miEditableObject::SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	miSceneObject::SelectSingle(em, km, sf);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
		_selectVerts(km, sf);
		break;
	case miEditMode::Edge:
		break;
	case miEditMode::Polygon:
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

