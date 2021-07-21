#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void miEditableObject::EdgeExtrude()
{
	miArray<miPair<miVertex*, miVertex*>> newEdgesVertices;


	miArray<miEdge*> selectedEdges;
	selectedEdges.reserve(10);

	miBinarySearchTree<miListNode2<miVertex*, v2f>> verticesTree;

	bool needUpdate = false;

	auto c = m_mesh->m_first_polygon;
	auto l = c->m_left;
	while (true)
	{
		selectedEdges.clear();

		auto ce = c->m_edges.m_head;
		auto le = ce->m_left;
		while(true)
		{
			if (ce->m_data->m_flags & miEdge::flag_isSelected)
			{
				if (!ce->m_data->m_polygon1 || !ce->m_data->m_polygon2)
				{
					if (ce->m_data->m_flags & miEdge::flag_User1)
						ce->m_data->m_flags ^= miEdge::flag_User1;

					selectedEdges.push_back(ce->m_data);
				}
			}
			if (ce == le)
				break;
			ce = ce->m_right;
		}

		for (u32 i = 0; i < selectedEdges.m_size; ++i)
		{
			auto e = selectedEdges.m_data[i];
			if ((e->m_flags & miEdge::flag_User1) == 0)
			{
				e->m_flags |= miEdge::flag_User1;
				// create polygon

				// find vertices in right order
				miListNode2<miVertex*, v2f>* v1 = 0;
				miListNode2<miVertex*, v2f>* v2 = 0;
				auto cv = c->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					if (!v1)
					{
						if (cv->m_data1 == e->m_vertex1 || cv->m_data1 == e->m_vertex2)
							v1 = cv;
					}
					else if (!v2)
					{
						if (cv->m_data1 == e->m_vertex1 || cv->m_data1 == e->m_vertex2)
						{
							v2 = cv;
							break;
						}
					}
					if (cv == lv)
						break;
					cv = cv->m_right;
				}

				miPolygon* newPolygon = m_allocatorPolygon->Allocate();
				if (newPolygon)
				{
					newPolygon->m_verts.push_back(v2->m_data1, v2->m_data2);
					newPolygon->m_verts.push_back(v1->m_data1, v1->m_data2);
					
					v1->m_data1->m_polygons.push_back(newPolygon);
					v2->m_data1->m_polygons.push_back(newPolygon);

					miPair<miVertex*, miVertex*> verticesPair;

					miListNode2<miVertex*, v2f> node;
					if (verticesTree.Get((uint64_t)v1->m_data1, node))
					{
						newPolygon->m_verts.push_back(node.m_data1, node.m_data2);
						node.m_data1->m_polygons.push_back(newPolygon);
						verticesPair.m_first = node.m_data1;
					}
					else
					{
						miVertex* newVertex = m_allocatorVertex->Allocate();
						newVertex->CopyData(v1->m_data1);
						newVertex->m_polygons.push_back(newPolygon);
						newPolygon->m_verts.push_back(newVertex, v1->m_data2);
						m_mesh->_add_vertex_to_list(newVertex);
						verticesPair.m_first = newVertex;

						verticesTree.Add((uint64_t)v1->m_data1, *newPolygon->m_verts.find(newVertex));
					}

					if (verticesTree.Get((uint64_t)v2->m_data1, node))
					{
						newPolygon->m_verts.push_back(node.m_data1, node.m_data2);
						node.m_data1->m_polygons.push_back(newPolygon);
						verticesPair.m_second = node.m_data1;
					}
					else
					{
						miVertex* newVertex = m_allocatorVertex->Allocate();
						newVertex->CopyData(v2->m_data1);
						newVertex->m_polygons.push_back(newPolygon);
						newPolygon->m_verts.push_back(newVertex, v2->m_data2);
						m_mesh->_add_vertex_to_list(newVertex);
						verticesPair.m_second = newVertex;

						verticesTree.Add((uint64_t)v2->m_data1, *newPolygon->m_verts.find(newVertex));
					}

					if (verticesPair.m_second < verticesPair.m_first)
					{
						auto old = verticesPair.m_second;
						verticesPair.m_second = verticesPair.m_first;
						verticesPair.m_first = old;
					}

					newEdgesVertices.push_back(verticesPair);

					m_mesh->_add_polygon_to_list(newPolygon);
					needUpdate = true;
				}
			}
		}

		if (c == l)
			break;
		c = c->m_right;
	}

	if (needUpdate)
	{
		DeselectAll(g_app->m_editMode);
		_updateModel();
		for (u32 i = 0; i < newEdgesVertices.m_size; ++i)
		{
			auto pair = &newEdgesVertices.m_data[i];

			auto ce = m_mesh->m_first_edge;
			auto le = ce->m_left;
			while (true)
			{
				if (ce->m_vertex1 == pair->m_first
					&& ce->m_vertex2 == pair->m_second)
				{
					ce->m_flags |= miEdge::flag_isSelected;
					break;
				}
				if (ce == le)
					break;
				ce = ce->m_right;
			}
		}
		OnSelect(g_app->m_editMode);
	}
}