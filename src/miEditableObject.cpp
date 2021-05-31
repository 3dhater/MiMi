#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;


miEditableObject::miEditableObject(miSDK* sdk, miPlugin*) {
	m_visualObject = sdk->CreateVisualObject(this);
	m_meshBuilder = 0;
}

miEditableObject::~miEditableObject() {
	if (m_visualObject) miDestroy(m_visualObject);
}

void miEditableObject::OnDraw() {
	if (m_visualObject) {
		m_visualObject->Draw();
	}
}

void miEditableObject::OnUpdate(float dt) {
}
void miEditableObject::OnCreation(miPluginGUI*) {
}
void miEditableObject::OnCreationLMBDown() {
}
void miEditableObject::OnCreationLMBUp() {
}
void miEditableObject::OnCreationMouseMove() {
}
void miEditableObject::OnCreationEnd() {
}


miPlugin* miEditableObject::GetPlugin() { 
	return 0; 
}

int miEditableObject::GetVisualObjectCount() { 
	return 1;
}

miVisualObject* miEditableObject::GetVisualObject(int){ 
	return m_visualObject;
}
