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

	miBinarySearchTree<miVertex*> newVerts;
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
			miVertex* newVertex = 0;// m_allocatorVertex->Allocate();

			if (!newVerts.Get((uint64_t)o, newVertex))
			{
				newVertex = m_allocatorVertex->Allocate();
				m_mesh->_add_vertex_to_list(newVertex);
				newVertex->m_position = o->m_position;
				newVerts.Add((uint64_t)o, newVertex);
			}

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
	PolygonDetachAsElement();

	std::set<miPolygon*> polygons;
	std::set<miVertex*> vertices;

	bool isPolygonSelected = false;
	{
		if (m_mesh->m_first_polygon)
		{
			auto c = m_mesh->m_first_polygon;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_flags & miPolygon::flag_isSelected)
				{
					isPolygonSelected = true;
					polygons.insert(c);

					auto cv = c->m_verts.m_head;
					auto lv = cv->m_left;
					while (true)
					{
						vertices.insert(cv->m_data1);
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
	}

	if (isPolygonSelected)
	{
		miEditableObject* newObject = (miEditableObject*)miMalloc(sizeof(miEditableObject));
		new(newObject)miEditableObject(g_app->m_sdk, 0);

		newObject->CopyBase(this);
		newObject->m_isSelected = false;
		newObject->m_plugin = m_plugin;
		newObject->m_mesh = miCreate<miMesh>();
		
		for (auto p : polygons)
		{
			m_mesh->_remove_polygon_from_list(p);
			newObject->m_mesh->_add_polygon_to_list(p);
		}

		for (auto v : vertices)
		{
			m_mesh->_remove_vertex_from_list(v);
			newObject->m_mesh->_add_vertex_to_list(v);
		}

		g_app->AddObjectToScene(newObject, this->m_name.data());

		newObject->_updateModel();
		newObject->DeselectAll(g_app->m_editMode);
		newObject->UpdateAabb();

		this->DeselectAll(g_app->m_editMode);
		
		_updateModel();
		this->OnSelect(g_app->m_editMode);
		_updateModel();
		this->UpdateAabb();
		g_app->UpdateSelectionAabb();
	}
}