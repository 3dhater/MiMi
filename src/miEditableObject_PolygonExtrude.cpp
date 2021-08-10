#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

#include <set>

extern miApplication * g_app;

void miEditableObject::PolygonExtrude() {
	// Collect all edges that have only 1 selected polygon
	std::set<miEdge*> edges;
	{
		auto c = m_mesh->m_first_edge;
		auto l = c->m_left;
		while (true)
		{
			auto p1 = c->m_polygon1;
			auto p2 = c->m_polygon2;

			miPolygon* selectedPolygon1 = 0;
			miPolygon* selectedPolygon2 = 0;

			if (p1)
			{
				if (p1->IsSelected())
					selectedPolygon1 = p1;
			}

			if (p2)
			{
				if (p2->IsSelected())
					selectedPolygon2 = p2;
			}

			if ((selectedPolygon1 && !selectedPolygon2) || (!selectedPolygon1 && selectedPolygon2))
			{
				edges.insert(c);
			}

			if (c == l)
				break;
			c = c->m_right;
		}
	}

	// Create new polygons.
	struct HelpStruct1
	{
		// Every unique vertex from edges must have 2 vertices
		miVertex* m_v1; // for selected polygon
		miVertex* m_v2; // for not selected polygon if exist
	};
	miBinarySearchTree<HelpStruct1> newVertices; // key - vertex from edge
	std::set<miVertex*> edgeVertices;
	for (auto e : edges)
	{
		miPolygon* newPolygon = m_allocatorPolygon->Allocate();
		m_mesh->_add_polygon_to_list(newPolygon);

		auto hs1 = newVertices.GetPtr((uint64_t)e->m_vertex1);
		auto hs2 = newVertices.GetPtr((uint64_t)e->m_vertex2);

		auto someVNode = e->m_vertex1->m_polygons.m_head->m_data->m_verts.m_head;

		if(!hs1)
		{
			HelpStruct1 hs;
			hs.m_v1 = m_allocatorVertex->Allocate();
			hs.m_v2 = m_allocatorVertex->Allocate();

			hs.m_v1->m_position = e->m_vertex1->m_position;
			hs.m_v2->m_position = e->m_vertex1->m_position;

			m_mesh->_add_vertex_to_list(hs.m_v1);
			m_mesh->_add_vertex_to_list(hs.m_v2);

			newVertices.Add((uint64_t)e->m_vertex1, hs);
			hs1 = newVertices.GetPtr((uint64_t)e->m_vertex1);
		}

		if (!hs2)
		{
			HelpStruct1 hs;
			hs.m_v1 = m_allocatorVertex->Allocate();
			hs.m_v2 = m_allocatorVertex->Allocate();

			hs.m_v1->m_position = e->m_vertex2->m_position;
			hs.m_v2->m_position = e->m_vertex2->m_position;

			m_mesh->_add_vertex_to_list(hs.m_v1);
			m_mesh->_add_vertex_to_list(hs.m_v2);
		
			newVertices.Add((uint64_t)e->m_vertex2, hs);
			hs2 = newVertices.GetPtr((uint64_t)e->m_vertex2);
		}

		auto selPol = e->m_polygon1;
		if(!selPol)
			selPol = e->m_polygon2;
		else
		{
			if(selPol->IsSelected() == false)
				selPol = e->m_polygon2;
		}
		auto vNode1 = selPol->m_verts.find(e->m_vertex1);
		auto vNode2 = selPol->m_verts.find(e->m_vertex2);

		if (vNode1->m_right == vNode2)
		{
			newPolygon->m_verts.push_back(miPolygon::_vertex_data(hs1->m_v1, someVNode->m_data.m_uv, someVNode->m_data.m_normal, 0));
			newPolygon->m_verts.push_back(miPolygon::_vertex_data(hs1->m_v2, someVNode->m_data.m_uv, someVNode->m_data.m_normal, 0));
			newPolygon->m_verts.push_back(miPolygon::_vertex_data(hs2->m_v2, someVNode->m_data.m_uv, someVNode->m_data.m_normal, 0));
			newPolygon->m_verts.push_back(miPolygon::_vertex_data(hs2->m_v1, someVNode->m_data.m_uv, someVNode->m_data.m_normal, 0));
		}
		else
		{
			newPolygon->m_verts.push_back(miPolygon::_vertex_data(hs1->m_v2, someVNode->m_data.m_uv, someVNode->m_data.m_normal, 0));
			newPolygon->m_verts.push_back(miPolygon::_vertex_data(hs1->m_v1, someVNode->m_data.m_uv, someVNode->m_data.m_normal, 0));
			newPolygon->m_verts.push_back(miPolygon::_vertex_data(hs2->m_v1, someVNode->m_data.m_uv, someVNode->m_data.m_normal, 0));
			newPolygon->m_verts.push_back(miPolygon::_vertex_data(hs2->m_v2, someVNode->m_data.m_uv, someVNode->m_data.m_normal, 0));
		}

		hs1->m_v1->m_polygons.push_back(newPolygon);
		hs1->m_v2->m_polygons.push_back(newPolygon);
		hs2->m_v1->m_polygons.push_back(newPolygon);
		hs2->m_v2->m_polygons.push_back(newPolygon);

		// I need to replace old vertex on new in every polygon that have vertex from edge
		// Better to collect all edge vertices
		edgeVertices.insert(e->m_vertex1);
		edgeVertices.insert(e->m_vertex2);
	}

	// now replace
	for (auto v : edgeVertices)
	{
		auto hs = newVertices.GetPtr((uint64_t)v);
		
		auto cp = v->m_polygons.m_head;
		auto lp = cp->m_left;
		while (true)
		{
			if (cp->m_data->IsSelected())
			{
				auto vn = cp->m_data->m_verts.find(v);
				if (vn)
				{
					vn->m_data.m_vertex = hs->m_v1;
					if (!hs->m_v1->m_polygons.find(cp->m_data))
						hs->m_v1->m_polygons.push_back(cp->m_data);
				}
			}
			else
			{
				auto vn = cp->m_data->m_verts.find(v);
				if (vn)
				{
					vn->m_data.m_vertex = hs->m_v2;
					if (!hs->m_v2->m_polygons.find(cp->m_data))
						hs->m_v2->m_polygons.push_back(cp->m_data);
				}
			}

			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}
	for (auto v : edgeVertices)
	{
		v->~miVertex();
		m_allocatorVertex->Deallocate(v);
		m_mesh->_remove_vertex_from_list(v);
	}

	m_mesh->UpdateCounts();
	_updateModel();
	this->OnSelect(g_app->m_editMode);
	g_app->_callVisualObjectOnSelect();
}
