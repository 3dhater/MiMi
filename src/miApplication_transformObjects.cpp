#include "miApplication.h"
#include "miRootObject.h"

void miApplication::_transformObjectsApply() {
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		switch (m_transformMode)
		{
		case miTransformMode::NoTransform:
			break;
		case miTransformMode::Move:
			m_selectedObjects.m_data[i]->UpdateAabb();
			break;
		case miTransformMode::Scale:
			break;
		case miTransformMode::Rotate:
			break;
		default:
			break;
		}
	}
}
void miApplication::_transformObjectsReset() {
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		switch (m_transformMode)
		{
		case miTransformMode::NoTransform:
			break;
		case miTransformMode::Move:
			_transformObjects_move(m_selectedObjects.m_data[i]);
			break;
		case miTransformMode::Scale:
			_transformObjects_scale(m_selectedObjects.m_data[i]);
			break;
		case miTransformMode::Rotate:
			_transformObjects_rotate(m_selectedObjects.m_data[i]);
			break;
		default:
			break;
		}
	}
}

void miApplication::_transformObjects_move(miSceneObject* o) {
//	auto n = o->GetName();
//	wprintf(L"%s\n", n.c_str());
	

	
	//m_gizmo->m_var_move += var;
	//printf("%f\n", m_gizmo->m_var_move);

	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
	default:
	{
		auto position = o->GetLocalPosition();
		*position = m_gizmo->m_selectionAabbCenterOnClick + o->m_selectionAabbOffset + m_gizmo->m_var_move;
	}break;
	}

	o->UpdateTransform();
	UpdateSceneAabb();
}

void miApplication::_transformObjects_scale(miSceneObject* o) {
	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
	default:
		break;
	}
}

void miApplication::_transformObjects_rotate(miSceneObject* o) {
	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
	default:
		break;
	}
}

void miApplication::_transformObjects() {
	f32 camera_zoom = m_activeViewportLayout->m_activeViewport->m_activeCamera->m_positionPlatform.w;
	auto camera_direction = m_activeViewportLayout->m_activeViewport->m_activeCamera->m_direction;
	f32 speed = 0.003f;
	//f32 var = 0.f;
	switch (m_gizmoMode)
	{
	case miGizmoMode::NoTransform:
		break;
	case miGizmoMode::MoveX:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		}
		break;
	case miGizmoMode::MoveY:
		m_gizmo->m_var_move.y -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
		break;
	case miGizmoMode::MoveZ:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		}
		break;
	case miGizmoMode::MoveXZ:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		}
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		}
		break;
	case miGizmoMode::MoveXY:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		}
		m_gizmo->m_var_move.y -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
		break;
	case miGizmoMode::MoveZY:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
			break;
		}
		m_gizmo->m_var_move.y -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
		break;
	case miGizmoMode::MoveScreen:
	{
		auto p1 = m_cursorPosition3DFar;
		auto p2 = m_cursorLMBClickPosition3DFar;

		f32 D = m_cursorPosition3DFar.distance(m_cursorLMBClickPosition3DFar);
		//printf("D: %f\n", D);
		if (D > 1.f)
		{
			p1 *= 0.0025f;
			p2 *= 0.0025f;
		}
		m_gizmo->m_var_move += (p1 - p2);
	}break;
	default:
		break;
	}

	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		switch (m_transformMode)
		{
		case miTransformMode::NoTransform:
			break;
		case miTransformMode::Move:
			_transformObjects_move(m_selectedObjects.m_data[i]);
			break;
		case miTransformMode::Scale:
			_transformObjects_scale(m_selectedObjects.m_data[i]);
			break;
		case miTransformMode::Rotate:
			_transformObjects_rotate(m_selectedObjects.m_data[i]);
			break;
		default:
			break;
		}
	}
}