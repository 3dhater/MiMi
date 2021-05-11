#include "miSDK.h"

#include "miplStd.h"
#include "miplPolygonObjectPlane.h"

void miplPolygonObjectPlane_generate(
	miPolygonCreator* pc, 
	miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>> * mb,
	miViewportCameraType ct,
	const miAabb& aabb, 
	const miVec3& firstPoint, 
	miVec2& size,
	unsigned int x_segments, 
	unsigned int y_segments)
{
	size.x = aabb.m_max.x - aabb.m_min.x;
	size.y = aabb.m_max.z - aabb.m_min.z;

	float half_width = size.x * 0.5f;
	float half_height = size.y * 0.5f;

	if (half_width == 0.f)
		half_width = 0.001f;

	if (half_height == 0.f)
		half_height = 0.001f;

	float center_x = half_width;
	float center_y = half_height;

	float segment_size_h = size.y / (float)y_segments;
	float segment_size_w = size.x / (float)x_segments;

	float uv_segment_size_h = 1.f / (float)y_segments;
	float uv_segment_size_w = 1.f / (float)x_segments;

	pc->Clear();

	float begin_x = aabb.m_min.x;
	float begin_z = aabb.m_min.z;

	float uv_begin_x = 0.f;
	float uv_begin_y = 1.f;
	for (int h_i = 0; h_i < y_segments; ++h_i)
	{
		for (int w_i = 0; w_i < x_segments; ++w_i)
		{
			switch (ct)
			{
			case miViewportCameraType::Perspective:
			case miViewportCameraType::Top:
				pc->Add(miVec3(begin_x - half_width + center_x, firstPoint.y, begin_z - half_height + center_y), 
					miVec3(0.f, 1.f, 0.f), 
					miVec2(uv_begin_x, uv_begin_y));

				pc->Add(miVec3(begin_x - half_width + center_x, firstPoint.y, begin_z + segment_size_h - half_height + center_y), 
					miVec3(0.f, 1.f, 0.f), 
					miVec2(uv_begin_x, uv_begin_y - uv_segment_size_h));

				pc->Add(miVec3(begin_x + segment_size_w - half_width + center_x, firstPoint.y, begin_z + segment_size_h - half_height + center_y), 
					miVec3(0.f, 1.f, 0.f), 
					miVec2(uv_begin_x + uv_segment_size_w, uv_begin_y - uv_segment_size_h));

				pc->Add(miVec3(begin_x + segment_size_w - half_width + center_x, firstPoint.y, begin_z - half_height + center_y), 
					miVec3(0.f, 1.f, 0.f), 
					miVec2(uv_begin_x + uv_segment_size_w, uv_begin_y));
				break;
			case miViewportCameraType::Left:
				break;
			case miViewportCameraType::Right:
				break;
			case miViewportCameraType::Bottom:
				break;
			case miViewportCameraType::Front:
				break;
			case miViewportCameraType::Back:
				break;
			}
			
			mb->AddPolygon(pc, true);
			pc->Clear();

			begin_x += segment_size_w;
			uv_begin_x += uv_segment_size_w;

			if (begin_x > size.x + aabb.m_max.x)
			{
				begin_x = aabb.m_min.x;
				uv_begin_x = 0;
			}
		}

		begin_z += segment_size_h;
		begin_x = aabb.m_min.x;

		uv_begin_y -= uv_segment_size_h;
		uv_begin_x = 0;
	}
	
	/*pc->Add(miVec3(min.x, y, max.z), miVec3(0.f, 1.f, 0.f));
	pc->Add(miVec3(max.x, y, max.z), miVec3(0.f, 1.f, 0.f));
	pc->Add(miVec3(max.x, y, min.z), miVec3(0.f, 1.f, 0.f));
	pc->Add(miVec3(min.x, y, min.z), miVec3(0.f, 1.f, 0.f));
	mb->AddPolygon(pc, true);
	pc->Clear();*/

}

miplPolygonObjectPlane::miplPolygonObjectPlane(miSDK* sdk, miPlugin* p) {
	m_sdk = sdk;
	m_plugin = p;
	m_pluginImpl = (miplStd*)p;
	m_isLMBDown = false;
	m_viewportCameraType = miViewportCameraType::Perspective;

	m_polygonCreator = new miPolygonCreator;
	m_visualObject = m_sdk->CreateVisualObject();
	m_meshBuilder = 0;
}

miplPolygonObjectPlane::~miplPolygonObjectPlane() {
	if (m_polygonCreator) delete m_polygonCreator;
	if (m_meshBuilder) delete m_meshBuilder;
	if (m_visualObject) miDestroy(m_visualObject);
	//printf("destroy\n");
}

void miplPolygonObjectPlane::OnUpdate(float dt) {

}

void miplPolygonObjectPlane::OnDraw() {
	if (m_visualObject) {
		m_visualObject->Draw(&m_worldMatrix);
	}
}

miPlugin* miplPolygonObjectPlane::GetPlugin() {
	return m_plugin;
}

void miplPolygonObjectPlane::OnCreationLMBDown() {
	m_firstPoint = m_sdk->GetCursorPosition3DFirstClick();
	m_isLMBDown = true;
}
void miplPolygonObjectPlane::OnCreationLMBUp() {
	bool isOk = true;

	if (m_size.x < 0.1f) isOk = false;
	if (m_size.y < 0.1f) isOk = false;

	if (isOk)
	{
		m_pluginImpl->_dropNewObject();
	}
	else
	{
		m_sdk->RemoveObjectFromScene(this);
	}
	m_isLMBDown = false;
}

void miplPolygonObjectPlane::_generate() {
	m_creationAabb.reset();
	m_creationAabb.add(m_firstPoint);
	m_creationAabb.add(m_secondPoint);

	m_meshBuilder->DeleteMesh();
	m_meshBuilder->Begin();

	miplPolygonObjectPlane_generate(
		m_polygonCreator,
		m_meshBuilder,
		m_viewportCameraType,
		m_creationAabb, m_firstPoint, 
		m_size,
		1,1);
	/*case miViewportCameraType::Left:
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_size.x = m_creationAabb.m_max.z - m_creationAabb.m_min.z;
		m_size.y = m_creationAabb.m_max.y - m_creationAabb.m_min.y;
		break;
	case miViewportCameraType::Right:
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_size.x = m_creationAabb.m_max.z - m_creationAabb.m_min.z;
		m_size.y = m_creationAabb.m_max.y - m_creationAabb.m_min.y;
		break;
	case miViewportCameraType::Bottom:
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, -1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_min.z), miVec3(0.f, -1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, -1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_max.z), miVec3(0.f, -1.f, 0.f));
		m_size.x = m_creationAabb.m_max.x - m_creationAabb.m_min.x;
		m_size.y = m_creationAabb.m_max.z - m_creationAabb.m_min.z;
		break;
	case miViewportCameraType::Front:
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_size.x = m_creationAabb.m_max.x - m_creationAabb.m_min.x;
		m_size.y = m_creationAabb.m_max.y - m_creationAabb.m_min.y;
		break;
	case miViewportCameraType::Back:
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_size.x = m_creationAabb.m_max.x - m_creationAabb.m_min.x;
		m_size.y = m_creationAabb.m_max.y - m_creationAabb.m_min.y;
		break;
	default:
		break;
	}*/
	m_meshBuilder->End();

	if (m_meshBuilder->m_mesh.m_first_polygon)
		m_visualObject->CreateNewGPUModels(&m_meshBuilder->m_mesh);
}

void miplPolygonObjectPlane::OnCreationMouseMove() {
	if (!m_polygonCreator) return;
	if (!m_meshBuilder) return;
	if (!m_isLMBDown) return;

	m_secondPoint = m_sdk->GetCursorPosition3D();
	m_viewportCameraType = m_sdk->GetActiveViewportCameraType();

	_generate();
}

void miplPolygonObjectPlane::OnCreation() {
	OnCreationMouseMove();

	m_meshBuilder = new miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>>
		(40000,80400,40401); // 200x200
	m_sdk->AddObjectToScene(this, L"Plane");
}