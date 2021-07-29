#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

#include <set>

extern miApplication * g_app;

void miEditableObject::PolygonDetachAsElement() {
	std::set<miPolygon*> polygons;
	{
		auto c = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			bool haveSelected = false;
			bool haveNotSelected = false;

			auto cp = c->m_polygons.m_head;
			auto lp = cp->m_left;
			while (true)
			{
				if (cp->m_data->m_flags & miPolygon::flag_isSelected)
					haveSelected = true;
				else
					haveNotSelected = true;

				if (haveNotSelected && haveSelected)
				{
					if (cp->m_data->m_flags & miPolygon::flag_isSelected)
						break;
				}

				if (cp == lp)
					break;
				cp = cp->m_right;
			}

			if (haveNotSelected && haveSelected)
			{
				cp = c->m_polygons.m_head;
				lp = cp->m_left;
				while (true)
				{
					if (cp->m_data->m_flags & miPolygon::flag_isSelected)
						polygons.insert(cp->m_data);
					if (cp == lp)
						break;
					cp = cp->m_right;
				}
			}

			if (c == l)
				break;
			c = c->m_right;
		}
	}
	for (auto p : polygons)
	{
		//printf("P");
		std::set<miVertex*> targetVertices;

		auto c = p->m_verts.m_head;
		auto l = c->m_left;
		while (true)
		{
			auto cp = c->m_data1->m_polygons.m_head;
			auto lp = cp->m_left;
			while (true)
			{
				if ((cp->m_data->m_flags & miPolygon::flag_isSelected)==0)
				{
					targetVertices.insert(c->m_data1);
					break;
				}

				if (cp == lp)
					break;
				cp = cp->m_right;
			}
			if (c == l)
				break;
			c = c->m_right;
		}

		for (auto o : targetVertices)
		{
			auto newVertex = m_allocatorVertex->Allocate();
			m_mesh->_add_vertex_to_list(newVertex);
			newVertex->m_position = o->m_position;

			if (!newVertex->m_polygons.find(p))
				newVertex->m_polygons.push_back(p);

			o->m_polygons.erase_first(p);

			auto vNode = p->FindVertex(o);
			vNode->m_data1 = newVertex;
		}
	}

	_updateModel();
	this->OnSelect(g_app->m_editMode);
}

void miEditableObject::PolygonDetachAsObject() {
	_updateModel();
}