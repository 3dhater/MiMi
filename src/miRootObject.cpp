#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miRootObject.h"

miRootObject::miRootObject(miSDK*, miPlugin*) {}
miRootObject::~miRootObject() {}

void miRootObject::OnDraw(miViewportDrawMode, miEditMode, float dt) {}
void miRootObject::OnUpdate(float dt) {}
void miRootObject::OnCreation(miPluginGUI*) {};
void miRootObject::OnCreationLMBDown() {}
void miRootObject::OnCreationLMBUp() {}
void miRootObject::OnCreationMouseMove() {}
void miRootObject::OnCreationEnd() {}
void miRootObject::OnConvertToEditableObject() {}


miPlugin* miRootObject::GetPlugin() { return 0; }

int miRootObject::GetVisualObjectCount() { return 0; }
miVisualObject* miRootObject::GetVisualObject(int){ return 0; }
void miRootObject::DeleteSelectedObjects(miEditMode em) {}