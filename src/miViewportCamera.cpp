#include "miApplication.h"
#include "miViewportCamera.h"

extern miApplication * g_app;

miViewportCamera::miViewportCamera(){
	m_type = miViewportCameraType::Perspective;
	Reset();
}

miViewportCamera::~miViewportCamera(){}

void miViewportCamera::Update() {
	switch (m_type)
	{
	case miViewportCameraType::Perspective:
	{
		math::makePerspectiveRHMatrix(m_projectionMatrix, m_fov, m_aspect, m_near, m_far);

		m_positionCamera = v3f(0.f, m_positionPlatform.w, 0.f);

		Mat4 MX(Quat(m_rotationPlatform.x, 0.f, 0.f));
		Mat4 MY(Quat(0.f, m_rotationPlatform.y, 0.f));
		//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));

		m_positionCamera = math::mul(m_positionCamera, (MY * MX));
		m_positionCamera += v3f(m_positionPlatform.x, m_positionPlatform.y, m_positionPlatform.z);

		Mat4 T;
		T.m_data[3] = m_positionCamera;

		Mat4 P(Quat(v4f(-m_rotationPlatform.x + math::degToRad(90.f), 0.f, 0.f, 1.f)));
		Mat4 Y(Quat(v4f(0.f, -m_rotationPlatform.y, 0.f, 1.f)));
		Mat4 R(Quat(v4f(0.f, 0.f, -m_rotationPlatform.z, 1.f)));

		m_viewMatrix = (R*(P * Y)) * T;
	}break;
	default:
		break;
	}
}

void miViewportCamera::Reset() {
	m_near = 0.01f;
	m_far  = 1000.f;
	m_fov  = math::degToRad(90.f);
	m_aspect = 800.f / 600.f;

	m_positionPlatform = v4f(0.f, 0.f, 0.f, 15.f);
	m_rotationPlatform = v3f(math::degToRad(-45.f), 0.f, 0.f );

	switch (m_type)
	{
	case miViewportCameraType::Perspective:
	{

	}break;
	default:
		break;
	}
}

void miViewportCamera::PanMove() {
	v4f vec(
		g_app->m_inputContext->m_mouseDelta.x * g_app->m_dt, 
		0.f, 
		-g_app->m_inputContext->m_mouseDelta.y * g_app->m_dt,
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
}

void miViewportCamera::Zoom() {
	m_positionPlatform.w -= g_app->m_inputContext->m_wheelDelta;
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