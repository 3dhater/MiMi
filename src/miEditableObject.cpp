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

bool miEditableObject::IsEdgeMouseHover(miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;

	while (true) {
		if (sf->LineInFrust(
			math::mul(current_edge->m_vertex1->m_position, M) + *position,
			math::mul(current_edge->m_vertex2->m_position, M) + *position))
			return true;

		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
	return false;
}

bool miEditableObject::IsVertexMouseHover(miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;

	while (true) {
		if (sf->PointInFrust(math::mul(current_vertex->m_position, M) + *position))
			return true;

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}
	return false;
}

void miEditableObject::_selectVertex(miKeyboardModifier km, miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;

	static yyArraySimple<miVertex*> verts_in_frust;
	verts_in_frust.clear();

	while (true) {
		if (sf->PointInFrust(math::mul(current_vertex->m_position, M) + *position))
			verts_in_frust.push_back(current_vertex);

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}

	//printf("Select verts %u\n", verts_in_frust.m_size);
	if (verts_in_frust.m_size)
	{
		static miEditableObject* o;
		o = this;
		struct _pred {
			bool operator() (miVertex* a, miVertex* b) const {
				auto camera = g_app->GetActiveCamera();
				return (a->m_position + *o->GetGlobalPosition()).distance(camera->m_positionCamera) > (b->m_position + *o->GetGlobalPosition()).distance(camera->m_positionCamera);
			}
		};
		verts_in_frust.sort_insertion(_pred());

		// Due to the fact that the user can select multiple objects
		//  and then try to select vertex, this simple solution will select
		//   one vertex in each model
		//  But we need to select only 1 vertex. So we need other solution.
		//if (km == miKeyboardModifier::Alt)
		//{
		//	if (verts_in_frust.m_data[0]->m_flags & miVertex::flag_isSelected)
		//		verts_in_frust.m_data[0]->m_flags ^= miVertex::flag_isSelected;
		//}
		//else
		//{
		//	//printf("Select\n");
		//	verts_in_frust.m_data[0]->m_flags |= miVertex::flag_isSelected;
		//}
		//m_visualObject_vertex->CreateNewGPUModels(m_mesh);

		/* Solution can be like this:
			Application must collect each vertex, and then decide who will be selected
			m_sdk->AddVertexToSelection(miVertex*);
		*/
		m_sdk->AddVertexToSelection(verts_in_frust.m_data[0], this);
	}
}

void miEditableObject::_selectEdge(miKeyboardModifier km, miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;

	static yyArraySimple<miEdge*> edges_in_frust;
	edges_in_frust.clear();

	while (true) {
		if (sf->LineInFrust(
			math::mul(current_edge->m_vertex1->m_position, M) + *position,
			math::mul(current_edge->m_vertex2->m_position, M) + *position))
		{
			edges_in_frust.push_back(current_edge);
		}

		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
	if (edges_in_frust.m_size)
	{
		static miEditableObject* o;
		o = this;
		struct _pred {
			bool operator() (miEdge* a, miEdge* b) const {
				auto camera = g_app->GetActiveCamera();
				
				auto center = a->m_vertex1->m_position + b->m_vertex1->m_position;
				center *= 0.5f;
				center += *o->GetGlobalPosition();

				return center.distance(camera->m_positionCamera) > center.distance(camera->m_positionCamera);
			}
		};
		edges_in_frust.sort_insertion(_pred());
		m_sdk->AddEdgeToSelection(edges_in_frust.m_data[0], this);
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
		if (m_isSelected) _selectVertex(km, sf);
		break;
	case miEditMode::Edge:
		if (m_isSelected) _selectEdge(km, sf);
		break;
	case miEditMode::Polygon:
		break;
	}
}

void miEditableObject::_selectEdges_rectangle(miKeyboardModifier km, miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;

	static yyArraySimple<miEdge*> edges_in_frust;
	edges_in_frust.clear();

	while (true) {
		if (sf->LineInFrust(
			math::mul(current_edge->m_vertex1->m_position, M) + *position,
			math::mul(current_edge->m_vertex2->m_position, M) + *position))
			edges_in_frust.push_back(current_edge);

		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}

	//printf("Select verts %u\n", verts_in_frust.m_size);
	if (edges_in_frust.m_size)
	{
		for (u32 i = 0; i < edges_in_frust.m_size; ++i)
		{
			auto v = edges_in_frust.m_data[i];
			if (km == miKeyboardModifier::Alt)
			{
				if (v->m_flags & miEdge::flag_isSelected)
					v->m_flags ^= miEdge::flag_isSelected;
			}
			else
				v->m_flags |= miEdge::flag_isSelected;
		}

		m_visualObject_edge->CreateNewGPUModels(m_mesh);
	}
}
void miEditableObject::_selectVerts_rectangle(miKeyboardModifier km, miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;

	static yyArraySimple<miVertex*> verts_in_frust;
	verts_in_frust.clear();

	while (true) {
		if (sf->PointInFrust(math::mul(current_vertex->m_position, M) + *position))
			verts_in_frust.push_back(current_vertex);

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}

	//printf("Select verts %u\n", verts_in_frust.m_size);
	if (verts_in_frust.m_size)
	{
		for (u32 i = 0; i < verts_in_frust.m_size; ++i)
		{
			auto v = verts_in_frust.m_data[i];
			if (km == miKeyboardModifier::Alt)
			{
				if (v->m_flags & miVertex::flag_isSelected)
					v->m_flags ^= miVertex::flag_isSelected;
			}
			else
				v->m_flags |= miVertex::flag_isSelected;
		}

		m_visualObject_vertex->CreateNewGPUModels(m_mesh);
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
		if(m_isSelected) _selectVerts_rectangle(km, sf);
		break;
	case miEditMode::Edge:
		if (m_isSelected) _selectEdges_rectangle(km, sf);
		break;
	case miEditMode::Polygon:
		break;
	}
}

void miEditableObject::_selectAllEdges() {
	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;
	while (true) {
		current_edge->m_flags |= current_edge->flag_isSelected;
		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
	m_visualObject_edge->CreateNewGPUModels(m_mesh);
}
void miEditableObject::_selectAllVerts() {
	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;
	while (true) {
		current_vertex->m_flags |= current_vertex->flag_isSelected;
		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}
	m_visualObject_vertex->CreateNewGPUModels(m_mesh);
}

void miEditableObject::SelectAll(miEditMode em) {
	miSceneObject::SelectAll(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
		if (m_isSelected) _selectAllVerts();
		break;
	case miEditMode::Edge:
		if (m_isSelected) _selectAllEdges();
		break;
	case miEditMode::Polygon:
		break;
	}
}

void miEditableObject::_deselectAllEdges() {
	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;
	while (true) {
		if(current_edge->m_flags & current_edge->flag_isSelected)
			current_edge->m_flags ^= current_edge->flag_isSelected;
		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
	m_visualObject_edge->CreateNewGPUModels(m_mesh);
}
void miEditableObject::_deselectAllVerts() {
	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;
	while (true) {
		if(current_vertex->m_flags & current_vertex->flag_isSelected)
			current_vertex->m_flags ^= current_vertex->flag_isSelected;
		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}
	m_visualObject_vertex->CreateNewGPUModels(m_mesh);
}

void miEditableObject::DeselectAll(miEditMode em) {
	miSceneObject::DeselectAll(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
		if (m_isSelected) _deselectAllVerts();
		break;
	case miEditMode::Edge:
		if (m_isSelected) _deselectAllEdges();
		break;
	case miEditMode::Polygon:
		break;
	}
}

void miEditableObject::_selectInvertEdges() {
	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;
	while (true) {
		if (current_edge->m_flags & current_edge->flag_isSelected)
			current_edge->m_flags ^= current_edge->flag_isSelected;
		else
			current_edge->m_flags |= current_edge->flag_isSelected;
		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
	m_visualObject_edge->CreateNewGPUModels(m_mesh);
}
void miEditableObject::_selectInvertVerts() {
	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;
	while (true) {
		if (current_vertex->m_flags & current_vertex->flag_isSelected)
			current_vertex->m_flags ^= current_vertex->flag_isSelected;
		else
			current_vertex->m_flags |= current_vertex->flag_isSelected;

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}
	m_visualObject_vertex->CreateNewGPUModels(m_mesh);
}

void miEditableObject::InvertSelection(miEditMode em) {
	miSceneObject::InvertSelection(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
		if (m_isSelected) _selectInvertVerts();
		break;
	case miEditMode::Edge:
		if (m_isSelected) _selectInvertEdges();
		break;
	case miEditMode::Polygon:
		break;
	}
}

