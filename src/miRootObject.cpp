#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miRootObject.h"

miRootObject::miRootObject(miSDK*, miPlugin*) {}
miRootObject::~miRootObject() {}

void miRootObject::OnDraw() {}
void miRootObject::OnUpdate(float dt) {}
void miRootObject::OnCreation() {};
void miRootObject::OnCreationLMBDown() {}
void miRootObject::OnCreationLMBUp() {}
void miRootObject::OnCreationMouseMove() {}
void miRootObject::OnCreationEnd() {}


miPlugin* miRootObject::GetPlugin() { return 0; }
