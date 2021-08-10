#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

#include <set>

extern miApplication * g_app;

void miEditableObject::PolygonDuplicate() {
	miArray<miPolygon*> newPolygons;
	newPolygons.reserve(0x1000);

	{
		miBinarySearchTree<miVertex*> newVerts;

		auto c = m_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miPolygon::flag_isSelected)
			{
				miPolygon* newPolygon = m_allocatorPolygon->Allocate();
				newPolygons.push_back(newPolygon);

				auto cv = c->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					miVertex* newVertex = 0;

					if (!newVerts.Get((uint64_t)cv->m_data.m_vertex, newVertex))
					{
						newVertex = m_allocatorVertex->Allocate();
						newVertex->m_position = cv->m_data.m_vertex->m_position;
						m_mesh->_add_vertex_to_list(newVertex);
						newVerts.Add((uint64_t)cv->m_data.m_vertex, newVertex);
					}

					newVertex->m_polygons.push_back(newPolygon);
					newPolygon->m_verts.push_back(miPolygon::_vertex_data(newVertex, cv->m_data.m_uv, cv->m_data.m_normal, 0));

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}
			if (c == l)
				break;
			c = c->m_right;
		}
	}

	if (!newPolygons.m_size)
		return;

	this->DeselectAll(g_app->m_editMode);

	for (u32 i = 0; i < newPolygons.m_size; ++i)
	{
		m_mesh->_add_polygon_to_list(newPolygons.m_data[i]);
		newPolygons.m_data[i]->m_flags |= miPolygon::flag_isSelected;
	}

	m_mesh->UpdateCounts();
	_updateModel();
	this->OnSelect(g_app->m_editMode);
	this->UpdateAabb();
	g_app->UpdateSelectionAabb();
	g_app->_callVisualObjectOnSelect();
	_updateModel();
}
