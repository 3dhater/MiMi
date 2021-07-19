#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

extern s32 g_SelectButtonID_More;
extern s32 g_SelectButtonID_Less;

void editableObjectGUI_selectButtons_onClick(s32 id) {
	if (g_app->m_selectedObjects.m_size != 1)
		return;

	auto object = g_app->m_selectedObjects.m_data[0];
	if (object->GetPlugin() != g_app->m_pluginForApp)
		return;

	if(object->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
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
						auto cp = cv->m_data1->m_polygons.m_head;
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
// error drawGroup...	g_app->OnSelect();
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

		printf("---------\n");
		printf("Vertex: %u\n", (u32)verts_in_frust.m_data[0]);
		u32 ecount = 0;
		auto ce = verts_in_frust.m_data[0]->m_edges.m_head;
		auto le = ce->m_left;
		while (true)
		{
			++ecount;
			printf("E: %u\n", (u32)ce->m_data);
			if (ce == le)
				break;
			ce = ce->m_right;
		}
		printf("%u edges in vertex\n", ecount);
		
		auto cp = verts_in_frust.m_data[0]->m_polygons.m_head;
		auto lp = cp->m_left;
		u32 pcount = 0;
		while (true)
		{
			++pcount;
			printf("P: %u\n", (u32)cp->m_data);
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
		printf("%u polygons in vertex\n", pcount);
		printf("---------\n");
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
		
		printf("---------\n");
		printf("Edge: %u\n", (u32)edges_in_frust.m_data[0]);
		printf("Polygons in edge: %u  and %u\n", edges_in_frust.m_data[0]->m_polygon1, edges_in_frust.m_data[0]->m_polygon2);
		printf("Vertices in edge: %u  and %u\n", edges_in_frust.m_data[0]->m_vertex1, edges_in_frust.m_data[0]->m_vertex2);
		printf("---------\n");
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
			tri.v1 = math::mul(first_vertex->m_data1->m_position, M)
				+ *this->GetGlobalPosition();
			tri.v2 = math::mul(next_vertex->m_data1->m_position, M)
				+ *this->GetGlobalPosition();
			tri.v3 = math::mul(next_vertex->m_right->m_data1->m_position, M)
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

		printf("---------\n");
		printf("Polygon: %u\n", (u32)polygons_in_frust.m_data[0].m_first);
		auto cv = polygons_in_frust.m_data[0].m_first->m_verts.m_head;
		auto lv = cv->m_left;
		u32 vcount = 0;
		while (true)
		{
			++vcount;
			printf("V: %u\n", (u32)cv->m_data1);
			if (cv == lv)
				break;
			cv = cv->m_right;
		}

		auto ce = polygons_in_frust.m_data[0].m_first->m_edges.m_head;
		auto le = ce->m_left;
		u32 ecount = 0;
		while (true)
		{
			++ecount;
			printf("E: %u\n", (u32)ce->m_data);
			if (ce == le)
				break;
			ce = ce->m_right;
		}
		printf("%u vertices in polygon\n", vcount);
		printf("%u edges in polygon\n", ecount);
		printf("---------\n");
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
	if (sf->m_data.m_TN == v3f())
		return;

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

	/*printf("SF TN %f %f %f\n", sf->m_data.m_TN.x, sf->m_data.m_TN.y, sf->m_data.m_TN.z);
	printf("SF BN %f %f %f\n", sf->m_data.m_BN.x, sf->m_data.m_BN.y, sf->m_data.m_BN.z);
	printf("SF RN %f %f %f\n", sf->m_data.m_RN.x, sf->m_data.m_RN.y, sf->m_data.m_RN.z);
	printf("SF LN %f %f %f\n", sf->m_data.m_LN.x, sf->m_data.m_LN.y, sf->m_data.m_LN.z);
	printf("\n");*/
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

void miEditableObject::OnSelect(miEditMode em) {
	_updateVertsForTransformArray(em);
	RebuildVisualObjects(true);
}

