#include "yy.h"
#include "yy_mesh.h"
#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"

#include "scene\common.h"
#include "scene\sprite.h"


extern miApplication * g_app;

miGizmo::miGizmo() 
{
	m_var_rotate_snap = 0.f;
	m_gizmo_arrow_body_size = 0.125f;
	m_gizmo_head_size = 0.01f;
	m_gizmo_head_len = 0.025f;
	m_gizmo_2pl_sz = 0.01f;
	m_gizmo_rot_sz = 0.1f;
	m_gizmo_rot_sz_screen = m_gizmo_rot_sz + 0.05f;
	m_gizmo_rot_sz_mn = 0.01;
	m_gizmo_rot_sz_mx = 0.01;

	auto trtex = yyGetTexture(L"../res/gui/tr.dds");
	m_rotateSprite = yyCreateSprite(v4f(0.f, 0.f, 1.f, 1.f), trtex, 8);
	m_rotateSprite->m_useAsBillboard = true;

	m_isRotationHoverX = false;
	m_isRotationHoverY = false;
	m_isRotationHoverZ = false;

	m_color_x = ColorRed;
	m_color_y = ColorDodgerBlue;
	m_color_z = ColorLime;

	m_size_2d = 10.f;
	m_isIn2d = false;
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
	m_rotateX = 0;
	m_rotateY = 0;
	m_rotateZ = 0;
	m_rotateScreen = 0;

	
	{
		
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = 4;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 6;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)mesh->m_vertices;
		vertex->Position.set(0.f, 0.f, 0.f);
		vertex->Color = ColorWhite.getV4f();
		vertex++;
		vertex->Position.set(m_gizmo_arrow_body_size, 0.f, 0.f);
		vertex->Color = ColorWhite.getV4f();
		vertex++;
		vertex->Position.set(0.f, m_gizmo_arrow_body_size, 0.f);
		vertex->Color = ColorWhite.getV4f();
		vertex++;
		vertex->Position.set(0.f, 0.f, m_gizmo_arrow_body_size);
		vertex->Color = ColorWhite.getV4f();

		u16* index = (u16*)mesh->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 3; index++;

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_pivotModel = yyCreateGPUMesh(&mi);
	}
	{ //
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = 2;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 2;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)mesh->m_vertices;
		vertex->Position.set(0.f, 0.f, 0.f);
		vertex->Color = m_color_x.getV4f();
		m_XAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_arrow_body_size, 0.f, 0.f);
		vertex->Color = m_color_x.getV4f();
		m_XAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_X = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_XAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_XAabb.m_max += v4f(0.f, sz, sz, 0.f);
	}
	{
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = 2;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 2;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)mesh->m_vertices;
		vertex->Position.set(0.f, 0.f, 0.f);
		vertex->Color = m_color_y.getV4f();
		m_YAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, m_gizmo_arrow_body_size, 0.f);
		vertex->Color = m_color_y.getV4f();
		m_YAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_Y = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_YAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_YAabb.m_max += v4f(sz, 0.f, sz, 0.f);
	}
	{
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = 2;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 2;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)mesh->m_vertices;
		vertex->Position.set(0.f, 0.f, 0.f);
		vertex->Color = m_color_z.getV4f();
		m_ZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, 0.f, m_gizmo_arrow_body_size);
		vertex->Color = m_color_z.getV4f();
		m_ZAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
		*index = 0; index++;
		*index = 1; index++;

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_Z = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_ZAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_ZAabb.m_max += v4f(sz, sz, 0.f, 0.f);
	}
	{
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 5;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 12;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex->Position.set(m_gizmo_arrow_body_size, 0.f, 0.f);
		vertex->Color = m_color_x.getV4f();
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_arrow_body_size - m_gizmo_head_len , -m_gizmo_head_size, -m_gizmo_head_size);
		vertex->Color = m_color_x.getV4f();
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_arrow_body_size - m_gizmo_head_len , -m_gizmo_head_size, m_gizmo_head_size);
		vertex->Color = m_color_x.getV4f();
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_arrow_body_size - m_gizmo_head_len, m_gizmo_head_size, m_gizmo_head_size);
		vertex->Color = m_color_x.getV4f();
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_arrow_body_size - m_gizmo_head_len, m_gizmo_head_size, -m_gizmo_head_size);
		vertex->Color = m_color_x.getV4f();
		m_HeadXAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
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

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_HeadMoveX = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_HeadXAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_HeadXAabb.m_max += v4f(0.f, sz, sz, 0.f);
	}
	{// Scale X
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 8;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 30;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex[0].Position.set(m_gizmo_arrow_body_size, -m_gizmo_head_size, -m_gizmo_head_size);
		vertex[1].Position.set(m_gizmo_arrow_body_size, -m_gizmo_head_size, m_gizmo_head_size);
		vertex[2].Position.set(m_gizmo_arrow_body_size, m_gizmo_head_size, m_gizmo_head_size);
		vertex[3].Position.set(m_gizmo_arrow_body_size, m_gizmo_head_size, -m_gizmo_head_size);
		vertex[4].Position.set(m_gizmo_arrow_body_size - m_gizmo_head_len, -m_gizmo_head_size, -m_gizmo_head_size);
		vertex[5].Position.set(m_gizmo_arrow_body_size - m_gizmo_head_len, -m_gizmo_head_size, m_gizmo_head_size);
		vertex[6].Position.set(m_gizmo_arrow_body_size - m_gizmo_head_len, m_gizmo_head_size, m_gizmo_head_size);
		vertex[7].Position.set(m_gizmo_arrow_body_size - m_gizmo_head_len, m_gizmo_head_size, -m_gizmo_head_size);

		for (int i = 0; i < 8; ++i) {
			vertex[i].Color = m_color_x.getV4f();
			m_HeadScaleXAabb.add(vertex[i].Position);
		}

		u16* index = (u16*)mesh->m_indices;
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

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_HeadScaleX = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_HeadScaleXAabb.m_min -= v4f(sz, sz, sz, 0.f);
		m_HeadScaleXAabb.m_max += v4f(sz, sz, sz, 0.f);
	}
	{// Head Scale Y
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 8;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 30;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex[0].Position.set(-m_gizmo_head_size, m_gizmo_arrow_body_size , -m_gizmo_head_size);
		vertex[1].Position.set(-m_gizmo_head_size, m_gizmo_arrow_body_size, m_gizmo_head_size);
		vertex[2].Position.set(m_gizmo_head_size, m_gizmo_arrow_body_size, m_gizmo_head_size);
		vertex[3].Position.set(m_gizmo_head_size, m_gizmo_arrow_body_size , -m_gizmo_head_size);
		vertex[4].Position.set(-m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len, -m_gizmo_head_size);
		vertex[5].Position.set(-m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len, m_gizmo_head_size);
		vertex[6].Position.set(m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len, m_gizmo_head_size);
		vertex[7].Position.set(m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len , -m_gizmo_head_size);

		for (int i = 0; i < 8; ++i) {
			vertex[i].Color = m_color_y.getV4f();
			m_HeadScaleYAabb.add(vertex[i].Position);
		}

		u16* index = (u16*)mesh->m_indices;
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

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_HeadScaleY = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_HeadScaleYAabb.m_min -= v4f(sz, sz, sz, 0.f);
		m_HeadScaleYAabb.m_max += v4f(sz, sz, sz, 0.f);
	}
	{// Scale Z
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 8;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 30;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex[0].Position.set(-m_gizmo_head_size, -m_gizmo_head_size, m_gizmo_arrow_body_size);
		vertex[1].Position.set(-m_gizmo_head_size, m_gizmo_head_size, m_gizmo_arrow_body_size);
		vertex[2].Position.set(m_gizmo_head_size, m_gizmo_head_size, m_gizmo_arrow_body_size);
		vertex[3].Position.set(m_gizmo_head_size, -m_gizmo_head_size, m_gizmo_arrow_body_size);
		vertex[4].Position.set(-m_gizmo_head_size, -m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len);
		vertex[5].Position.set(-m_gizmo_head_size, m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len);
		vertex[6].Position.set(m_gizmo_head_size, m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len);
		vertex[7].Position.set(m_gizmo_head_size, -m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len);

		for (int i = 0; i < 8; ++i) {
			vertex[i].Color = m_color_z.getV4f();
			m_HeadScaleZAabb.add(vertex[i].Position);
		}

		u16* index = (u16*)mesh->m_indices;
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

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_HeadScaleZ = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_HeadScaleZAabb.m_min -= v4f(sz, sz, sz, 0.f);
		m_HeadScaleZAabb.m_max += v4f(sz, sz, sz, 0.f);
	}
	{// Head Move Y
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 5;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 12;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex->Position.set(0.f, m_gizmo_arrow_body_size, 0.f);
		vertex->Color = m_color_y.getV4f();
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len, -m_gizmo_head_size);
		vertex->Color = m_color_y.getV4f();
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len, m_gizmo_head_size);
		vertex->Color = m_color_y.getV4f();
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len, m_gizmo_head_size);
		vertex->Color = m_color_y.getV4f();
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len, -m_gizmo_head_size);
		vertex->Color = m_color_y.getV4f();
		m_HeadYAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
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

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_HeadMoveY = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_HeadYAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_HeadYAabb.m_max += v4f(sz, 0.f, sz, 0.f);
	}
	{// Head Move Z
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 5;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 12;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex->Position.set(0.f, 0.f, m_gizmo_arrow_body_size);
		vertex->Color = m_color_z.getV4f();
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-m_gizmo_head_size, -m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len);
		vertex->Color = m_color_z.getV4f();
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(-m_gizmo_head_size, m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len);
		vertex->Color = m_color_z.getV4f();
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_head_size, m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len);
		vertex->Color = m_color_z.getV4f();
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(m_gizmo_head_size, -m_gizmo_head_size, m_gizmo_arrow_body_size - m_gizmo_head_len);
		vertex->Color = m_color_z.getV4f();
		m_HeadZAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
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

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_HeadMoveZ = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_HeadZAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_HeadZAabb.m_max += v4f(sz, sz, 0.f, 0.f);
	}
	{// XZ
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 4;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 6;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		f32 xzsz = m_gizmo_arrow_body_size * 0.5f;
		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex->Position.set(xzsz - m_gizmo_2pl_sz, 0.f, xzsz - m_gizmo_2pl_sz);
		vertex->Color = m_color_y.getV4f();
		m_XZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xzsz - m_gizmo_2pl_sz, 0.f, xzsz + m_gizmo_2pl_sz);
		vertex->Color = m_color_y.getV4f();
		m_XZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xzsz + m_gizmo_2pl_sz, 0.f, xzsz + m_gizmo_2pl_sz);
		vertex->Color = m_color_y.getV4f();
		m_XZAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xzsz + m_gizmo_2pl_sz, 0.f, xzsz - m_gizmo_2pl_sz);
		vertex->Color = m_color_y.getV4f();
		m_XZAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_XZ = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_XZAabb.m_min -= v4f(sz, 0.f, sz, 0.f);
		m_XZAabb.m_max += v4f(sz, 0.f, sz, 0.f);
	}
	{// XY
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 4;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 6;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		f32 xysz = m_gizmo_arrow_body_size * 0.5f;
		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex->Position.set(xysz - m_gizmo_2pl_sz, xysz - m_gizmo_2pl_sz, 0.f);
		vertex->Color = m_color_z.getV4f();
		m_XYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xysz - m_gizmo_2pl_sz, xysz + m_gizmo_2pl_sz, 0.f);
		vertex->Color = m_color_z.getV4f();
		m_XYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xysz + m_gizmo_2pl_sz, xysz + m_gizmo_2pl_sz, 0.f);
		vertex->Color = m_color_z.getV4f();
		m_XYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(xysz + m_gizmo_2pl_sz, xysz - m_gizmo_2pl_sz, 0.f);
		vertex->Color = m_color_z.getV4f();
		m_XYAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_XY = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_XYAabb.m_min -= v4f(sz, sz, 0.f, 0.f);
		m_XYAabb.m_max += v4f(sz, sz, 0.f, 0.f);
	}
	{// ZY
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_vertexType = yyMeshVertexType::Triangle;
		mesh->m_stride = sizeof(yyVertexTriangle);
		mesh->m_vCount = 4;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 6;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		f32 zysz = m_gizmo_arrow_body_size * 0.5f;
		auto vertex = (yyVertexTriangle*)mesh->m_vertices;
		vertex->Position.set(0.f, zysz - m_gizmo_2pl_sz, zysz - m_gizmo_2pl_sz);
		vertex->Color = m_color_x.getV4f();
		m_ZYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, zysz + m_gizmo_2pl_sz, zysz - m_gizmo_2pl_sz);
		vertex->Color = m_color_x.getV4f();
		m_ZYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, zysz + m_gizmo_2pl_sz, zysz + m_gizmo_2pl_sz);
		vertex->Color = m_color_x.getV4f();
		m_ZYAabb.add(vertex->Position);
		vertex++;

		vertex->Position.set(0.f, zysz - m_gizmo_2pl_sz, zysz + m_gizmo_2pl_sz);
		vertex->Color = m_color_x.getV4f();
		m_ZYAabb.add(vertex->Position);
		vertex++;

		u16* index = (u16*)mesh->m_indices;
		*index = 0; index++;
		*index = 1; index++;
		*index = 2; index++;
		*index = 0; index++;
		*index = 2; index++;
		*index = 3; index++;

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_ZY = yyCreateGPUMesh(&mi);

		f32 sz = 0.0025;
		m_ZYAabb.m_min -= v4f(0.f, sz, sz, 0.f);
		m_ZYAabb.m_max += v4f(0.f, sz, sz, 0.f);
	}
	{ // rotate X
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = 36;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 72;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)mesh->m_vertices;
		
		for (int i = 0; i < 36; ++i)
		{
			vertex[i].Position.x = 0.f;
			vertex[i].Position.y = std::cos(math::degToRad((f32)i * 10.f)) * m_gizmo_rot_sz;
			vertex[i].Position.z = std::sin(math::degToRad((f32)i * 10.f)) * m_gizmo_rot_sz;
			vertex[i].Color = ColorWhite.getV4f();
		}

		u16* index = (u16*)mesh->m_indices;
		for (s32 i = 0, ind = 0; i < 72; i += 2)
		{
			index[i] = (u16)ind++;
			index[i+1] = (u16)ind;
			if (i == 70)
				index[i + 1] = 0;
		}

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_rotateX = yyCreateGPUMesh(&mi);
	}
	{ // rotate Y
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = 36;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 72;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)mesh->m_vertices;

		for (int i = 0; i < 36; ++i)
		{
			vertex[i].Position.y = 0.f;
			vertex[i].Position.x = std::cos(math::degToRad((f32)i * 10.f)) * m_gizmo_rot_sz;
			vertex[i].Position.z = std::sin(math::degToRad((f32)i * 10.f)) * m_gizmo_rot_sz;
			vertex[i].Color = ColorWhite.getV4f();
		}

		u16* index = (u16*)mesh->m_indices;
		for (s32 i = 0, ind = 0; i < 72; i += 2)
		{
			index[i] = (u16)ind++;
			index[i + 1] = (u16)ind;
			if (i == 70)
				index[i + 1] = 0;
		}

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_rotateY = yyCreateGPUMesh(&mi);
	}
	{ // rotate Z
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = 36;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 72;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)mesh->m_vertices;

		for (int i = 0; i < 36; ++i)
		{
			vertex[i].Position.z = 0.f;
			vertex[i].Position.x = std::cos(math::degToRad((f32)i * 10.f)) * m_gizmo_rot_sz;
			vertex[i].Position.y = std::sin(math::degToRad((f32)i * 10.f)) * m_gizmo_rot_sz;
			vertex[i].Color = ColorWhite.getV4f();
		}

		u16* index = (u16*)mesh->m_indices;
		for (s32 i = 0, ind = 0; i < 72; i += 2)
		{
			index[i] = (u16)ind++;
			index[i + 1] = (u16)ind;
			if (i == 70)
				index[i + 1] = 0;
		}

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_rotateZ = yyCreateGPUMesh(&mi);
	}
	{ // rotate screen
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = 36;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = 72;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));

		auto vertex = (yyVertexLine*)mesh->m_vertices;

		for (int i = 0; i < 36; ++i)
		{
			vertex[i].Position.z = 0.05f;
			vertex[i].Position.x = std::cos(math::degToRad((f32)i * 10.f)) * m_gizmo_rot_sz_screen;
			vertex[i].Position.y = std::sin(math::degToRad((f32)i * 10.f)) * m_gizmo_rot_sz_screen;
			vertex[i].Color = ColorWhite.getV4f();
		}

		u16* index = (u16*)mesh->m_indices;
		for (s32 i = 0, ind = 0; i < 72; i += 2)
		{
			index[i] = (u16)ind++;
			index[i + 1] = (u16)ind;
			if (i == 70)
				index[i + 1] = 0;
		}

		yyGPUMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_rotateScreen = yyCreateGPUMesh(&mi);
	}
}

miGizmo::~miGizmo() {
	if (m_rotateSprite)
	{
		m_rotateSprite->DropTexture();
		yyDestroy(m_rotateSprite);
	}
	if (m_pivotModel) yyDestroy(m_pivotModel);
	if (m_Y) yyDestroy(m_Y);
	if (m_X) yyDestroy(m_X);
	if (m_Z) yyDestroy(m_Z);
	if (m_HeadMoveX) yyDestroy(m_HeadMoveX);
	if (m_HeadMoveY) yyDestroy(m_HeadMoveY);
	if (m_HeadMoveZ) yyDestroy(m_HeadMoveZ);
	if (m_HeadScaleX) yyDestroy(m_HeadScaleX);
	if (m_HeadScaleY) yyDestroy(m_HeadScaleY);
	if (m_HeadScaleZ) yyDestroy(m_HeadScaleZ);
	if (m_XZ) yyDestroy(m_XZ);
	if (m_XY) yyDestroy(m_XY);
	if (m_ZY) yyDestroy(m_ZY);
	if (m_rotateX) yyDestroy(m_rotateX);
	if (m_rotateY) yyDestroy(m_rotateY);
	if (m_rotateZ) yyDestroy(m_rotateZ);
	if (m_rotateScreen) yyDestroy(m_rotateScreen);
}

void miGizmo::Draw(miViewport* vp) {
	g_app->m_gpu->UseDepth(false);

	yySetMatrix(yyMatrixType::World, &m_W);

	m_WVP.identity();
	m_WVP = vp->m_activeCamera->m_projectionMatrix
		* vp->m_activeCamera->m_viewMatrix
		* m_W;
	yySetMatrix(yyMatrixType::WorldViewProjection, &m_WVP);
	
	yySetMaterial(&m_commonMaterial);

	m_commonMaterial.m_baseColor = ColorWhite;

	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::NoTransform:
		yySetMaterial(&m_pivotModelMaterial);
		g_app->m_gpu->SetMesh(m_pivotModel);
		g_app->m_gpu->Draw();
		break;
	case miTransformMode::Move:
		g_app->m_gpu->SetMesh(m_X);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbX)
			g_app->DrawAabb(m_XAabbMod, m_color_x.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_Y);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbY)
			g_app->DrawAabb(m_YAabbMod, m_color_y.getV4f(), m_var_move);
		
		g_app->m_gpu->SetMesh(m_Z);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbZ)
			g_app->DrawAabb(m_ZAabbMod, m_color_z.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_HeadMoveX);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbHeadX)
			g_app->DrawAabb(m_HeadXAabbMod, m_color_x.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_HeadMoveY);
		g_app->m_gpu->Draw();
		if(m_isDrawAabbHeadY)
			g_app->DrawAabb(m_HeadYAabbMod, m_color_y.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_HeadMoveZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbHeadZ)
			g_app->DrawAabb(m_HeadZAabbMod, m_color_z.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_XZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbXZ)
			g_app->DrawAabb(m_XZAabbMod, m_color_y.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_XY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbXY)
			g_app->DrawAabb(m_XYAabbMod, m_color_z.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_ZY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbZY)
			g_app->DrawAabb(m_ZYAabbMod, m_color_x.getV4f(), m_var_move);
		
		{
			f32 _x = vp->m_currentRect.x + m_2d_point.x;
			f32 _y = vp->m_currentRect.y + m_2d_point.y;
			v4f old_vp;
			auto color2d = ColorWhite;
			if (m_isIn2d)color2d = ColorYellow;
			g_app->m_gpu->SetViewport(0, 0, g_app->m_window->m_currentSize.x, g_app->m_window->m_currentSize.y, g_app->m_window, &old_vp);
			f32 rsz = m_isIn2d ? m_size_2d + 2.f : m_size_2d;
			g_app->m_gpu->DrawLine2D(v3f(_x - rsz, _y - rsz, 0.f), v3f(_x + rsz, _y - rsz, 0.f), color2d);
			g_app->m_gpu->DrawLine2D(v3f(_x - rsz, _y + rsz, 0.f), v3f(_x + rsz, _y + rsz, 0.f), color2d);
			g_app->m_gpu->DrawLine2D(v3f(_x - rsz, _y - rsz, 0.f), v3f(_x - rsz, _y + rsz, 0.f), color2d);
			g_app->m_gpu->DrawLine2D(v3f(_x + rsz, _y - rsz, 0.f), v3f(_x + rsz, _y + rsz, 0.f), color2d);
			g_app->m_gpu->SetViewport(old_vp.x, old_vp.y, old_vp.z, old_vp.w, g_app->m_window, 0);
		}

		break;
	case miTransformMode::Scale:
		g_app->m_gpu->SetMesh(m_X);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbX)
			g_app->DrawAabb(m_XAabbMod, m_color_x.getV4f(), m_var_move);

		yySetMaterial(&m_commonMaterial);
		g_app->m_gpu->SetMesh(m_Y);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbY)
			g_app->DrawAabb(m_YAabbMod, m_color_y.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_Z);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbZ)
			g_app->DrawAabb(m_ZAabbMod, m_color_z.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_HeadScaleX);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbScaleHeadX)
			g_app->DrawAabb(m_HeadScaleXAabbMod, m_color_x.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_HeadScaleY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbScaleHeadY)
			g_app->DrawAabb(m_HeadScaleYAabbMod, m_color_y.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_HeadScaleZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbScaleHeadZ)
			g_app->DrawAabb(m_HeadScaleZAabbMod, m_color_z.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_XZ);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbXZ)
			g_app->DrawAabb(m_XZAabbMod, m_color_y.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_XY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbXY)
			g_app->DrawAabb(m_XYAabbMod, m_color_z.getV4f(), m_var_move);

		g_app->m_gpu->SetMesh(m_ZY);
		g_app->m_gpu->Draw();
		if (m_isDrawAabbZY)
			g_app->DrawAabb(m_ZYAabbMod, m_color_x.getV4f(), m_var_move);

		{
			f32 _x = vp->m_currentRect.x + m_2d_point.x;
			f32 _y = vp->m_currentRect.y + m_2d_point.y;
			v4f old_vp;
			auto color2d = ColorWhite;
			if (m_isIn2d)color2d = ColorYellow;
			g_app->m_gpu->SetViewport(0, 0, g_app->m_window->m_currentSize.x, g_app->m_window->m_currentSize.y, g_app->m_window, &old_vp);
			f32 rsz = m_isIn2d ? m_size_2d + 2.f : m_size_2d;
			g_app->m_gpu->DrawLine2D(v3f(_x - rsz, _y - rsz, 0.f), v3f(_x + rsz, _y - rsz, 0.f), color2d);
			g_app->m_gpu->DrawLine2D(v3f(_x - rsz, _y + rsz, 0.f), v3f(_x + rsz, _y + rsz, 0.f), color2d);
			g_app->m_gpu->DrawLine2D(v3f(_x - rsz, _y - rsz, 0.f), v3f(_x - rsz, _y + rsz, 0.f), color2d);
			g_app->m_gpu->DrawLine2D(v3f(_x + rsz, _y - rsz, 0.f), v3f(_x + rsz, _y + rsz, 0.f), color2d);
			g_app->m_gpu->SetViewport(old_vp.x, old_vp.y, old_vp.z, old_vp.w, g_app->m_window, 0);
		}
		break;
	case miTransformMode::Rotate:
	{
		g_app->m_gpu->ClearDepth();
		g_app->m_gpu->UseDepth(true);
		g_app->m_gpu->UseBlend(true);

		//m_rotateSprite->m_objectBase.m_globalMatrix.identity();
		//m_rotateSprite->m_objectBase.m_globalMatrix.setBasis(vp->m_activeCamera->m_viewMatrixInvert);
		//m_rotateSprite->m_objectBase.m_globalMatrix.setTranslation(m_W.m_data[3]);

		Mat4 WVP;
		WVP = vp->m_activeCamera->m_projectionMatrix * vp->m_activeCamera->m_viewMatrix * m_rotateSprite->m_objectBase.m_globalMatrix;
		auto oldWVP = yyGetMatrix(yyMatrixType::WorldViewProjection);
		yySetMatrix(yyMatrixType::WorldViewProjection, &WVP);
		g_app->m_gpu->DrawSprite(m_rotateSprite);
		m_isRotationHoverScreen ? m_commonMaterial.m_baseColor = ColorYellow : m_commonMaterial.m_baseColor = ColorWhite;
		g_app->m_gpu->SetMesh(m_rotateScreen);
		g_app->m_gpu->Draw();
		yySetMatrix(yyMatrixType::WorldViewProjection, oldWVP);
	}
	
		m_isRotationHoverX ? m_commonMaterial.m_baseColor = ColorYellow : m_commonMaterial.m_baseColor = m_color_x;
		g_app->m_gpu->SetMesh(m_rotateX);
		g_app->m_gpu->Draw();

		m_isRotationHoverY ? m_commonMaterial.m_baseColor = ColorYellow : m_commonMaterial.m_baseColor = m_color_y;
		g_app->m_gpu->SetMesh(m_rotateY);
		g_app->m_gpu->Draw();
		
		m_isRotationHoverZ ? m_commonMaterial.m_baseColor = ColorYellow : m_commonMaterial.m_baseColor = m_color_z;
		g_app->m_gpu->SetMesh(m_rotateZ);
		g_app->m_gpu->Draw();


		break;
	}
}

//bool miGizmo::Update(miViewport* vp) {
void miGizmo::Update(miViewport* vp) {
	bool goodVP = vp == g_app->m_viewportUnderCursor;

	//g_app->m_selectionAabb_center;
	m_S.identity();

	f32 camera_distance = vp->m_activeCamera->m_positionPlatform.w;
	if (g_app->m_selectedObjects.m_size == 1)
	{
	//	camera_distance += g_app->m_selectionAabb_center.distance(*g_app->m_selectedObjects.m_data[0]->GetGlobalPosition());
	}
	m_S.setScale(v3f(camera_distance));

	m_T.identity();

	v4f point3D_for_2D;

	if (g_app->m_selectedObjects.m_size == 1)
	{
		//aabb_position = *g_app->m_selectedObjects.m_data[0]->GetGlobalPosition();
		//point3D_for_2D = aabb_position;
		point3D_for_2D = m_position + m_var_move;
		m_T.setTranslation(m_position + m_var_move);
		//m_T.setTranslation(*g_app->m_selectedObjects.m_data[0]->GetGlobalPosition() );
	}
	else
	{
		//aabb_position = g_app->m_selectionAabb_center;
		//point3D_for_2D = aabb_position + m_var_move;
		point3D_for_2D = m_position + m_var_move;		
		m_T.setTranslation(m_position + m_var_move);
		//m_T.setTranslation(g_app->m_selectionAabb_center + m_var_move);
	}

	m_W = m_T * m_S;
	
	m_2d_point = math::worldToScreen(
		vp->m_activeCamera->m_viewProjectionMatrix, 
		point3D_for_2D,
		vp->m_currentRectSize, 
		v2f(vp->m_currentRect.x, vp->m_currentRect.y)
	);
	m_2d_point = math::screenToClient(m_2d_point, vp->m_currentRect);

	if (g_app->m_transformMode == miTransformMode::Rotate)
	{
		m_rotateSprite->m_objectBase.m_globalMatrix.identity();
		m_rotateSprite->m_objectBase.m_globalMatrix.setBasis(vp->m_activeCamera->m_viewMatrixInvert);
		m_rotateSprite->m_objectBase.m_globalMatrix = m_rotateSprite->m_objectBase.m_globalMatrix * m_S;
		m_rotateSprite->m_objectBase.m_globalMatrix.setTranslation(m_W.m_data[3]);
	}

	if (g_app->m_gizmoMode != miGizmoMode::NoTransform) return;

	//printf("%f %f\n", m_2d_point.x, m_2d_point.y);

	f32 spriteRayTestLen = 999990.f;
	f32 TX = 99999.f;
	f32 TY = 99999.f;
	f32 TZ = 99999.f;

	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::NoTransform:
		break;
	case miTransformMode::Rotate:
		

		if (m_rotateSprite->RayTest(g_app->m_rayCursor, &spriteRayTestLen))
		{
			//printf("Len: %f\n", spriteRayTestLen);
		}
		
		g_app->m_rayCursor.planeIntersection(m_W.m_data[3], v4f(1.f, 0.f, 0.f, 0.f), TX);
		g_app->m_rayCursor.planeIntersection(m_W.m_data[3], v4f(0.f, 1.f, 0.f, 0.f), TY);
		g_app->m_rayCursor.planeIntersection(m_W.m_data[3], v4f(0.f, 0.f, 1.f, 0.f), TZ);

		{
			v4f ipX;
			v4f ipY;
			v4f ipZ;
			v4f ipSprite;
			g_app->m_rayCursor.getIntersectionPoint(TX, ipX);
			g_app->m_rayCursor.getIntersectionPoint(TY, ipY);
			g_app->m_rayCursor.getIntersectionPoint(TZ, ipZ);
			g_app->m_rayCursor.getIntersectionPoint(spriteRayTestLen, ipSprite);

			f32 dX = ipX.distance(m_W.m_data[3]);
			f32 dY = ipY.distance(m_W.m_data[3]);
			f32 dZ = ipZ.distance(m_W.m_data[3]);
			f32 dSprite = ipSprite.distance(m_W.m_data[3]);

			auto gizmo_rot_sz_scaled_min = (m_gizmo_rot_sz - m_gizmo_rot_sz_mn) * m_S.m_data[0].x;
			auto gizmo_rot_sz_scaled_max = (m_gizmo_rot_sz + m_gizmo_rot_sz_mx) * m_S.m_data[0].x;

			m_isRotationHoverX = dX >= gizmo_rot_sz_scaled_min && dX <= gizmo_rot_sz_scaled_max ? true : false;
			m_isRotationHoverY = dY >= gizmo_rot_sz_scaled_min && dY <= gizmo_rot_sz_scaled_max ? true : false;
			m_isRotationHoverZ = dZ >= gizmo_rot_sz_scaled_min && dZ <= gizmo_rot_sz_scaled_max ? true : false;

			if (m_isRotationHoverX && TX > spriteRayTestLen) m_isRotationHoverX = false;
			if (m_isRotationHoverY && TY > spriteRayTestLen) m_isRotationHoverY = false;
			if (m_isRotationHoverZ && TZ > spriteRayTestLen) m_isRotationHoverZ = false;

			if (m_isRotationHoverX && m_isRotationHoverY) m_isRotationHoverY = false;
			if (m_isRotationHoverX && m_isRotationHoverZ) m_isRotationHoverZ = false;
			if (m_isRotationHoverY && m_isRotationHoverZ) m_isRotationHoverY = false;



			auto gizmo_rot_sz_scaled_min_screen = (m_gizmo_rot_sz_screen - m_gizmo_rot_sz_mn) * m_S.m_data[0].x;
			auto gizmo_rot_sz_scaled_max_screen = (m_gizmo_rot_sz_screen + m_gizmo_rot_sz_mx) * m_S.m_data[0].x;
			m_isRotationHoverScreen = dSprite >= gizmo_rot_sz_scaled_min_screen && dSprite <= gizmo_rot_sz_scaled_max_screen ? true : false;
			
			if (m_isRotationHoverX && goodVP) g_app->m_isGizmoMouseHover = true;
			if (m_isRotationHoverY && goodVP) g_app->m_isGizmoMouseHover = true;
			if (m_isRotationHoverZ && goodVP) g_app->m_isGizmoMouseHover = true;
			if (m_isRotationHoverScreen && goodVP) g_app->m_isGizmoMouseHover = true;
		//	printf("%f\n", dSprite);
		}

		//printf("%f %f %f %f\n", spriteRayTestLen, TX, TY, TZ);

		break;
	case miTransformMode::Move:
	case miTransformMode::Scale:
	{
		f32 _x = vp->m_currentRect.x + m_2d_point.x;
		f32 _y = vp->m_currentRect.y + m_2d_point.y;

		if (math::pointInRect(g_app->m_inputContext->m_cursorCoords.x, g_app->m_inputContext->m_cursorCoords.y,
			v4f(
				_x - m_size_2d,
				_y - m_size_2d,
				_x + m_size_2d,
				_y + m_size_2d
			)
		) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isIn2d = true;
		}
	}

		m_XAabbMod.m_min = math::mul(m_XAabb.m_min, m_S);
		m_XAabbMod.m_max = math::mul(m_XAabb.m_max, m_S);
		m_XAabbMod.m_max += m_position;
		m_XAabbMod.m_min += m_position;
		if (m_XAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbX = true;
		}

		m_YAabbMod.m_min = math::mul(m_YAabb.m_min, m_S);
		m_YAabbMod.m_max = math::mul(m_YAabb.m_max, m_S);
		m_YAabbMod.m_max += m_position;
		m_YAabbMod.m_min += m_position;
		if (m_YAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbY = true;
		}

		m_ZAabbMod.m_min = math::mul(m_ZAabb.m_min, m_S);
		m_ZAabbMod.m_max = math::mul(m_ZAabb.m_max, m_S);
		m_ZAabbMod.m_max += m_position;
		m_ZAabbMod.m_min += m_position;
		if (m_ZAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbZ = true;
		}

		m_XZAabbMod.m_min = math::mul(m_XZAabb.m_min, m_S);
		m_XZAabbMod.m_max = math::mul(m_XZAabb.m_max, m_S);
		m_XZAabbMod.m_max += m_position;
		m_XZAabbMod.m_min += m_position;
		if (m_XZAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbXZ = true;
		}

		m_XYAabbMod.m_min = math::mul(m_XYAabb.m_min, m_S);
		m_XYAabbMod.m_max = math::mul(m_XYAabb.m_max, m_S);
		m_XYAabbMod.m_max += m_position;
		m_XYAabbMod.m_min += m_position;
		if (m_XYAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbXY = true;
		}

		m_ZYAabbMod.m_min = math::mul(m_ZYAabb.m_min, m_S);
		m_ZYAabbMod.m_max = math::mul(m_ZYAabb.m_max, m_S);
		m_ZYAabbMod.m_max += m_position;
		m_ZYAabbMod.m_min += m_position;
		if (m_ZYAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbZY = true;
		}
		break;
	}

	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::NoTransform:
		break;
	case miTransformMode::Scale:
		m_HeadScaleXAabbMod.m_min = math::mul(m_HeadScaleXAabb.m_min, m_S);
		m_HeadScaleXAabbMod.m_max = math::mul(m_HeadScaleXAabb.m_max, m_S);
		m_HeadScaleXAabbMod.m_max += m_position;
		m_HeadScaleXAabbMod.m_min += m_position;
		if (m_HeadScaleXAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbScaleHeadX = true;
		}

		m_HeadScaleYAabbMod.m_min = math::mul(m_HeadScaleYAabb.m_min, m_S);
		m_HeadScaleYAabbMod.m_max = math::mul(m_HeadScaleYAabb.m_max, m_S);
		m_HeadScaleYAabbMod.m_max += m_position;
		m_HeadScaleYAabbMod.m_min += m_position;
		if (m_HeadScaleYAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbScaleHeadY = true;
		}

		m_HeadScaleZAabbMod.m_min = math::mul(m_HeadScaleZAabb.m_min, m_S);
		m_HeadScaleZAabbMod.m_max = math::mul(m_HeadScaleZAabb.m_max, m_S);
		m_HeadScaleZAabbMod.m_max += m_position;
		m_HeadScaleZAabbMod.m_min += m_position;
		if (m_HeadScaleZAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbScaleHeadZ = true;
		}
		break;
	case miTransformMode::Move:
		m_HeadXAabbMod.m_min = math::mul(m_HeadXAabb.m_min, m_S);
		m_HeadXAabbMod.m_max = math::mul(m_HeadXAabb.m_max, m_S);
		m_HeadXAabbMod.m_max += m_position;
		m_HeadXAabbMod.m_min += m_position;
		if (m_HeadXAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbHeadX = true;
		}

		m_HeadYAabbMod.m_min = math::mul(m_HeadYAabb.m_min, m_S);
		m_HeadYAabbMod.m_max = math::mul(m_HeadYAabb.m_max, m_S);
		m_HeadYAabbMod.m_max += m_position;
		m_HeadYAabbMod.m_min += m_position;
		if (m_HeadYAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbHeadY = true;
		}

		m_HeadZAabbMod.m_min = math::mul(m_HeadZAabb.m_min, m_S);
		m_HeadZAabbMod.m_max = math::mul(m_HeadZAabb.m_max, m_S);
		m_HeadZAabbMod.m_max += m_position;
		m_HeadZAabbMod.m_min += m_position;
		if (m_HeadZAabbMod.rayTest(g_app->m_rayCursor) && goodVP)
		{
			g_app->m_isGizmoMouseHover = true;
			m_isDrawAabbHeadZ = true;
		}

		break;
	case miTransformMode::Rotate:
		break;
	}
	

//	return false;
}

void miGizmo::OnClick() {
	_reset_variables();
	for (u32 i = 0; i < g_app->m_selectedObjects.m_size; ++i)
	{
		g_app->m_selectedObjects.m_data[i]->m_localPositionOnGizmoClick = g_app->m_selectedObjects.m_data[i]->m_localPosition;
		g_app->m_selectedObjects.m_data[i]->m_rotationScaleMatrixOnGizmoClick = g_app->m_selectedObjects.m_data[i]->m_rotationScaleMatrix;
		g_app->m_selectedObjects.m_data[i]->m_rotationOnlyMatrixOnGizmoClick = g_app->m_selectedObjects.m_data[i]->m_rotationOnlyMatrix;
		//g_app->m_selectedObjects.m_data[i]->m_scaleMatrixOnGizmoClick = g_app->m_selectedObjects.m_data[i]->m_scaleMatrix;
	}

	miGizmoMode gm = miGizmoMode::NoTransform;
	if (m_isIn2d)
	{
		if(g_app->m_transformMode == miTransformMode::Scale)
			gm = miGizmoMode::ScaleScreen;
		else if (g_app->m_transformMode == miTransformMode::Move)
			gm = miGizmoMode::MoveScreen;
	}
	else if (m_isDrawAabbHeadZ) gm = miGizmoMode::MoveZ;
	else if (m_isDrawAabbHeadY) gm = miGizmoMode::MoveY;
	else if (m_isDrawAabbHeadX) gm = miGizmoMode::MoveX;
	else if (m_isDrawAabbScaleHeadZ) gm = miGizmoMode::ScaleZ;
	else if (m_isDrawAabbScaleHeadY) gm = miGizmoMode::ScaleY;
	else if (m_isDrawAabbScaleHeadX) gm = miGizmoMode::ScaleX;
	else
	{
		switch (g_app->m_transformMode)
		{
		default:
		case miTransformMode::NoTransform:
			break;
		case miTransformMode::Rotate:
			if (m_isRotationHoverY) gm = miGizmoMode::RotateY;
			else if (m_isRotationHoverX) gm = miGizmoMode::RotateX;
			else if (m_isRotationHoverZ) gm = miGizmoMode::RotateZ;
			else if (m_isRotationHoverScreen) gm = miGizmoMode::RotateScreen;
			break;
		case miTransformMode::Scale:
			if (m_isDrawAabbX) gm = miGizmoMode::ScaleX;
			else if (m_isDrawAabbY) gm = miGizmoMode::ScaleY;
			else if (m_isDrawAabbZ) gm = miGizmoMode::ScaleZ;
			else if (m_isDrawAabbZY) gm = miGizmoMode::ScaleZY;
			else if (m_isDrawAabbXY) gm = miGizmoMode::ScaleXY;
			else if (m_isDrawAabbXZ) gm = miGizmoMode::ScaleXZ;
			break;
		case miTransformMode::Move:
			if (m_isDrawAabbX) gm = miGizmoMode::MoveX;
			else if (m_isDrawAabbY) gm = miGizmoMode::MoveY;
			else if (m_isDrawAabbZ) gm = miGizmoMode::MoveZ;
			else if (m_isDrawAabbZY) gm = miGizmoMode::MoveZY;
			else if (m_isDrawAabbXY) gm = miGizmoMode::MoveXY;
			else if (m_isDrawAabbXZ) gm = miGizmoMode::MoveXZ;
			break;
		}
	}

	//if (gm == miGizmoMode::MoveScreen || gm == miGizmoMode::ScaleScreen)
	{
		g_app->SetCursorBehaviorMode(miCursorBehaviorMode::HideCursor);
		v4f cclip;
		f32 _x = g_app->m_cursorLMBClickPosition.x;
		f32 _y = g_app->m_cursorLMBClickPosition.y;
		//printf("%f %f\n", _x, _y);
		//const f32 clpsz = 3.f;
		//cclip.set(_x - clpsz, _y - clpsz, _x + clpsz, _y + clpsz);
		cclip = g_app->m_activeViewportLayout->m_activeViewport->m_currentRect;
		yySetCursorClip(&cclip, 0, g_app->m_window);
	}

	g_app->_setGizmoMode(gm);
	//printf("%s %i\n", __FUNCTION__, (s32)gm);

	for (u32 i = 0; i < g_app->m_selectedObjects.m_size; ++i)
	{
		g_app->m_selectedObjects.m_data[i]->GetPlugin()->OnClickGizmo(g_app->m_keyboardModifier, gm, g_app->m_editMode, g_app->m_selectedObjects.m_data[i]);
	}
}

void miGizmo::OnStartFrame() {
	if (g_app->m_gizmoMode != miGizmoMode::NoTransform) return;
	m_isDrawAabbHeadZ = false;
	m_isDrawAabbHeadY = false;
	m_isDrawAabbHeadX = false;
	m_isDrawAabbScaleHeadZ = false;
	m_isDrawAabbScaleHeadY = false;
	m_isDrawAabbScaleHeadX = false;
	m_isDrawAabbZY = false;
	m_isDrawAabbXY = false;
	m_isDrawAabbXZ = false;
	m_isDrawAabbZ = false;
	m_isDrawAabbY = false;
	m_isDrawAabbX = false;
	m_isIn2d = false;
	m_isRotationHoverX = false;
	m_isRotationHoverY = false;
	m_isRotationHoverZ = false;
	m_isRotationHoverScreen = false;
}

void miGizmo::OnEndFrame() {
	if (g_app->m_gizmoMode != miGizmoMode::NoTransform) return;
	if (m_isDrawAabbHeadZ) return;
	if (m_isDrawAabbHeadY) return;
	if (m_isDrawAabbHeadX) return;
	if (m_isDrawAabbScaleHeadZ) return;
	if (m_isDrawAabbScaleHeadY) return;
	if (m_isDrawAabbScaleHeadX) return;
	if (m_isDrawAabbZY) return;
	if (m_isDrawAabbXY) return;
	if (m_isDrawAabbXZ) return;
	if (m_isDrawAabbZ) return;
	if (m_isDrawAabbY) return;
	if (m_isDrawAabbX) return;
	if (m_isIn2d) return;
	if (m_isRotationHoverX) return;
	if (m_isRotationHoverY) return;
	if (m_isRotationHoverZ) return;
	if (m_isRotationHoverScreen) return;
	g_app->m_isGizmoMouseHover = false;
}

void miGizmo::OnRelease() {
	g_app->_setGizmoMode(miGizmoMode::NoTransform);
	_reset_variables();
}

void miGizmo::OnEscape() {
	m_var_move_onEscape = m_var_move;
	g_app->_setGizmoMode(miGizmoMode::NoTransform);
	_reset_variables();
}

void miGizmo::_reset_variables() {
	m_var_move_old.set(0.f);
	m_var_move.set(0.f);
	m_var_scale.set(1.f);
	m_var_scale2.set(1.f);
	m_var_rotate.set(0.f);
	m_var_rotate_snap = 0.f;
//	m_selectionAabbCenterOnClick = g_app->m_selectionAabb_center;
}