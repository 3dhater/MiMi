#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void editableObjectGUI_bridgeEdge_onClick(s32) {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->EdgeBridge();
}

void miEditableObject::EdgeBridge()
{
	// Bridge can be only if selected 2 groups of edges
	// Group is 1 or more selected edges, connected together
	// If selected more groups - return;
	// In future this function can be used for PolygonBridge
	// In future I will add other EdgeBridge that works with one group 
	//     - sometimes its really need

	miArray<miEdge*> selectedEdges;
	{
		auto ce = m_mesh->m_first_edge;
		auto le = ce->m_left;
		while (true)
		{
			if ((ce->m_flags & miEdge::flag_isSelected)
				&& (!ce->m_polygon1 || !ce->m_polygon2))
			{
				// only edge with one polygon
				selectedEdges.push_back(ce);
			}
			if (ce == le)
				break;
			ce = ce->m_right;
		}
	}

	if (!selectedEdges.m_size)
		return;

	printf("Edge bridge\n");

	miList<miEdge*> group1;
	miList<miEdge*> group2;

	miBinarySearchTree<miEdge*> alreadyInGroup;

	u32 first_group_count = 0;
	u32 second_group_count = 0;
	// find group1
	{
		// first edge for first group
		auto e_base = selectedEdges.m_data[0];
		group1.push_back(e_base);
		first_group_count++;
		alreadyInGroup.Add((uint64_t)e_base, e_base); // not necessary

		// next, find other edges connected with first group;
		// find using vertex1
		auto e_last = e_base;
		auto e_last_base_vertex = e_last->m_vertex1;
	begin1:;
		for (u32 i = 1; i < selectedEdges.m_size; ++i)
		{
			auto e = selectedEdges.m_data[i];

			// if this edge not in group
			if (alreadyInGroup.Get((uint64_t)e, e) == false)
			{
				if (e->m_vertex1 == e_last_base_vertex || e->m_vertex2 == e_last_base_vertex)
				{
					// connected
					group1.push_back(e);
					first_group_count++;
					e_last = e;
					if (e->m_vertex1 == e_last_base_vertex)
					{
						e_last_base_vertex = e->m_vertex2;
					}
					else
					{
						e_last_base_vertex = e->m_vertex1;
					}
					alreadyInGroup.Add((uint64_t)e, e);
					goto begin1;
				}
			}
		}

		// using m_vertex2
		e_last = e_base;
		e_last_base_vertex = e_last->m_vertex2;
	begin2:;
		for (u32 i = 1; i < selectedEdges.m_size; ++i)
		{
			auto e = selectedEdges.m_data[i];

			// if this edge not in group
			if (alreadyInGroup.Get((uint64_t)e, e) == false)
			{
				if (e->m_vertex1 == e_last_base_vertex || e->m_vertex2 == e_last_base_vertex)
				{
					// connected
					group1.push_front(e);
					first_group_count++;
					e_last = e;
					if (e->m_vertex1 == e_last_base_vertex)
					{
						e_last_base_vertex = e->m_vertex2;
					}
					else
					{
						e_last_base_vertex = e->m_vertex1;
					}
					alreadyInGroup.Add((uint64_t)e, e);
					goto begin2;
				}
			}
		}
	}

	// find group2
	{
		// now e_base must be first edge not in alreadyInGroup
		miEdge * e_base = 0;
		for (u32 i = 0; i < selectedEdges.m_size; ++i)
		{
			auto e = selectedEdges.m_data[i];
			if (alreadyInGroup.Get((uint64_t)e, e) == false)
			{
				e_base = e;
				break;
			}
		}
		if (!e_base)
			return;
		group2.push_back(e_base);
		second_group_count++;
		alreadyInGroup.Add((uint64_t)e_base, e_base); // not necessary

		auto e_last = e_base;
		auto e_last_base_vertex = e_last->m_vertex1;
	begin3:;
		for (u32 i = 1; i < selectedEdges.m_size; ++i)
		{
			auto e = selectedEdges.m_data[i];

			if (alreadyInGroup.Get((uint64_t)e, e) == false)
			{
				if (e->m_vertex1 == e_last_base_vertex || e->m_vertex2 == e_last_base_vertex)
				{
					// connected
					group2.push_back(e);
					second_group_count++;
					e_last = e;
					if (e->m_vertex1 == e_last_base_vertex)
					{
						e_last_base_vertex = e->m_vertex2;
					}
					else
					{
						e_last_base_vertex = e->m_vertex1;
					}
					alreadyInGroup.Add((uint64_t)e, e);
					goto begin3;
				}
			}
		}

		e_last = e_base;
		e_last_base_vertex = e_last->m_vertex2;
	begin4:;
		for (u32 i = 1; i < selectedEdges.m_size; ++i)
		{
			auto e = selectedEdges.m_data[i];

			if (alreadyInGroup.Get((uint64_t)e, e) == false)
			{
				if (e->m_vertex1 == e_last_base_vertex || e->m_vertex2 == e_last_base_vertex)
				{
					group2.push_front(e);
					second_group_count++;
					e_last = e;
					if (e->m_vertex1 == e_last_base_vertex)
					{
						e_last_base_vertex = e->m_vertex2;
					}
					else
					{
						e_last_base_vertex = e->m_vertex1;
					}
					alreadyInGroup.Add((uint64_t)e, e);
					goto begin4;
				}
			}
		}
	}

	if (first_group_count + second_group_count < selectedEdges.m_size)
		return;

	//printf("EdgeBridge: %u %u\n", first_group_count, second_group_count);

	// now, first edge in one group must be near with first edge in other
	// if first edge in second group further than last edge, then flip elements in this group
	{
		v3f firstGroupEdgePosition = group1.m_head->m_data->m_vertex1->m_position
			+ group1.m_head->m_data->m_vertex2->m_position;
		firstGroupEdgePosition *= 0.5f;

		v3f secondGroupEdgePositionFirst = group2.m_head->m_data->m_vertex1->m_position
			+ group2.m_head->m_data->m_vertex2->m_position;
		secondGroupEdgePositionFirst *= 0.5f;

		v3f secondGroupEdgePositionLast = group2.m_head->m_left->m_data->m_vertex1->m_position
			+ group2.m_head->m_left->m_data->m_vertex2->m_position;
		secondGroupEdgePositionLast *= 0.5f;

		f32 d1 = firstGroupEdgePosition.distance(secondGroupEdgePositionFirst);
		f32 d2 = firstGroupEdgePosition.distance(secondGroupEdgePositionLast);

		if (d2 < d1)
		{
			// need flip/reverse this list
			group2.reverse();
		//	printf("Reverse\n");
		}
	}

	// create polygons
	// I need to use that group, that have lowest element count
	miList<miEdge*> * g1 = &group1;
	miList<miEdge*> * g2 = &group2;
	if (second_group_count < first_group_count)
	{
		g1 = &group2;
		g2 = &group1;
	}

	// take current edge in g1, and in g2, create polygon
	// create until g1 have edges
	// when g1 is empty, and g2 is not, create triangles using g2
	{
	begin_creating:;
		miEdge * g1_edge = 0; 
		if(g1->m_head)
			g1_edge = g1->m_head->m_data;

		miEdge * g2_edge = 0; 
		if (g2->m_head)
			g2_edge = g2->m_head->m_data;

		g1->pop_front();
		g2->pop_front();

		// create polygon
		if (g1_edge && g2_edge)
		{
			// need to find g1_edge->m_vertex1 and g1_edge->m_vertex2
			//  in polygon in right order
			miListNode3<miVertex*, v2f, v3f>* v1 = 0;
			miListNode3<miVertex*, v2f, v3f>* v2 = 0;
			miListNode3<miVertex*, v2f, v3f>* v3 = 0;
			miListNode3<miVertex*, v2f, v3f>* v4 = 0;

			{
				auto edge_polygon = g1_edge->m_polygon1;
				if (!edge_polygon)
					edge_polygon = g1_edge->m_polygon2;
				auto cv = edge_polygon->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					auto nv = cv->m_right;

					if (cv->m_data1 == g1_edge->m_vertex1 && nv->m_data1 == g1_edge->m_vertex2)
					{
						v1 = cv;
						v2 = nv;
						break;
					}
					else if (cv->m_data1 == g1_edge->m_vertex2 && nv->m_data1 == g1_edge->m_vertex1)
					{
						v1 = cv;
						v2 = nv;
						break;
					}

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}

			miPolygon* newPolygon = m_allocatorPolygon->Allocate();
			
			newPolygon->m_verts.push_back(v2->m_data1, v2->m_data2, v2->m_data3);
			newPolygon->m_verts.push_back(v1->m_data1, v1->m_data2, v1->m_data3);

			/*f32 d1 = v2->m_data1->m_position.distance(g2_edge->m_vertex1->m_position);
			f32 d2 = v2->m_data1->m_position.distance(g2_edge->m_vertex2->m_position);
			if (d1 < d2)
			{
				edge_polygon = g2_edge->m_polygon1;
				if (!edge_polygon)
					edge_polygon = g2_edge->m_polygon2;
				auto _v1 = edge_polygon->FindVertex(g2_edge->m_vertex1);
				auto _v2 = edge_polygon->FindVertex(g2_edge->m_vertex2);
				newPolygon->m_verts.push_back(g2_edge->m_vertex2, _v2->m_data2, _v2->m_data3);
				newPolygon->m_verts.push_back(g2_edge->m_vertex1, _v1->m_data2, _v1->m_data3);
			}
			else
			{
				edge_polygon = g2_edge->m_polygon1;
				if (!edge_polygon)
					edge_polygon = g2_edge->m_polygon2;
				auto _v1 = edge_polygon->FindVertex(g2_edge->m_vertex1);
				auto _v2 = edge_polygon->FindVertex(g2_edge->m_vertex2);

				newPolygon->m_verts.push_back(g2_edge->m_vertex1, _v1->m_data2, _v1->m_data3);
				newPolygon->m_verts.push_back(g2_edge->m_vertex2, _v2->m_data2, _v2->m_data3);
			}*/
			{
				auto edge_polygon = g2_edge->m_polygon1;
				if (!edge_polygon)
					edge_polygon = g2_edge->m_polygon2;
				auto cv = edge_polygon->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					auto nv = cv->m_right;

					if (cv->m_data1 == g2_edge->m_vertex1 && nv->m_data1 == g2_edge->m_vertex2)
					{
						v3 = cv;
						v4 = nv;
						break;
					}
					else if (cv->m_data1 == g2_edge->m_vertex2 && nv->m_data1 == g2_edge->m_vertex1)
					{
						v3 = cv;
						v4 = nv;
						break;
					}

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}

			newPolygon->m_verts.push_back(v4->m_data1, v4->m_data2, v4->m_data3);
			newPolygon->m_verts.push_back(v3->m_data1, v3->m_data2, v3->m_data3);

			g1_edge->m_vertex1->m_polygons.push_back(newPolygon);
			g1_edge->m_vertex2->m_polygons.push_back(newPolygon);
			g2_edge->m_vertex1->m_polygons.push_back(newPolygon);
			g2_edge->m_vertex2->m_polygons.push_back(newPolygon);

			m_mesh->_add_polygon_to_list(newPolygon);

			goto begin_creating;
		}
		else if (g2_edge)
		{
			goto begin_creating;
		}
	}

	DeselectAll(g_app->m_editMode);
	_updateModel();
}
