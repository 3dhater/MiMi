#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

const s32 g_SelectButtonID_More = 0;
const s32 g_SelectButtonID_Less = 1;
const s32 g_SelectButtonID_ConnectVerts1 = 2;
const s32 g_SelectButtonID_BreakVerts = 3;

void editableObjectGUI_attachButton_onClick(s32 id, bool isChecked) {
}
void editableObjectGUI_attachButton_onCheck(s32 id) {
	//printf("on check\n");
}
void editableObjectGUI_attachButton_onUncheck(s32 id) {
	//printf("on uncheck\n");
}

void editableObjectGUI_editButton_onClick(s32 id) {
	if (g_app->m_selectedObjects.m_size != 1)
		return;

	auto object = g_app->m_selectedObjects.m_data[0];
	if (object->GetPlugin() != g_app->m_pluginForApp)
		return;

	if (object->GetTypeForPlugin() != miEditableObject_pluginType)
		return;

	miEditableObject* editableObject = (miEditableObject*)object;
	switch (id)
	{
	default:
		break;
	case g_SelectButtonID_ConnectVerts1:
		editableObject->ConnectVerts1();
		break;
	case g_SelectButtonID_BreakVerts:
		editableObject->BreakVerts();
		break;
	}
}
void editableObjectGUI_selectButtons_onClick(s32 id) {
	if (g_app->m_selectedObjects.m_size != 1)
		return;

	auto object = g_app->m_selectedObjects.m_data[0];
	if (object->GetPlugin() != g_app->m_pluginForApp)
		return;

	if(object->GetTypeForPlugin() != miEditableObject_pluginType)
		return;

	miEditableObject* o = (miEditableObject*)object;

	auto em = g_app->GetEditMode();
	auto mc = o->GetMeshCount();
	for (s32 i = 0; i < mc; ++i)
	{
		auto m = o->GetMesh(i);

		switch (em)
		{
		case miEditMode::Vertex:
		{
			yyArraySimple<miVertex*> va;
			va.reserve(0xffff);

			auto c = m->m_first_vertex;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_flags & c->flag_isSelected)
				{
					auto ce = c->m_edges.m_head;
					auto le = ce->m_left;
					while (true)
					{
						if (id == g_SelectButtonID_More) {
							if ((ce->m_data->m_vertex1->m_flags & miVertex::flag_isSelected) == 0)
								va.push_back(ce->m_data->m_vertex1);
						
							if ((ce->m_data->m_vertex2->m_flags & miVertex::flag_isSelected) == 0)
								va.push_back(ce->m_data->m_vertex2);
						}
						else if (id == g_SelectButtonID_Less) {
							if ((ce->m_data->m_vertex1->m_flags & miVertex::flag_isSelected) == 0)
								va.push_back(c);
							if ((ce->m_data->m_vertex2->m_flags & miVertex::flag_isSelected) == 0)
								va.push_back(c);
						}

						if (ce == le)
							break;
						ce = ce->m_right;
					}

				}
				if (c == l)
					break;
				c = c->m_right;
			}

			for (u32 o = 0; o < va.m_size; ++o)
			{
				if (id == g_SelectButtonID_More) {
					va.m_data[o]->m_flags |= miVertex::flag_isSelected;
				}
				else if (id == g_SelectButtonID_Less) {
					if(va.m_data[o]->m_flags & miVertex::flag_isSelected)
						va.m_data[o]->m_flags ^= miVertex::flag_isSelected;
				}
			}
		}break;
		case miEditMode::Edge: {
			yyArraySimple<miEdge*> ea;
			ea.reserve(0xffff);
			auto c = m->m_first_edge;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_flags & miEdge::flag_isSelected)
				{
					auto v1 = c->m_vertex1;
					auto v2 = c->m_vertex2;

					{
						auto ce = v1->m_edges.m_head;
						auto le = ce->m_left;
						while (true)
						{
							if (id == g_SelectButtonID_More) {
								if ((ce->m_data->m_flags & miEdge::flag_isSelected) == 0)
									ea.push_back(ce->m_data);
							}
							else if (id == g_SelectButtonID_Less) {
								if ((ce->m_data->m_flags & miEdge::flag_isSelected) == 0)
									ea.push_back(c);
							}
							if (ce == le)
								break;
							ce = ce->m_right;
						}
					}
					{
						auto ce = v2->m_edges.m_head;
						auto le = ce->m_left;
						while (true)
						{
							if (id == g_SelectButtonID_More) {
								if ((ce->m_data->m_flags & miEdge::flag_isSelected) == 0)
									ea.push_back(ce->m_data);
							}
							else if (id == g_SelectButtonID_Less) {
								if ((ce->m_data->m_flags & miEdge::flag_isSelected) == 0)
									ea.push_back(c);
							}
							if (ce == le)
								break;
							ce = ce->m_right;
						}
					}
				}

				if (c == l)
					break;
				c = c->m_right;
			}
			
			for (u32 o = 0; o < ea.m_size; ++o)
			{
				if (id == g_SelectButtonID_More) {
					ea.m_data[o]->m_flags |= miEdge::flag_isSelected;
				}
				else if (id == g_SelectButtonID_Less) {
					if (ea.m_data[o]->m_flags & miEdge::flag_isSelected)
						ea.m_data[o]->m_flags ^= miEdge::flag_isSelected;
				}
			}
		}break;
		case miEditMode::Polygon: {
			yyArraySimple<miPolygon*> pa;
			pa.reserve(0xffff);
			auto c = m->m_first_polygon;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_flags & miPolygon::flag_isSelected)
				{
					auto cv = c->m_verts.m_head;
					auto lv = cv->m_left;
					while (true)
					{
						auto cp = cv->m_data->m_polygons.m_head;
						auto lp = cp->m_left;
						while (true)
						{
							if (id == g_SelectButtonID_More) {
								if ((cp->m_data->m_flags & miPolygon::flag_isSelected) == 0)
									pa.push_back(cp->m_data);
							}
							else if (id == g_SelectButtonID_Less) {
								if ((cp->m_data->m_flags & miPolygon::flag_isSelected) == 0)
									pa.push_back(c);
							}
							if (cp == lp)
								break;
							cp = cp->m_right;
						}


						if (cv == lv)
							break;
						cv = cv->m_right;
					}
				}

				if (c == l)
					break;
				c = c->m_right;
			}
			
			for (u32 o = 0; o < pa.m_size; ++o)
			{
				if (id == g_SelectButtonID_More) {
					pa.m_data[o]->m_flags |= miPolygon::flag_isSelected;
				}
				else if (id == g_SelectButtonID_Less) {
					if (pa.m_data[o]->m_flags & miPolygon::flag_isSelected)
						pa.m_data[o]->m_flags ^= miPolygon::flag_isSelected;
				}
			}
		}break;
		case miEditMode::Object:
		default:
			break;
		}
	}
	o->OnSelect(em);
	g_app->OnSelect();
}

void miApplication::_initEditableObjectGUI() {
	m_pluginGuiForEditableObject = (miPluginGUIImpl*)m_sdk->CreatePluginGUI(miPluginGUIType::ObjectParams);

	m_pluginGuiForEditableObject->AddButtonAsCheckbox(
		v4f(0.f, 0.f, 50.f, 15.f), 
		L"Attach", 
		-1, 
		editableObjectGUI_attachButton_onClick, 
		editableObjectGUI_attachButton_onCheck,
		editableObjectGUI_attachButton_onUncheck,
		1,
		0);
	m_pluginGuiForEditableObject->AddButtonAsCheckbox(
		v4f(55.f, 0.f, 50.f, 15.f),
		L"Attach 2",
		-1,
		editableObjectGUI_attachButton_onClick,
		editableObjectGUI_attachButton_onCheck,
		editableObjectGUI_attachButton_onUncheck,
		1,
		0);

	m_pluginGuiForEditableObject->AddText(v2f(0.f, 0.f), L"Select:", 0,
		miPluginGUI::Flag_ForEdgeEditMode |
		miPluginGUI::Flag_ForVertexEditMode |
		miPluginGUI::Flag_ForPolygonEditMode);
	f32 y = 0.f;
	m_pluginGuiForEditableObject->AddButton(v4f(50.f, y, 50.f, 15.f), L"More", g_SelectButtonID_More,
		editableObjectGUI_selectButtons_onClick,
		miPluginGUI::Flag_ForEdgeEditMode |
		miPluginGUI::Flag_ForVertexEditMode |
		miPluginGUI::Flag_ForPolygonEditMode);
	m_pluginGuiForEditableObject->AddButton(v4f(105.f, y, 50.f, 15.f), L"Less", g_SelectButtonID_Less,
		editableObjectGUI_selectButtons_onClick,
		miPluginGUI::Flag_ForEdgeEditMode |
		miPluginGUI::Flag_ForVertexEditMode |
		miPluginGUI::Flag_ForPolygonEditMode);
	y += 15.f + 3.f;

	m_pluginGuiForEditableObject->AddText(v2f(0.f, y), L"Edit:", 0, 
		miPluginGUI::Flag_ForVertexEditMode);
	m_pluginGuiForEditableObject->AddButton(v4f(50.f, y, 50.f, 15.f), L"Connect", g_SelectButtonID_ConnectVerts1,
		editableObjectGUI_editButton_onClick, miPluginGUI::Flag_ForVertexEditMode );
	y += 15.f;
	m_pluginGuiForEditableObject->AddButton(v4f(50.f, y, 50.f, 15.f), L"Break", g_SelectButtonID_BreakVerts,
		editableObjectGUI_editButton_onClick, miPluginGUI::Flag_ForVertexEditMode);
}

miEditableObject::miEditableObject(miSDK* sdk, miPlugin*) {
	m_sdk = sdk;
	m_visualObject_polygon = m_sdk->CreateVisualObject(this, miVisualObjectType::Polygon);
	m_visualObject_vertex = m_sdk->CreateVisualObject(this, miVisualObjectType::Vertex);
	m_visualObject_edge = m_sdk->CreateVisualObject(this, miVisualObjectType::Edge);
	m_flags = 0;
	//m_meshBuilder = 0;
	m_allocatorPolygon = new miDefaultAllocator<miPolygon>(0);
	m_allocatorEdge = new miDefaultAllocator<miEdge>(0);
	m_allocatorVertex = new miDefaultAllocator<miVertex>(0);
	m_mesh = miCreate<miMesh>();
	m_gui = (miPluginGUI*)g_app->m_pluginGuiForEditableObject;
	m_plugin = g_app->m_pluginForApp;
	m_typeForPlugin = 0;
}

miEditableObject::~miEditableObject() {
	if (m_visualObject_polygon) miDestroy(m_visualObject_polygon);
	if (m_visualObject_vertex) miDestroy(m_visualObject_vertex);
	if (m_visualObject_edge) miDestroy(m_visualObject_edge);
	_destroyMesh();
}

void miEditableObject::_destroyMesh() {
	if (!m_mesh) return;

	if (m_mesh->m_first_edge)
	{
		auto e = m_mesh->m_first_edge;
		auto last = e->m_left;
		while (true) {
			auto next = e->m_right;
			e->~miEdge();
			m_allocatorEdge->Deallocate(e);
			if (e == last)
				break;
			e = next;
		}
		m_mesh->m_first_edge = 0;
	}

	if (m_mesh->m_first_polygon)
	{
		auto p = m_mesh->m_first_polygon;
		auto last = p->m_left;
		while (true) {
			auto next = p->m_right;
			p->~miPolygon();
			m_allocatorPolygon->Deallocate(p);
			if (p == last)
				break;
			p = next;
		}
	}


	if (m_mesh->m_first_vertex)
	{
		auto v = m_mesh->m_first_vertex;
		auto last = v->m_left;
		while (true) {
			auto next = v->m_right;
			v->~miVertex();
			m_allocatorVertex->Deallocate(v);
			if (v == last)
				break;
			v = next;
		}
	}
	miDestroy(m_mesh);
	m_mesh = 0;
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
	return m_plugin; 
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
	yyArray<miPolygon*> polygonsForDelete;
	polygonsForDelete.reserve(0xffff);
	polygonsForDelete.setAddMemoryValue(0xffff);

	miBinarySearchTree<miPolygon*> uniquePolygons;
	switch (em)
	{
	case miEditMode::Vertex:
	{
		auto c = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & c->flag_isSelected)
			{
				auto cp = c->m_polygons.m_head;
				auto lp = cp->m_left;
				while (true)
				{
					uniquePolygons.Add((uint64_t)cp->m_data, cp->m_data);
					if (cp == lp)
						break;
					cp = cp->m_right;
				}
			}
			if (c == l)
				break;
			c = c->m_right;
		}
	}break;
	case miEditMode::Edge:
	{
		auto c = m_mesh->m_first_edge;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & c->flag_isSelected)
			{
				if(c->m_polygon1)
					uniquePolygons.Add((uint64_t)c->m_polygon1, c->m_polygon1);
				if(c->m_polygon2)
					uniquePolygons.Add((uint64_t)c->m_polygon2, c->m_polygon2);
			}
			if (c == l)
				break;
			c = c->m_right;
		}
	}break;
	case miEditMode::Polygon:
	{
		auto c = m_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & c->flag_isSelected)
				uniquePolygons.Add((uint64_t)c, c);
			if (c == l)
				break;
			c = c->m_right;
		}
	}break;
	case miEditMode::Object:
	default:
		break;
	}

	uniquePolygons.Get(&polygonsForDelete);

	yyLogWriteInfo("Delete [%u] polygons\n", polygonsForDelete.size());
	for (u32 i = 0, sz = polygonsForDelete.size(); i < sz; ++i)
	{
		DeletePolygon(polygonsForDelete[i]);
	}
	auto voc = GetVisualObjectCount();
	for (int i = 0; i < voc; ++i)
	{
		GetVisualObject(i)->CreateNewGPUModels(m_mesh);
	}

}

void miEditableObject::DeletePolygon(miPolygon* _polygon) {
	static yyArraySimple<miVertex*> vertsForDelete;
	static yyArraySimple<miEdge*> edgesForDelete;
	vertsForDelete.clear();
	edgesForDelete.clear();

	// check all verts of this polygons
	{
		auto c = _polygon->m_verts.m_head;
		auto l = c->m_left;
		while (true)
		{
			// remove polygon from list
			c->m_data->m_polygons.erase_first(_polygon);
			// if there was last polygon, then add this vertex in to array
			if (!c->m_data->m_polygons.m_head)
				vertsForDelete.push_back(c->m_data);

			if (c == l)
				break;
			c = c->m_right;
		}
	}

	// check edges
	{
		auto c = _polygon->m_edges.m_head;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_data->m_polygon1 == _polygon)
				c->m_data->m_polygon1 = 0;
			if (c->m_data->m_polygon2 == _polygon)
				c->m_data->m_polygon2 = 0;

			if (c->m_data->m_polygon1 == 0 && c->m_data->m_polygon2 == 0)
			{
				edgesForDelete.push_back(c->m_data);
				// remove this edge from lists in verts
				c->m_data->m_vertex1->m_edges.erase_first(c->m_data);
				c->m_data->m_vertex2->m_edges.erase_first(c->m_data);
			}

			if (c == l)
				break;
			c = c->m_right;
		}
	}

	{
		auto l = _polygon->m_left;
		auto r = _polygon->m_right;
		l->m_right = r;
		r->m_left = l;

		if (m_mesh->m_first_polygon == _polygon)
			m_mesh->m_first_polygon = r;
		if (m_mesh->m_first_polygon == _polygon)
			m_mesh->m_first_polygon = 0;
	}

	//printf("Verts for delete %u\n", vertsForDelete.m_size);
	//printf("Edges for delete %u\n", edgesForDelete.m_size);

	for (u32 i = 0; i < vertsForDelete.m_size; ++i)
	{
		auto c = vertsForDelete.m_data[i];

		auto l = c->m_left;
		auto r = c->m_right;
		l->m_right = r;
		r->m_left = l;

		if (m_mesh->m_first_vertex == c)
			m_mesh->m_first_vertex = r;
		if (m_mesh->m_first_vertex == c)
			m_mesh->m_first_vertex = 0;

		c->~miVertex();
		m_allocatorVertex->Deallocate(c);
	}

	for (u32 i = 0; i < edgesForDelete.m_size; ++i)
	{
		auto c = edgesForDelete.m_data[i];

		auto l = c->m_left;
		auto r = c->m_right;
		l->m_right = r;
		r->m_left = l;

		if (m_mesh->m_first_edge == c)
			m_mesh->m_first_edge = r;
		if (m_mesh->m_first_edge == c)
			m_mesh->m_first_edge = 0;

		c->~miEdge();
		m_allocatorEdge->Deallocate(c);
	}

	_polygon->~miPolygon();
	m_allocatorPolygon->Deallocate(_polygon);
}

void miEditableObject::OnConvertToEditableObject() {

}

bool miEditableObject::IsEdgeMouseHover(miSelectionFrust* sf) {
	auto current_edge = m_mesh->m_first_edge;
	if (!current_edge)
		return false;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

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
	auto current_vertex = m_mesh->m_first_vertex;
	if (!current_vertex)
		return false;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

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
	auto current_vertex = m_mesh->m_first_vertex;
	if (!current_vertex)
		return;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

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
	auto current_edge = m_mesh->m_first_edge;
	if (!current_edge)
		return;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

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
	auto current_polygon = m_mesh->m_first_polygon;
	if (!current_polygon)
		return;

	yyRay r;
	r.m_origin = sf->m_data.m_BackC;
	r.m_end = sf->m_data.m_FrontC;
	r.update();
	
	if (!m_aabbTransformed.rayTest(r))
		return;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

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

	//if (m_isSelected)
	//	_updateVertsForTransformArray(em);
	// App will call miSceneObject::OnSelect
}

void miEditableObject::_selectPolygons_rectangle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	auto current_edge = m_mesh->m_first_edge;
	if (!current_edge)
		return;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

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
		OnSelect(em);
}

void miEditableObject::_selectEdges_rectangle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	auto current_edge = m_mesh->m_first_edge;
	if (!current_edge)
		return;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

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

		OnSelect(em);
	}
}
void miEditableObject::_selectVerts_rectangle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	auto current_vertex = m_mesh->m_first_vertex;
	if (!current_vertex)
		return;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

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
		OnSelect(em);
}

void miEditableObject::Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	miSceneObject::Select(em, km, sf);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
		if(m_isSelected) _selectVerts_rectangle(em, km, sf);
		break;
	case miEditMode::Edge:
		if (m_isSelected) _selectEdges_rectangle(em, km, sf);
		break;
	case miEditMode::Polygon:
		if (m_isSelected) _selectPolygons_rectangle(em, km, sf);
		break;
	}
	if (m_isSelected)
		_updateVertsForTransformArray(em);
}

void miEditableObject::_selectAllPolygons() {
	auto current_polygon = m_mesh->m_first_polygon;
	if (!current_polygon)
		return;
	auto last_polygon = current_polygon->m_left;
	while (true) {
		current_polygon->m_flags |= current_polygon->flag_isSelected;
		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}
}
void miEditableObject::_selectAllEdges() {
	auto current_edge = m_mesh->m_first_edge;
	if (!current_edge)
		return;
	auto last_edge = current_edge->m_left;
	while (true) {
		current_edge->m_flags |= current_edge->flag_isSelected;
		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
}
void miEditableObject::_selectAllVerts() {
	auto current_vertex = m_mesh->m_first_vertex;
	if (!current_vertex)
		return;
	auto last_vertex = current_vertex->m_left;
	while (true) {
		current_vertex->m_flags |= current_vertex->flag_isSelected;
		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
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
		if (m_isSelected) _selectAllVerts();
		break;
	case miEditMode::Edge:
		if (m_isSelected) _selectAllEdges();
		break;
	case miEditMode::Polygon:
		if (m_isSelected) _selectAllPolygons();
		break;
	}
	if (m_isSelected)
		_updateVertsForTransformArray(em);
	OnSelect(em);
}

void miEditableObject::_deselectAllPolygons() {
	auto current_polygon = m_mesh->m_first_polygon;
	if (!current_polygon)
		return;
	auto last_polygon = current_polygon->m_left;
	while (true) {
		if (current_polygon->m_flags & current_polygon->flag_isSelected)
			current_polygon->m_flags ^= current_polygon->flag_isSelected;
		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}
}
void miEditableObject::_deselectAllEdges() {
	auto current_edge = m_mesh->m_first_edge;
	if (!current_edge)
		return;
	auto last_edge = current_edge->m_left;
	while (true) {
		if(current_edge->m_flags & current_edge->flag_isSelected)
			current_edge->m_flags ^= current_edge->flag_isSelected;
		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
}
void miEditableObject::_deselectAllVerts() {
	auto current_vertex = m_mesh->m_first_vertex;
	if (!current_vertex)
		return;
	auto last_vertex = current_vertex->m_left;
	while (true) {
		if(current_vertex->m_flags & current_vertex->flag_isSelected)
			current_vertex->m_flags ^= current_vertex->flag_isSelected;
		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
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
		if (m_isSelected) _deselectAllVerts();
		break;
	case miEditMode::Edge:
		if (m_isSelected) _deselectAllEdges();
		break;
	case miEditMode::Polygon:
		if (m_isSelected) _deselectAllPolygons();
		break;
	}
	//_updateVertsForTransformArray(em);
	if (m_isSelected)
		m_vertsForTransform.clear();
	OnSelect(em);
}

void miEditableObject::_selectInvertPolygons() {
	auto current_polygon = m_mesh->m_first_polygon;
	if (!current_polygon)
		return;
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
}
void miEditableObject::_selectInvertEdges() {
	auto current_edge = m_mesh->m_first_edge;
	if (!current_edge)
		return;
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
}
void miEditableObject::_selectInvertVerts() {
	auto current_vertex = m_mesh->m_first_vertex;
	if (!current_vertex)
		return;
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
	if (m_isSelected)
		_updateVertsForTransformArray(em);
	OnSelect(em);
}

bool miEditableObject::IsVertexSelected() {
	auto c = m_mesh->m_first_vertex;
	if (!c)
		return false;
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
	if (!c)
		return false;
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
	if (!c)
		return false;
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

void miEditableObject::_transformMove(const v3f& move_delta, bool isCancel) {
	for (u32 i = 0; i < m_vertsForTransform.m_size; ++i)
	{
		if (isCancel)
			m_vertsForTransform.m_data[i].m_first->m_position = m_vertsForTransform.m_data[i].m_second;
		else
			m_vertsForTransform.m_data[i].m_first->m_position += move_delta;
	}
}

void miEditableObject::_transformScale(Mat4* S, const v3f& selectionCenter, bool isCancel) {
	auto M = *S;
	Mat4 W = m_rotationOnlyMatrix;

	auto WT = W;
	WT.transpose();

	auto WI = W;
	WI.invert();

	M = M * W;
	M = WT * M;

	auto C = selectionCenter;
	auto W2 = m_worldMatrix;
	W2.invert();
	C = math::mul(C, W2);

	for (u32 i = 0; i < m_vertsForTransform.m_size; ++i)
	{
		if (isCancel)
			m_vertsForTransform.m_data[i].m_first->m_position = m_vertsForTransform.m_data[i].m_second;
		else
			m_vertsForTransform.m_data[i].m_first->m_position = math::mulBasis(m_vertsForTransform.m_data[i].m_second - C, M) + C;
	}
}

void miEditableObject::_transformRotate(Mat4* R, const v3f& selectionCenter, bool isCancel) {
	_transformScale(R, selectionCenter, isCancel);
}

void miEditableObject::OnTransformEdge(
	miTransformMode tm, 
	const v3f& move_delta, 
	Mat4* S,
	const v3f& C,
	bool isCancel) 
{
	switch (tm)
	{
	case miTransformMode::Move:
		_transformMove(move_delta, isCancel);
		break;
	case miTransformMode::Scale:
		_transformScale(S, C, isCancel);
		break;
	case miTransformMode::Rotate:
		_transformRotate(S, C, isCancel);
		break;
	case miTransformMode::NoTransform:
	default:
		break;
	}
	_callVisualObjectOnTransform();
}

void miEditableObject::OnTransformVertex(
	miTransformMode tm, 
	const v3f& move_delta, 
	Mat4* S,
	const v3f& C,
	bool isCancel) 
{
	switch (tm)
	{
	case miTransformMode::Move:
		_transformMove(move_delta, isCancel);
	break;
	case miTransformMode::Scale:
		_transformScale(S, C, isCancel);
	break;
	case miTransformMode::Rotate:
		_transformRotate(S, C, isCancel);
		break;
	case miTransformMode::NoTransform:
	default:
		break;
	}
	_callVisualObjectOnTransform();
}

void miEditableObject::_callVisualObjectOnTransform() {
	auto voc = GetVisualObjectCount();
	for (int i = 0; i < voc; ++i)
	{
		auto vo = GetVisualObject(i);
		vo->OnTransform();
	}
}

void miEditableObject::OnTransformPolygon(
	miTransformMode tm, 
	const v3f& move_delta, 
	Mat4* S,
	const v3f& C,
	bool isCancel) 
{
	switch (tm)
	{
	case miTransformMode::Move:
		_transformMove(move_delta, isCancel);
		break;
	case miTransformMode::Scale:
		_transformScale(S, C, isCancel);
		break;
	case miTransformMode::Rotate:
		_transformRotate(S, C, isCancel);
		break;
	case miTransformMode::NoTransform:
	default:
		break;
	}
	_callVisualObjectOnTransform();
}

void miEditableObject::OnSelect(miEditMode em) {
	_updateVertsForTransformArray(em);
	RebuildVisualObjects(true);
}

void miEditableObject::RebuildVisualObjects(bool onlyEditMode) {
	if (onlyEditMode)
	{
		auto em = g_app->GetEditMode();
		switch (em)
		{
		default:
		case miEditMode::Object:
			break;
		case miEditMode::Vertex:
			m_visualObject_vertex->CreateNewGPUModels(m_mesh);
			break;
		case miEditMode::Edge:
			m_visualObject_edge->CreateNewGPUModels(m_mesh);
			break;
		case miEditMode::Polygon:
			m_visualObject_polygon->CreateNewGPUModels(m_mesh);
			break;
		}
	}
	else
	{
		auto voc = GetVisualObjectCount();
		for (int i = 0; i < voc; ++i)
		{

			GetVisualObject(i)->CreateNewGPUModels(m_mesh);
		}
	}
}

void miEditableObject::OnSetEditMode(miEditMode em) {
	_updateVertsForTransformArray(em);
}

void miEditableObject::OnEndTransform(miEditMode em) {
	_updateVertsForTransformArray(em);
}

void miEditableObject::_updateVertsForTransformArray(miEditMode em) {
	m_vertsForTransform.clear();
	switch (em)
	{
	case miEditMode::Vertex:
	{
		auto c = m_mesh->m_first_vertex;
		if (!c)
			return;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & c->flag_isSelected)
				m_vertsForTransform.push_back(miPair<miVertex*, v3f>(c, c->m_position));

			if (c == l)
				break;
			c = c->m_right;
		}
	}break;
	case miEditMode::Edge:
	{
		auto c = m_mesh->m_first_edge;
		if (!c)
			return;
		miBinarySearchTree<miPair<miVertex*, v3f>> bst;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miEdge::flag_isSelected)
			{
				bst.Add((uint64_t)c->m_vertex1, miPair<miVertex*, v3f>(c->m_vertex1, c->m_vertex1->m_position));
				bst.Add((uint64_t)c->m_vertex2, miPair<miVertex*, v3f>(c->m_vertex2, c->m_vertex2->m_position));
			}

			if (c == l)
				break;
			c = c->m_right;
		}
		bst.Get(&m_vertsForTransform);

	}break;
	case miEditMode::Polygon:
	{
		auto c = m_mesh->m_first_polygon;
		if (!c)
			return;
		miBinarySearchTree<miPair<miVertex*, v3f>> bst;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miPolygon::flag_isSelected)
			{
				auto cv = c->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					bst.Add((uint64_t)cv->m_data, miPair<miVertex*, v3f>(cv->m_data, cv->m_data->m_position));

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}

			if (c == l)
				break;
			c = c->m_right;
		}
		bst.Get(&m_vertsForTransform);
	}break;
	case miEditMode::Object:
	default:
		break;
	}
	printf("VERTS FOR TRANSFORM: %u\n", m_vertsForTransform.m_size);
}

void miEditableObject::ConnectVerts1() {
	miArray<miListNode<miVertex*>*> vertsForNewpolygon;

	/*for (s32 o = 0, osz = GetMeshCount(); o < osz; ++o)
	{
		auto mesh = GetMesh(o);*/
		auto mesh = m_mesh;
		auto cp = mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto np = cp->m_right;
			vertsForNewpolygon.clear();
			u32 vertexCount = 0;
			{// remove miVertex::flag_User1
				auto currentVertex = cp->m_verts.m_head;
				auto lastVertex = currentVertex->m_left;
				while (true) {
					if (currentVertex->m_data->m_flags & miVertex::flag_User1)
						currentVertex->m_data->m_flags ^= miVertex::flag_User1;
					++vertexCount;
					if (currentVertex == lastVertex)
						break;
					currentVertex = currentVertex->m_right;
				}
			}

			miPolygon* newPolygon = 0;
			bool isStarted = false;
			auto currentVertex = cp->m_verts.m_head;
			auto lastVertex = currentVertex->m_left;
			while (true) {
				if (currentVertex->m_data->m_flags & miVertex::flag_isSelected)
				{
					if (!isStarted) {
						isStarted = true;
						lastVertex = currentVertex->m_left;
					}
				}

				if (isStarted)
				{
					if ((currentVertex->m_data->m_flags & miVertex::flag_User1) == 0)
						vertsForNewpolygon.push_back(currentVertex);

					if (currentVertex->m_data->m_flags & miVertex::flag_isSelected)
					{
						if (vertsForNewpolygon.m_size > 2)
						{
							newPolygon = m_allocatorPolygon->Allocate();

							for (u32 i = 0, l = vertsForNewpolygon.m_size - 1; i < vertsForNewpolygon.m_size; ++i)
							{
								auto v = vertsForNewpolygon.m_data[i];

								if (i != 0 && i != l) {
									v->m_data->m_flags |= miVertex::flag_User1;
									--vertexCount;
								}

								newPolygon->m_verts.push_back(v->m_data);
								v->m_data->m_polygons.push_back(newPolygon);

								if (v == lastVertex)
									lastVertex = vertsForNewpolygon.m_data[0];
							}

							cp->m_left->m_right = newPolygon;
							newPolygon->m_left = cp->m_left;
							cp->m_left = newPolygon;
							newPolygon->m_right = cp;

							vertsForNewpolygon.clear();

							vertsForNewpolygon.push_back(currentVertex);

							if (vertexCount == 2)
								break;
						}
					}
				}

				if (currentVertex->m_right == lastVertex)
				{
					if (!isStarted)
						break;
					else if (isStarted && !newPolygon)
						break;
				}

				currentVertex = currentVertex->m_right;
			}

			if (newPolygon)
			{
				// need to delete old polygon
				DeletePolygon(cp);

				mesh->_delete_edges(m_allocatorEdge);
				mesh->CreateEdges(m_allocatorPolygon, m_allocatorEdge, m_allocatorVertex);
			}

			if (cp == lp)
				break;
			cp = np;
		}
	//}

	RebuildVisualObjects(false);
}

void miEditableObject::BreakVerts() {
	miArray<miVertex*> verts;
	verts.reserve(0x1000);
	{
		auto c = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miVertex::flag_isSelected)
			{
				auto p1 = c->m_polygons.m_head;
				auto p2 = c->m_polygons.m_head->m_left;
				if (p1 != p2)
					verts.push_back(c);
			}
			if (c == l)
				break;
			c = c->m_right;
		}
	}
	if (!verts.m_size)
		return;

	miArray<miPolygon*> removeThisPolygons;
	for (u32 i = 0; i < verts.m_size; ++i)
	{
		removeThisPolygons.clear();

		auto v = verts.m_data[i];
		auto cp = v->m_polygons.m_head;
		auto lp = cp->m_left;
		auto fp = cp;
		while (true)
		{
			if (cp != fp)
			{
				miVertex* newVertex = m_allocatorVertex->Allocate();
				newVertex->CopyData(v);
				newVertex->m_left = v;
				newVertex->m_right = v->m_right;
				v->m_right->m_left = newVertex;
				v->m_right = newVertex;

				cp->m_data->m_verts.replace(v, newVertex);
				newVertex->m_polygons.push_back(cp->m_data);

				removeThisPolygons.push_back(cp->m_data);
			}

			if (cp == lp)
				break;
			cp = cp->m_right;
		}

		for (u32 o = 0; o < removeThisPolygons.m_size; ++o)
		{
			v->m_polygons.erase_first(removeThisPolygons.m_data[o]);
		}
	}

	m_mesh->_delete_edges(m_allocatorEdge);
	m_mesh->CreateEdges(m_allocatorPolygon, m_allocatorEdge, m_allocatorVertex);
	RebuildVisualObjects(false);
}