#include "miSDK.h"

#include "miplStd.h"
#include "miplPolygonObjectPlane.h"

miplPolygonObjectPlane::miplPolygonObjectPlane(miSDK* sdk, miPlugin* p) {
	m_sdk = sdk;
	m_plugin = p;
	m_pluginImpl = (miplStd*)p;
	
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

}
void miplPolygonObjectPlane::OnCreationLMBUp() {
	bool isOk = true;

	if (m_size.x < 0.1f) isOk = false;
	if (m_size.y < 0.1f) isOk = false;

	m_pluginImpl->_dropNewObject();

	if (isOk)
	{
		//m_sdk->AddObjectToScene(this, L"Plane");
	}
	else
	{
		m_sdk->RemoveObjectFromScene(this);
	}
}
void miplPolygonObjectPlane::OnCreationMouseMove() {
	//printf("m");
}

void miplPolygonObjectPlane::OnCreation() {
	m_polygonCreator->Clear();
	m_polygonCreator->Add(miVec3(0.f, 0.f, 0.f), miVec3(0.f, 1.f, 0.f));
	m_polygonCreator->Add(miVec3(1.f, 0.f, 0.f), miVec3(0.f, 1.f, 0.f));
	m_polygonCreator->Add(miVec3(1.f, 0.f, 1.f),miVec3(0.f, 1.f, 0.f));

	m_meshBuilder = new miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>>(1,3,3);
	m_meshBuilder->AddPolygon(m_polygonCreator, true);
	m_meshBuilder->CreateEdges();
	
	m_visualObject->CreateNewGPUModels(&m_meshBuilder->m_mesh);
	
	m_sdk->AddObjectToScene(this, L"Plane");
}