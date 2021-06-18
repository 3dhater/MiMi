#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"

extern miApplication * g_app;

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
