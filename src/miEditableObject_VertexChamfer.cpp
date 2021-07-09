#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void editableObjectGUI_chamferButton_onCheck(s32 id);

void editableObjectGUI_chamferButton_onSelect(miEditMode em)
{
	switch (em)
	{
	case miEditMode::Vertex: {
		editableObjectGUI_chamferButton_onCheck(-1);
	}break;
	case miEditMode::Edge:
	case miEditMode::Polygon:
	case miEditMode::Object:
	default:
		break;
	}
}

void editableObjectGUI_chamferButton_onCancel() {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	auto gui = object->GetGui();
	gui->UncheckButtonGroup(1);
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
	g_app->m_sdk->SetSelectObjectCallbacks(0);
	object->DestroyTMPModelWithPoolAllocator();
	object->m_isChamfer = false;
}

void editableObjectGUI_chamferButton_onClick(s32 id, bool isChecked) {
	g_app->m_sdk->SetTransformMode(miTransformMode::NoTransform);
}

void editableObjectGUI_chamferButton_onCheck(s32 id) {
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::Other);
	g_app->m_sdk->SetPickVertexCallbacks(
		0, 0, 0,
		editableObjectGUI_chamferButton_onCancel);
	g_app->m_sdk->SetSelectObjectCallbacks(editableObjectGUI_chamferButton_onSelect);

	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->OnChamfer();
}

void editableObjectGUI_chamferButton_onUncheck(s32 id) {
	if (g_app->m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::Other)
	{
		g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
		g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
		g_app->m_sdk->SetSelectObjectCallbacks(0);
	}
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->DestroyTMPModelWithPoolAllocator();
}

void miEditableObject::OnChamfer() {
	m_isChamfer = true;
	DestroyTMPModelWithPoolAllocator();
	CreateTMPModelWithPoolAllocator();
	printf("C");
}

void miEditableObject::OnChamferApply() {
	if (!m_isChamfer)
		return;
	m_isChamfer = false;

	_createMeshFromTMPMesh_meshBuilder();

	this->DestroyTMPModelWithPoolAllocator();
	_updateModel();
}

