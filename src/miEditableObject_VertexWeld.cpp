#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void editableObjectGUI_weldButton_onCancel();

float* editableObjectGUI_weldRange_onSelectObject(miSceneObject* obj) {
	if (obj->GetPlugin() != g_app->m_pluginForApp)
		return 0;

	if (obj->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return 0;

	return &((miEditableObject*)obj)->m_weldValue;
}

void editableObjectGUI_weldRange_onValueChanged(miSceneObject* obj, float* fptr) {
	if (obj->GetPlugin() != g_app->m_pluginForApp)
		return;

	if (obj->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return;
	auto object = (miEditableObject*)obj;
	if (*fptr < 0.f)
		*fptr = 0.f;
	object->OnWeld();
}

void editableObjectGUI_weldButtonOK_onClick(s32 id) {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->OnWeldApply();
	editableObjectGUI_weldButton_onCancel();
}
void editableObjectGUI_weldButton_onClick(s32 id, bool isChecked) {
	g_app->m_sdk->SetTransformMode(miTransformMode::NoTransform);
}

void editableObjectGUI_weldButton_onCancel() {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	auto gui = object->GetGui();
	gui->UncheckButtonGroup(1);
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
	g_app->m_sdk->SetSelectObjectCallbacks(0);
	object->DestroyTMPModelWithPoolAllocator();
	object->m_isWeld = false;
}

void editableObjectGUI_weldButton_onCheck(s32 id);
void editableObjectGUI_weldButton_onSelect(miEditMode em)
{
	switch (em)
	{
	case miEditMode::Vertex: {
		editableObjectGUI_weldButton_onCheck(-1);
	}break;
	case miEditMode::Edge:
	case miEditMode::Polygon:
	case miEditMode::Object:
	default:
		break;
	}
}
void editableObjectGUI_weldButton_onCheck(s32 id) {
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::Other);
	g_app->m_sdk->SetPickVertexCallbacks(
		0,0,0,
		editableObjectGUI_weldButton_onCancel);
	g_app->m_sdk->SetSelectObjectCallbacks(editableObjectGUI_weldButton_onSelect);

	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->UpdateCounts();
	object->OnWeld();
}

void editableObjectGUI_weldButton_onUncheck(s32 id) {
	if (g_app->m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::Other)
	{
		g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
		g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
		g_app->m_sdk->SetSelectObjectCallbacks(0);
	}
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->DestroyTMPModelWithPoolAllocator();
}

void miEditableObject::OnWeld() {
	m_isWeld = true;
	DestroyTMPModelWithPoolAllocator();
	CreateTMPModelWithPoolAllocator(GetPolygonCount(), GetEdgeCount(), GetVertexCount());

	auto mesh = m_meshBuilderTmpModelPool->m_mesh;
	{
		auto c = mesh->m_first_vertex;
		auto c_base = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if (c_base->m_flags & c_base->flag_User1) c_base->m_flags ^= c_base->flag_User1;
			if (c_base->m_flags & c_base->flag_User2) c_base->m_flags ^= c_base->flag_User2;
			if (c->m_flags & c->flag_User1) c->m_flags ^= c->flag_User1;
			if (c->m_flags & c->flag_User2) c->m_flags ^= c->flag_User2;

			if (c == l)
				break;
			c = c->m_right;
			c_base = c_base->m_right;
		}
	}
	struct helpPair {
		helpPair() :
			m_vertex1(0),
			m_vertex2(0)
		{}
		helpPair(miVertex* v1, miVertex* v2):
			m_vertex1(v1),
			m_vertex2(v2),
			m_position1(v1->m_position),
			m_position2(v2->m_position)
		{}
		miVertex* m_vertex1;
		miVertex* m_vertex2;
		v3f m_position1;
		v3f m_position2;
	};
	struct helpVertex {
		helpVertex(){
			m_num = 0;
		}
		miList<helpPair> m_pairs;
		u32 m_num;
	};
	miList<helpVertex*> vertices;
	helpVertex* newHelpVertex = 0;
	{
		auto c = mesh->m_first_vertex;
		auto base_c = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if ((c->m_flags & miVertex::flag_User1) == 0 && c->m_flags & miVertex::flag_isSelected)
			{
				base_c->m_flags |= miVertex::flag_User1;
				c->m_flags |= miVertex::flag_User1;
				
				newHelpVertex = 0;

				auto c2 = c->m_right;
				auto base_c2 = base_c->m_right;
				while (true)
				{
					if ((c2->m_flags & miVertex::flag_User1) == 0 && c2->m_flags & miVertex::flag_isSelected)
					{
						f32 d = base_c->m_position.distance(base_c2->m_position);
						if (d <= m_weldValue)
						{
							base_c2->m_flags |= miVertex::flag_User1;
							c2->m_flags |= miVertex::flag_User1;

							if (!newHelpVertex) {
								newHelpVertex = new helpVertex;
								newHelpVertex->m_num = 1;
								vertices.push_back(newHelpVertex);
							}
							++newHelpVertex->m_num;

							newHelpVertex->m_pairs.push_back(helpPair(c,c2));
						}
					}
					if (c2 == l)
						break;
					c2 = c2->m_right;
					base_c2 = base_c2->m_right;
				}

			}
			if (c == l)
				break;
			c = c->m_right;
			base_c = base_c->m_right;
		}
	}

	{
		auto c = vertices.m_head;
		if (c)
		{
			auto l = c->m_left;
			while (true)
			{
				auto n = c->m_right;

				if (c->m_data->m_pairs.m_head)
				{
					auto c2 = c->m_data->m_pairs.m_head;

					v3f position = c2->m_data.m_position1;

					auto l2 = c2->m_left;
					while (true)
					{
						position += c2->m_data.m_position2;

						auto n2 = c2->m_right;

						if (c2 == l2)
							break;
						c2 = n2;
					}

					position *= 1.f / (f32)c->m_data->m_num;

					c2 = c->m_data->m_pairs.m_head;
					auto firstVertex = c2->m_data.m_vertex1;
					firstVertex->m_position = position;
					
					l2 = c2->m_left;
					while (true)
					{
						this->VertexMoveTo(c2->m_data.m_vertex2, firstVertex);

						if (c2 == l2)
							break;
						c2 = c2->m_right;
					}
				}

				delete c->m_data;
				if (c == l)
					break;
				c = n;
			}
		}

	}

	_updateModel();
}

void miEditableObject::OnWeldApply() {
	if (!m_isWeld)
		return;
	m_isWeld = false;
	this->DeleteInvisiblePolygons(true);
	
	_createMeshFromTMPMesh_meshBuilder();

	this->DestroyTMPModelWithPoolAllocator();
	_updateModel();
	DeselectAll(g_app->m_editMode);
}