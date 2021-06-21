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

	if (em == miEditMode::Polygon)
	{
		if (m_visualObject_polygon && m_isSelected) m_visualObject_polygon->Draw();
	}

	if ((dm == miViewportDrawMode::Wireframe || dm == miViewportDrawMode::MaterialWireframe)
		|| (m_isSelected && em == miEditMode::Edge)
		|| (m_isSelected && em == miEditMode::Polygon)
		)
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

void miEditableObject::_selectPolygon(miKeyboardModifier km, miSelectionFrust* sf) {
	yyRay r;
	r.m_origin = sf->m_data.m_BackC;
	r.m_end = sf->m_data.m_FrontC;
	r.update();
	
	if (!m_aabbTransformed.rayTest(r))
		return;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_polygon = m_mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;

	// m_second - distance T
	static yyArraySimple<miPair<miPolygon*,f32>> polygons_in_frust;
	polygons_in_frust.clear();

	while (true) {

		auto current_vertex = current_polygon->m_verts.m_head;
		auto last_vertex = current_vertex->m_left;
		auto first_vertex = current_vertex;
		while (true)
		{
			auto next_vertex = current_vertex->m_right;

			miTriangle tri;
			tri.v1 = math::mul(first_vertex->m_data->m_position, M)
				+ *this->GetGlobalPosition();
			tri.v2 = math::mul(next_vertex->m_data->m_position, M)
				+ *this->GetGlobalPosition();
			tri.v3 = math::mul(next_vertex->m_right->m_data->m_position, M)
				+ *this->GetGlobalPosition();
			tri.update();
			
			f32 T, U, V, W;
			T = U = V = W = 0.f;
			if (tri.rayTest_MT(r, true, T, U, V, W))
			{
				polygons_in_frust.push_back(miPair<miPolygon*, f32>(current_polygon,T));
				break;
			}

			if (current_vertex == last_vertex)
				break;
			current_vertex = current_vertex->m_right;
		}

		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}
	if (polygons_in_frust.m_size)
	{
		static miEditableObject* o;
		o = this;
		struct _pred {
			bool operator() (
				const miPair<miPolygon*, f32>& a, 
				const miPair<miPolygon*, f32>& b) const 
			{
				return a.m_second < b.m_second;
			}
		};
		polygons_in_frust.sort_insertion(_pred());
		m_sdk->AddPolygonToSelection(polygons_in_frust.m_data[0], this);
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
		if (m_isSelected) _selectPolygon(km, sf);
		break;
	}
}

void miEditableObject::_selectPolygons_rectangle(miKeyboardModifier km, miSelectionFrust* sf) {
	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;

	bool needUpdate = false;
	if (km == miKeyboardModifier::Alt)
	{
		while (true) {
			if (sf->LineInFrust(
				math::mul(current_edge->m_vertex1->m_position, M) + *position,
				math::mul(current_edge->m_vertex2->m_position, M) + *position))
			{
				needUpdate = true;
				if (current_edge->m_polygon1)
				{
					if (current_edge->m_polygon1->m_flags & miPolygon::flag_isSelected)
						current_edge->m_polygon1->m_flags ^= miPolygon::flag_isSelected;
				}
				if (current_edge->m_polygon2)
				{
					if (current_edge->m_polygon2->m_flags & miPolygon::flag_isSelected)
						current_edge->m_polygon2->m_flags ^= miPolygon::flag_isSelected;
				}
			}
			if (current_edge == last_edge)
				break;
			current_edge = current_edge->m_right;
		}
	}
	else
	{
		while (true) {
			if (sf->LineInFrust(
				math::mul(current_edge->m_vertex1->m_position, M) + *position,
				math::mul(current_edge->m_vertex2->m_position, M) + *position))
			{
				needUpdate = true;
				if (current_edge->m_polygon1)
					current_edge->m_polygon1->m_flags |= miPolygon::flag_isSelected;
				if (current_edge->m_polygon2)
					current_edge->m_polygon2->m_flags |= miPolygon::flag_isSelected;
			}
			if (current_edge == last_edge)
				break;
			current_edge = current_edge->m_right;
		}
	}
	if(needUpdate)
		m_visualObject_polygon->CreateNewGPUModels(m_mesh);
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

	bool need_update = false;
	while (true) {
		if (sf->PointInFrust(math::mul(current_vertex->m_position, M) + *position))
		{
			need_update = true;
			if (km == miKeyboardModifier::Alt)
			{
				if (current_vertex->m_flags & miVertex::flag_isSelected)
					current_vertex->m_flags ^= miVertex::flag_isSelected;
			}
			else
				current_vertex->m_flags |= miVertex::flag_isSelected;
		}

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}

	if(need_update)
		m_visualObject_vertex->CreateNewGPUModels(m_mesh);
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
		if (m_isSelected) _selectPolygons_rectangle(km, sf);
		break;
	}
}

void miEditableObject::_selectAllPolygons() {
	auto current_polygon = m_mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	while (true) {
		current_polygon->m_flags |= current_polygon->flag_isSelected;
		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}
	m_visualObject_polygon->CreateNewGPUModels(m_mesh);
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
		if (m_isSelected) _selectAllPolygons();
		break;
	}
}

void miEditableObject::_deselectAllPolygons() {
	auto current_polygon = m_mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	while (true) {
		if (current_polygon->m_flags & current_polygon->flag_isSelected)
			current_polygon->m_flags ^= current_polygon->flag_isSelected;
		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}
	m_visualObject_polygon->CreateNewGPUModels(m_mesh);
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
		if (m_isSelected) _deselectAllPolygons();
		break;
	}
}

void miEditableObject::_selectInvertPolygons() {
	auto current_polygon = m_mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	while (true) {
		if (current_polygon->m_flags & current_polygon->flag_isSelected)
			current_polygon->m_flags ^= current_polygon->flag_isSelected;
		else
			current_polygon->m_flags |= current_polygon->flag_isSelected;
		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}
	m_visualObject_polygon->CreateNewGPUModels(m_mesh);
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
		if (m_isSelected) _selectInvertPolygons();
		break;
	}
}

bool miEditableObject::IsVertexSelected() {
	auto c = m_mesh->m_first_vertex;
	auto l = c->m_left;
	while (true) {
		if (c->m_flags & c->flag_isSelected)
			return true;

		if (c== l)
			break;
		c = c->m_right;
	}
	return false;
}
bool miEditableObject::IsEdgeSelected() {
	auto c = m_mesh->m_first_edge;
	auto l = c->m_left;
	while (true) {
		if (c->m_flags & c->flag_isSelected)
			return true;

		if (c == l)
			break;
		c = c->m_right;
	}
	return false;
}
bool miEditableObject::IsPolygonSelected() {
	auto c = m_mesh->m_first_polygon;
	auto l = c->m_left;
	while (true) {
		if (c->m_flags & c->flag_isSelected)
			return true;

		if (c == l)
			break;
		c = c->m_right;
	}
	return false;
}