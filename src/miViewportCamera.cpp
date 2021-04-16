#include "miApplication.h"
#include "miViewportCamera.h"

extern miApplication * g_app;

miViewportCamera::miViewportCamera(miViewport* vp, miViewportCameraType ct){
	m_viewport = vp;
	m_type = ct;
	Reset();
}

miViewportCamera::~miViewportCamera(){}

void miViewportCamera::Update() {
	switch (m_type)
	{
	case miViewportCameraType::Perspective:
		math::makePerspectiveRHMatrix(m_projectionMatrix, m_fov, m_aspect, m_near, m_far);
	break;
	default:
		f32 zoom = m_positionPlatform.w;

		math::makeOrthoRHMatrix(m_projectionMatrix, 
			zoom * m_aspect,
			zoom,
			m_near, m_far);
		break;
	}


	Mat4 MX(Quat(m_rotationPlatform.x, 0.f, 0.f));
	Mat4 MY(Quat(0.f, m_rotationPlatform.y, 0.f));
	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));

	m_positionCamera = v3f(0.f, m_positionPlatform.w, 0.f);
	m_positionCamera = math::mul(m_positionCamera, (MY * MX));
	m_positionCamera += v3f(m_positionPlatform.x, m_positionPlatform.y, m_positionPlatform.z);

	Mat4 T;
	T.m_data[3] = m_positionCamera;

	Mat4 P(Quat(v4f(-m_rotationPlatform.x + math::degToRad(90.f), 0.f, 0.f, 1.f)));
	Mat4 Y(Quat(v4f(0.f, -m_rotationPlatform.y, 0.f, 1.f)));
	Mat4 R(Quat(v4f(0.f, 0.f, -m_rotationPlatform.z, 1.f)));

	m_viewMatrix = (R*(P * Y)) * T;
}

void miViewportCamera::Reset() {
	m_near = 0.01f;
	m_far  = 1000.f;
	m_fov  = math::degToRad(90.f);
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
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(-180.f), 0.f);
		break;
	case miViewportCameraType::Right:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(180.f), 0.f);
		break;
	case miViewportCameraType::Back:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(0.f), 0.f);
		break;
	case miViewportCameraType::Front:
		m_rotationPlatform = v3f(math::degToRad(-90.f), math::degToRad(180.f), 0.f);
		break;
	default:
		m_near = -m_far;
		break;
	}
}

void miViewportCamera::PanMove() {

	f32 speed = 30.f * (m_positionPlatform.w*0.01f);

	v4f vec(
		speed * g_app->m_inputContext->m_mouseDelta.x * g_app->m_dt,
		0.f, 
		speed * -g_app->m_inputContext->m_mouseDelta.y * g_app->m_dt,
		0.f);
	Mat4 MX(Quat(m_rotationPlatform.x, 0.f, 0.f));
	Mat4 MY(Quat(0.f, m_rotationPlatform.y, 0.f));
	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));
	vec = math::mul(vec, MY * MX);
	m_positionPlatform += vec;
}

void miViewportCamera::Rotate() {
	m_rotationPlatform.x += g_app->m_inputContext->m_mouseDelta.y * g_app->m_dt;
	m_rotationPlatform.y += -g_app->m_inputContext->m_mouseDelta.x * g_app->m_dt;
	if (m_rotationPlatform.x > math::PIPI) m_rotationPlatform.x = 0.f;
	if (m_rotationPlatform.y > math::PIPI) m_rotationPlatform.y = 0.f;
	if (m_rotationPlatform.x < -math::PIPI) m_rotationPlatform.x = 0.f;
	if (m_rotationPlatform.y < -math::PIPI) m_rotationPlatform.y = 0.f;
}

void miViewportCamera::Zoom() {
	f32 mult = 1.f;
	if (g_app->m_keyboardModifier == miKeyboardModifier::Shift)
		mult = 20.f;
	m_positionPlatform.w -= g_app->m_inputContext->m_wheelDelta * mult;
	if (m_positionPlatform.w < 0.01f)
		m_positionPlatform.w = 0.01f;
}

void miViewportCamera::ChangeFOV() {
	m_fov += g_app->m_inputContext->m_mouseDelta.x * g_app->m_dt;
	if (m_fov < 0.01f)
		m_fov = 0.01f;
	if (m_fov > math::PI)
		m_fov = math::PI;
}

void miViewportCamera::RotateZ() {
	m_rotationPlatform.z += g_app->m_inputContext->m_mouseDelta.x * g_app->m_dt;
}