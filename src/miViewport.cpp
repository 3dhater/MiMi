#include "miApplication.h"
#include "miViewport.h"
#include "miViewportCamera.h"

extern miApplication * g_app;
extern Mat4 g_emptyMatrix;

miViewport::miViewport(){
	m_cameraPerspective = new miViewportCamera;
	m_activeCamera = m_cameraPerspective;
	m_creationRect = v4f(0.f, 0.f, 1.f, 1.f);
	m_currentRect = m_creationRect;
	m_currentRectSize = v2f(800.f, 600.f);

	OnWindowSize();
}

miViewport::~miViewport(){
	if (m_cameraPerspective) delete m_cameraPerspective;
}

void miViewport::OnWindowSize() {
	f32 windowSizeX_1 = 1.f / g_app->m_window->m_currentSize.x;
	f32 windowSizeY_1 = 1.f / g_app->m_window->m_currentSize.y;

	m_currentRect = m_creationRect;
	
	if (m_creationRect.x > 0.f) m_currentRect.x = m_creationRect.x / windowSizeX_1;
	if (m_creationRect.z > 0.f) m_currentRect.z = m_creationRect.z / windowSizeX_1;
	if (m_creationRect.y > 0.f) m_currentRect.y = m_creationRect.y / windowSizeY_1;
	if (m_creationRect.w > 0.f) m_currentRect.w = m_creationRect.w / windowSizeY_1;


	m_currentRectSize.x = m_currentRect.z - m_currentRect.x;
	m_currentRectSize.y = m_currentRect.w - m_currentRect.y;
	m_activeCamera->m_aspect = m_currentRectSize.x / m_currentRectSize.y;

	/*printf("%f %f %f %f - %f\n", 
		m_currentRect.x, m_currentRect.y, m_currentRect.z, m_currentRect.w, windowSizeX_1);*/
}

void miViewport::OnDraw(yyVideoDriverAPI* gpu) {
	m_activeCamera->Update();

	gpu->SetScissorRect(m_currentRect, g_app->m_window);
	gpu->SetViewport(m_currentRect.x, m_currentRect.y, m_currentRectSize.x, m_currentRectSize.y, g_app->m_window);

	bool isCameraLowerThanWorld = false;
	if (m_activeCamera->m_positionCamera.y < 0.f)
		isCameraLowerThanWorld = true;

	gpu->SetMatrix(yyVideoDriverAPI::MatrixType::View, m_activeCamera->m_viewMatrix);
	gpu->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, m_activeCamera->m_projectionMatrix);
	gpu->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection,
		m_activeCamera->m_projectionMatrix * m_activeCamera->m_viewMatrix);

	/*m_gpu->DrawLine3D(v4f(-10.f, 0.f, 0.f, 0.f), v4f(10.f, 0.f, 0.f, 0.f), ColorRed);
	m_gpu->DrawLine3D(v4f(0.f, -10.f, 0.f, 0.f), v4f(0.f, 10.f, 0.f, 0.f), ColorGreen);
	m_gpu->DrawLine3D(v4f(0.f, 0.f, -10.f, 0.f), v4f(0.f, 0.f, 10.f, 0.f), ColorBlue);*/

	gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, g_emptyMatrix);
	gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection,
		m_activeCamera->m_projectionMatrix * m_activeCamera->m_viewMatrix * g_emptyMatrix);
	if (isCameraLowerThanWorld)
		gpu->SetModel(g_app->m_gridModel_perspective2);
	else
		gpu->SetModel(g_app->m_gridModel_perspective1);
	gpu->SetMaterial(&g_app->m_gridModelMaterial);
	gpu->Draw();
}