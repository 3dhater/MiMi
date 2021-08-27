#include "miApplication.h"
#include "miSDKImpl.h"
#include "miAppPlugin.h"
#include "miEditableObject.h"

extern miApplication * g_app;

u32 miApplicationPlugin::m_objectType_editableObject = 1;

miApplicationPlugin::miApplicationPlugin() {}
miApplicationPlugin::~miApplicationPlugin() {}

void miApplicationPlugin::Init(miSDK* sdk) {
}

void miApplicationPlugin::OnClickGizmo(miKeyboardModifier km, miGizmoMode gm, miEditMode em, miSceneObject* o) {
	if (o->GetPlugin() != this)
		return;

	if (km == miKeyboardModifier::ShiftCtrl)
	{
		switch (em)
		{
		case miEditMode::Vertex:
			break;
		case miEditMode::Edge: {
			if (o->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
				((miEditableObject*)o)->EdgeExtrude();
		}break;
		case miEditMode::Polygon: {
			if (o->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
				((miEditableObject*)o)->PolygonExtrude();
		}break;
		case miEditMode::Object: {
			/*if (o->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
			printf("Clone object\n");*/
		}break;
		default:
			break;
		}
	}
}

miSceneObject* miApplicationPlugin::OnDuplicate(miSceneObject* object, bool isMirror, miAxis axis) {
	if (object->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
	{
		return _duplicateEditableObject(object, isMirror, axis);
	}

	return 0;
}

miSceneObject* miApplicationPlugin::_duplicateEditableObject(miSceneObject* object, bool isMirror, miAxis axis) {
	miEditableObject* eo = (miEditableObject*)object;
	miEditableObject* new_eo = (miEditableObject*)miMalloc(sizeof(miEditableObject));
	new(new_eo)miEditableObject((miSDK*)g_app->m_sdk, 0);
	new_eo->CopyBase(eo);

	g_app->m_sdk->AppendMesh(new_eo->m_mesh, eo->m_mesh, isMirror, axis);
	new_eo->RebuildVisualObjects(false);

	miString name = eo->GetName().data();
	if(isMirror)
		name += "_mirror";
	else
		name += "_copy";
	
	//auto newName = g_app->GetFreeName(name.data());
	//new_eo->SetName(newName.data());

	g_app->AddObjectToScene(new_eo, name.data());
	new_eo->UpdateAabb();

	return new_eo;
}