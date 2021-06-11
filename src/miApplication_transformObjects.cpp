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
			m_selectedObjects.m_data[i]->UpdateAabb();
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
		*position = o->m_localPositionOnGizmoClick + m_gizmo->m_var_move;
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
		auto scale = o->GetScale();
		*scale = o->m_scaleOnGizmoClick + (m_gizmo->m_var_scale - 1.f);
		break;
	}

	o->UpdateTransform();
	UpdateSceneAabb();
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
	f32 move_speed = 0.003f * camera_zoom;
	
	switch (m_gizmoMode)
	{
	case miGizmoMode::MoveX:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * move_speed;
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * move_speed;
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		}
		break;
	case miGizmoMode::MoveY:
		m_gizmo->m_var_move.y -= m_inputContext->m_mouseDelta.y * move_speed;
		break;
	case miGizmoMode::MoveZ:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * move_speed;
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * move_speed;
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		}
		break;
	case miGizmoMode::MoveXZ:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * move_speed;
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * move_speed;
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		}
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * move_speed;
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * move_speed;
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		}
		break;
	case miGizmoMode::MoveXY:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.x += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.x -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		}
		m_gizmo->m_var_move.y -= m_inputContext->m_mouseDelta.y * move_speed;
		break;
	case miGizmoMode::MoveZY:
		switch (camera_direction)
		{
		case miDirection::North:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::NorthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::East:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::SouthEast:
			m_gizmo->m_var_move.z -= m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::South:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.y * move_speed;
			break;
		case miDirection::SouthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::West:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		case miDirection::NorthWest:
			m_gizmo->m_var_move.z += m_inputContext->m_mouseDelta.x * move_speed;
			break;
		}
		m_gizmo->m_var_move.y -= m_inputContext->m_mouseDelta.y * move_speed;
		break;
	case miGizmoMode::MoveScreen:
	{
		auto p1 = m_screenRayCurrent.m_end;
		auto p2 = m_screenRayOnClick.m_end;

		f32 D = m_screenRayCurrent.m_end.distance(m_screenRayOnClick.m_end);
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

	auto mouse_delta_d = m_inputContext->m_mouseDelta.distance(v2f());

	f32 scale_speed = 0.05f * camera_zoom;
	f32 scale_v = mouse_delta_d * 0.01f * scale_speed;
	//printf("%f\n", mouse_delta_d);
	
	auto s = m_screenRayCurrent.m_origin - m_screenRayOnClick.m_origin;
	//printf("%f %f %f\n", s.x, s.y, s.z);
	switch (m_gizmoMode)
	{
	case miGizmoMode::ScaleX:
		if (s.x < 0.f)m_gizmo->m_var_scale.x -= scale_v;else if (s.x > 0.f)m_gizmo->m_var_scale.x += scale_v;
		break;
	case miGizmoMode::ScaleY:
		if (s.y < 0.f)m_gizmo->m_var_scale.y -= scale_v; else if (s.y > 0.f)m_gizmo->m_var_scale.y += scale_v;
		break;
	case miGizmoMode::ScaleZ:
		if (s.z < 0.f)m_gizmo->m_var_scale.z -= scale_v; else if (s.z > 0.f)m_gizmo->m_var_scale.z += scale_v;
		break;
	case miGizmoMode::ScaleXZ:
		if (s.x < 0.f)m_gizmo->m_var_scale.x -= scale_v; else if (s.x > 0.f)m_gizmo->m_var_scale.x += scale_v;
		if (s.z < 0.f)m_gizmo->m_var_scale.z -= scale_v; else if (s.z > 0.f)m_gizmo->m_var_scale.z += scale_v;
		break;
	case miGizmoMode::ScaleXY:
		if (s.x < 0.f)m_gizmo->m_var_scale.x -= scale_v; else if (s.x > 0.f)m_gizmo->m_var_scale.x += scale_v;
		if (s.y < 0.f)m_gizmo->m_var_scale.y -= scale_v; else if (s.y > 0.f)m_gizmo->m_var_scale.y += scale_v;
		break;
	case miGizmoMode::ScaleZY:
		if (s.y < 0.f)m_gizmo->m_var_scale.y -= scale_v; else if (s.y > 0.f)m_gizmo->m_var_scale.y += scale_v;
		if (s.z < 0.f)m_gizmo->m_var_scale.z -= scale_v; else if (s.z > 0.f)m_gizmo->m_var_scale.z += scale_v;
		break;
	case miGizmoMode::ScaleScreen:
		if (s.x < 0.f)m_gizmo->m_var_scale.x -= scale_v; else if (s.x > 0.f)m_gizmo->m_var_scale.x += scale_v;
		m_gizmo->m_var_scale.y = m_gizmo->m_var_scale.z = m_gizmo->m_var_scale.x;
		break;
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