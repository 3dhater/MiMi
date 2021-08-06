#include "miApplication.h"
#include "miViewportCamera.h"

extern miApplication * g_app;

miViewportCamera::miViewportCamera(miViewport* vp, miViewportCameraType ct){
	m_viewport = vp;
	m_type = ct;
	m_forceOrtho = false;
}

miViewportCamera::~miViewportCamera(){}

void miViewportCamera::Copy(miViewportCamera* other) {
	m_type = other->m_type;
	m_aspect = other->m_aspect;
	m_far = other->m_far;
	m_fov = other->m_fov;
	m_near = other->m_near;
	m_positionCamera = other->m_positionCamera;
	m_rotationPlatform = other->m_rotationPlatform;
	m_positionPlatform = other->m_positionPlatform;
}

void miViewportCamera::Update() {
	{
		f32 zoom = m_positionPlatform.w;
		math::makeOrthoRHMatrix(m_projectionMatrixOrtho,
			zoom * m_aspect,
			zoom,
			-m_far,
			m_far);
		math::makePerspectiveRHMatrix(m_projectionMatrixPersp, m_fov, m_aspect, m_near, m_far);
	}

	if (m_type != miViewportCameraType::Perspective || m_forceOrtho)
	{
		m_projectionMatrix = m_projectionMatrixOrtho;
	}
	else
	{
		m_projectionMatrix = m_projectionMatrixPersp;
	}
	
	Mat4 MX(Quat(m_rotationPlatform.x, 0.f, 0.f));
	Mat4 MY(Quat(0.f, m_rotationPlatform.y, 0.f));
	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));

	m_positionCamera = v3f(0.f, m_positionPlatform.w, 0.f);
	m_positionCamera = math::mul(m_positionCamera, (MY * MX));
	m_positionCamera += v3f(m_positionPlatform.x, m_positionPlatform.y, m_positionPlatform.z);

	Mat4 T;
	T.m_data[3].x = -m_positionCamera.x;
	T.m_data[3].y = -m_positionCamera.y;
	T.m_data[3].z = -m_positionCamera.z;
	T.m_data[3].w = 1.f;

	Mat4 P(Quat(v4f(-m_rotationPlatform.x + math::degToRad(-90.f), 0.f, 0.f, 1.f)));
	Mat4 Y(Quat(v4f(0.f, -m_rotationPlatform.y + math::degToRad(0.f), 0.f, 1.f)));
	Mat4 R(Quat(v4f(0.f, 0.f, m_rotationPlatform.z, 1.f)));

	m_viewMatrix = (R*(P * Y)) * T;
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	

	m_viewMatrixInvert = m_viewMatrix; m_viewMatrixInvert.invert();
	auto pi = m_projectionMatrix; pi.invert();
	m_viewProjectionInvertMatrix = m_viewMatrixInvert*pi;
	m_frust.CalculateFrustum(m_projectionMatrix, m_viewMatrix);

	m_direction = miDirection::NorthEast;

	if (m_rotationPlatform.y >= 2.7488936 && m_rotationPlatform.y <= 3.5342917)
	{
		m_direction = miDirection::North;
	}
	else if (m_rotationPlatform.y >= 5.8904862 || m_rotationPlatform.y <= 0.3926991)
	{
		m_direction = miDirection::South;
	}
	else if (m_rotationPlatform.y >= 1.1780972 && m_rotationPlatform.y <= 1.9634954)
	{
		m_direction = miDirection::West;
	}
	else if (m_rotationPlatform.y >= 4.3196899 && m_rotationPlatform.y <= 5.1050881)
	{
		m_direction = miDirection::East;
	}
	else if (m_rotationPlatform.y >= 1.9634954 && m_rotationPlatform.y <= 2.7488936)
	{
		m_direction = miDirection::NorthWest;
	}
	else if (m_rotationPlatform.y >= 0.3926991 && m_rotationPlatform.y <= 1.1780972)
	{
		m_direction = miDirection::SouthWest;
	}
	else if (m_rotationPlatform.y >= 5.1050881 && m_rotationPlatform.y <= 5.8904862)
	{
		m_direction = miDirection::SouthEast;
	}
	//printf("%f %s\n", m_rotationPlatform.y, miGetDirectionName(m_direction));
}

void miViewportCamera::MoveToSelection() {
	if (m_type == miViewportCameraType::UV)
	{
		Reset();
		return;
	}
	if (!g_app->m_selectionAabb.isEmpty())
	{
		m_positionPlatform = g_app->m_selectionAabb_center;
		m_positionPlatform.w = g_app->m_selectionAabb_extent.length();
	}
	else if (!g_app->m_sceneAabb.isEmpty())
	{
		v4f e;
		g_app->m_sceneAabb.extent(e);

		v4f c;
		g_app->m_sceneAabb.center(c);
		m_positionPlatform = c;
		m_positionPlatform.w = e.length();
	}
	else
	{
		Reset();
	}

}

void miViewportCamera::Reset() {
	m_near = 0.01f;
	m_far  = 2500.f;
	m_fov  = 0.683264;
	m_aspect = 800.f / 600.f;
	m_positionPlatform = v4f(0.f, 0.f, 0.f, 15.f);
	switch (m_type)
	{
	case miViewportCameraType::Perspective:
		m_rotationPlatform = v3f(math::degToRad(-45.f), 0.f, 0.f);
	break;
	case miViewportCameraType::Bottom:
		m_rotationPlatform = v3f(math::degToRad(-180.f), 0.f, 0.f);
		break;
	case miViewportCameraType::Left:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(-90.f), 0.f);
		break;
	case miViewportCameraType::Right:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(90.f), 0.f);
		break;
	case miViewportCameraType::Back:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(180.f), 0.f);
		break;
	case miViewportCameraType::Front:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(0.f), 0.f);
		break;
	case miViewportCameraType::Top:
		m_rotationPlatform = v3f();
		break;
	case miViewportCameraType::UV:
		m_rotationPlatform = v3f();
		m_positionPlatform = v4f(0.5f, 0.f, 0.5f, 1.5f);
		break;
	}

	Rotate(0, 0);

	m_viewport->SetCameraType(m_viewport->m_cameraType);
	m_viewport->UpdateAspect();
	//m_viewport->OnWindowSize();

}

void miViewportCamera::PanMove() {
	f32 speed = 10.f * (m_positionPlatform.w*0.01f);

	v4f vec(
		speed * -g_app->m_inputContext->m_mouseDelta.x * g_app->m_dt,
		0.f, 
		speed * -g_app->m_inputContext->m_mouseDelta.y * g_app->m_dt,
		0.f);
	Mat4 MX(Quat(m_rotationPlatform.x, 0.f, 0.f));
	Mat4 MY(Quat(0.f, m_rotationPlatform.y, 0.f));
	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));
	vec = math::mul(vec, MY * MX);
	m_positionPlatform += vec;
}

void miViewportCamera::Rotate(f32 x, f32 y) {
	if (m_type == miViewportCameraType::UV) return;

	const f32 speed = 0.69f * g_app->m_dt;
	m_rotationPlatform.x += y * speed;
	m_rotationPlatform.y += x * speed;

	if (m_rotationPlatform.y < 0.f) m_rotationPlatform.y = m_rotationPlatform.y + math::PIPI;

	if (m_rotationPlatform.x > math::PIPI) m_rotationPlatform.x = 0.f;
	if (m_rotationPlatform.y > math::PIPI) m_rotationPlatform.y = 0.f;
	//if (m_rotationPlatform.x < -math::PIPI) m_rotationPlatform.x = 0.f;
	//if (m_rotationPlatform.y < -math::PIPI) m_rotationPlatform.y = 0.f;


	if(m_type != miViewportCameraType::Perspective)
		m_viewport->SetViewportName(L"Orthogonal");

	Update();
}

void miViewportCamera::Zoom() {
	f32 mult = 1.f;
	if (g_app->m_keyboardModifier == miKeyboardModifier::Shift)
		mult = 3.f;
	
	if(g_app->m_inputContext->m_wheelDelta > 0)
		m_positionPlatform.w *= 0.9f * (1.f/ mult);
	else
		m_positionPlatform.w *= 1.1f * mult;

	if (m_positionPlatform.w < 0.01f)
		m_positionPlatform.w = 0.01f;
	
	Update();
}

void miViewportCamera::ChangeFOV() {
	if (m_type == miViewportCameraType::UV) return;
	m_fov += g_app->m_inputContext->m_mouseDelta.x * g_app->m_dt;
	if (m_fov < 0.01f)
		m_fov = 0.01f;
	if (m_fov > math::PI)
		m_fov = math::PI;
	//printf("m_fov %f\n", m_fov);
}

void miViewportCamera::RotateZ() {
	if (m_type == miViewportCameraType::UV) return;
	m_rotationPlatform.z += g_app->m_inputContext->m_mouseDelta.x * g_app->m_dt;
}