#include "yy.h"
#include "yy_model.h"
#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"


extern miApplication * g_app;

miGizmo::miGizmo() {
	m_pivotModel = 0;
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_stride = sizeof(yyVertexLine);
		model->m_vertexType = yyVertexType::LineModel;
		model->m_vCount = 4;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 6;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)model->m_vertices;
		vertex->Position.set(0.f, 0.f, 0.f);
		vertex->Color = ColorWhite.getV4f();
		vertex++;
		vertex->Position.set(1.f, 0.f, 0.f);
		vertex->Color = ColorWhite.getV4f();
		vertex++;
		vertex->Position.set(0.f, 1.f, 0.f);
		vertex->Color = ColorWhite.getV4f();
		vertex++;
		vertex->Position.set(0.f, 0.f, 1.f);
		vertex->Color = ColorWhite.getV4f();

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 3; index++;

		m_pivotModel = yyCreateModel(model);
		m_pivotModel->Load();
	}
}

miGizmo::~miGizmo() {
	if (m_pivotModel) yyMegaAllocator::Destroy(m_pivotModel);
}

void miGizmo::Draw() {
	m_W.identity();
	m_W.setTranslation(g_app->m_selectionAabb_center);

	yySetMatrix(yyMatrixType::World, &m_W);
	
	m_WVP.identity();
	m_WVP = g_app->m_currentViewportDraw->m_activeCamera->m_projectionMatrix 
		* g_app->m_currentViewportDraw->m_activeCamera->m_viewMatrix 
		* m_W;
	yySetMatrix(yyMatrixType::WorldViewProjection, &m_WVP);

	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::NoTransform:
		g_app->m_gpu->SetModel(m_pivotModel);
		g_app->m_gpu->Draw();
		break;
	case miTransformMode::Move:
		break;
	case miTransformMode::Scale:
		break;
	case miTransformMode::Rotate:
		break;
	}
}

bool miGizmo::Update() {
	//g_app->m_selectionAabb_center;

	return false;
}