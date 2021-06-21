#include "miApplication.h"
#include "miRootObject.h"
#include "miGUIManager.h"

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
			m_selectedObjects.m_data[i]->UpdateAabb();
			break;
		default:
			break;
		}
	}

	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
		{
			auto voc = m_selectedObjects.m_data[i]->GetVisualObjectCount();
			for (s32 o = 0; o < voc; ++o)
			{
				m_selectedObjects.m_data[i]->GetVisualObject(o)->UpdateAabb();
			}
			m_selectedObjects.m_data[i]->UpdateAabb();
		}
		break;
	case miEditMode::Object:
		break;
	default:
		break;
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
	bool isCancel = m_inputContext->m_isRMBUp || m_inputContext->IsKeyHit(yyKey::K_ESCAPE);
	
	auto var_move = m_gizmo->m_var_move;
	if (isCancel)
	{
		var_move = m_gizmo->m_var_move_onEscape;
		//printf("isCancel\n");
	}

	auto M = *o->GetRotationScaleMatrix();
	M.invert();

	switch (m_editMode)
	{
	case miEditMode::Vertex:
		o->OnTransformVertex(miTransformMode::Move, math::mulBasis(var_move, M), math::mulBasis(m_gizmo->m_moveDelta, M), isCancel);
		break;
	case miEditMode::Edge:
		o->OnTransformEdge(miTransformMode::Move, math::mulBasis(var_move, M), math::mulBasis(m_gizmo->m_moveDelta, M));
		break;
	case miEditMode::Polygon:
		o->OnTransformPolygon(miTransformMode::Move, math::mulBasis(var_move, M), math::mulBasis(m_gizmo->m_moveDelta, M));
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
	m_GUIManager->SetCommonParamsRangePosition();
}

void miApplication::_transformObjects_scale(miSceneObject* o) {
	Mat4 S;
	S.setScale(m_gizmo->m_var_scale);

	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
	default:
		auto R = o->GetRotationScaleMatrix();
		*R = S * o->m_rotationScaleMatrixOnGizmoClick;
		if (!m_isLocalTransform)
		{
			//auto C = m_selectionAabb_center;
			auto C = m_gizmo->m_position;
			auto position = o->GetLocalPosition();
			*position = math::mul(o->m_localPositionOnGizmoClick - C, S) + C;
		}
		break;
	}

	o->UpdateTransform();
	UpdateSceneAabb();
}

void miApplication::_transformObjects_rotate(miSceneObject* o) {
	Mat4 RX, RY, RZ;
	RX.setRotation(Quat(m_gizmo->m_var_rotate.x, 0.f, 0.f));
	RY.setRotation(Quat(0.f, m_gizmo->m_var_rotate.y, 0.f));
	RZ.setRotation(Quat(0.f, 0.f, m_gizmo->m_var_rotate.z));

	Mat4 R;
	if (m_gizmoMode == miGizmoMode::RotateScreen)
	{
		R = m_gizmo->m_rotateScreenMatrix;
	}
	else
	{
		R = RX * RY * RZ;
	}

	switch (m_editMode)
	{
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
	default:
		auto R2 = o->GetRotationScaleMatrix();
		auto R3 = o->GetRotationOnlyMatrix();
		*R2 = R * o->m_rotationScaleMatrixOnGizmoClick;
		*R3 = R * o->m_rotationOnlyMatrixOnGizmoClick;

		if (!m_isLocalTransform)
		{
			//auto C = m_selectionAabb_center;
			auto C = m_gizmo->m_position;
			
			auto position = o->GetLocalPosition();
			*position = math::mul(o->m_localPositionOnGizmoClick - C, R) + C;
		}
		//auto a = R3->getAngles();
		//printf("a: %f %f %f\n", a.x, a.y, a.z);
		break;
	}
	o->UpdateTransform();
	UpdateSceneAabb();
}

void miApplication::_transformObjects() {
	f32 camera_zoom = m_activeViewportLayout->m_activeViewport->m_activeCamera->m_positionPlatform.w;
	auto camera_direction = m_activeViewportLayout->m_activeViewport->m_activeCamera->m_direction;
	f32 move_speed = 0.003f * camera_zoom;
	if (m_keyboardModifier == miKeyboardModifier::Alt)
		move_speed *= 0.01f;


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

	m_gizmo->m_moveDelta = m_gizmo->m_var_move - m_gizmo->m_var_move_old;
	m_gizmo->m_var_move_old = m_gizmo->m_var_move;
	//printf("MD: %f %f %f\n", m_gizmo->m_moveDelta.x, m_gizmo->m_moveDelta.y, m_gizmo->m_moveDelta.z);

	auto mouse_delta_d = m_inputContext->m_mouseDelta.distance(v2f());

	f32 scale_speed = 0.05f * camera_zoom;
	if (m_keyboardModifier == miKeyboardModifier::Alt)
		scale_speed *= 0.01f;
	f32 scale_v = mouse_delta_d * 0.01f * scale_speed;
	//printf("%f\n", mouse_delta_d);
	
	auto s = m_screenRayCurrent.m_origin - m_screenRayOnClick.m_origin;
	//printf("%f %f %f\n", s.x, s.y, s.z);
	switch (m_gizmoMode)
	{
	case miGizmoMode::ScaleX:
		if (s.x < 0.f)m_gizmo->m_var_scale2.x -= scale_v;else if (s.x > 0.f)m_gizmo->m_var_scale2.x += scale_v;
		break;
	case miGizmoMode::ScaleY:
		if (s.y < 0.f)m_gizmo->m_var_scale2.y -= scale_v; else if (s.y > 0.f)m_gizmo->m_var_scale2.y += scale_v;
		break;
	case miGizmoMode::ScaleZ:
		if (s.z < 0.f)m_gizmo->m_var_scale2.z -= scale_v; else if (s.z > 0.f)m_gizmo->m_var_scale2.z += scale_v;
		break;
	case miGizmoMode::ScaleXZ:
		if (s.x < 0.f)m_gizmo->m_var_scale2.x -= scale_v; else if (s.x > 0.f)m_gizmo->m_var_scale2.x += scale_v;
		if (s.z < 0.f)m_gizmo->m_var_scale2.z -= scale_v; else if (s.z > 0.f)m_gizmo->m_var_scale2.z += scale_v;
		break;
	case miGizmoMode::ScaleXY:
		if (s.x < 0.f)m_gizmo->m_var_scale2.x -= scale_v; else if (s.x > 0.f)m_gizmo->m_var_scale2.x += scale_v;
		if (s.y < 0.f)m_gizmo->m_var_scale2.y -= scale_v; else if (s.y > 0.f)m_gizmo->m_var_scale2.y += scale_v;
		break;
	case miGizmoMode::ScaleZY:
		if (s.y < 0.f)m_gizmo->m_var_scale2.y -= scale_v; else if (s.y > 0.f)m_gizmo->m_var_scale2.y += scale_v;
		if (s.z < 0.f)m_gizmo->m_var_scale2.z -= scale_v; else if (s.z > 0.f)m_gizmo->m_var_scale2.z += scale_v;
		break;
	case miGizmoMode::ScaleScreen:
		if (s.x < 0.f)m_gizmo->m_var_scale2.x -= scale_v; else if (s.x > 0.f)m_gizmo->m_var_scale2.x += scale_v;
		m_gizmo->m_var_scale2.y = m_gizmo->m_var_scale2.z = m_gizmo->m_var_scale2.x;
		break;
	default:
		break;
	}

	{
		f32 x = m_gizmo->m_var_scale2.x - 1.f;
		f32 y = m_gizmo->m_var_scale2.y - 1.f;
		f32 z = m_gizmo->m_var_scale2.z - 1.f;
		if (x<0.f) { x = x * 1.f / (1.f - x); }
		if (y<0.f) { y = y * 1.f / (1.f - y); }
		if (z<0.f) { z = z * 1.f / (1.f - z); }
		x += 1.f;
		y += 1.f;
		z += 1.f;
		if (x <= 0.f) x = FLT_MIN;
		if (y <= 0.f) y = FLT_MIN;
		if (z <= 0.f) z = FLT_MIN;
		//printf("%f %f %f\n", x, y, z);
		m_gizmo->m_var_scale.x = x;
		m_gizmo->m_var_scale.y = y;
		m_gizmo->m_var_scale.z = z;
	}

	//printf("%f %f %f\n", m_gizmo->m_var_scale2.x, m_gizmo->m_var_scale2.y, m_gizmo->m_var_scale2.z);

	f32 rot = m_inputContext->m_mouseDelta.x * 0.005f;
	if (m_keyboardModifier == miKeyboardModifier::Alt)
	{
		rot *= 0.001f;
	}
	if (m_keyboardModifier == miKeyboardModifier::Shift)
	{
		f32 angle_snap = math::degToRad(10.f);
		
		m_gizmo->m_var_rotate_snap += rot;
		rot = 0.f;

		if (m_gizmo->m_var_rotate_snap >= angle_snap || m_gizmo->m_var_rotate_snap <= -angle_snap)
		{
			if(m_gizmo->m_var_rotate_snap <= -angle_snap)
				rot = (-angle_snap);
			else
				rot = (angle_snap);

			m_gizmo->m_var_rotate_snap = 0.f;
		}
	}

	switch (m_gizmoMode)
	{
	case miGizmoMode::RotateX:
		m_gizmo->m_var_rotate.x += rot;
		//printf("x %f\n", math::radToDeg(m_gizmo->m_var_rotate.x));
		break;
	case miGizmoMode::RotateY:
		m_gizmo->m_var_rotate.y += rot;
		//printf("y %f\n", math::radToDeg(m_gizmo->m_var_rotate.y));
		break;
	case miGizmoMode::RotateZ:
		m_gizmo->m_var_rotate.z += rot;
		//printf("z %f\n", math::radToDeg(m_gizmo->m_var_rotate.z));
		break;
	case miGizmoMode::RotateScreen:
	{
		m_gizmo->m_var_rotate.w += rot;
		Mat4 m;
		m.setRotation(Quat(0.f, m_gizmo->m_var_rotate.w, 0.f));
		
		Mat4 mX, mY, mYi, mXi;
		mX.setRotation(Quat(m_activeViewportLayout->m_activeViewport->m_activeCamera->m_rotationPlatform.x, 0.f, 0.f));
		mY.setRotation(Quat(0.f, m_activeViewportLayout->m_activeViewport->m_activeCamera->m_rotationPlatform.y, 0.f));
		mYi = mY;
		mYi.invert();
		mXi = mX;
		mXi.invert();

		m = mY * mX * m;
		m = m * mXi;
		m = m * mYi;

		m_gizmo->m_rotateScreenMatrix = m;
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