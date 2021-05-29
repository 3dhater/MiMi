#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miVisualObjectImpl.h"

#include "yy_model.h"

extern miApplication * g_app;

miVisualObjectImpl::miVisualObjectImpl() {
	m_texture = 0;
	m_mesh = 0;

}

miVisualObjectImpl::~miVisualObjectImpl() {
	_destroy();
	if (m_texture)
	{
		if (m_texture->IsLoaded())
			m_texture->Unload();
		yyDeleteTexture(m_texture, true);
	}
}

void miVisualObjectImpl::_destroy() {
	for(u32 i = 0, sz = m_nodes_polygons_GPU.size(); i < sz; ++i){delete m_nodes_polygons_GPU[i];}
	m_nodes_polygons_GPU.clear();
	for (u32 i = 0, sz = m_nodes_polygons_CPU.size(); i < sz; ++i) { delete m_nodes_polygons_CPU[i]; }
	m_nodes_polygons_CPU.clear();

	for (u32 i = 0, sz = m_nodes_edges_GPU.size(); i < sz; ++i){delete m_nodes_edges_GPU[i];}
	m_nodes_edges_GPU.clear();
	for (u32 i = 0, sz = m_nodes_edges_CPU.size(); i < sz; ++i) { delete m_nodes_edges_CPU[i]; }
	m_nodes_edges_CPU.clear();

	for (u32 i = 0, sz = m_nodes_verts_GPU.size(); i < sz; ++i) { delete m_nodes_verts_GPU[i]; }
	m_nodes_verts_GPU.clear();
	for (u32 i = 0, sz = m_nodes_verts_CPU.size(); i < sz; ++i) { delete m_nodes_verts_CPU[i]; }
	m_nodes_verts_CPU.clear();
}

void miVisualObjectImpl::_createSoftwareModel_verts() {
	const s32 pointLimit = 200;
	s32 pointCount = 0;
	yyModel* softwareModel = 0;
	miVisualObjectImpl::model_node_CPU * _modelNode = 0;
	yyVertexPoint* vertex_ptr = 0;
	yyVertexAnimatedPoint* vertexAnimated_ptr = 0;

	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;
	while (true) {

		v4f color(0.f, 0.f, 1.f, 1.f);
		if (g_app->m_editMode == miEditMode::Vertex)
		{
			if (current_vertex->m_flags & miEdge::flag_isSelected)
				color.set(1.0f, 0.f, 0.f, 1.f);
		}

		if (pointCount == 0)
		{
			softwareModel = yyMegaAllocator::CreateModel();
			softwareModel->m_indexType = yyMeshIndexType::u16;

			if (m_mesh->m_skeleton)
			{
				softwareModel->m_vertexType = yyVertexType::AnimatedPoint;
				softwareModel->m_stride = sizeof(yyVertexAnimatedPoint);
				softwareModel->m_vertices = (u8*)yyMemAlloc(softwareModel->m_stride * 4 * pointLimit);
				vertexAnimated_ptr = (yyVertexAnimatedPoint*)softwareModel->m_vertices;
			}
			else
			{
				softwareModel->m_vertexType = yyVertexType::Point;
				softwareModel->m_stride = sizeof(yyVertexPoint);
				softwareModel->m_vertices = (u8*)yyMemAlloc(softwareModel->m_stride * 4 * pointLimit);
				vertex_ptr = (yyVertexPoint*)softwareModel->m_vertices;
			}

			_modelNode = new miVisualObjectImpl::model_node_CPU;
			_modelNode->m_modelCPU = softwareModel;

			m_nodes_verts_CPU.push_back(_modelNode);
		}

		auto vpos = mimath::miVec3_to_v3f(current_vertex->m_position);
		float size = 1.f;

		if (m_mesh->m_skeleton)
		{
			vertexAnimated_ptr->Color = color;
			vertexAnimated_ptr->Position = vpos;
			vertexAnimated_ptr++;
		}
		else
		{
			vertex_ptr->Color = color;
			vertex_ptr->Position = vpos;
			vertex_ptr++;
		}

		_modelNode->m_modelCPU->m_vCount++;
		_modelNode->m_modelCPU->m_iCount++;

		++pointCount;
		if (pointCount == pointLimit)
			pointCount = 0;

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}
}
void miVisualObjectImpl::_createSoftwareModel_edges() {
	const s32 lineLimit = 400;
	s32 lineCount = 0;
	yyModel* softwareModel = 0;

	miVisualObjectImpl::model_node_CPU * _modelNode = 0;

	yyVertexLine* vertex_ptr = 0;
	yyVertexAnimatedLine* vertexAnimated_ptr = 0;

	u16* inds_ptr = 0;
	u16 index = 0;

	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;
	while (true) {

		//miVec4 color = *m_parentSceneObject->GetEdgeColor();
		miVec4 color(1.f);
		if (g_app->m_editMode == miEditMode::Edge)
		{
			if (current_edge->m_flags & miEdge::flag_isSelected)
				color.set(1.0f, 0.f, 0.f, 0.f);
		}

		if (lineCount == 0)
		{
			softwareModel = yyMegaAllocator::CreateModel();
			softwareModel->m_indexType = yyMeshIndexType::u16;

			if (m_mesh->m_skeleton)
			{
				softwareModel->m_vertexType = yyVertexType::AnimatedLineModel;
				softwareModel->m_stride = sizeof(yyVertexAnimatedLine);
				softwareModel->m_vertices = (u8*)yyMemAlloc(softwareModel->m_stride * 2 * lineLimit);
				softwareModel->m_indices = (u8*)yyMemAlloc(sizeof(u16) * 2 * lineLimit);
				vertexAnimated_ptr = (yyVertexAnimatedLine*)softwareModel->m_vertices;
			}
			else
			{
				softwareModel->m_vertexType = yyVertexType::LineModel;
				softwareModel->m_stride = sizeof(yyVertexLine);
				softwareModel->m_vertices = (u8*)yyMemAlloc(softwareModel->m_stride * 2 * lineLimit);
				softwareModel->m_indices = (u8*)yyMemAlloc(sizeof(u16) * 2 * lineLimit);
				vertex_ptr = (yyVertexLine*)softwareModel->m_vertices;
			}

			inds_ptr = (u16*)softwareModel->m_indices;

			_modelNode = new miVisualObjectImpl::model_node_CPU;
			_modelNode->m_modelCPU = softwareModel;

			m_nodes_edges_CPU.push_back(_modelNode);

			index = 0;
		}

		if (m_mesh->m_skeleton)
		{
			vertexAnimated_ptr->Color = mimath::miVec4_to_v4f(color);
			vertexAnimated_ptr->Position = mimath::miVec3_to_v3f(current_edge->m_vertex1->m_position);
			vertexAnimated_ptr->Normal.x = current_edge->m_vertex1->m_normal[0];
			vertexAnimated_ptr->Normal.y = current_edge->m_vertex1->m_normal[1];
			vertexAnimated_ptr->Normal.z = current_edge->m_vertex1->m_normal[2];
			++vertexAnimated_ptr;
		}
		else
		{
			vertex_ptr->Color = mimath::miVec4_to_v4f(color);
			vertex_ptr->Position = mimath::miVec3_to_v3f(current_edge->m_vertex1->m_position);
			vertex_ptr->Normal.x = current_edge->m_vertex1->m_normal[0];
			vertex_ptr->Normal.y = current_edge->m_vertex1->m_normal[1];
			vertex_ptr->Normal.z = current_edge->m_vertex1->m_normal[2];
			++vertex_ptr;
		}
		*inds_ptr = index;
		++index;
		++inds_ptr;

		if (m_mesh->m_skeleton)
		{
			vertexAnimated_ptr->Color = mimath::miVec4_to_v4f(color);
			vertexAnimated_ptr->Position = mimath::miVec3_to_v3f(current_edge->m_vertex2->m_position);
			vertexAnimated_ptr->Normal.x = current_edge->m_vertex2->m_normal[0];
			vertexAnimated_ptr->Normal.y = current_edge->m_vertex2->m_normal[1];
			vertexAnimated_ptr->Normal.z = current_edge->m_vertex2->m_normal[2];
			++vertexAnimated_ptr;
		}
		else
		{
			vertex_ptr->Color = mimath::miVec4_to_v4f(color);
			vertex_ptr->Position = mimath::miVec3_to_v3f(current_edge->m_vertex2->m_position);
			vertex_ptr->Normal.x = current_edge->m_vertex2->m_normal[0];
			vertex_ptr->Normal.y = current_edge->m_vertex2->m_normal[1];
			vertex_ptr->Normal.z = current_edge->m_vertex2->m_normal[2];
			++vertex_ptr;
		}
		*inds_ptr = index;
		++index;
		++inds_ptr;

		_modelNode->m_modelCPU->m_vCount += 2;
		_modelNode->m_modelCPU->m_iCount += 2;

		++lineCount;
		if (lineCount == lineLimit)
			lineCount = 0;

		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
}

void miVisualObjectImpl::_createSoftwareModel_polys() {
	const s32 triLimit = 5000;
	s32 triangleCount = 0;
	yyModel* softwareModel = 0;

	miVisualObjectImpl::model_node_CPU * _modelNode = 0;

	yyVertexTriangle* vertexModel_ptr = 0;
	yyVertexAnimatedTriangle* vertexAnimatedModel_ptr = 0;

	u16* inds_ptr = 0;
	u16 index = 0;

	auto current_polygon = m_mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	while (true) {

		miVec4 color(0.f,0.f,0.f,0.0f);
		if (g_app->m_editMode == miEditMode::Polygon)
		{
			if (current_polygon->m_flags & miPolygon::flag_isSelected)
				color.set(1.0f, 0.f, 0.f, 1.f);
		}

		auto vertex_1 = current_polygon->m_verts.m_head;
		auto vertex_3 = vertex_1->m_right;
		auto vertex_2 = vertex_3->m_right;
		while (true) {
			if (triangleCount == 0)
			{
				softwareModel = yyMegaAllocator::CreateModel();
				softwareModel->m_indexType = yyMeshIndexType::u16;

				if (m_mesh->m_skeleton)
				{
					softwareModel->m_vertexType = yyVertexType::AnimatedModel;
					softwareModel->m_stride = sizeof(yyVertexAnimatedTriangle);
					softwareModel->m_vertices = (u8*)yyMemAlloc(softwareModel->m_stride * 3 * triLimit);
					softwareModel->m_indices = (u8*)yyMemAlloc(sizeof(u16) * 3 * triLimit);
					vertexAnimatedModel_ptr = (yyVertexAnimatedTriangle*)softwareModel->m_vertices;
				}
				else
				{
					softwareModel->m_vertexType = yyVertexType::Model;
					softwareModel->m_stride = sizeof(yyVertexTriangle);
					softwareModel->m_vertices = (u8*)yyMemAlloc(softwareModel->m_stride * 3 * triLimit);
					softwareModel->m_indices = (u8*)yyMemAlloc(sizeof(u16) * 3* triLimit);
					vertexModel_ptr = (yyVertexTriangle*)softwareModel->m_vertices;
				}

				inds_ptr = (u16*)softwareModel->m_indices;

				_modelNode = new miVisualObjectImpl::model_node_CPU;
				_modelNode->m_modelCPU = softwareModel;

				m_nodes_polygons_CPU.push_back(_modelNode);

				//softwareModelIndex = (u32)m_SoftwareModels_polys.size() - 1;
				index = 0;
			}

			if (m_mesh->m_skeleton)
			{
				vertexAnimatedModel_ptr->Color = mimath::miVec4_to_v4f(color);
				vertexAnimatedModel_ptr->Position = mimath::miVec3_to_v3f(vertex_1->m_data->m_position);
				vertexAnimatedModel_ptr->TCoords = mimath::miVec2_to_v2f(vertex_1->m_data->m_tCoords);
				vertexAnimatedModel_ptr->Normal.x = vertex_1->m_data->m_normal[0];
				vertexAnimatedModel_ptr->Normal.y = vertex_1->m_data->m_normal[1];
				vertexAnimatedModel_ptr->Normal.z = vertex_1->m_data->m_normal[2];
				++vertexAnimatedModel_ptr;
			}
			else
			{
				vertexModel_ptr->Color = mimath::miVec4_to_v4f(color);
				vertexModel_ptr->Position = mimath::miVec3_to_v3f(vertex_1->m_data->m_position);
				vertexModel_ptr->TCoords = mimath::miVec2_to_v2f(vertex_1->m_data->m_tCoords);
				vertexModel_ptr->Normal.x = vertex_1->m_data->m_normal[0];
				vertexModel_ptr->Normal.y = vertex_1->m_data->m_normal[1];
				vertexModel_ptr->Normal.z = vertex_1->m_data->m_normal[2];
				++vertexModel_ptr;
			}
			m_aabb.add(vertex_1->m_data->m_position);

			//printf("Y: %f\n", vertex_1->m_data->m_position.y);

			*inds_ptr = index;
			++index;
			++inds_ptr;

			if (m_mesh->m_skeleton)
			{
				vertexAnimatedModel_ptr->Color = mimath::miVec4_to_v4f(color);
				vertexAnimatedModel_ptr->Position = mimath::miVec3_to_v3f(vertex_2->m_data->m_position);
				vertexAnimatedModel_ptr->TCoords = mimath::miVec2_to_v2f(vertex_2->m_data->m_tCoords);
				vertexAnimatedModel_ptr->Normal.x = vertex_2->m_data->m_normal[0];
				vertexAnimatedModel_ptr->Normal.y = vertex_2->m_data->m_normal[1];
				vertexAnimatedModel_ptr->Normal.z = vertex_2->m_data->m_normal[2];
				++vertexAnimatedModel_ptr;
			}
			else
			{
				vertexModel_ptr->Color = mimath::miVec4_to_v4f(color);
				vertexModel_ptr->Position = mimath::miVec3_to_v3f(vertex_2->m_data->m_position);
				vertexModel_ptr->TCoords = mimath::miVec2_to_v2f(vertex_2->m_data->m_tCoords);
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
				vertexAnimatedModel_ptr->Color = mimath::miVec4_to_v4f(color);
				vertexAnimatedModel_ptr->Position = mimath::miVec3_to_v3f(vertex_3->m_data->m_position);
				vertexAnimatedModel_ptr->TCoords = mimath::miVec2_to_v2f(vertex_3->m_data->m_tCoords);
				vertexAnimatedModel_ptr->Normal.x = vertex_3->m_data->m_normal[0];
				vertexAnimatedModel_ptr->Normal.y = vertex_3->m_data->m_normal[1];
				vertexAnimatedModel_ptr->Normal.z = vertex_3->m_data->m_normal[2];
				++vertexAnimatedModel_ptr;
			}
			else
			{
				vertexModel_ptr->Color = mimath::miVec4_to_v4f(color);
				vertexModel_ptr->Position = mimath::miVec3_to_v3f(vertex_3->m_data->m_position);
				vertexModel_ptr->TCoords = mimath::miVec2_to_v2f(vertex_3->m_data->m_tCoords);
				vertexModel_ptr->Normal.x = vertex_3->m_data->m_normal[0];
				vertexModel_ptr->Normal.y = vertex_3->m_data->m_normal[1];
				vertexModel_ptr->Normal.z = vertex_3->m_data->m_normal[2];
				++vertexModel_ptr;
			}
			m_aabb.add(vertex_3->m_data->m_position);

			*inds_ptr = index;
			++index;
			++inds_ptr;

			_modelNode->m_modelCPU->m_vCount += 3;
			_modelNode->m_modelCPU->m_iCount += 3;

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

//void miVisualObjectImpl::UpdateCPUModelsOnly(miMesh* mesh) {
//	for (u32 i = 0, sz = m_nodes_polygons_CPU.size(); i < sz; ++i)
//	{
//		delete m_nodes_polygons_CPU[i];
//	}
//	m_nodes_polygons_CPU.clear();
//
//	for (u32 i = 0, sz = m_nodes_edges_CPU.size(); i < sz; ++i)
//	{
//		delete m_nodes_edges_CPU[i];
//	}
//	m_nodes_edges_CPU.clear();
//
//	m_aabb.reset();
//	_createSoftwareModel_polys();
//	_createSoftwareModel_edges();
//	_createSoftwareModel_verts();
//}
void miVisualObjectImpl::CreateNewGPUModels(miMesh* mesh) {
	m_mesh = mesh;
	_destroy();
	m_aabb.reset();
	_createSoftwareModel_polys();
	_createSoftwareModel_edges();
	_createSoftwareModel_verts();

	for (u32 i = 0, sz = m_nodes_polygons_CPU.size(); i < sz; ++i)
	{
		auto node = m_nodes_polygons_CPU[i];

		auto _modelNode = new miVisualObjectImpl::model_node_GPU;

		_modelNode->m_modelGPU = yyCreateModel(node->m_modelCPU);
		_modelNode->m_modelGPU->Load();
		m_nodes_polygons_GPU.push_back(_modelNode);
	}

	for (u32 i = 0, sz = m_nodes_edges_CPU.size(); i < sz; ++i)
	{
		auto node = m_nodes_edges_CPU[i];
		auto _modelNode = new miVisualObjectImpl::model_node_GPU;
		_modelNode->m_modelGPU = yyCreateModel(node->m_modelCPU);
		_modelNode->m_modelGPU->Load();
		m_nodes_edges_GPU.push_back(_modelNode);
	}

	for (u32 i = 0, sz = m_nodes_verts_CPU.size(); i < sz; ++i)
	{
		auto node = m_nodes_verts_CPU[i];
		auto _modelNode = new miVisualObjectImpl::model_node_GPU;
		_modelNode->m_modelGPU = yyCreateModel(node->m_modelCPU);
		_modelNode->m_modelGPU->Load();
		m_nodes_verts_GPU.push_back(_modelNode);
	}
}

miAabb miVisualObjectImpl::GetAabb() {
	return m_aabb;
}

size_t miVisualObjectImpl::GetBufferCount() {
	//return (size_t)m_nodes_polygons.size();
	return 0;
}

unsigned char* miVisualObjectImpl::GetVertexBuffer(size_t index) {
	//return m_nodes_polygons[index]->m_modelCPU->m_vertices;
	return 0;
}

//void miVisualObjectImpl::MarkBufferToRemap(size_t index) {
//	m_nodes_polygons_GPU[index]->m_remap = true;
//	//m_nodes_edges[index]->m_remap = true;
//}

//void miVisualObjectImpl::RemapAllBuffers() {
//	for (u32 i = 0, sz = m_nodes_polygons_GPU.size(); i < sz; ++i)
//	{
//		auto gpu_node = m_nodes_polygons_GPU[i];
//		auto cpu_node = m_nodes_polygons_CPU[i];
//		u8* verts = 0;
//		gpu_node->m_modelGPU->MapModelForWriteVerts(&verts);
//		memcpy(verts, cpu_node->m_modelCPU->m_vertices, cpu_node->m_modelCPU->m_vCount * cpu_node->m_modelCPU->m_stride);
//		gpu_node->m_modelGPU->UnmapModelForWriteVerts();
//	}
//	for (u32 i = 0, sz = m_nodes_edges_GPU.size(); i < sz; ++i)
//	{
//		auto gpu_node = m_nodes_edges_GPU[i];
//		auto cpu_node = m_nodes_edges_CPU[i];
//		u8* verts = 0;
//		gpu_node->m_modelGPU->MapModelForWriteVerts(&verts);
//		memcpy(verts, cpu_node->m_modelCPU->m_vertices, cpu_node->m_modelCPU->m_vCount * cpu_node->m_modelCPU->m_stride);
//		gpu_node->m_modelGPU->UnmapModelForWriteVerts();
//	}
//}
void miVisualObjectImpl::RemapBuffers() {
	for (u32 i = 0, sz = m_nodes_polygons_GPU.size(); i < sz; ++i)
	{
		auto gpu_node = m_nodes_polygons_GPU[i];
		//if (!gpu_node->m_remap)
		//	continue;
		auto cpu_node = m_nodes_polygons_CPU[i];
		u8* verts = 0;
		gpu_node->m_modelGPU->MapModelForWriteVerts(&verts);
		memcpy(verts, cpu_node->m_modelCPU->m_vertices, cpu_node->m_modelCPU->m_vCount * cpu_node->m_modelCPU->m_stride);
		gpu_node->m_modelGPU->UnmapModelForWriteVerts();
	}
	for (u32 i = 0, sz = m_nodes_edges_GPU.size(); i < sz; ++i)
	{
		auto gpu_node = m_nodes_edges_GPU[i];
		//if (!gpu_node->m_remap)
		//	continue;
		auto cpu_node = m_nodes_edges_CPU[i];
		u8* verts = 0;
		gpu_node->m_modelGPU->MapModelForWriteVerts(&verts);
		memcpy(verts, cpu_node->m_modelCPU->m_vertices, cpu_node->m_modelCPU->m_vCount * cpu_node->m_modelCPU->m_stride);
		gpu_node->m_modelGPU->UnmapModelForWriteVerts();
	}
	for (u32 i = 0, sz = m_nodes_verts_GPU.size(); i < sz; ++i)
	{
		auto gpu_node = m_nodes_verts_GPU[i];
		//if (!gpu_node->m_remap)
		//	continue;
		auto cpu_node = m_nodes_verts_CPU[i];
		u8* verts = 0;
		gpu_node->m_modelGPU->MapModelForWriteVerts(&verts);
		memcpy(verts, cpu_node->m_modelCPU->m_vertices, cpu_node->m_modelCPU->m_vCount * cpu_node->m_modelCPU->m_stride);
		gpu_node->m_modelGPU->UnmapModelForWriteVerts();
	}
}

void miVisualObjectImpl::Draw(miMatrix* mim) {	
	auto camera = g_app->m_currentViewportDraw->m_activeCamera;
	
	Mat4 World = mimath::miMatrix_to_Mat4(*mim);

	static yyMaterial default_polygon_material;
	default_polygon_material.m_baseColor.set(1.f, 1.f, 1.f, 1.f);

	if (!m_texture) {
		m_texture = yyGetTextureFromCache("../res/exit.png");
		//m_texture = yyGetDefaultTexture();
		if(!m_texture->IsLoaded())
			m_texture->Load();
	}

	if (g_app->m_currentViewportDraw->m_drawMode == miViewport::Draw_Material
		|| g_app->m_currentViewportDraw->m_drawMode == miViewport::Draw_MaterialWireframe)
	{
		for (u32 i = 0, sz = m_nodes_polygons_GPU.size(); i < sz; ++i)
		{
			auto node = m_nodes_polygons_GPU[i];

			yySetMatrix(yyMatrixType::WorldViewProjection,
				camera->m_projectionMatrix * camera->m_viewMatrix * World);
			yySetMatrix(yyMatrixType::World, World);
			g_app->m_gpu->SetModel(node->m_modelGPU);
			g_app->m_gpu->SetTexture(0, m_texture);

			default_polygon_material.m_cullBackFace = true;
			yySetMaterial(default_polygon_material);

			g_app->m_gpu->Draw();
		}
	}

	if (g_app->m_currentViewportDraw->m_drawMode == miViewport::Draw_Wireframe
		|| g_app->m_currentViewportDraw->m_drawMode == miViewport::Draw_MaterialWireframe
		|| g_app->m_editMode == miEditMode::Edge)
	{
		static yyMaterial wireframe_model_material;
		auto ec = this->m_parentSceneObject->GetEdgeColor();
		if (this->m_parentSceneObject->IsSelected())
		{
			wireframe_model_material.m_baseColor.m_data[0] = 1.f;
			wireframe_model_material.m_baseColor.m_data[1] = 1.f;
			wireframe_model_material.m_baseColor.m_data[2] = 1.f;
		}
		else
		{
			wireframe_model_material.m_baseColor.m_data[0] = ec->x;
			wireframe_model_material.m_baseColor.m_data[1] = ec->y;
			wireframe_model_material.m_baseColor.m_data[2] = ec->z;
		}
		yySetMaterial(wireframe_model_material);

		for (u32 i = 0, sz = m_nodes_edges_GPU.size(); i < sz; ++i)
		{
			auto node = m_nodes_edges_GPU[i];

			yySetMatrix(yyMatrixType::WorldViewProjection,
				camera->m_projectionMatrix * camera->m_viewMatrix * World);
			yySetMatrix(yyMatrixType::World, World);
			g_app->m_gpu->SetModel(node->m_modelGPU);
			g_app->m_gpu->SetTexture(0, m_texture);
			g_app->m_gpu->Draw();
		}
	}

	if ( g_app->m_editMode == miEditMode::Vertex )
	{
		auto Vi = camera->m_viewMatrix;
		Vi.m_data[3].set(0.f, 0.f, 0.f, 1.f);
		Vi.invert();

		for (u32 i = 0, sz = m_nodes_verts_GPU.size(); i < sz; ++i)
		{
			auto node = m_nodes_verts_GPU[i];

			yySetMatrix(yyMatrixType::WorldViewProjection,
				camera->m_projectionMatrix * camera->m_viewMatrix * World);
			yySetMatrix(yyMatrixType::ViewInvert, Vi);
			yySetMatrix(yyMatrixType::World, World);
			g_app->m_gpu->SetModel(node->m_modelGPU);
			//		g_app->m_gpu->SetMaterial();
			g_app->m_gpu->SetTexture(0, m_texture);
			g_app->m_gpu->Draw();
		}
	}
}

bool miVisualObjectImpl::IsInSelectionFrust(miSelectionFrust* sf) {
	assert(sf);
	if (!m_mesh->m_first_edge)
		return false;
	auto rM = this->m_parentSceneObject->GetRotationMatrix();
	auto position = this->m_parentSceneObject->GetGlobalPosition();

	bool result = false;

	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;
	
	while (true) {

		if (sf->LineInFrust(
				mimath::mul(current_edge->m_vertex1->m_position, *rM) + *position,
				mimath::mul(current_edge->m_vertex2->m_position, *rM) + *position)
			)
		{
			result = true;
			break;
		}

		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}

	return result;
}

void miVisualObjectImpl::SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	assert(sf);

}

void miVisualObjectImpl::Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	assert(sf);

}

void miVisualObjectImpl::SelectAll() {

}

void miVisualObjectImpl::DeselectAll() {

}

bool miVisualObjectImpl::IsRayIntersect(miRay* r, miVec4* ip, float* d) {
	if (!m_mesh)
		return false;
	assert(r);
	assert(ip);
	assert(d);
	if (!m_mesh->m_first_polygon)
		return false;

	auto rM = this->m_parentSceneObject->GetRotationMatrix();
	auto position = this->m_parentSceneObject->GetGlobalPosition();

	if (!this->m_parentSceneObject->GetAABBTransformed()->rayTest(*r))
		return false;

	auto current_polygon = m_mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	
	while (true) {
		auto vertex_1 = current_polygon->m_verts.m_head;
		auto vertex_2 = vertex_1->m_right;
		auto vertex_3 = vertex_2->m_right;
		while (true) {
			auto p1 = mimath::mul( vertex_1->m_data->m_position, *rM );
			auto p2 = mimath::mul(vertex_2->m_data->m_position, *rM);
			auto p3 = mimath::mul(vertex_3->m_data->m_position, *rM);

			p1.add(*position);
			p2.add(*position);
			p3.add(*position);

			miTriangle tri(p1,p2,p3);
			f32 U = 0.f;
			f32 V = 0.f;
			f32 W = 0.f;
			if (tri.rayTest_MT(*r, true, *d, U, V, W))
			{
				//printf("a");
				return true;
			}
			

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

	return false;
}