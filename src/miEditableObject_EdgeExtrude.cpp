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

	//miArray<miEdge*> selectedEdges;
	//selectedEdges.reserve(10);

	miBinarySearchTree<miListNode3<miVertex*, v2f, v3f>> verticesTree;

	bool needUpdate = false;

	auto c = m_mesh->m_first_polygon;
	auto l = c->m_left;
	while (true)
	{
		// go through vertices in right order
		auto cv = c->m_verts.m_head;
		auto lv = cv->m_left;
		while (true)
		{
			// take first and second vertices
			auto nv = cv->m_right;

			// find edge with this vertices
			miEdge* edge = 0;
			{
				auto v1 = cv->m_data1;
				auto v2 = nv->m_data1;
				if (v2 < v1)
				{
					v1 = nv->m_data1;
					v2 = cv->m_data1;
				}
				auto ce = c->m_edges.m_head;
				auto le = ce->m_left;
				while (true)
				{
					if (ce->m_data->m_vertex1 == v1
						&& ce->m_data->m_vertex2 == v2)
					{
						// found
						// it must be selected edge with only 1 polygon
						if (ce->m_data->m_flags & miEdge::flag_isSelected)
						{
							if (!ce->m_data->m_polygon1 || !ce->m_data->m_polygon2)
							{
								edge = ce->m_data;
								break;
							}
						}
					}
					if (ce == le)
						break;
					ce = ce->m_right;
				}
			}
			if (edge)
			{
				//printf("EDGE\n");
				// create polygon
				miPolygon* newPolygon = m_allocatorPolygon->Allocate();
				newPolygon->m_verts.push_back(nv->m_data1, nv->m_data2, nv->m_data3);
				newPolygon->m_verts.push_back(cv->m_data1, cv->m_data2, cv->m_data3);

				cv->m_data1->m_polygons.push_back(newPolygon);
				nv->m_data1->m_polygons.push_back(newPolygon);

				miPair<miVertex*, miVertex*> verticesPair;
				miListNode3<miVertex*, v2f, v3f> node;

				if (verticesTree.Get((uint64_t)cv->m_data1, node))
				{
					newPolygon->m_verts.push_back(node.m_data1, node.m_data2, node.m_data3);
					node.m_data1->m_polygons.push_back(newPolygon);
					verticesPair.m_first = node.m_data1;
									
				}
				else
				{
					miVertex* newVertex = m_allocatorVertex->Allocate();
					newVertex->CopyData(cv->m_data1);
					newVertex->m_polygons.push_back(newPolygon);
					auto v3 = newPolygon->m_verts.push_back(newVertex, cv->m_data2, cv->m_data3);
					m_mesh->_add_vertex_to_list(newVertex);
					verticesPair.m_first = newVertex;

					verticesTree.Add((uint64_t)cv->m_data1, *v3);
				}

				if (verticesTree.Get((uint64_t)nv->m_data1, node))
				{
					newPolygon->m_verts.push_back(node.m_data1, node.m_data2, node.m_data3);
					node.m_data1->m_polygons.push_back(newPolygon);
					verticesPair.m_second = node.m_data1;
				}
				else
				{
					miVertex* newVertex = m_allocatorVertex->Allocate();
					newVertex->CopyData(nv->m_data1);
					newVertex->m_polygons.push_back(newPolygon);
					auto v4 = newPolygon->m_verts.push_back(newVertex, nv->m_data2, nv->m_data3);
					m_mesh->_add_vertex_to_list(newVertex);
					verticesPair.m_second = newVertex;

					verticesTree.Add((uint64_t)nv->m_data1, *v4);
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

			if (cv == lv)
				break;
			cv = cv->m_right;
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