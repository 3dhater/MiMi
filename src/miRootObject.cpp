#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miRootObject.h"

miRootObject::miRootObject(miSDK*, miPlugin*) {}
miRootObject::~miRootObject() {}

void miRootObject::OnDraw() {}
void miRootObject::OnUpdate(float dt) {}
void miRootObject::OnCreation(miPluginGUI*) {};
void miRootObject::OnCreationLMBDown() {}
void miRootObject::OnCreationLMBUp() {}
void miRootObject::OnCreationMouseMove() {}
void miRootObject::OnCreationEnd() {}


miPlugin* miRootObject::GetPlugin() { return 0; }

int miRootObject::GetVisualObjectCount() { return 0; }
miVisualObject* miRootObject::GetVisualObject(int){ return 0; }
