#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miVisualObjectImpl.h"

#include "yy_model.h"

extern miApplication * g_app;

miVisualObjectImpl::miVisualObjectImpl() {
	//m_texture = 0;
	m_mesh = 0;
	m_type = miVisualObjectType::Polygon;

	m_node_UV_GPU = 0;
	m_node_UV_CPU = 0;
}

miVisualObjectImpl::~miVisualObjectImpl() {
	_destroy();
}

void miVisualObjectImpl::_destroy_UV() {
	if (m_node_UV_GPU) {
		delete m_node_UV_GPU;
		m_node_UV_GPU = 0;
	}
	if (m_node_UV_CPU) {
		delete m_node_UV_CPU;
		m_node_UV_CPU = 0;
	}
}

void miVisualObjectImpl::_destroy() {
	_destroy_UV();
	for (u32 i = 0, sz = m_nodes_GPU.size(); i < sz; ++i) { delete m_nodes_GPU[i]; }
	m_nodes_GPU.clear();
	for (u32 i = 0, sz = m_nodes_CPU.size(); i < sz; ++i) { delete m_nodes_CPU[i]; }
	m_nodes_CPU.clear();
}

void miVisualObjectImpl::_createSoftwareModel_polys_UV() {
	if (!m_mesh->m_first_polygon)
		return;

	u32 vCount = 3;
	u32 iCount = 3;
	{
		auto c = m_mesh->m_first_polygon;
		if (c)
		{
			auto l = c->m_left;
			while (true)
			{
				if (c->IsSelected())
				{
					auto vertex_1 = c->m_verts.m_head;
					auto vertex_3 = vertex_1->m_right;
					auto vertex_2 = vertex_3->m_right;
					while (true) {
						vertex_2 = vertex_2->m_right;
						vertex_3 = vertex_3->m_right;
						vCount += 3;
						iCount += 3;

						if (vertex_2 == vertex_1)
							break;
					}

				}
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}
	if (!vCount)
		return;
	m_node_UV_CPU = new miVisualObjectImpl::model_node_UV_CPU;
	m_node_UV_CPU->m_modelCPU = yyMegaAllocator::CreateModel();
	m_node_UV_CPU->m_modelCPU->m_indexType = yyMeshIndexType::u32;

	m_node_UV_CPU->m_modelCPU->m_vertexType = yyVertexType::Model;
	m_node_UV_CPU->m_modelCPU->m_stride = sizeof(yyVertexTriangle);
	m_node_UV_CPU->m_modelCPU->m_vertices = (u8*)yyMemAlloc(m_node_UV_CPU->m_modelCPU->m_stride * vCount);
	yyVertexTriangle* vertex_ptr = (yyVertexTriangle*)m_node_UV_CPU->m_modelCPU->m_vertices;

	m_node_UV_CPU->m_modelCPU->m_indices = (u8*)yyMemAlloc(sizeof(u32) * iCount);

	u32 * inds_ptr = (u32*)m_node_UV_CPU->m_modelCPU->m_indices;
	u32 index = 0;

	auto c = m_mesh->m_first_polygon;
	if (c)
	{
		auto l = c->m_left;
		while (true)
		{
			if (c->IsSelected())
			{
				auto vertex_1 = c->m_verts.m_head;
				auto vertex_3 = vertex_1->m_right;
				auto vertex_2 = vertex_3->m_right;
				while (true) {
					vertex_ptr->Color.set(1.f,1.f, 1.f, 0.f);
					vertex_ptr->Position.set(vertex_1->m_data.m_uv.x, 0.01, vertex_1->m_data.m_uv.y);
					vertex_ptr->TCoords = vertex_1->m_data.m_uv;
					vertex_ptr->Normal = v3f(0.f,1.f,0.f);
					
					++vertex_ptr;
					*inds_ptr = index;
					++index;
					++inds_ptr;

					vertex_ptr->Color.set(1.f, 1.f, 1.f, 0.f);
					vertex_ptr->Position.set(vertex_2->m_data.m_uv.x, 0.01, vertex_2->m_data.m_uv.y);
					vertex_ptr->TCoords = vertex_2->m_data.m_uv;
					vertex_ptr->Normal = v3f(0.f, 1.f, 0.f);

					++vertex_ptr;
					*inds_ptr = index;
					++index;
					++inds_ptr;

					vertex_ptr->Color.set(1.f, 1.f, 1.f, 0.f);
					vertex_ptr->Position.set(vertex_3->m_data.m_uv.x, 0.01, vertex_3->m_data.m_uv.y);
					vertex_ptr->TCoords = vertex_3->m_data.m_uv;
					vertex_ptr->Normal = v3f(0.f, 1.f, 0.f);

					++vertex_ptr;
					*inds_ptr = index;
					++index;
					++inds_ptr;

					vertex_2 = vertex_2->m_right;
					vertex_3 = vertex_3->m_right;

					m_node_UV_CPU->m_modelCPU->m_vCount += 3;
					m_node_UV_CPU->m_modelCPU->m_iCount += 3;
					
					if (vertex_2 == vertex_1)
						break;
				}

			}
			if (c == l)
				break;
			c = c->m_right;
		}
	}
}

void miVisualObjectImpl::_createSoftwareModel_edges_UV() {
	if (!m_mesh->m_first_polygon)
		return;

	if (!m_mesh->m_uvCount)
		m_mesh->UpdateCounts();
	u32 vCount = m_mesh->m_uvCount;

	m_node_UV_CPU = new miVisualObjectImpl::model_node_UV_CPU;
	m_node_UV_CPU->m_modelCPU = yyMegaAllocator::CreateModel();
	m_node_UV_CPU->m_modelCPU->m_indexType = yyMeshIndexType::u32;

	m_node_UV_CPU->m_modelCPU->m_vertexType = yyVertexType::LineModel;
	m_node_UV_CPU->m_modelCPU->m_stride = sizeof(yyVertexLine);
	m_node_UV_CPU->m_modelCPU->m_vertices = (u8*)yyMemAlloc(m_node_UV_CPU->m_modelCPU->m_stride * vCount * 2);
	yyVertexLine* vertex_ptr = (yyVertexLine*)m_node_UV_CPU->m_modelCPU->m_vertices;
	
	m_node_UV_CPU->m_modelCPU->m_indices = (u8*)yyMemAlloc(sizeof(u32) * vCount * 2);

	u32 * inds_ptr = (u32*)m_node_UV_CPU->m_modelCPU->m_indices;
	u32 index = 0;

	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				auto nv = cv->m_right;

				v4f color(0.2f, 1.f, 0.2f, 1.f);
				/*if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
					color.set(1.0f, 0.f, 0.f, 1.f);*/

				*inds_ptr = index;
				++index;
				++inds_ptr;

				vertex_ptr->Color = color;
				vertex_ptr->Position.x = cv->m_data.m_uv.x;
				vertex_ptr->Position.y = 0.02f;
				vertex_ptr->Position.z = cv->m_data.m_uv.y;
				vertex_ptr++;

				*inds_ptr = index;
				++index;
				++inds_ptr;

				vertex_ptr->Color = color;
				vertex_ptr->Position.x = nv->m_data.m_uv.x;
				vertex_ptr->Position.y = 0.02f;
				vertex_ptr->Position.z = nv->m_data.m_uv.y;
				vertex_ptr++;

				m_node_UV_CPU->m_modelCPU->m_vCount += 2;
				m_node_UV_CPU->m_modelCPU->m_iCount += 2;

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}
}

void miVisualObjectImpl::_createSoftwareModel_verts_UV() {
	if (!m_mesh->m_first_polygon)
		return;
	if (!m_mesh->m_uvCount)
		m_mesh->UpdateCounts();
	u32 vCount = m_mesh->m_uvCount;
	//printf("Create UV model\n");

	m_node_UV_CPU = new miVisualObjectImpl::model_node_UV_CPU;
	m_node_UV_CPU->m_modelCPU = yyMegaAllocator::CreateModel();
	m_node_UV_CPU->m_modelCPU->m_indexType = yyMeshIndexType::u32;

	m_node_UV_CPU->m_modelCPU->m_vertexType = yyVertexType::Point;
	m_node_UV_CPU->m_modelCPU->m_stride = sizeof(yyVertexPoint);
	m_node_UV_CPU->m_modelCPU->m_vertices = (u8*)yyMemAlloc(m_node_UV_CPU->m_modelCPU->m_stride * vCount);
	yyVertexPoint* vertex_ptr = (yyVertexPoint*)m_node_UV_CPU->m_modelCPU->m_vertices;
	m_node_UV_CPU->m_ptrs.reserve(vCount);

	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{

				v4f color(1.f, 1.f, 0.f, 1.f);
				f32 y = 0.03f;
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
				{
					color.set(1.0f, 0.f, 0.f, 1.f);
					y = 0.04f;
				}

				m_node_UV_CPU->m_ptrs.push_back(cv);
				
				vertex_ptr->Color = color;
				vertex_ptr->Position.x = cv->m_data.m_uv.x;
				vertex_ptr->Position.y = y;
				vertex_ptr->Position.z = cv->m_data.m_uv.y;
				vertex_ptr++;

				m_node_UV_CPU->m_modelCPU->m_vCount++;
				m_node_UV_CPU->m_modelCPU->m_iCount++;

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}
}

void miVisualObjectImpl::_createSoftwareModel_verts() {
	if (!m_mesh->m_first_vertex)
		return;

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
			_modelNode->m_ptrs.reserve(0xffff);
			_modelNode->m_modelCPU = softwareModel;

			m_nodes_CPU.push_back(_modelNode);
		}

		auto vpos = current_vertex->m_position;
		float size = 1.f;

		if (m_mesh->m_skeleton)
		{
			_modelNode->m_ptrs.push_back(miPair<void*, void*>(current_vertex, vertexAnimated_ptr));
			vertexAnimated_ptr->Color = color;
			vertexAnimated_ptr->Position = vpos;
			vertexAnimated_ptr++;
		}
		else
		{
			_modelNode->m_ptrs.push_back(miPair<void*, void*>(current_vertex, vertex_ptr));
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
	if (!m_mesh->m_first_edge)
		return;

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

		//v4f color = *m_parentSceneObject->GetEdgeColor();
		v4f color(1.f);
		if (g_app->m_editMode == miEditMode::Edge && m_parentSceneObject->m_isSelected)
		{
			if (current_edge->m_flags & miEdge::flag_isSelected)
				color.set(1.0f, 0.f, 0.f, 1.f);
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
			_modelNode->m_ptrs.reserve(0xffff);
			_modelNode->m_modelCPU = softwareModel;

			m_nodes_CPU.push_back(_modelNode);

			index = 0;
		}

		if (m_mesh->m_skeleton)
		{
			_modelNode->m_ptrs.push_back(miPair<void*, void*>(current_edge->m_vertex1, vertexAnimated_ptr));
			vertexAnimated_ptr->Color = color;
			vertexAnimated_ptr->Position = current_edge->m_vertex1->m_position;
			vertexAnimated_ptr->Normal = v3f();
			++vertexAnimated_ptr;
		}
		else
		{
			_modelNode->m_ptrs.push_back(miPair<void*, void*>(current_edge->m_vertex1, vertex_ptr));
			vertex_ptr->Color = color;
			vertex_ptr->Position = current_edge->m_vertex1->m_position;
			vertex_ptr->Normal = v3f();
			++vertex_ptr;
		}
		*inds_ptr = index;
		++index;
		++inds_ptr;

		if (m_mesh->m_skeleton)
		{
			_modelNode->m_ptrs.push_back(miPair<void*, void*>(current_edge->m_vertex2, vertexAnimated_ptr));
			vertexAnimated_ptr->Color = color;
			vertexAnimated_ptr->Position = current_edge->m_vertex2->m_position;
			vertexAnimated_ptr->Normal = v3f();
			++vertexAnimated_ptr;
		}
		else
		{
			_modelNode->m_ptrs.push_back(miPair<void*, void*>(current_edge->m_vertex2, vertex_ptr));
			vertex_ptr->Color = color;
			vertex_ptr->Position = current_edge->m_vertex2->m_position;
			vertex_ptr->Normal = v3f();
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
	if (!m_mesh->m_first_polygon)
		return;

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

		v4f color(0.f, 0.f, 0.f, 0.0f);
		if (g_app->m_editMode == miEditMode::Polygon && m_parentSceneObject->m_isSelected)
		{
			if (current_polygon->m_flags & miPolygon::flag_isSelected)
				color.set(1.0f, 0.f, 0.f, 0.48f);
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
				_modelNode->m_ptrs.reserve(0xffff);
				_modelNode->m_modelCPU = softwareModel;

				m_nodes_CPU.push_back(_modelNode);

				index = 0;
			}

			if (m_mesh->m_skeleton)
			{
				_modelNode->m_ptrs.push_back(miPair<void*, void*>(vertex_1->m_data.m_vertex, vertexAnimatedModel_ptr));
				vertexAnimatedModel_ptr->Color = color;
				vertexAnimatedModel_ptr->Position = vertex_1->m_data.m_vertex->m_position;
				vertexAnimatedModel_ptr->TCoords = vertex_1->m_data.m_uv;
				vertexAnimatedModel_ptr->Normal = vertex_1->m_data.m_normal;
				++vertexAnimatedModel_ptr;
			}
			else
			{
				_modelNode->m_ptrs.push_back(miPair<void*, void*>(vertex_1->m_data.m_vertex, vertexModel_ptr));
				vertexModel_ptr->Color = color;
				vertexModel_ptr->Position = vertex_1->m_data.m_vertex->m_position;
				vertexModel_ptr->TCoords = vertex_1->m_data.m_uv;
				vertexModel_ptr->Normal = vertex_1->m_data.m_normal;
				++vertexModel_ptr;
			}

			//printf("Y: %f\n", vertex_1->m_data->m_position.y);

			*inds_ptr = index;
			++index;
			++inds_ptr;

			if (m_mesh->m_skeleton)
			{
				_modelNode->m_ptrs.push_back(miPair<void*, void*>(vertex_2->m_data.m_vertex, vertexAnimatedModel_ptr));
				vertexAnimatedModel_ptr->Color = color;
				vertexAnimatedModel_ptr->Position = vertex_2->m_data.m_vertex->m_position;
				vertexAnimatedModel_ptr->TCoords = vertex_2->m_data.m_uv;
				vertexAnimatedModel_ptr->Normal = vertex_2->m_data.m_normal;
				++vertexAnimatedModel_ptr;
			}
			else
			{
				_modelNode->m_ptrs.push_back(miPair<void*, void*>(vertex_2->m_data.m_vertex, vertexModel_ptr));
				vertexModel_ptr->Color = color;
				vertexModel_ptr->Position = vertex_2->m_data.m_vertex->m_position;
				vertexModel_ptr->TCoords = vertex_2->m_data.m_uv;
				vertexModel_ptr->Normal = vertex_2->m_data.m_normal;
				++vertexModel_ptr;
			}

			*inds_ptr = index;
			++index;
			++inds_ptr;

			if (m_mesh->m_skeleton)
			{
				_modelNode->m_ptrs.push_back(miPair<void*, void*>(vertex_3->m_data.m_vertex, vertexAnimatedModel_ptr));
				vertexAnimatedModel_ptr->Color = color;
				vertexAnimatedModel_ptr->Position = vertex_3->m_data.m_vertex->m_position;
				vertexAnimatedModel_ptr->TCoords = vertex_3->m_data.m_uv;
				vertexAnimatedModel_ptr->Normal = vertex_3->m_data.m_normal;
				++vertexAnimatedModel_ptr;
			}
			else
			{
				_modelNode->m_ptrs.push_back(miPair<void*, void*>(vertex_3->m_data.m_vertex, vertexModel_ptr));
				vertexModel_ptr->Color = color;
				vertexModel_ptr->Position = vertex_3->m_data.m_vertex->m_position;
				vertexModel_ptr->TCoords = vertex_3->m_data.m_uv;
				vertexModel_ptr->Normal = vertex_3->m_data.m_normal;
				++vertexModel_ptr;
			}

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

			if (vertex_2 == vertex_1)
				break;
		}


		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}
}

void miVisualObjectImpl::CreateNewGPUModelsUV(miMesh* m) {
	_destroy_UV();
	switch (m_type)
	{
	case miVisualObjectType::Vertex:
		_createSoftwareModel_verts_UV();
		break;
	case miVisualObjectType::Edge:
		_createSoftwareModel_edges_UV();
		break;
	case miVisualObjectType::Polygon:
		_createSoftwareModel_polys_UV();
		break;
	default:
		break;
	}

	if (m_node_UV_CPU)
	{
		m_node_UV_GPU = new miVisualObjectImpl::model_node_GPU;
		m_node_UV_GPU->m_modelGPU = yyCreateModel(m_node_UV_CPU->m_modelCPU);
		m_node_UV_GPU->m_modelGPU->Load();
	}
}

void miVisualObjectImpl::CreateNewGPUModels(miMesh* mesh) {
	if(mesh)
		m_mesh = mesh;
	_destroy();
	switch (m_type)
	{
	case miVisualObjectType::Vertex:
		_createSoftwareModel_verts();
		break;
	case miVisualObjectType::Edge:
		_createSoftwareModel_edges();
		break;
	case miVisualObjectType::Polygon:
		_createSoftwareModel_polys();
		break;
	default:
		break;
	}

	for (u32 i = 0, sz = m_nodes_CPU.size(); i < sz; ++i)
	{
		auto node = m_nodes_CPU[i];
		auto _modelNode = new miVisualObjectImpl::model_node_GPU;
		_modelNode->m_modelGPU = yyCreateModel(node->m_modelCPU);
		_modelNode->m_modelGPU->Load();
		m_nodes_GPU.push_back(_modelNode);
	}
	UpdateAabb();

	CreateNewGPUModelsUV(mesh);
}

Aabb miVisualObjectImpl::GetAabb() {
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
//void miVisualObjectImpl::RemapBuffers() {
//	for (u32 i = 0, sz = m_nodes_polygons_GPU.size(); i < sz; ++i)
//	{
//		auto gpu_node = m_nodes_polygons_GPU[i];
//		//if (!gpu_node->m_remap)
//		//	continue;
//		auto cpu_node = m_nodes_polygons_CPU[i];
//		u8* verts = 0;
//		gpu_node->m_modelGPU->MapModelForWriteVerts(&verts);
//		memcpy(verts, cpu_node->m_modelCPU->m_vertices, cpu_node->m_modelCPU->m_vCount * cpu_node->m_modelCPU->m_stride);
//		gpu_node->m_modelGPU->UnmapModelForWriteVerts();
//	}
//	for (u32 i = 0, sz = m_nodes_edges_GPU.size(); i < sz; ++i)
//	{
//		auto gpu_node = m_nodes_edges_GPU[i];
//		//if (!gpu_node->m_remap)
//		//	continue;
//		auto cpu_node = m_nodes_edges_CPU[i];
//		u8* verts = 0;
//		gpu_node->m_modelGPU->MapModelForWriteVerts(&verts);
//		memcpy(verts, cpu_node->m_modelCPU->m_vertices, cpu_node->m_modelCPU->m_vCount * cpu_node->m_modelCPU->m_stride);
//		gpu_node->m_modelGPU->UnmapModelForWriteVerts();
//	}
//	for (u32 i = 0, sz = m_nodes_verts_GPU.size(); i < sz; ++i)
//	{
//		auto gpu_node = m_nodes_verts_GPU[i];
//		//if (!gpu_node->m_remap)
//		//	continue;
//		auto cpu_node = m_nodes_verts_CPU[i];
//		u8* verts = 0;
//		gpu_node->m_modelGPU->MapModelForWriteVerts(&verts);
//		memcpy(verts, cpu_node->m_modelCPU->m_vertices, cpu_node->m_modelCPU->m_vCount * cpu_node->m_modelCPU->m_stride);
//		gpu_node->m_modelGPU->UnmapModelForWriteVerts();
//	}
//}

void miVisualObjectImpl::Draw(bool uv) {
	auto camera = g_app->m_currentViewportDraw->m_activeCamera;
	
	static yyMaterial default_polygon_material;
	default_polygon_material.m_baseColor.set(0.5f, 0.5f, 0.5f, 1.f);
	default_polygon_material.m_type = yyMaterialType::Standart;
	default_polygon_material.m_sunPos = camera->m_positionCamera + v4f(0.f, 0.f, 0.f, 0.f);
	//default_polygon_material.m_sunPos = v4f(0.f, 1000.f, 0.f, 0.f);

	yyMaterial* curPolygonMaterial = &default_polygon_material;

	static yyResource * defTexture0 = yyGetDefaultTexture();
	yyResource * texture_0 = defTexture0;

	static yyMaterial wireframe_model_material;
	static Mat4 Vi;

	switch (m_type)
	{
	case miVisualObjectType::Vertex:
		Vi = camera->m_viewMatrix;
		Vi.m_data[3].set(0.f, 0.f, 0.f, 1.f);
		Vi.invert();
		yySetMatrix(yyMatrixType::ViewInvert, &Vi);

		if (uv)
		{
			if (m_node_UV_GPU)
			{
				if (m_node_UV_GPU->m_modelGPU)
				{
					auto W = Mat4();
					auto WVP = camera->m_projectionMatrix* camera->m_viewMatrix * W;
					yySetMatrix(yyMatrixType::WorldViewProjection, &WVP);
					yySetMatrix(yyMatrixType::World, &W);

					g_app->m_gpu->SetModel(m_node_UV_GPU->m_modelGPU);
					g_app->m_gpu->SetTexture(0, defTexture0);
					g_app->m_gpu->Draw();
				}
			}
		}
		else
		{
			for (u32 i = 0, sz = m_nodes_GPU.size(); i < sz; ++i)
			{
				auto node = m_nodes_GPU[i];

				yySetMatrix(yyMatrixType::WorldViewProjection, &m_parentSceneObject->m_worldViewProjection);
				yySetMatrix(yyMatrixType::World, &m_parentSceneObject->m_worldMatrix);
				g_app->m_gpu->SetModel(node->m_modelGPU);
				g_app->m_gpu->SetTexture(0, defTexture0);
				g_app->m_gpu->Draw();
			}
		}
		break;
	case miVisualObjectType::Edge:
	{
		if (uv)
		{
			if (m_node_UV_GPU)
			{
				if (m_node_UV_GPU->m_modelGPU)
				{
					auto W = Mat4();
					auto WVP = camera->m_projectionMatrix* camera->m_viewMatrix * W;
					yySetMatrix(yyMatrixType::WorldViewProjection, &WVP);
					yySetMatrix(yyMatrixType::World, &W);

					g_app->m_gpu->SetModel(m_node_UV_GPU->m_modelGPU);
					g_app->m_gpu->SetTexture(0, defTexture0);
					g_app->m_gpu->Draw();
				}
			}
		}
		else
		{
			v4f* ec = this->m_parentSceneObject->GetEdgeColor();
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
			yySetMaterial(&wireframe_model_material);

			for (u32 i = 0, sz = m_nodes_GPU.size(); i < sz; ++i)
			{
				auto node = m_nodes_GPU[i];

				yySetMatrix(yyMatrixType::WorldViewProjection, &m_parentSceneObject->m_worldViewProjection);
				yySetMatrix(yyMatrixType::World, &m_parentSceneObject->m_worldMatrix);
				g_app->m_gpu->SetModel(node->m_modelGPU);
				g_app->m_gpu->SetTexture(0, defTexture0);
				g_app->m_gpu->Draw();
			}
		}
	}break;
	case miVisualObjectType::Polygon:
	{
		auto old_cullBF = default_polygon_material.m_cullBackFace;
		default_polygon_material.m_cullBackFace = false; 
		if (uv)
		{
			default_polygon_material.m_type = yyMaterialType::Simple;
			if (m_node_UV_GPU)
			{
				if (m_node_UV_GPU->m_modelGPU)
				{
					auto W = Mat4();
					auto WVP = camera->m_projectionMatrix* camera->m_viewMatrix * W;
					yySetMatrix(yyMatrixType::WorldViewProjection, &WVP);
					yySetMatrix(yyMatrixType::World, &W);

					default_polygon_material.m_baseColor.m_data[0] = 1.f;
					default_polygon_material.m_baseColor.m_data[1] = 1.f;
					default_polygon_material.m_baseColor.m_data[2] = 0.f;
					default_polygon_material.m_baseColor.m_data[3] = 0.3f;
					yySetMaterial(&default_polygon_material);

					g_app->m_gpu->SetModel(m_node_UV_GPU->m_modelGPU);
					g_app->m_gpu->SetTexture(0, yyGetDefaultTexture());
					g_app->m_gpu->Draw();
				}
			}
			default_polygon_material.m_type = yyMaterialType::Standart;
		}
		else
		{

			if (m_parentSceneObject->m_material && m_parentSceneObject->m_useMaterial)
			{
				if (m_parentSceneObject->m_material->m_second == 1)
				{
					curPolygonMaterial->m_baseColor.m_data[0] = m_parentSceneObject->m_material->m_first->m_baseColor.x;
					curPolygonMaterial->m_baseColor.m_data[1] = m_parentSceneObject->m_material->m_first->m_baseColor.y;
					curPolygonMaterial->m_baseColor.m_data[2] = m_parentSceneObject->m_material->m_first->m_baseColor.z;
					curPolygonMaterial->m_baseColor.m_data[3] = m_parentSceneObject->m_material->m_first->m_baseColor.w;

					if (m_parentSceneObject->m_material->m_first->m_maps[0].m_GPUTexture)
						texture_0 = (yyResource*)m_parentSceneObject->m_material->m_first->m_maps[0].m_GPUTexture;
				}
			}
			g_app->m_gpu->SetTexture(0, texture_0);

			for (u32 i = 0, sz = m_nodes_GPU.size(); i < sz; ++i)
			{
				auto node = m_nodes_GPU[i];

				yySetMatrix(yyMatrixType::WorldViewProjection, &m_parentSceneObject->m_worldViewProjection);
				yySetMatrix(yyMatrixType::World, &m_parentSceneObject->m_worldMatrix);
				g_app->m_gpu->SetModel(node->m_modelGPU);

				if (g_app->m_currentViewportDraw->m_drawMode == miViewportDrawMode::Wireframe)
				{
					g_app->m_gpu->SetTexture(0, g_app->m_transparentTexture);
				}

				

				if (g_app->m_editMode == miEditMode::Polygon
					&& g_app->m_currentViewportDraw->m_drawMode == miViewportDrawMode::Wireframe)
				{
					//default_polygon_material.m_cullBackFace = false;
				}

				yySetMaterial(&default_polygon_material);

				g_app->m_gpu->Draw();

			}
		}
		default_polygon_material.m_cullBackFace = old_cullBF;
	}break;
	default:
		break;
	}
}

bool miVisualObjectImpl::IsInSelectionFrust(miSelectionFrust* sf) {
	assert(sf);
	if (!m_mesh->m_first_edge)
		return false;
	
	Mat4 M = this->m_parentSceneObject->GetWorldMatrix()->getBasis();

	auto position = this->m_parentSceneObject->GetGlobalPosition();

	bool result = false;

	auto current_edge = m_mesh->m_first_edge;
	auto last_edge = current_edge->m_left;
	
	while (true) {

		if (sf->LineInFrust(
				math::mul(current_edge->m_vertex1->m_position, M) + *position,
				math::mul(current_edge->m_vertex2->m_position, M) + *position)
			)
		{
			result = true;
			break;
		}

		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}

	if (!result)
	{
		v4f ip;
		f32 d;
		if (IsRayIntersect(&sf->m_data.m_ray1, &ip, &d)) return true;
		if (IsRayIntersect(&sf->m_data.m_ray2, &ip, &d)) return true;
		if (IsRayIntersect(&sf->m_data.m_ray3, &ip, &d)) return true;
		if (IsRayIntersect(&sf->m_data.m_ray4, &ip, &d)) return true;
		if (IsRayIntersect(&sf->m_data.m_ray5, &ip, &d)) return true;
	}

	return result;
}

bool miVisualObjectImpl::IsRayIntersect(yyRay* r, v4f* ip, float* d) {
	if (!m_mesh)
		return false;
	assert(r);
	assert(ip);
	assert(d);
	if (!m_mesh->m_first_polygon)
		return false;

	Mat4 M = this->m_parentSceneObject->GetWorldMatrix()->getBasis();

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
			/*auto p1 = math::mul(vertex_1->m_data1->m_position, M);
			auto p2 = math::mul(vertex_2->m_data1->m_position, M);
			auto p3 = math::mul(vertex_3->m_data1->m_position, M);*/
			auto p1 = math::mul(vertex_1->m_data.m_vertex->m_position, M);
			auto p2 = math::mul(vertex_2->m_data.m_vertex->m_position, M);
			auto p3 = math::mul(vertex_3->m_data.m_vertex->m_position, M);

			p1.add(*position);
			p2.add(*position);
			p3.add(*position);

			miTriangle tri(p1,p2,p3);
			f32 U = 0.f;
			f32 V = 0.f;
			f32 W = 0.f;
			if (tri.rayTest_MT(*r, true, *d, U, V, W))
			{
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

miVisualObjectType miVisualObjectImpl::GetType() {
	return m_type;
}

void miVisualObjectImpl::OnTransform() {
	//CreateNewGPUModels(0);
	switch (m_type)
	{
	case miVisualObjectType::Vertex:
		for (u32 i = 0; i < m_nodesForUpdate.m_size; ++i)
		{
			auto cpu_node = m_nodes_CPU[m_nodesForUpdate.m_data[i]];
			auto gpu_node = m_nodes_GPU[m_nodesForUpdate.m_data[i]];

			for (u32 o = 0; o < cpu_node->m_ptrs.m_size; ++o)
			{
				auto & pair = cpu_node->m_ptrs.m_data[o];
				auto mi_vertex = (miVertex*)pair.m_first;

				if (m_mesh->m_skeleton)
				{
					auto point_vertex = (yyVertexAnimatedPoint*)pair.m_second;
					point_vertex->Position = mi_vertex->m_position;
				}
				else
				{
					auto point_vertex = (yyVertexPoint*)pair.m_second;
					point_vertex->Position = mi_vertex->m_position;
				}
			}

			gpu_node->m_modelGPU->Unload();
			gpu_node->m_modelGPU->Load();
		}
		break;
	case miVisualObjectType::Edge:
		for (u32 i = 0; i < m_nodesForUpdate.m_size; ++i)
		{
			auto cpu_node = m_nodes_CPU[m_nodesForUpdate.m_data[i]];
			auto gpu_node = m_nodes_GPU[m_nodesForUpdate.m_data[i]];

			for (u32 o = 0; o < cpu_node->m_ptrs.m_size; ++o)
			{
				auto & pair = cpu_node->m_ptrs.m_data[o];
				auto mi_vertex = (miVertex*)pair.m_first;

				if (m_mesh->m_skeleton)
				{
					auto edge_vertex = (yyVertexAnimatedLine*)pair.m_second;
					edge_vertex->Position = mi_vertex->m_position;
				}
				else
				{
					auto edge_vertex = (yyVertexLine*)pair.m_second;
					edge_vertex->Position = mi_vertex->m_position;
				}
			}

			gpu_node->m_modelGPU->Unload();
			gpu_node->m_modelGPU->Load();
		}
		break;
	case miVisualObjectType::Polygon:
		for (u32 i = 0; i < m_nodesForUpdate.m_size; ++i)
		{
			auto cpu_node = m_nodes_CPU[m_nodesForUpdate.m_data[i]];
			auto gpu_node = m_nodes_GPU[m_nodesForUpdate.m_data[i]];

			for (u32 o = 0; o < cpu_node->m_ptrs.m_size; ++o)
			{
				auto & pair = cpu_node->m_ptrs.m_data[o];
				auto mi_vertex = (miVertex*)pair.m_first;

				if (m_mesh->m_skeleton)
				{
					auto polygon_vertex = (yyVertexAnimatedTriangle*)pair.m_second;
					polygon_vertex->Position = mi_vertex->m_position;
				}
				else
				{
					auto polygon_vertex = (yyVertexTriangle*)pair.m_second;
					polygon_vertex->Position = mi_vertex->m_position;
				}
			}

			gpu_node->m_modelGPU->Unload();
			gpu_node->m_modelGPU->Load();
		}
		break;
	default:
		break;
	}
}

void miVisualObjectImpl::OnSelect(miEditMode em) {
	m_nodesForUpdate.clear();

	switch (em)
	{
	case miEditMode::Vertex:
	{
		for (u32 i = 0, sz = m_nodes_CPU.size(); i < sz; ++i)
		{
			auto n = m_nodes_CPU[i];
			for (u32 o = 0; o < n->m_ptrs.m_size; ++o)
			{
				auto vertex = (miVertex*)n->m_ptrs.m_data[o].m_first;
				if (vertex->m_flags & vertex->flag_isSelected)
				{
					m_nodesForUpdate.push_back(i);
					break;
				}
			}
		}
	}break;
	case miEditMode::Edge:
		for (u32 i = 0, sz = m_nodes_CPU.size(); i < sz; ++i)
		{
			auto n = m_nodes_CPU[i];
			for (u32 o = 0; o < n->m_ptrs.m_size; ++o)
			{
				auto vertex = (miVertex*)n->m_ptrs.m_data[o].m_first;
					
				auto c = vertex->m_edges.m_head;
				auto l = c->m_left;
				while (true)
				{
					if (c->m_data->m_flags & miEdge::flag_isSelected)
					{
						m_nodesForUpdate.push_back(i);
						o = n->m_ptrs.m_size;
						break;
					}
					if (c == l)
						break;
					c = c->m_right;
				}
			}
		}
		break;
	case miEditMode::Polygon:
		for (u32 i = 0, sz = m_nodes_CPU.size(); i < sz; ++i)
		{
			auto n = m_nodes_CPU[i];
			for (u32 o = 0; o < n->m_ptrs.m_size; ++o)
			{
				auto vertex = (miVertex*)n->m_ptrs.m_data[o].m_first;

				auto c = vertex->m_polygons.m_head;
				auto l = c->m_left;
				while (true)
				{
					if (c->m_data->m_flags & miPolygon::flag_isSelected)
					{
						m_nodesForUpdate.push_back(i);
						o = n->m_ptrs.m_size;
						break;
					}
					if (c == l)
						break;
					c = c->m_right;
				}
			}
		}
		break;
	case miEditMode::Object:
	default:
		break;
	}
}

void miVisualObjectImpl::UpdateAabb() {
	m_aabb.reset();
	if (!m_mesh->m_first_vertex)
		return;

	auto current_vertex = m_mesh->m_first_vertex;
	auto last_vertex = current_vertex->m_left;
	while (true) {
		m_aabb.add(current_vertex->m_position);
		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}
}


