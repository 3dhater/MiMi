#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void editableObjectGUI_connectEdge_onClick(s32) {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->EdgeConnect();
}

void miEditableObject::EdgeConnect()
{
	miBinarySearchTree<miVertex*> newVertices;

	// create new vertices
	// select them
	// call VertexConnect

	// I need to collect all selected edges, where I will create new vertices
	miArray<miEdge*> edges;
	edges.reserve(0x1000);
	{
		auto ce = m_mesh->m_first_edge;
		auto le = ce->m_left;
		while (true)
		{
			if (ce->m_flags & miEdge::flag_isSelected)
			{
				// I need to check if some polygon have another selected edge
				bool good = false;
				if (ce->m_polygon1)
				{
					auto pce = ce->m_polygon1->m_edges.m_head;
					auto ple = pce->m_left;
					while (true)
					{
						if (pce->m_data != ce && (pce->m_data->m_flags & miEdge::flag_isSelected))
						{
							good = true;
							break;
						}
						if (pce == ple)
							break;
						pce = pce->m_right;
					}
				}
				if (ce->m_polygon2 && !good)
				{
					auto pce = ce->m_polygon2->m_edges.m_head;
					auto ple = pce->m_left;
					while (true)
					{
						if (pce->m_data != ce && (pce->m_data->m_flags & miEdge::flag_isSelected))
						{
							good = true;
							break;
						}
						if (pce == ple)
							break;
						pce = pce->m_right;
					}
				}
				if (good)
				{
					edges.push_back(ce);
				}
			}
			if (ce == le)
				break;
			ce = ce->m_right;
		}
	}

	// deselect vertices
	// for vertexConnect
	{
		auto c = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miVertex::flag_isSelected)
				c->m_flags ^= miVertex::flag_isSelected;
			if (c == l)
				break;
			c = c->m_right;
		}
	}

	// create new vertices
	for (u32 i = 0; i < edges.m_size; ++i)
	{
		auto e = edges.m_data[i];

		miVertex* newVertex = m_allocatorVertex->Allocate();
		newVertices.Add((uint64_t)newVertex,newVertex);

		newVertex->m_flags |= miVertex::flag_isSelected; // for vertexConnect
		newVertex->m_position = e->m_vertex1->m_position + e->m_vertex2->m_position;
		newVertex->m_position *= 0.5f;

		m_mesh->_add_vertex_to_list(newVertex);

		// I need to put this vertex into polygons in right place
		if (e->m_polygon1)
		{
			// I can find right place when I go through vertex list
			auto cv = e->m_polygon1->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				auto nv = cv->m_right;
				
				// I need to compare vertices with edge vertices to find current edge
				auto v1 = cv->m_data.m_vertex;
				auto v2 = nv->m_data.m_vertex;
				if (v2 < v1)
				{
					v1 = nv->m_data.m_vertex;
					v2 = cv->m_data.m_vertex;
				}

				if (e->m_vertex1 == v1 && e->m_vertex2 == v2)
				{
					v2f uv;
					uv.x = cv->m_data.m_uv.x + nv->m_data.m_uv.x;
					uv.y = cv->m_data.m_uv.y + nv->m_data.m_uv.y;
					uv *= 0.5f;

					e->m_polygon1->m_verts.insert_after(cv->m_data, miPolygon::_vertex_data(newVertex, uv, cv->m_data.m_normal, 0));
					break;
				}

				if (cv == lv)
					break;
				cv = cv->m_right;
			}

			newVertex->m_polygons.push_back(e->m_polygon1);
		}

		if (e->m_polygon2)
		{
			auto cv = e->m_polygon2->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				auto nv = cv->m_right;
				auto v1 = cv->m_data.m_vertex;
				auto v2 = nv->m_data.m_vertex;
				if (v2 < v1)
				{
					v1 = nv->m_data.m_vertex;
					v2 = cv->m_data.m_vertex;
				}

				if (e->m_vertex1 == v1 && e->m_vertex2 == v2)
				{
					v2f uv;
					uv.x = cv->m_data.m_uv.x + nv->m_data.m_uv.x;
					uv.y = cv->m_data.m_uv.y + nv->m_data.m_uv.y;
					uv *= 0.5f;
					e->m_polygon2->m_verts.insert_after(cv->m_data, miPolygon::_vertex_data(newVertex, uv, cv->m_data.m_normal, 0));
					break;
				}

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			newVertex->m_polygons.push_back(e->m_polygon2);
		}
	}

	if (edges.m_size)
	{
		VertexConnect();
		DeselectAll(g_app->m_editMode);
		m_mesh->UpdateCounts();
		_updateModel();

		// select new edges
		auto ce = m_mesh->m_first_edge;
		auto le = ce->m_left;
		while (true)
		{
			miVertex* v1 = 0;
			miVertex* v2 = 0;

			if (newVertices.Get((uint64_t)ce->m_vertex1, v1))
			{
				if (newVertices.Get((uint64_t)ce->m_vertex2, v2))
				{
					ce->m_flags |= miEdge::flag_isSelected;
				}
			}


			if (ce == le)
				break;
			ce = ce->m_right;
		}
		OnSelect(g_app->m_editMode);
	}
}