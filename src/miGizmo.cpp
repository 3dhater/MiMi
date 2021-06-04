#include "yy.h"
#include "yy_model.h"
#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"


extern miApplication * g_app;

miGizmo::miGizmo() {
	m_isDrawAabbY = false;
	m_isDrawAabbHeadY = false;
	m_pivotModel = 0;
	
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
	m_HeadX = 0;
	m_HeadY = 0;
	m_HeadZ = 0;
	m_XZ = 0;
	m_XY = 0;
	m_ZY = 0;

	const f32 gizmo_arrow_body_size = 0.125f;
	const f32 gizmo_head_size = 0.01f;
	const f32 gizmo_head_len = 0.025f;
	const f32 gizmo_2pl_sz = 0.01f;
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
		m_XAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size, 0.f, 0.f);
		vertex->Color = ColorRed.getV4f();
		m_XAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		m_X = yyCreateModel(model);
		m_X->Load();

		f32 sz = 0.0025;
		m_XAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_XAabb.m_max += v4f(0.f, sz, sz, 0.f);
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
		m_YAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, gizmo_arrow_body_size, 0.f);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_YAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		m_Y = yyCreateModel(model);
		m_Y->Load();

		f32 sz = 0.0025;
		m_YAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_YAabb.m_max += v4f(sz, 0.f, sz, 0.f);
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
		m_ZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, 0.f, gizmo_arrow_body_size);
		vertex->Color = ColorLime.getV4f();
		m_ZAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		m_Z = yyCreateModel(model);
		m_Z->Load();

		f32 sz = 0.0025;
		m_ZAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_ZAabb.m_max += v4f(sz, sz, 0.f, 0.f);
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
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size - gizmo_head_len , -gizmo_head_size, -gizmo_head_size);
		vertex->Color = ColorRed.getV4f();
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size - gizmo_head_len , -gizmo_head_size, gizmo_head_size);
		vertex->Color = ColorRed.getV4f();
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size, gizmo_head_size);
		vertex->Color = ColorRed.getV4f();
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size, -gizmo_head_size);
		vertex->Color = ColorRed.getV4f();
		m_HeadXAabb.add(vertex->Position);
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

		m_HeadX = yyCreateModel(model);
		m_HeadX->Load();

		f32 sz = 0.0025;
		m_HeadXAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_HeadXAabb.m_max += v4f(0.f, sz, sz, 0.f);
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
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, -gizmo_head_size);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, -gizmo_head_size);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_HeadYAabb.add(vertex->Position);
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

		m_HeadY = yyCreateModel(model);
		m_HeadY->Load();

		f32 sz = 0.0025;
		m_HeadYAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_HeadYAabb.m_max += v4f(sz, 0.f, sz, 0.f);
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
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-gizmo_head_size, -gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex->Color = ColorLime.getV4f();
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-gizmo_head_size, gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex->Color = ColorLime.getV4f();
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_head_size, gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex->Color = ColorLime.getV4f();
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(gizmo_head_size, -gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex->Color = ColorLime.getV4f();
		m_HeadZAabb.add(vertex->Position);
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

		m_HeadZ = yyCreateModel(model);
		m_HeadZ->Load();

		f32 sz = 0.0025;
		m_HeadZAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_HeadZAabb.m_max += v4f(sz, sz, 0.f, 0.f);
	}
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 4;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 6;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		f32 xzsz = gizmo_arrow_body_size * 0.5f;
		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex->Position.set(xzsz - gizmo_2pl_sz, 0.f, xzsz - gizmo_2pl_sz);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_XZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xzsz - gizmo_2pl_sz, 0.f, xzsz + gizmo_2pl_sz);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_XZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xzsz + gizmo_2pl_sz, 0.f, xzsz + gizmo_2pl_sz);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_XZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xzsz + gizmo_2pl_sz, 0.f, xzsz - gizmo_2pl_sz);
		vertex->Color = ColorDodgerBlue.getV4f();
		m_XZAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;

		m_XZ = yyCreateModel(model);
		m_XZ->Load();

		f32 sz = 0.0025;
		m_XZAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_XZAabb.m_max += v4f(sz, 0.f, sz, 0.f);
	}
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 4;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 6;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		f32 xysz = gizmo_arrow_body_size * 0.5f;
		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex->Position.set(xysz - gizmo_2pl_sz, xysz - gizmo_2pl_sz, 0.f);
		vertex->Color = ColorLime.getV4f();
		m_XYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xysz - gizmo_2pl_sz, xysz + gizmo_2pl_sz, 0.f);
		vertex->Color = ColorLime.getV4f();
		m_XYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xysz + gizmo_2pl_sz, xysz + gizmo_2pl_sz, 0.f);
		vertex->Color = ColorLime.getV4f();
		m_XYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xysz + gizmo_2pl_sz, xysz - gizmo_2pl_sz, 0.f);
		vertex->Color = ColorLime.getV4f();
		m_XYAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;

		m_XY = yyCreateModel(model);
		m_XY->Load();

		f32 sz = 0.0025;
		m_XYAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_XYAabb.m_max += v4f(sz, sz, 0.f, 0.f);
	}
	{
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 4;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 6;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		f32 zysz = gizmo_arrow_body_size * 0.5f;
		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex->Position.set(0.f, zysz - gizmo_2pl_sz, zysz - gizmo_2pl_sz);
		vertex->Color = ColorRed.getV4f();
		m_ZYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, zysz + gizmo_2pl_sz, zysz - gizmo_2pl_sz);
		vertex->Color = ColorRed.getV4f();
		m_ZYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, zysz + gizmo_2pl_sz, zysz + gizmo_2pl_sz);
		vertex->Color = ColorRed.getV4f();
		m_ZYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, zysz - gizmo_2pl_sz, zysz + gizmo_2pl_sz);
		vertex->Color = ColorRed.getV4f();
		m_ZYAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)model->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;

		m_ZY = yyCreateModel(model);
		m_ZY->Load();

		f32 sz = 0.0025;
		m_ZYAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_ZYAabb.m_max += v4f(0.f, sz, sz, 0.f);
	}
}

miGizmo::~miGizmo() {
	if (m_pivotModel) yyMegaAllocator::Destroy(m_pivotModel);
	if (m_Y) yyMegaAllocator::Destroy(m_Y);
	if (m_X) yyMegaAllocator::Destroy(m_X);
	if (m_Z) yyMegaAllocator::Destroy(m_Z);
	if (m_HeadX) yyMegaAllocator::Destroy(m_HeadX);
	if (m_HeadY) yyMegaAllocator::Destroy(m_HeadY);
	if (m_HeadZ) yyMegaAllocator::Destroy(m_HeadZ);
	if (m_XZ) yyMegaAllocator::Destroy(m_XZ);
	if (m_XY) yyMegaAllocator::Destroy(m_XY);
	if (m_ZY) yyMegaAllocator::Destroy(m_ZY);
}

void miGizmo::Draw(miViewport* vp) {
	g_app->m_gpu->UseDepth(false);

	yySetMatrix(yyMatrixType::World, &m_W);

	m_WVP.identity();
	m_WVP = vp->m_activeCamera->m_projectionMatrix
		* vp->m_activeCamera->m_viewMatrix
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
		g_app->m_gpu->SetModel(m_X);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbX)
			g_app->DrawAabb(m_XAabbMod, ColorRed.getV4f());

		yySetMaterial(&m_YMaterial);
		g_app->m_gpu->SetModel(m_Y);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbY)
			g_app->DrawAabb(m_YAabbMod, ColorDodgerBlue.getV4f());
		
		g_app->m_gpu->SetModel(m_Z);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbZ)
			g_app->DrawAabb(m_ZAabbMod, ColorLime.getV4f());

		g_app->m_gpu->SetModel(m_HeadX);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbHeadX)
			g_app->DrawAabb(m_HeadXAabbMod, ColorRed.getV4f());

		g_app->m_gpu->SetModel(m_HeadY);
		g_app->m_gpu->Draw();
		if(m_isDrawAabbHeadY)
			g_app->DrawAabb(m_HeadYAabbMod, ColorDodgerBlue.getV4f());

		g_app->m_gpu->SetModel(m_HeadZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbHeadZ)
			g_app->DrawAabb(m_HeadZAabbMod, ColorLime.getV4f());

		g_app->m_gpu->SetModel(m_XZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbXZ)
			g_app->DrawAabb(m_XZAabbMod, ColorDodgerBlue.getV4f());

		g_app->m_gpu->SetModel(m_XY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbXY)
			g_app->DrawAabb(m_XYAabbMod, ColorLime.getV4f());

		g_app->m_gpu->SetModel(m_ZY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbZY)
			g_app->DrawAabb(m_ZYAabbMod, ColorRed.getV4f());
		break;
	case miTransformMode::Scale:
		g_app->m_gpu->SetModel(m_X);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbX)
			g_app->DrawAabb(m_XAabbMod, ColorRed.getV4f());

		yySetMaterial(&m_YMaterial);
		g_app->m_gpu->SetModel(m_Y);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbY)
			g_app->DrawAabb(m_YAabbMod, ColorDodgerBlue.getV4f());

		g_app->m_gpu->SetModel(m_Z);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbZ)
			g_app->DrawAabb(m_ZAabbMod, ColorLime.getV4f());

		g_app->m_gpu->SetModel(m_XZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbXZ)
			g_app->DrawAabb(m_XZAabbMod, ColorDodgerBlue.getV4f());

		g_app->m_gpu->SetModel(m_XY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbXY)
			g_app->DrawAabb(m_XYAabbMod, ColorLime.getV4f());

		g_app->m_gpu->SetModel(m_ZY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbZY)
			g_app->DrawAabb(m_ZYAabbMod, ColorRed.getV4f());
		break;
	case miTransformMode::Rotate:
		break;
	}
}

bool miGizmo::Update(miViewport* vp) {
	//g_app->m_selectionAabb_center;
	m_S.identity();
	m_S.setScale(v3f(vp->m_activeCamera->m_positionPlatform.w / ((1.f / 600.f) * vp->m_currentRectSize.y)));

	m_T.identity();
	m_T.setTranslation(g_app->m_selectionAabb_center);

	m_W = m_T * m_S;

	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::Rotate:
	case miTransformMode::NoTransform:
		break;
	case miTransformMode::Move:
	case miTransformMode::Scale:
		m_XAabbMod.m_min = math::mul(m_XAabb.m_min, m_S);
		m_XAabbMod.m_max = math::mul(m_XAabb.m_max, m_S);
		m_XAabbMod.m_max += g_app->m_selectionAabb_center;
		m_XAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_XAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbX = true;
		}
		else
		{
			m_isDrawAabbX = false;
		}

		m_YAabbMod.m_min = math::mul(m_YAabb.m_min, m_S);
		m_YAabbMod.m_max = math::mul(m_YAabb.m_max, m_S);
		m_YAabbMod.m_max += g_app->m_selectionAabb_center;
		m_YAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_YAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbY = true;
		}
		else
		{
			m_isDrawAabbY = false;
		}

		m_ZAabbMod.m_min = math::mul(m_ZAabb.m_min, m_S);
		m_ZAabbMod.m_max = math::mul(m_ZAabb.m_max, m_S);
		m_ZAabbMod.m_max += g_app->m_selectionAabb_center;
		m_ZAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_ZAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbZ = true;
		}
		else
		{
			m_isDrawAabbZ = false;
		}

		m_XZAabbMod.m_min = math::mul(m_XZAabb.m_min, m_S);
		m_XZAabbMod.m_max = math::mul(m_XZAabb.m_max, m_S);
		m_XZAabbMod.m_max += g_app->m_selectionAabb_center;
		m_XZAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_XZAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbXZ = true;
		}
		else
		{
			m_isDrawAabbXZ = false;
		}

		m_XYAabbMod.m_min = math::mul(m_XYAabb.m_min, m_S);
		m_XYAabbMod.m_max = math::mul(m_XYAabb.m_max, m_S);
		m_XYAabbMod.m_max += g_app->m_selectionAabb_center;
		m_XYAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_XYAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbXY = true;
		}
		else
		{
			m_isDrawAabbXY = false;
		}

		m_ZYAabbMod.m_min = math::mul(m_ZYAabb.m_min, m_S);
		m_ZYAabbMod.m_max = math::mul(m_ZYAabb.m_max, m_S);
		m_ZYAabbMod.m_max += g_app->m_selectionAabb_center;
		m_ZYAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_ZYAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbZY = true;
		}
		else
		{
			m_isDrawAabbZY = false;
		}
		break;
	}

	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::NoTransform:
		break;
	case miTransformMode::Scale:
	case miTransformMode::Move:
		m_HeadXAabbMod.m_min = math::mul(m_HeadXAabb.m_min, m_S);
		m_HeadXAabbMod.m_max = math::mul(m_HeadXAabb.m_max, m_S);
		m_HeadXAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadXAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadXAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbHeadX = true;
		}
		else
		{
			m_isDrawAabbHeadX = false;
		}

		m_HeadYAabbMod.m_min = math::mul(m_HeadYAabb.m_min, m_S);
		m_HeadYAabbMod.m_max = math::mul(m_HeadYAabb.m_max, m_S);
		m_HeadYAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadYAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadYAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbHeadY = true;
		}
		else
		{
			m_isDrawAabbHeadY = false;
		}

		m_HeadZAabbMod.m_min = math::mul(m_HeadZAabb.m_min, m_S);
		m_HeadZAabbMod.m_max = math::mul(m_HeadZAabb.m_max, m_S);
		m_HeadZAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadZAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadZAabbMod.rayTest(g_app->m_rayCursor))
		{
			m_isDrawAabbHeadZ = true;
		}
		else
		{
			m_isDrawAabbHeadZ = false;
		}

		break;
	case miTransformMode::Rotate:
		break;
	}
	

	return false;
}