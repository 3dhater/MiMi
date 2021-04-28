#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miVisualObjectImpl.h"

extern miApplication * g_app;

void miMatrixToMat4(const miMatrix& m1, const Mat4& m2) {
	auto p1 = m1.getPtrConst();
	auto p2 = m2.getPtrConst();
	p2[0] = p1[0];
	p2[1] = p1[1];
	p2[2] = p1[2];
	p2[3] = p1[3];
	p2[4] = p1[4];
	p2[5] = p1[5];
	p2[6] = p1[6];
	p2[7] = p1[7];
	p2[8] = p1[8];
	p2[9] = p1[9];
	p2[10] = p1[10];
	p2[11] = p1[11];
	p2[12] = p1[12];
	p2[13] = p1[13];
	p2[14] = p1[14];
	p2[15] = p1[15];
}

miVisualObjectImpl::miVisualObjectImpl() {
	m_modelGPU = 0;
}

miVisualObjectImpl::~miVisualObjectImpl() {
	if (m_modelGPU)
	{
		yyMegaAllocator::Destroy(m_modelGPU);
	}
}

void miVisualObjectImpl::Draw() {
	if (!m_modelGPU)
		return;

	auto camera = g_app->GetActiveCamera();

	Mat4 World;
	miMatrixToMat4(m_transform, World);

	g_app->m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, 
		camera->m_projectionMatrix * camera->m_viewMatrix * World);
	g_app->m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, World);
	g_app->m_gpu->SetModel(m_modelGPU);
	g_app->m_gpu->Draw();
}

miMatrix* miVisualObjectImpl::GetTransform() {
	return &m_transform;
}
