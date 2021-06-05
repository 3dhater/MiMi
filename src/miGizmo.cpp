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
	m_HeadMoveX = 0;
	m_HeadMoveY = 0;
	m_HeadMoveZ = 0;
	m_HeadScaleX = 0;
	m_HeadScaleY = 0;
	m_HeadScaleZ = 0;
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

		m_HeadMoveX = yyCreateModel(model);
		m_HeadMoveX->Load();

		f32 sz = 0.0025;
		m_HeadXAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_HeadXAabb.m_max += v4f(0.f, sz, sz, 0.f);
	}
	{// Scale X
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 8;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 30;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex[0].Position.set(gizmo_arrow_body_size, -gizmo_head_size, -gizmo_head_size);
		vertex[1].Position.set(gizmo_arrow_body_size, -gizmo_head_size, gizmo_head_size);
		vertex[2].Position.set(gizmo_arrow_body_size, gizmo_head_size, gizmo_head_size);
		vertex[3].Position.set(gizmo_arrow_body_size, gizmo_head_size, -gizmo_head_size);
		vertex[4].Position.set(gizmo_arrow_body_size - gizmo_head_len, -gizmo_head_size, -gizmo_head_size);
		vertex[5].Position.set(gizmo_arrow_body_size - gizmo_head_len, -gizmo_head_size, gizmo_head_size);
		vertex[6].Position.set(gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size, gizmo_head_size);
		vertex[7].Position.set(gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size, -gizmo_head_size);

		for (int i = 0; i < 8; ++i) {
			vertex[i].Color = ColorRed.getV4f();
			m_HeadScaleXAabb.add(vertex[i].Position);
		}

		u16* index = (u16*)model->m_indices;
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 0;
		index[4] = 2;
		index[5] = 3;

		index[6] = 0;
		index[7] = 4;
		index[8] = 5;
		index[9] = 0;
		index[10] = 5;
		index[11] = 1;

		index[12] = 0;
		index[13] = 4;
		index[14] = 7;
		index[15] = 0;
		index[16] = 7;
		index[17] = 3;

		index[18] = 3;
		index[19] = 7;
		index[20] = 6;
		index[21] = 3;
		index[22] = 6;
		index[23] = 2;

		index[24] = 1;
		index[25] = 5;
		index[26] = 6;
		index[27] = 1;
		index[28] = 6;
		index[29] = 2;

		m_HeadScaleX = yyCreateModel(model);
		m_HeadScaleX->Load();

		f32 sz = 0.0025;
		m_HeadScaleXAabb.m_min -= v4f(sz, sz, sz, 0.f);
		m_HeadScaleXAabb.m_max += v4f(sz, sz, sz, 0.f);
	}
	{// Head Scale Y
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 8;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 30;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex[0].Position.set(-gizmo_head_size, gizmo_arrow_body_size , -gizmo_head_size);
		vertex[1].Position.set(-gizmo_head_size, gizmo_arrow_body_size, gizmo_head_size);
		vertex[2].Position.set(gizmo_head_size, gizmo_arrow_body_size, gizmo_head_size);
		vertex[3].Position.set(gizmo_head_size, gizmo_arrow_body_size , -gizmo_head_size);
		vertex[4].Position.set(-gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, -gizmo_head_size);
		vertex[5].Position.set(-gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size);
		vertex[6].Position.set(gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len, gizmo_head_size);
		vertex[7].Position.set(gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len , -gizmo_head_size);

		for (int i = 0; i < 8; ++i) {
			vertex[i].Color = ColorDodgerBlue.getV4f();
			m_HeadScaleYAabb.add(vertex[i].Position);
		}

		u16* index = (u16*)model->m_indices;
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 0;
		index[4] = 2;
		index[5] = 3;

		index[6] = 0;
		index[7] = 4;
		index[8] = 5;
		index[9] = 0;
		index[10] = 5;
		index[11] = 1;

		index[12] = 0;
		index[13] = 4;
		index[14] = 7;
		index[15] = 0;
		index[16] = 7;
		index[17] = 3;

		index[18] = 3;
		index[19] = 7;
		index[20] = 6;
		index[21] = 3;
		index[22] = 6;
		index[23] = 2;

		index[24] = 1;
		index[25] = 5;
		index[26] = 6;
		index[27] = 1;
		index[28] = 6;
		index[29] = 2;

		m_HeadScaleY = yyCreateModel(model);
		m_HeadScaleY->Load();

		f32 sz = 0.0025;
		m_HeadScaleYAabb.m_min -= v4f(sz, sz, sz, 0.f);
		m_HeadScaleYAabb.m_max += v4f(sz, sz, sz, 0.f);
	}
	{// Scale Z
		yyModel * model = yyCreate<yyModel>();
		model->m_vertexType = yyVertexType::Model;
		model->m_stride = sizeof(yyVertexTriangle);
		model->m_vCount = 8;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = 30;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)model->m_vertices;
		vertex[0].Position.set(-gizmo_head_size, -gizmo_head_size, gizmo_arrow_body_size);
		vertex[1].Position.set(-gizmo_head_size, gizmo_head_size, gizmo_arrow_body_size);
		vertex[2].Position.set(gizmo_head_size, gizmo_head_size, gizmo_arrow_body_size);
		vertex[3].Position.set(gizmo_head_size, -gizmo_head_size, gizmo_arrow_body_size);
		vertex[4].Position.set(-gizmo_head_size, -gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex[5].Position.set(-gizmo_head_size, gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex[6].Position.set(gizmo_head_size, gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);
		vertex[7].Position.set(gizmo_head_size, -gizmo_head_size, gizmo_arrow_body_size - gizmo_head_len);

		for (int i = 0; i < 8; ++i) {
			vertex[i].Color = ColorLime.getV4f();
			m_HeadScaleZAabb.add(vertex[i].Position);
		}

		u16* index = (u16*)model->m_indices;
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 0;
		index[4] = 2;
		index[5] = 3;

		index[6] = 0;
		index[7] = 4;
		index[8] = 5;
		index[9] = 0;
		index[10] = 5;
		index[11] = 1;

		index[12] = 0;
		index[13] = 4;
		index[14] = 7;
		index[15] = 0;
		index[16] = 7;
		index[17] = 3;

		index[18] = 3;
		index[19] = 7;
		index[20] = 6;
		index[21] = 3;
		index[22] = 6;
		index[23] = 2;

		index[24] = 1;
		index[25] = 5;
		index[26] = 6;
		index[27] = 1;
		index[28] = 6;
		index[29] = 2;

		m_HeadScaleZ = yyCreateModel(model);
		m_HeadScaleZ->Load();

		f32 sz = 0.0025;
		m_HeadScaleZAabb.m_min -= v4f(sz, sz, sz, 0.f);
		m_HeadScaleZAabb.m_max += v4f(sz, sz, sz, 0.f);
	}
	{// Head Move Y
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

		m_HeadMoveY = yyCreateModel(model);
		m_HeadMoveY->Load();

		f32 sz = 0.0025;
		m_HeadYAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_HeadYAabb.m_max += v4f(sz, 0.f, sz, 0.f);
	}
	{// Head Move Z
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

		m_HeadMoveZ = yyCreateModel(model);
		m_HeadMoveZ->Load();

		f32 sz = 0.0025;
		m_HeadZAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_HeadZAabb.m_max += v4f(sz, sz, 0.f, 0.f);
	}
	{// XZ
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
	{// XY
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
	{// ZY
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
	if (m_HeadMoveX) yyMegaAllocator::Destroy(m_HeadMoveX);
	if (m_HeadMoveY) yyMegaAllocator::Destroy(m_HeadMoveY);
	if (m_HeadMoveZ) yyMegaAllocator::Destroy(m_HeadMoveZ);
	if (m_HeadScaleX) yyMegaAllocator::Destroy(m_HeadScaleX);
	if (m_HeadScaleY) yyMegaAllocator::Destroy(m_HeadScaleY);
	if (m_HeadScaleZ) yyMegaAllocator::Destroy(m_HeadScaleZ);
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
	
	f32 size_2d = 10.f;

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

		g_app->m_gpu->SetModel(m_HeadMoveX);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbHeadX)
			g_app->DrawAabb(m_HeadXAabbMod, ColorRed.getV4f());

		g_app->m_gpu->SetModel(m_HeadMoveY);
		g_app->m_gpu->Draw();
		if(m_isDrawAabbHeadY)
			g_app->DrawAabb(m_HeadYAabbMod, ColorDodgerBlue.getV4f());

		g_app->m_gpu->SetModel(m_HeadMoveZ);
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
		
		{
			f32 _x = vp->m_currentRect.x + m_2d_point.x;
			f32 _y = vp->m_currentRect.y + m_2d_point.y;
			v4f old_vp;
			g_app->m_gpu->SetViewport(0, 0, g_app->m_window->m_currentSize.x, g_app->m_window->m_currentSize.y, g_app->m_window, &old_vp);
			g_app->m_gpu->DrawLine2D(v3f(_x - size_2d, _y - size_2d, 0.f), v3f(_x + size_2d, _y - size_2d, 0.f), ColorYellow);
			g_app->m_gpu->DrawLine2D(v3f(_x - size_2d, _y + size_2d, 0.f), v3f(_x + size_2d, _y + size_2d, 0.f), ColorYellow);
			g_app->m_gpu->DrawLine2D(v3f(_x - size_2d, _y - size_2d, 0.f), v3f(_x - size_2d, _y + size_2d, 0.f), ColorYellow);
			g_app->m_gpu->DrawLine2D(v3f(_x + size_2d, _y - size_2d, 0.f), v3f(_x + size_2d, _y + size_2d, 0.f), ColorYellow);
			g_app->m_gpu->SetViewport(old_vp.x, old_vp.y, old_vp.z, old_vp.w, g_app->m_window, 0);
		}

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

		g_app->m_gpu->SetModel(m_HeadScaleX);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbScaleHeadX)
			g_app->DrawAabb(m_HeadScaleXAabbMod, ColorRed.getV4f());

		g_app->m_gpu->SetModel(m_HeadScaleY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbScaleHeadY)
			g_app->DrawAabb(m_HeadScaleYAabbMod, ColorDodgerBlue.getV4f());

		g_app->m_gpu->SetModel(m_HeadScaleZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbScaleHeadZ)
			g_app->DrawAabb(m_HeadScaleZAabbMod, ColorLime.getV4f());

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

		{
			f32 _x = vp->m_currentRect.x + m_2d_point.x;
			f32 _y = vp->m_currentRect.y + m_2d_point.y;
			v4f old_vp;
			g_app->m_gpu->SetViewport(0, 0, g_app->m_window->m_currentSize.x, g_app->m_window->m_currentSize.y, g_app->m_window, &old_vp);
			g_app->m_gpu->DrawLine2D(v3f(_x - size_2d, _y - size_2d, 0.f), v3f(_x + size_2d, _y - size_2d, 0.f), ColorYellow);
			g_app->m_gpu->DrawLine2D(v3f(_x - size_2d, _y + size_2d, 0.f), v3f(_x + size_2d, _y + size_2d, 0.f), ColorYellow);
			g_app->m_gpu->DrawLine2D(v3f(_x - size_2d, _y - size_2d, 0.f), v3f(_x - size_2d, _y + size_2d, 0.f), ColorYellow);
			g_app->m_gpu->DrawLine2D(v3f(_x + size_2d, _y - size_2d, 0.f), v3f(_x + size_2d, _y + size_2d, 0.f), ColorYellow);
			g_app->m_gpu->SetViewport(old_vp.x, old_vp.y, old_vp.z, old_vp.w, g_app->m_window, 0);
		}
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

	m_2d_point = math::worldToScreen(
		vp->m_activeCamera->m_viewProjectionMatrix, 
		g_app->m_selectionAabb_center, 
		vp->m_currentRectSize, 
		v2f(vp->m_currentRect.x, vp->m_currentRect.y)
	);
	m_2d_point = math::screenToClient(m_2d_point, vp->m_currentRect);

	//printf("%f %f\n", m_2d_point.x, m_2d_point.y);

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
			m_isDrawAabbZY = true;
		else
			m_isDrawAabbZY = false;
		break;
	}

	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::NoTransform:
		break;
	case miTransformMode::Scale:
	case miTransformMode::Move:
		m_HeadScaleXAabbMod.m_min = math::mul(m_HeadScaleXAabb.m_min, m_S);
		m_HeadScaleXAabbMod.m_max = math::mul(m_HeadScaleXAabb.m_max, m_S);
		m_HeadScaleXAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadScaleXAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadScaleXAabbMod.rayTest(g_app->m_rayCursor))
			m_isDrawAabbScaleHeadX = true;
		else
			m_isDrawAabbScaleHeadX = false;

		m_HeadScaleYAabbMod.m_min = math::mul(m_HeadScaleYAabb.m_min, m_S);
		m_HeadScaleYAabbMod.m_max = math::mul(m_HeadScaleYAabb.m_max, m_S);
		m_HeadScaleYAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadScaleYAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadScaleYAabbMod.rayTest(g_app->m_rayCursor))
			m_isDrawAabbScaleHeadY = true;
		else
			m_isDrawAabbScaleHeadY = false;

		m_HeadScaleZAabbMod.m_min = math::mul(m_HeadScaleZAabb.m_min, m_S);
		m_HeadScaleZAabbMod.m_max = math::mul(m_HeadScaleZAabb.m_max, m_S);
		m_HeadScaleZAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadScaleZAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadScaleZAabbMod.rayTest(g_app->m_rayCursor))
			m_isDrawAabbScaleHeadZ = true;
		else
			m_isDrawAabbScaleHeadZ = false;

		m_HeadXAabbMod.m_min = math::mul(m_HeadXAabb.m_min, m_S);
		m_HeadXAabbMod.m_max = math::mul(m_HeadXAabb.m_max, m_S);
		m_HeadXAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadXAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadXAabbMod.rayTest(g_app->m_rayCursor))
			m_isDrawAabbHeadX = true;
		else
			m_isDrawAabbHeadX = false;


		m_HeadYAabbMod.m_min = math::mul(m_HeadYAabb.m_min, m_S);
		m_HeadYAabbMod.m_max = math::mul(m_HeadYAabb.m_max, m_S);
		m_HeadYAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadYAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadYAabbMod.rayTest(g_app->m_rayCursor))
			m_isDrawAabbHeadY = true;
		else
			m_isDrawAabbHeadY = false;

		m_HeadZAabbMod.m_min = math::mul(m_HeadZAabb.m_min, m_S);
		m_HeadZAabbMod.m_max = math::mul(m_HeadZAabb.m_max, m_S);
		m_HeadZAabbMod.m_max += g_app->m_selectionAabb_center;
		m_HeadZAabbMod.m_min += g_app->m_selectionAabb_center;
		if (m_HeadZAabbMod.rayTest(g_app->m_rayCursor))
			m_isDrawAabbHeadZ = true;
		else
			m_isDrawAabbHeadZ = false;

		break;
	case miTransformMode::Rotate:
		break;
	}
	

	return false;
}