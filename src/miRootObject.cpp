#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miRootObject.h"

miRootObject::miRootObject(miSDK*, miPlugin*) {}
miRootObject::~miRootObject() {
}

void miRootObject::SetParent(miSceneObject* o){}


void miRootObject::OnDraw() {}
void miRootObject::OnCreationLMBDown() {}
void miRootObject::OnCreationLMBUp() {}
void miRootObject::OnCreationMouseMove() {}

miPlugin* miRootObject::GetPlugin() { return 0; }
