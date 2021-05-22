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

int miRootObject::GetVisualObjectCount() { return 0; }
miVisualObject* miRootObject::GetVisualObject(int){ return 0; }

//void miRootObject::SelectSingle(miSelectionFrust*) {
//}
//
//void miRootObject::Select(miSelectionFrust*){
//}
//
//void miRootObject::Deselect(miSelectionFrust*) {
//}
//
//void miRootObject::SelectAll() {
//}
//
//void miRootObject::DeselectAll() {
//}

//bool miRootObject::IsRayIntersect(miRay*, miVec4* ip, float* d) {
//	return false;
//}