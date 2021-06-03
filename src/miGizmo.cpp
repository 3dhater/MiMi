#include "yy.h"
#include "yy_model.h"
#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"


extern miApplication * g_app;

miGizmo::miGizmo() {
	m_isDrawAabbMoveBodyY = false;
	m_isDrawAabbMoveHeadY = false;
	m_zoomScaleValue = 0.f;
	m_pivotModel = 0;
	
	m_moveModelX = 0;
	m_moveModelY = 0;
	m_moveModelZ = 0;
	m_moveModelHeadX = 0;
	m_moveModelHeadY = 0;
	m_moveModelHeadZ = 0;

	const f32 gizmo_arrow_body_size = 0.125f;
	const f32 gizmo_head_size = 0.0125f;
	const f32 gizmo_head_len = 0.025f;
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
		vertex->Position.set(gizmo_arrow_body_size, 0.f, 0.f);
		vertex->Color = ColorWhite.getV4f();
		vertex++;
		vertex->Position.set(0.f, gizmo_arrow_body_size, 0.f);
		vertex->Color = ColorWhite.getV4f();
		vertex++;
		vertex->Position.set(0.f, 0.f, gizmo_arrow_body_size);
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
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_stride = sizeof(yyVertexLine);
		model->m_vertexType = yyVertexType::LineModel;
		model->m_vCount = 2;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 2;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)model->m_vertices;
		vertex->Position.set(0.f, 0.f, 0.f);
		vertex->Color = ColorRed.getV4f();
		m_moveBodyModelXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size, 0.f, 0.f);
		vertex->Color = ColorRed.getV4f();
		m_moveBodyModelXAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		m_moveModelX = yyCreateModel(model);
		m_moveModelX->Load();

		f32 sz = 0.0025;
		m_moveBodyModelXAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_moveBodyModelXAabb.m_max += v4f(0.f, sz, sz, 0.f);
	}
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_stride = sizeof(yyVertexLine);
		model->m_vertexType = yyVertexType::LineModel;
		model->m_vCount = 2;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 2;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)model->m_vertices;
		vertex->Position.set(0.f, 0.f, 0.f);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_moveBodyModelYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, gizmo_arrow_body_size, 0.f);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_moveBodyModelYAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		m_moveModelY = yyCreateModel(model);
		m_moveModelY->Load();

		f32 sz = 0.0025;
		m_moveBodyModelYAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_moveBodyModelYAabb.m_max += v4f(sz, 0.f, sz, 0.f);
	}
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_stride = sizeof(yyVertexLine);
		model->m_vertexType = yyVertexType::LineModel;
		model->m_vCount = 2;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 2;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)model->m_vertices;
		vertex->Position.set(0.f, 0.f, 0.f);
		vertex->Color = ColorLime.getV4f();
		m_moveBodyModelZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, 0.f, gizmo_arrow_body_size);
		vertex->Color = ColorLime.getV4f();
		m_moveBodyModelZAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		m_moveModelZ = yyCreateModel(model);
		m_moveModelZ->Load();

		f32 sz = 0.0025;
		m_moveBodyModelZAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_moveBodyModelZAabb.m_max += v4f(sz, sz, 0.f, 0.f);
	}
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 5;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 12;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex->Position.set(gizmo_arrow_body_size, 0.f, 0.f);
		vertex->Color = ColorRed.getV4f();
		m_moveHeadModelXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size - gizmo_head_len , -gizmo_head_size, -gizmo_head_size);
		vertex->Color = ColorRed.getV4f();
		m_moveHeadModelXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size - gizmo_head_len , -gizmo_head_size, gizmo_head_size);
		vertex->Color = ColorRed.getV4f();
		m_moveHeadModelXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size, gizmo_head_size);
		vertex->Color = ColorRed.getV4f();
		m_moveHeadModelXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size, -gizmo_head_size);
		vertex->Color = ColorRed.getV4f();
		m_moveHeadModelXAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;
		*index = 0; index++;
		*index = 3; index++;
		*index = 4; index++;
		*index = 0; index++;
		*index = 4; index++;
		*index = 1; index++;

		m_moveModelHeadX = yyCreateModel(model);
		m_moveModelHeadX->Load();

		f32 sz = 0.0025;
		m_moveHeadModelXAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_moveHeadModelXAabb.m_max += v4f(0.f, sz, sz, 0.f);
	}
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 5;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 12;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex->Position.set(0.f, gizmo_arrow_body_size, 0.f);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_moveHeadModelYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, -gizmo_head_size);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_moveHeadModelYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_moveHeadModelYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_moveHeadModelYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, -gizmo_head_size);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_moveHeadModelYAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;
		*index = 0; index++;
		*index = 3; index++;
		*index = 4; index++;
		*index = 0; index++;
		*index = 4; index++;
		*index = 1; index++;

		m_moveModelHeadY = yyCreateModel(model);
		m_moveModelHeadY->Load();

		f32 sz = 0.0025;
		m_moveHeadModelYAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_moveHeadModelYAabb.m_max += v4f(sz, 0.f, sz, 0.f);
	}
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 5;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 12;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex->Position.set(0.f, 0.f, gizmo_arrow_body_size);
		vertex->Color = ColorLime.getV4f();
		m_moveHeadModelZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-gizmo_head_size, -gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex->Color = ColorLime.getV4f();
		m_moveHeadModelZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-gizmo_head_size, gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex->Color = ColorLime.getV4f();
		m_moveHeadModelZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_head_size, gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex->Color = ColorLime.getV4f();
		m_moveHeadModelZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_head_size, -gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex->Color = ColorLime.getV4f();
		m_moveHeadModelZAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;
		*index = 0; index++;
		*index = 3; index++;
		*index = 4; index++;
		*index = 0; index++;
		*index = 4; index++;
		*index = 1; index++;

		m_moveModelHeadZ = yyCreateModel(model);
		m_moveModelHeadZ->Load();

		f32 sz = 0.0025;
		m_moveHeadModelZAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_moveHeadModelZAabb.m_max += v4f(sz, sz, 0.f, 0.f);
	}
}

miGizmo::~miGizmo() {
	if (m_pivotModel) yyMegaAllocator::Destroy(m_pivotModel);
	if (m_moveModelY) yyMegaAllocator::Destroy(m_moveModelY);
	if (m_moveModelX) yyMegaAllocator::Destroy(m_moveModelX);
	if (m_moveModelZ) yyMegaAllocator::Destroy(m_moveModelZ);
	if (m_moveModelHeadX) yyMegaAllocator::Destroy(m_moveModelHeadX);
	if (m_moveModelHeadY) yyMegaAllocator::Destroy(m_moveModelHeadY);
	if (m_moveModelHeadZ) yyMegaAllocator::Destroy(m_moveModelHeadZ);
}

void miGizmo::ClaculateZoomScaleValue(miViewportCamera* vc) {
	if (!g_app->m_currentViewportDraw) return;
	m_zoomScaleValue = vc->m_positionPlatform.w;
}
void miGizmo::SetZoomScaleValue(f32 v) {
	if (!g_app->m_currentViewportDraw) return;
	m_zoomScaleValue = v;
	m_zoomScaleValueCameraDistance = v;
}

void miGizmo::Draw() {
	g_app->m_gpu->UseDepth(false);

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
		yySetMaterial(&m_pivotModelMaterial);
		g_app->m_gpu->SetModel(m_pivotModel);
		g_app->m_gpu->Draw();
		break;
	case miTransformMode::Move:
		//m_moveBodyModelYMaterial.m_baseColor = ColorDodgerBlue;
		g_app->m_gpu->SetModel(m_moveModelX);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbMoveBodyX)
			g_app->DrawAabb(m_moveBodyModelXAabbMod, ColorRed.getV4f());

		yySetMaterial(&m_moveModelYMaterial);
		g_app->m_gpu->SetModel(m_moveModelY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbMoveBodyY)
			g_app->DrawAabb(m_moveBodyModelYAabbMod, ColorDodgerBlue.getV4f());
		
		g_app->m_gpu->SetModel(m_moveModelZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbMoveBodyZ)
			g_app->DrawAabb(m_moveBodyModelZAabbMod, ColorLime.getV4f());

		g_app->m_gpu->SetModel(m_moveModelHeadX);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbMoveHeadX)
			g_app->DrawAabb(m_moveHeadModelXAabbMod, ColorRed.getV4f());

		g_app->m_gpu->SetModel(m_moveModelHeadY);
		g_app->m_gpu->Draw();
		if(m_isDrawAabbMoveHeadY)
			g_app->DrawAabb(m_moveHeadModelYAabbMod, ColorDodgerBlue.getV4f());

		g_app->m_gpu->SetModel(m_moveModelHeadZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbMoveHeadZ)
			g_app->DrawAabb(m_moveHeadModelZAabbMod, ColorLime.getV4f());
		break;
	case miTransformMode::Scale:
		break;
	case miTransformMode::Rotate:
		break;
	}
}

bool miGizmo::Update() {
	//g_app->m_selectionAabb_center;
	m_S.identity();
	m_S.setScale(v3f(m_zoomScaleValue));

	m_T.identity();
	m_T.setTranslation(g_app->m_selectionAabb_center);

	m_W = m_T * m_S;

	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::NoTransform:
		break;
	case miTransformMode::Move:
		m_moveBodyModelXAabbMod.m_min = math::mul(m_moveBodyModelXAabb.m_min, m_S);
		m_moveBodyModelXAabbMod.m_max = math::mul(m_moveBodyModelXAabb.m_max, m_S);
		m_moveBodyModelXAabbMod.m_max += g_app->m_selectionAabb_center;
		m_moveBodyModelXAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_moveBodyModelXAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbMoveBodyX = true;
		}
		else
		{
			m_isDrawAabbMoveBodyX = false;
		}

		m_moveBodyModelYAabbMod.m_min = math::mul(m_moveBodyModelYAabb.m_min, m_S);
		m_moveBodyModelYAabbMod.m_max = math::mul(m_moveBodyModelYAabb.m_max, m_S);
		m_moveBodyModelYAabbMod.m_max += g_app->m_selectionAabb_center;
		m_moveBodyModelYAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_moveBodyModelYAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbMoveBodyY = true;
		}
		else
		{
			m_isDrawAabbMoveBodyY = false;
		}

		m_moveBodyModelZAabbMod.m_min = math::mul(m_moveBodyModelZAabb.m_min, m_S);
		m_moveBodyModelZAabbMod.m_max = math::mul(m_moveBodyModelZAabb.m_max, m_S);
		m_moveBodyModelZAabbMod.m_max += g_app->m_selectionAabb_center;
		m_moveBodyModelZAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_moveBodyModelZAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbMoveBodyZ = true;
		}
		else
		{
			m_isDrawAabbMoveBodyZ = false;
		}

		m_moveHeadModelXAabbMod.m_min = math::mul(m_moveHeadModelXAabb.m_min, m_S);
		m_moveHeadModelXAabbMod.m_max = math::mul(m_moveHeadModelXAabb.m_max, m_S);
		m_moveHeadModelXAabbMod.m_max += g_app->m_selectionAabb_center;
		m_moveHeadModelXAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_moveHeadModelXAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbMoveHeadX = true;
		}
		else
		{
			m_isDrawAabbMoveHeadX = false;
		}

		m_moveHeadModelYAabbMod.m_min = math::mul(m_moveHeadModelYAabb.m_min, m_S);
		m_moveHeadModelYAabbMod.m_max = math::mul(m_moveHeadModelYAabb.m_max, m_S);
		m_moveHeadModelYAabbMod.m_max += g_app->m_selectionAabb_center;
		m_moveHeadModelYAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_moveHeadModelYAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbMoveHeadY = true;
		}
		else
		{
			m_isDrawAabbMoveHeadY = false;
		}

		m_moveHeadModelZAabbMod.m_min = math::mul(m_moveHeadModelZAabb.m_min, m_S);
		m_moveHeadModelZAabbMod.m_max = math::mul(m_moveHeadModelZAabb.m_max, m_S);
		m_moveHeadModelZAabbMod.m_max += g_app->m_selectionAabb_center;
		m_moveHeadModelZAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_moveHeadModelZAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbMoveHeadZ = true;
		}
		else
		{
			m_isDrawAabbMoveHeadZ = false;
		}

		break;
	case miTransformMode::Scale:
		break;
	case miTransformMode::Rotate:
		break;
	}
	

	return false;
}