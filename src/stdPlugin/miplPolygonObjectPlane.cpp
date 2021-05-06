#include "miSDK.h"

#include "miplStd.h"
#include "miplPolygonObjectPlane.h"

miplPolygonObjectPlane::miplPolygonObjectPlane(miSDK* sdk, miPlugin* p) {
	m_sdk = sdk;
	m_plugin = p;
	m_pluginImpl = (miplStd*)p;
	m_sdk->AddObjectToScene(this, "Plane");
}

miplPolygonObjectPlane::~miplPolygonObjectPlane() {

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
		m_pluginImpl->_saveNewObject();
	}
	else
	{
		m_sdk->RemoveObjectFromScene(m_pluginImpl->m_newObjectPtr);
	}
}
void miplPolygonObjectPlane::OnCreationMouseMove() {

}

