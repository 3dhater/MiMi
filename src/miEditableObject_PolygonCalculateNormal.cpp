#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void miEditableObject::PolygonFlip() {
	if (m_mesh->m_first_polygon)
	{
		auto curr = m_mesh->m_first_polygon;
		auto last = curr->m_left;
		while (true)
		{
			if (curr->m_flags & miPolygon::flag_isSelected)
			{
				curr->Flip();
				curr->CalculateNormal();
			}
			if (curr == last)
				break;
			curr = curr->m_right;
		}
	}
	_updateModel();
}

void miEditableObject::PolygonCalculateNormal(bool smooth) {
	if (m_mesh->m_first_polygon)
	{
		auto curr = m_mesh->m_first_polygon;
		auto last = curr->m_left;
		while (true)
		{
			if (curr->m_flags & miPolygon::flag_isSelected)
			{
				curr->CalculateNormal();
			}
			if (curr == last)
				break;
			curr = curr->m_right;
		}
	}

	if (smooth)
	{
		if (m_mesh->m_first_vertex)
		{
			auto curr = m_mesh->m_first_vertex;
			auto last = curr->m_left;
			while (true)
			{
				v3f normal;

				{
					auto cp = curr->m_polygons.m_head;
					auto lp = cp->m_left;
					while (true)
					{
						if (cp->m_data->m_flags & miPolygon::flag_isSelected)
						{
							auto vNode = cp->m_data->FindVertex(curr);
							if (vNode)
								normal += vNode->m_data3;
						}

						if (cp == lp)
							break;
						cp = cp->m_right;
					}
				}

				normal.normalize2();

				{
					auto cp = curr->m_polygons.m_head;
					auto lp = cp->m_left;
					while (true)
					{
						if (cp->m_data->m_flags & miPolygon::flag_isSelected)
						{
							auto vNode = cp->m_data->FindVertex(curr);
							if (vNode)
								vNode->m_data3 = normal;
						}

						if (cp == lp)
							break;
						cp = cp->m_right;
					}
				}

				if (curr == last)
					break;
				curr = curr->m_right;
			}
		}
	}

	_updateModel();
}
