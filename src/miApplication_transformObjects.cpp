#include "miApplication.h"
#include "miRootObject.h"

void miApplication::_transformObjects_move(miSceneObject* o) {
//	auto n = o->GetName();
//	wprintf(L"%s\n", n.c_str());
	f32 camera_zoom = m_activeViewportLayout->m_activeViewport->m_activeCamera->m_positionPlatform.w;

	f32 speed = 0.003f;
	//f32 var = 0.f;
	switch (m_gizmoMode)
	{
	case miGizmoMode::NoTransform:
		break;
	case miGizmoMode::MoveX:
		m_gizmo->m_variable.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
		break;
	case miGizmoMode::MoveY:
		m_gizmo->m_variable.y += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
		break;
	case miGizmoMode::MoveZ:
		m_gizmo->m_variable.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
		break;
	case miGizmoMode::MoveXZ:
		m_gizmo->m_variable.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
		m_gizmo->m_variable.z += m_inputContext->m_mouseDelta.y * speed * camera_zoom;
		break;
	case miGizmoMode::MoveXY:
		m_gizmo->m_variable.x += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
		m_gizmo->m_variable.y -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
		break;
	case miGizmoMode::MoveZY:
		m_gizmo->m_variable.z += m_inputContext->m_mouseDelta.x * speed * camera_zoom;
		m_gizmo->m_variable.y -= m_inputContext->m_mouseDelta.y * speed * camera_zoom;
		break;
	case miGizmoMode::MoveScreen:
		break;
	case miGizmoMode::ScaleX:
		break;
	case miGizmoMode::ScaleY:
		break;
	case miGizmoMode::ScaleZ:
		break;
	case miGizmoMode::ScaleXZ:
		break;
	case miGizmoMode::ScaleXY:
		break;
	case miGizmoMode::ScaleZY:
		break;
	case miGizmoMode::ScaleScreen:
		break;
	case miGizmoMode::RotateX:
		break;
	case miGizmoMode::RotateY:
		break;
	case miGizmoMode::RotateZ:
		break;
	case miGizmoMode::RotateScreen:
		break;
	default:
		break;
	}
	//m_gizmo->m_variable += var;
	//printf("%f\n", m_gizmo->m_variable);

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
		*position = m_gizmo->m_selectionAabbCenterOnClick + o->m_selectionAabbOffset + m_gizmo->m_variable;
		//*position += dir * m_gizmo->m_variable;
//		printf("%f %f %f\n", m_gizmo->m_selectionAabbCenterOnClick.x, m_gizmo->m_selectionAabbCenterOnClick.y, m_gizmo->m_selectionAabbCenterOnClick.z);

		
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