#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miVisualObjectImpl.h"

#include "yy_model.h"

extern miApplication * g_app;

miVisualObjectImpl::miVisualObjectImpl() {
}

miVisualObjectImpl::~miVisualObjectImpl() {
	_destroy();
}

void miVisualObjectImpl::_destroy() {
	for(u32 i = 0, sz = m_nodes.size(); i < sz; ++i)
	{
		delete m_nodes[i];
	}
}

void miVisualObjectImpl::CreateNewGPUModels(miMesh* mesh) {
	_destroy();
	m_aabb.reset();

	const s32 triLimit = 5000;
	s32 triangleCount = 0;
	yyModel* softwareModel = 0;
	yyVertexModel* verts_ptr = 0;
	u16* inds_ptr = 0;
	u16 index = 0;

	auto current_polygon = mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	while (true) {
		
		auto vertex_1 = current_polygon->m_verts.m_head;
		//auto last_vertex = first_vertex->m_left;
		auto vertex_2 = vertex_1->m_right;
		auto vertex_3 = vertex_2->m_right;
		while (true) {
			
			vertex_2 = vertex_2->m_right;
			vertex_3 = vertex_3->m_right;

			if (vertex_3 == vertex_1)
				break;
		}


		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}
}

miAabb miVisualObjectImpl::GetAabb() {
	return m_aabb;
}

size_t miVisualObjectImpl::GetBufferCount() {
	return (size_t)m_nodes.size();
}

unsigned char* miVisualObjectImpl::GetVertexBuffer(size_t index) {
	return m_nodes[index]->m_modelCPU->m_vertices;
}

void miVisualObjectImpl::MarkBufferToRemap(size_t index) {
	m_nodes[index]->m_remap = true;
}

void miVisualObjectImpl::RemapBuffers() {
	for (u32 i = 0, sz = m_nodes.size(); i < sz; ++i)
	{
		auto node = m_nodes[i];
		if (!node->m_remap)
			continue;

		u8* verts = 0;
		node->m_modelGPU->MapModelForWriteVerts(&verts);
		memcpy(verts, node->m_modelCPU->m_vertices, node->m_modelCPU->m_vCount * node->m_modelCPU->m_stride);
		node->m_modelGPU->UnmapModelForWriteVerts();
	}
}

void miVisualObjectImpl::Draw(miMatrix* mim) {	
	auto camera = g_app->GetActiveCamera();
	
	Mat4 World = math::miMatrix_to_Mat4(*mim);

	for (u32 i = 0, sz = m_nodes.size(); i < sz; ++i)
	{
		auto node = m_nodes[i];

		g_app->m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection,
			camera->m_projectionMatrix * camera->m_viewMatrix * World);
		g_app->m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, World);
		g_app->m_gpu->SetModel(node->m_modelGPU);
		g_app->m_gpu->Draw();
	}
}
