#include "miSDK.h"

#include "miplStd.h"
#include "miplPolygonObjectPlane.h"

miplPolygonObjectPlane::miplPolygonObjectPlane(miSDK* sdk, miPlugin* p) {
	m_sdk = sdk;
	m_plugin = p;
	m_pluginImpl = (miplStd*)p;
	m_isLMBDown = false;

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
void miplPolygonObjectPlane::OnCreationMouseMove() {
	if (!m_polygonCreator) return;
	if (!m_meshBuilder) return;
	if (!m_isLMBDown) return;

	m_creationAabb.reset();
	m_creationAabb.add(m_firstPoint);
	m_creationAabb.add(m_sdk->GetCursorPosition3D());
	
//	printf("%f %f - %f %f\n", m_creationAabb.m_min.x, m_creationAabb.m_max.x, m_creationAabb.m_min.z, m_creationAabb.m_max.z);

	m_polygonCreator->Clear();

	auto camera_type = m_sdk->GetActiveViewportCameraType();
	switch (camera_type)
	{
	case miViewportCameraType::Perspective:
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_max.z),miVec3(0.f, 1.f, 0.f));

		m_size.x = m_creationAabb.m_max.x - m_creationAabb.m_min.x;
		m_size.y = m_creationAabb.m_max.z - m_creationAabb.m_min.z;
		break;
	case miViewportCameraType::Left:
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
	case miViewportCameraType::Top:
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_size.x = m_creationAabb.m_max.x - m_creationAabb.m_min.x;
		m_size.y = m_creationAabb.m_max.z - m_creationAabb.m_min.z;
		break;
	case miViewportCameraType::Bottom:
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_min.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_max.x, m_creationAabb.m_min.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
		m_polygonCreator->Add(miVec3(m_creationAabb.m_min.x, m_creationAabb.m_max.y, m_creationAabb.m_max.z), miVec3(0.f, 1.f, 0.f));
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
	}

	
	/*m_polygonCreator->Add(miVec3(0.f, 0.f, 0.f), miVec3(0.f, 1.f, 0.f));
	m_polygonCreator->Add(miVec3(1.f, 0.f, 0.f), miVec3(0.f, 1.f, 0.f));
	m_polygonCreator->Add(miVec3(1.f, 0.f, 1.f), miVec3(0.f, 1.f, 0.f));
	m_polygonCreator->Add(miVec3(0.f, 0.f, 1.f), miVec3(0.f, 1.f, 0.f));*/

	m_meshBuilder->DeleteMesh();
	m_meshBuilder->Begin();
	m_meshBuilder->AddPolygon(m_polygonCreator, true);
	m_meshBuilder->End();
	
	if(m_meshBuilder->m_mesh.m_first_polygon)
		m_visualObject->CreateNewGPUModels(&m_meshBuilder->m_mesh);
}

void miplPolygonObjectPlane::OnCreation() {
	OnCreationMouseMove();

	m_meshBuilder = new miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>>(1,4,4);
	m_sdk->AddObjectToScene(this, L"Plane");
}