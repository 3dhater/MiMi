#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miVisualObjectImpl.h"

#include "yy_model.h"

extern miApplication * g_app;

miVisualObjectImpl::miVisualObjectImpl() {
	m_mesh = 0;
}

miVisualObjectImpl::~miVisualObjectImpl() {
	_destroy();
}

void miVisualObjectImpl::_destroy() {
	for(u32 i = 0, sz = m_nodes.size(); i < sz; ++i)
	{
		delete m_nodes[i];
	}
	m_nodes.clear();
}

void miVisualObjectImpl::_createSoftwareModel_polys() {
	const s32 triLimit = 5000;
	s32 triangleCount = 0;
	yyModel* softwareModel = 0;

	miVisualObjectImpl::node * _modelNode = 0;

	yyVertexModel* vertexModel_ptr = 0;
	yyVertexAnimatedModel* vertexAnimatedModel_ptr = 0;

	u16* inds_ptr = 0;
	u16 index = 0;

	auto current_polygon = m_mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	while (true) {

		miVec4 color(0.f,0.f,1.f,1.f);
		if (g_app->m_editMode == miEditMode::Polygon)
		{
			if (current_polygon->m_flags & miPolygon::flag_isSelected)
				color.set(0.8f, 0.f, 0.f, 0.f);
		}

		auto vertex_1 = current_polygon->m_verts.m_head;
		auto vertex_2 = vertex_1->m_right;
		auto vertex_3 = vertex_2->m_right;
		while (true) {
			if (triangleCount == 0)
			{
				softwareModel = yyMegaAllocator::CreateModel();
				softwareModel->m_indexType = yyMeshIndexType::u16;

				if (m_mesh->m_skeleton)
				{
					softwareModel->m_vertexType = yyVertexType::AnimatedModel;
					softwareModel->m_stride = sizeof(yyVertexAnimatedModel);
					softwareModel->m_vertices = (u8*)yyMemAlloc(softwareModel->m_stride * 3 * triLimit);
					softwareModel->m_indices = (u8*)yyMemAlloc(sizeof(u16) * 3 * triLimit);
					vertexAnimatedModel_ptr = (yyVertexAnimatedModel*)softwareModel->m_vertices;
				}
				else
				{
					softwareModel->m_vertexType = yyVertexType::Model;
					softwareModel->m_stride = sizeof(yyVertexModel);
					softwareModel->m_vertices = (u8*)yyMemAlloc(softwareModel->m_stride * 3 * triLimit);
					softwareModel->m_indices = (u8*)yyMemAlloc(sizeof(u16) * 3* triLimit);
					vertexModel_ptr = (yyVertexModel*)softwareModel->m_vertices;
				}

				inds_ptr = (u16*)softwareModel->m_indices;

				_modelNode = new miVisualObjectImpl::node;
				_modelNode->m_modelCPU_p = softwareModel;

				m_nodes.push_back(_modelNode);

				//softwareModelIndex = (u32)m_SoftwareModels_polys.size() - 1;
				index = 0;
			}

			if (m_mesh->m_skeleton)
			{
				vertexAnimatedModel_ptr->Color = math::miVec4_to_v4f(color);
				vertexAnimatedModel_ptr->Position = math::miVec3_to_v3f(vertex_1->m_data->m_position);
				vertexAnimatedModel_ptr->TCoords.x = vertexAnimatedModel_ptr->Position.x;
				vertexAnimatedModel_ptr->TCoords.y = vertexAnimatedModel_ptr->Position.y;
				vertexAnimatedModel_ptr->Normal.x = vertex_1->m_data->m_normal[0];
				vertexAnimatedModel_ptr->Normal.y = vertex_1->m_data->m_normal[1];
				vertexAnimatedModel_ptr->Normal.z = vertex_1->m_data->m_normal[2];
				++vertexAnimatedModel_ptr;
			}
			else
			{
				vertexModel_ptr->Color = math::miVec4_to_v4f(color);
				vertexModel_ptr->Position = math::miVec3_to_v3f(vertex_1->m_data->m_position);
				vertexModel_ptr->TCoords.x = vertexModel_ptr->Position.x;
				vertexModel_ptr->TCoords.y = vertexModel_ptr->Position.y;
				vertexModel_ptr->Normal.x = vertex_1->m_data->m_normal[0];
				vertexModel_ptr->Normal.y = vertex_1->m_data->m_normal[1];
				vertexModel_ptr->Normal.z = vertex_1->m_data->m_normal[2];
				++vertexModel_ptr;
			}
			m_aabb.add(vertex_1->m_data->m_position);

			*inds_ptr = index;
			++index;
			++inds_ptr;

			if (m_mesh->m_skeleton)
			{
				vertexAnimatedModel_ptr->Color = math::miVec4_to_v4f(color);
				vertexAnimatedModel_ptr->Position = math::miVec3_to_v3f(vertex_2->m_data->m_position);
				vertexAnimatedModel_ptr->TCoords.x = vertexAnimatedModel_ptr->Position.x;
				vertexAnimatedModel_ptr->TCoords.y = vertexAnimatedModel_ptr->Position.y;
				vertexAnimatedModel_ptr->Normal.x = vertex_2->m_data->m_normal[0];
				vertexAnimatedModel_ptr->Normal.y = vertex_2->m_data->m_normal[1];
				vertexAnimatedModel_ptr->Normal.z = vertex_2->m_data->m_normal[2];
				++vertexAnimatedModel_ptr;
			}
			else
			{
				vertexModel_ptr->Color = math::miVec4_to_v4f(color);
				vertexModel_ptr->Position = math::miVec3_to_v3f(vertex_2->m_data->m_position);
				vertexModel_ptr->TCoords.x = vertexModel_ptr->Position.x;
				vertexModel_ptr->TCoords.y = vertexModel_ptr->Position.y;
				vertexModel_ptr->Normal.x = vertex_2->m_data->m_normal[0];
				vertexModel_ptr->Normal.y = vertex_2->m_data->m_normal[1];
				vertexModel_ptr->Normal.z = vertex_2->m_data->m_normal[2];
				++vertexModel_ptr;
			}
			m_aabb.add(vertex_2->m_data->m_position);

			*inds_ptr = index;
			++index;
			++inds_ptr;

			if (m_mesh->m_skeleton)
			{
				vertexAnimatedModel_ptr->Color = math::miVec4_to_v4f(color);
				vertexAnimatedModel_ptr->Position = math::miVec3_to_v3f(vertex_3->m_data->m_position);
				vertexAnimatedModel_ptr->TCoords.x = vertexAnimatedModel_ptr->Position.x;
				vertexAnimatedModel_ptr->TCoords.y = vertexAnimatedModel_ptr->Position.y;
				vertexAnimatedModel_ptr->Normal.x = vertex_3->m_data->m_normal[0];
				vertexAnimatedModel_ptr->Normal.y = vertex_3->m_data->m_normal[1];
				vertexAnimatedModel_ptr->Normal.z = vertex_3->m_data->m_normal[2];
				++vertexAnimatedModel_ptr;
			}
			else
			{
				vertexModel_ptr->Color = math::miVec4_to_v4f(color);
				vertexModel_ptr->Position = math::miVec3_to_v3f(vertex_3->m_data->m_position);
				vertexModel_ptr->TCoords.x = vertexModel_ptr->Position.x;
				vertexModel_ptr->TCoords.y = vertexModel_ptr->Position.y;
				vertexModel_ptr->Normal.x = vertex_3->m_data->m_normal[0];
				vertexModel_ptr->Normal.y = vertex_3->m_data->m_normal[1];
				vertexModel_ptr->Normal.z = vertex_3->m_data->m_normal[2];
				++vertexModel_ptr;
			}
			m_aabb.add(vertex_3->m_data->m_position);

			*inds_ptr = index;
			++index;
			++inds_ptr;

			_modelNode->m_modelCPU_p->m_vCount += 3;
			_modelNode->m_modelCPU_p->m_iCount += 3;

			++triangleCount;
			if (triangleCount == triLimit)
				triangleCount = 0;

			// ===============================
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

void miVisualObjectImpl::CreateNewGPUModels(miMesh* mesh) {
	m_mesh = mesh;
	_destroy();
	m_aabb.reset();
	_createSoftwareModel_polys();
	for (u32 i = 0, sz = m_nodes.size(); i < sz; ++i)
	{
		auto node = m_nodes[i];
		node->m_texture = yyGetDefaultTexture();
		node->m_modelGPU_p = yyCreateModel(node->m_modelCPU_p);
		node->m_modelGPU_p->Load();
	}
}

miAabb miVisualObjectImpl::GetAabb() {
	return m_aabb;
}

size_t miVisualObjectImpl::GetBufferCount() {
	return (size_t)m_nodes.size();
}

unsigned char* miVisualObjectImpl::GetVertexBuffer(size_t index) {
	return m_nodes[index]->m_modelCPU_p->m_vertices;
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
		node->m_modelGPU_p->MapModelForWriteVerts(&verts);
		memcpy(verts, node->m_modelCPU_p->m_vertices, node->m_modelCPU_p->m_vCount * node->m_modelCPU_p->m_stride);
		node->m_modelGPU_p->UnmapModelForWriteVerts();
	}
}

void miVisualObjectImpl::Draw(miMatrix* mim) {	
	auto camera = g_app->GetActiveCamera();
	
	Mat4 World;

	for (u32 i = 0, sz = m_nodes.size(); i < sz; ++i)
	{
		auto node = m_nodes[i];

		g_app->m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection,
			camera->m_projectionMatrix * camera->m_viewMatrix * World);
		g_app->m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, World);
		g_app->m_gpu->SetModel(node->m_modelGPU_p);
//		g_app->m_gpu->SetMaterial();
		g_app->m_gpu->SetTexture(0, node->m_texture);
		g_app->m_gpu->Draw();
	}
}
