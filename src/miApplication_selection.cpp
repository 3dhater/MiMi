#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"
#include "miRootObject.h"
#include "miGUIManager.h"
#include "miPluginGUIImpl.h"

extern miApplication * g_app;

void miApplication::_deselect_all(miSceneObject* o) {
	if (o != m_rootObject)
		o->DeselectAll(m_editMode);
	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_deselect_all(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}

void miApplication::_select_all(miSceneObject* o) {
	if (o != m_rootObject)
		o->SelectAll(m_editMode);

	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_select_all(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}

void miApplication::_invert_selection(miSceneObject* o) {
	if (o != m_rootObject)
		o->InvertSelection(m_editMode);
	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_invert_selection(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}

void miApplication::DeselectAll() {
	_deselect_all(m_rootObject);
	UpdateSelectedObjectsArray();
	UpdateSelectionAabb();
	m_GUIManager->SetCommonParamsRangePosition();
}

void miApplication::SelectAll() {
	_select_all(m_rootObject);
	UpdateSelectedObjectsArray();
	UpdateSelectionAabb();
	m_GUIManager->SetCommonParamsRangePosition();
}

void miApplication::InvertSelection() {
	_invert_selection(m_rootObject);
	UpdateSelectedObjectsArray();
	UpdateSelectionAabb();
	m_GUIManager->SetCommonParamsRangePosition();
}

void miApplication::_select_multiple() {
	if (m_keyboardModifier != miKeyboardModifier::Alt && m_keyboardModifier != miKeyboardModifier::Ctrl)
		DeselectAll();

	for (u32 i = 0; i < m_activeViewportLayout->m_activeViewport->m_visibleObjects.m_size; ++i)
	{
		m_activeViewportLayout->m_activeViewport->m_visibleObjects.m_data[i]->Select(m_editMode, m_keyboardModifier, m_selectionFrust);
	}
	UpdateSelectedObjectsArray();
	/*UpdateSelectionAabb();
	m_GUIManager->SetCommonParamsRangePosition();*/
}

void miApplication::_select_single_call(miSceneObject* o) {
	if (o != m_rootObject && o->IsSelected())
		o->SelectSingle(m_editMode, m_keyboardModifier, m_selectionFrust);

	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_select_single_call(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}

void miApplication::_select_single() {
	if (m_keyboardModifier != miKeyboardModifier::Alt && m_keyboardModifier != miKeyboardModifier::Ctrl)
		DeselectAll();

	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		_select_single_call(m_rootObject);
		break;
	case miEditMode::Object:
	default:
		if (m_objectsUnderCursor.m_size)
		{
			m_objectsUnderCursor.m_data[0]->SelectSingle(m_editMode, m_keyboardModifier, m_selectionFrust);
			UpdateSelectedObjectsArray();
		}
		break;
	}
	/*UpdateSelectionAabb();
	m_GUIManager->SetCommonParamsRangePosition();*/
}

void miApplication::_update_selected_objects_array(miSceneObject* o) {
	if (o->IsSelected())
		m_selectedObjects.push_back(o);

	auto node = o->GetChildren()->m_head;
	if (node) {
		auto last = node->m_left;
		while (true) {
			_update_selected_objects_array(node->m_data);
			if (node == last)
				break;
			node = node->m_right;
		}
	}
}
void miApplication::UpdateSelectedObjectsArray() {
	m_selectedObjects.clear();
	_update_selected_objects_array(m_rootObject);
	//printf("selected objects: %u\n", m_selectedObjects.m_size);
	m_GUIManager->m_textInput_rename->DeleteAll();

	if (m_currentPluginGUI)
	{
		m_currentPluginGUI->Show(false);
	}

	if (m_selectedObjects.m_size == 1)
	{
		m_currentPluginGUI = (miPluginGUIImpl*)m_selectedObjects.m_data[0]->m_gui;

		if (m_currentPluginGUI)
		{
			m_currentPluginGUI->OnSelectObject(m_selectedObjects.m_data[0]);
		}
		if (m_currentPluginGUI && m_objectParametersMode == miObjectParametersMode::ObjectParameters)
		{
			m_currentPluginGUI->Show(true);
		}

		m_GUIManager->m_textInput_rename->SetText(L"%s", m_selectedObjects.m_data[0]->m_name.data());
	}
	else if (m_selectedObjects.m_size > 1)
	{
		m_GUIManager->m_textInput_rename->SetText(L"Many objects");
	}
}

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
							const miPair<miVertex*, miSceneObject*>& a,
							const miPair<miVertex*, miSceneObject*>& b
							)
							const
						{
							auto camera = g_app->GetActiveCamera();
							return (
								a.m_first->m_position + *a.m_second->GetGlobalPosition()).distance(camera->m_positionCamera) >
								(b.m_first->m_position + *b.m_second->GetGlobalPosition()).distance(camera->m_positionCamera);
						}
					};
					m_sdk->m_vertsForSelect.sort_insertion(_pred());
				}

				if (m_keyboardModifier == miKeyboardModifier::Alt)
				{
					if (m_sdk->m_vertsForSelect.m_data[0].m_first->m_flags & miVertex::flag_isSelected)
						m_sdk->m_vertsForSelect.m_data[0].m_first->m_flags ^= miVertex::flag_isSelected;
				}
				else
				{
					m_sdk->m_vertsForSelect.m_data[0].m_first->m_flags |= miVertex::flag_isSelected;

					auto v = m_sdk->m_vertsForSelect.m_data[0].m_first;
					/*auto cp = v->m_polygons.m_head;
					auto lp = cp->m_left;
					while (true){
						cp->m_data->m_flags |= miPolygon::flag_isSelected;
						if (cp == lp)
							break;
						cp = cp->m_right;
					}*/
					/*auto ce = v->m_edges.m_head;
					auto le = ce->m_left;
					while (true) {
						ce->m_data->m_flags |= miEdge::flag_isSelected;
						if (ce == le)
							break;
						ce = ce->m_right;
					}*/
				}


				for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
				{
					auto obj = m_selectedObjects.m_data[i];
					auto voc = obj->GetVisualObjectCount();
					for (s32 i2 = 0; i2 < voc; ++i2)
					{
						auto vo = obj->GetVisualObject(i2);
						if (vo->GetType() == miVisualObjectType::Vertex)
							vo->CreateNewGPUModels(0);
						/*if (vo->GetType() == miVisualObjectType::Polygon)
							vo->CreateNewGPUModels(0);*/
						/*if (vo->GetType() == miVisualObjectType::Edge)
							vo->CreateNewGPUModels(0);*/
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
							const miPair<miEdge*, miSceneObject*>& a,
							const miPair<miEdge*, miSceneObject*>& b
							)
							const
						{
							auto camera = g_app->GetActiveCamera();
							auto center = a.m_first->m_vertex1->m_position + b.m_first->m_vertex1->m_position;
							center *= 0.5f;
							center += *a.m_second->GetGlobalPosition();

							return center.distance(camera->m_positionCamera) > center.distance(camera->m_positionCamera);
						}
					};
					m_sdk->m_edgesForSelect.sort_insertion(_pred());
				}

				if (m_keyboardModifier == miKeyboardModifier::Alt)
				{
					if (m_sdk->m_edgesForSelect.m_data[0].m_first->m_flags & miEdge::flag_isSelected)
						m_sdk->m_edgesForSelect.m_data[0].m_first->m_flags ^= miEdge::flag_isSelected;
				}
				else
				{
					m_sdk->m_edgesForSelect.m_data[0].m_first->m_flags |= miEdge::flag_isSelected;

					/*if (m_sdk->m_edgesForSelect.m_data[0].m_first->m_polygon1)
						m_sdk->m_edgesForSelect.m_data[0].m_first->m_polygon1->m_flags |= miPolygon::flag_isSelected;
					if (m_sdk->m_edgesForSelect.m_data[0].m_first->m_polygon2)
						m_sdk->m_edgesForSelect.m_data[0].m_first->m_polygon2->m_flags |= miPolygon::flag_isSelected;*/
				}


				for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
				{
					auto obj = m_selectedObjects.m_data[i];
					auto voc = obj->GetVisualObjectCount();
					for (s32 i2 = 0; i2 < voc; ++i2)
					{
						auto vo = obj->GetVisualObject(i2);
						if (vo->GetType() == miVisualObjectType::Edge)
							vo->CreateNewGPUModels(0);
						/*if (vo->GetType() == miVisualObjectType::Polygon)
							vo->CreateNewGPUModels(0);*/
					}
				}
				m_sdk->m_edgesForSelect.clear();
			}
		}
		else if (m_editMode == miEditMode::Polygon)
		{
			if (m_sdk->m_polygonsForSelect.m_size)
			{
				if (m_sdk->m_polygonsForSelect.m_size > 1)
				{
					struct _pred {
						bool operator() (
							const miPair<miPair<miPolygon*, f32>, miSceneObject*>& a,
							const miPair<miPair<miPolygon*, f32>, miSceneObject*>& b
							)
							const
						{
							return a.m_first.m_second < b.m_first.m_second;
						}
					};
					m_sdk->m_polygonsForSelect.sort_insertion(_pred());
				}

				if (m_keyboardModifier == miKeyboardModifier::Alt)
				{
					if (m_sdk->m_polygonsForSelect.m_data[0].m_first.m_first->m_flags & miPolygon::flag_isSelected)
						m_sdk->m_polygonsForSelect.m_data[0].m_first.m_first->m_flags ^= miPolygon::flag_isSelected;
				}
				else
				{
					m_sdk->m_polygonsForSelect.m_data[0].m_first.m_first->m_flags |= miPolygon::flag_isSelected;

					auto p = m_sdk->m_polygonsForSelect.m_data[0].m_first.m_first;
					/*auto cv = p->m_verts.m_head;
					auto lv = cv->m_left;
					while (true) {
						cv->m_data->m_flags |= miVertex::flag_isSelected;
						if (cv == lv)
							break;
						cv = cv->m_right;
					}*/
					/*auto ce = p->m_edges.m_head;
					auto le = ce->m_left;
					while (true) {
						ce->m_data->m_flags |= miEdge::flag_isSelected;
						if (ce == le)
							break;
						ce = ce->m_right;
					}*/
				}


				for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
				{
					auto obj = m_selectedObjects.m_data[i];
					auto voc = obj->GetVisualObjectCount();
					for (s32 i2 = 0; i2 < voc; ++i2)
					{
						auto vo = obj->GetVisualObject(i2);
						if (vo->GetType() == miVisualObjectType::Polygon)
							vo->CreateNewGPUModels(0);
						/*if (vo->GetType() == miVisualObjectType::Vertex)
							vo->CreateNewGPUModels(0);*/
						/*if (vo->GetType() == miVisualObjectType::Edge)
							vo->CreateNewGPUModels(0);*/
					}
				}
				m_sdk->m_polygonsForSelect.clear();
			}
		}
	}

	UpdateSelectionAabb();
	m_GUIManager->SetCommonParamsRangePosition();
}
