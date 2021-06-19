#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"

extern miApplication * g_app;

void miApplication::UpdateSelectionAabb() {
	m_gizmo->m_position.set(0.f);
	m_selectionAabb.reset();
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto obj = m_selectedObjects.m_data[i];
		auto mc = obj->GetMeshCount();
		Mat4 M = obj->GetWorldMatrix()->getBasis();

		switch (m_editMode)
		{
		case miEditMode::Vertex:
			for (s32 o = 0; o < mc; ++o)
			{
				auto m = obj->GetMesh(o);
				auto current_vertex = m->m_first_vertex;
				auto last_vertex = current_vertex->m_left;
				while (true) {
					if (current_vertex->m_flags & current_vertex->flag_isSelected)
					{
						m_selectionAabb.add(math::mul(current_vertex->m_position, M)
							+ *m_selectedObjects.m_data[i]->GetGlobalPosition());
					}
					if (current_vertex == last_vertex)
						break;
					current_vertex = current_vertex->m_right;
				}
			}
			break;
		case miEditMode::Edge:
			for (s32 o = 0; o < mc; ++o)
			{
				auto m = obj->GetMesh(o);
				auto current_edge = m->m_first_edge;
				auto last_edge = current_edge->m_left;
				while (true) {
					if (current_edge->m_flags & current_edge->flag_isSelected)
					{
						m_selectionAabb.add(math::mul(current_edge->m_vertex1->m_position, M)
							+ *m_selectedObjects.m_data[i]->GetGlobalPosition());
						m_selectionAabb.add(math::mul(current_edge->m_vertex2->m_position, M)
							+ *m_selectedObjects.m_data[i]->GetGlobalPosition());
					}
					if (current_edge == last_edge)
						break;
					current_edge = current_edge->m_right;
				}
			}
			break;
		case miEditMode::Polygon:
			for (s32 o = 0; o < mc; ++o)
			{
				auto m = obj->GetMesh(o);
				auto current_polygon = m->m_first_polygon;
				auto last_polygon = current_polygon->m_left;
				while (true) {
					if (current_polygon->m_flags & current_polygon->flag_isSelected)
					{
						auto current_vertex = current_polygon->m_verts.m_head;
						auto last_vertex = current_vertex->m_left;
						while (true)
						{
							m_selectionAabb.add(math::mul(current_vertex->m_data->m_position, M)
								+ *m_selectedObjects.m_data[i]->GetGlobalPosition());

							if (current_vertex == last_vertex)
								break;
							current_vertex = current_vertex->m_right;
						}

					}
					if (current_polygon == last_polygon)
						break;
					current_polygon = current_polygon->m_right;
				}
			}
			break;
		case miEditMode::Object: {
			m_selectionAabb.add(*m_selectedObjects.m_data[i]->GetAABBTransformed());
			m_gizmo->m_position += *m_selectedObjects.m_data[i]->GetGlobalPosition();
			if (m_selectedObjects.m_size)
			{
				if (m_gizmo->m_position.x != 0.f) m_gizmo->m_position.x /= (f32)m_selectedObjects.m_size;
				if (m_gizmo->m_position.y != 0.f) m_gizmo->m_position.y /= (f32)m_selectedObjects.m_size;
				if (m_gizmo->m_position.z != 0.f) m_gizmo->m_position.z /= (f32)m_selectedObjects.m_size;
			}
	
			if (m_selectedObjects.m_size == 1)
			{
				m_gizmo->m_position = m_selectedObjects.m_data[0]->m_globalPosition;
			}
		}break;
		default:
			break;
		}
	}

	m_selectionAabb.center(m_selectionAabb_center);
	m_selectionAabb.extent(m_selectionAabb_extent);
	//	printf("UpdateSelectionAabb %f %f %f\n", m_selectionAabb_center.x, m_selectionAabb_center.y, m_selectionAabb_center.z);
}

void miApplication::_onSelect() {
	if (m_isSelectByRectangle)
	{
		Aabb aabb;
		aabb.add(v3f(m_inputContext->m_cursorCoords.x, m_inputContext->m_cursorCoords.y, 0.f));
		aabb.add(v3f(m_cursorLMBClickPosition.x, m_cursorLMBClickPosition.y, 0.f));

		m_selectionFrust->CreateWithFrame(
			v4f(aabb.m_min.x, aabb.m_min.y, aabb.m_max.x, aabb.m_max.y),
			m_activeViewportLayout->m_activeViewport->m_currentRect,
			m_activeViewportLayout->m_activeViewport->m_activeCamera->m_viewProjectionInvertMatrix);

		_select_multiple();
	}
	else
	{
		_select_single();
		if (m_editMode == miEditMode::Vertex)
		{
			if (m_sdk->m_vertsForSelect.m_size)
			{

				if (m_sdk->m_vertsForSelect.m_size > 1)
				{
					struct _pred {
						bool operator() (
							const std::pair<miVertex*, miSceneObject*>& a,
							const std::pair<miVertex*, miSceneObject*>& b
							)
							const
						{
							auto camera = g_app->GetActiveCamera();
							return (
								a.first->m_position + *a.second->GetGlobalPosition()).distance(camera->m_positionCamera) >
								(b.first->m_position + *b.second->GetGlobalPosition()).distance(camera->m_positionCamera);
						}
					};
					m_sdk->m_vertsForSelect.sort_insertion(_pred());
				}

				if (m_keyboardModifier == miKeyboardModifier::Alt)
				{
					if (m_sdk->m_vertsForSelect.m_data[0].first->m_flags & miVertex::flag_isSelected)
						m_sdk->m_vertsForSelect.m_data[0].first->m_flags ^= miVertex::flag_isSelected;
				}
				else
				{
					m_sdk->m_vertsForSelect.m_data[0].first->m_flags |= miVertex::flag_isSelected;
				}


				for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
				{
					auto obj = m_selectedObjects.m_data[i];
					auto voc = obj->GetVisualObjectCount();
					for (s32 i2 = 0; i2 < voc; ++i2)
					{
						auto vo = obj->GetVisualObject(i2);
						if (vo->GetType() == miVisualObjectType::Vertex)
						{
							vo->CreateNewGPUModels(0);
						}
					}
				}
				m_sdk->m_vertsForSelect.clear();
			}
		}
		else if (m_editMode == miEditMode::Edge)
		{
			if (m_sdk->m_edgesForSelect.m_size)
			{
				if (m_sdk->m_edgesForSelect.m_size > 1)
				{
					struct _pred {
						bool operator() (
							const std::pair<miEdge*, miSceneObject*>& a,
							const std::pair<miEdge*, miSceneObject*>& b
							)
							const
						{
							auto camera = g_app->GetActiveCamera();
							auto center = a.first->m_vertex1->m_position + b.first->m_vertex1->m_position;
							center *= 0.5f;
							center += *a.second->GetGlobalPosition();

							return center.distance(camera->m_positionCamera) > center.distance(camera->m_positionCamera);
						}
					};
					m_sdk->m_edgesForSelect.sort_insertion(_pred());
				}

				if (m_keyboardModifier == miKeyboardModifier::Alt)
				{
					if (m_sdk->m_edgesForSelect.m_data[0].first->m_flags & miEdge::flag_isSelected)
						m_sdk->m_edgesForSelect.m_data[0].first->m_flags ^= miEdge::flag_isSelected;
				}
				else
				{
					m_sdk->m_edgesForSelect.m_data[0].first->m_flags |= miEdge::flag_isSelected;
				}


				for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
				{
					auto obj = m_selectedObjects.m_data[i];
					auto voc = obj->GetVisualObjectCount();
					for (s32 i2 = 0; i2 < voc; ++i2)
					{
						auto vo = obj->GetVisualObject(i2);
						if (vo->GetType() == miVisualObjectType::Edge)
						{
							vo->CreateNewGPUModels(0);
						}
					}
				}
				m_sdk->m_edgesForSelect.clear();
			}
		}
	}
}
