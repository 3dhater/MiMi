#include "miApplication.h"
#include "miViewportCamera.h"

extern miApplication * g_app;

miViewportCamera::miViewportCamera(){
	m_type = miViewportCameraType::Perspective;
	Reset();
}

miViewportCamera::~miViewportCamera(){}

void miViewportCamera::Update() {
	/*static f32 x = 0.01f;
	x += 0.01f;
	printf("%f\n",x);*/
	//m_rotationPlatform.y += 0.01f;
	switch (m_type)
	{
	case miViewportCameraType::Perspective:
	{
		math::makePerspectiveRHMatrix(m_projectionMatrix, m_fov, m_aspect, m_near, m_far);

		v3f newCameraPosition = v3f(0.f, m_positionPlatform.w, 0.f);

		Mat4 MX(Quat(m_rotationPlatform.x, 0.f, 0.f));
		Mat4 MY(Quat(0.f, m_rotationPlatform.y, 0.f));

		newCameraPosition = math::mul(newCameraPosition, MX);
		newCameraPosition = math::mul(newCameraPosition, MY);
		newCameraPosition += v3f(m_positionPlatform.x, m_positionPlatform.y, m_positionPlatform.z);

		Mat4 T;
		T.m_data[3] = newCameraPosition;

		Mat4 P(Quat(v4f(-m_rotationPlatform.x + math::degToRad(90.f), 0.f, 0.f, 1.f)));
		Mat4 Y(Quat(v4f(0.f, -m_rotationPlatform.y, 0.f, 1.f)));

		m_viewMatrix = (P*Y) * T;
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