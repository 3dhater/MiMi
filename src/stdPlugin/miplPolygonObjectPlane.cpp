#include "miSDK.h"

#include "miplStd.h"
#include "miplPolygonObjectPlane.h"

miplPolygonObjectPlane::miplPolygonObjectPlane(miSDK* sdk, miPlugin* p) {
	m_sdk = sdk;
	m_plugin = p;
	m_pluginImpl = (miplStd*)p;
}

miplPolygonObjectPlane::~miplPolygonObjectPlane() {
	//printf("destroy\n");
}

void miplPolygonObjectPlane::OnDraw() {

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

	if (isOk)
	{
		m_sdk->AddObjectToScene(this, L"Plane");
		m_pluginImpl->_saveNewObject();
	}
}
void miplPolygonObjectPlane::OnCreationMouseMove() {

}

