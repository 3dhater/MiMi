#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

float* editableObjectGUI_weldRange_onSelectObject(miSceneObject* obj) {
	if (obj->GetPlugin() != g_app->m_pluginForApp)
		return 0;

	if (obj->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return 0;

	return &((miEditableObject*)obj)->m_weldValue;
}

void editableObjectGUI_weldRange_onValueChanged(miSceneObject* obj, float) {
	if (obj->GetPlugin() != g_app->m_pluginForApp)
		return;

	if (obj->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return;
	auto object = (miEditableObject*)obj;
	object->OnWeld();
}

void editableObjectGUI_weldButton_onClick(s32 id, bool isChecked) {
	g_app->m_sdk->SetTransformMode(miTransformMode::NoTransform);
}

void editableObjectGUI_weldButton_onCancel() {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	auto gui = object->GetGui();
	gui->UncheckButtonGroup(1);
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	g_app->m_sdk->SetSelectVertexCallbacks(0, 0, 0, 0);
	object->DestroyTMPModelWithPoolAllocator();
}

void editableObjectGUI_weldButton_onCheck(s32 id) {
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::Other);
	g_app->m_sdk->SetSelectVertexCallbacks(
		0,0,0,
		editableObjectGUI_weldButton_onCancel);

	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->OnWeld();
}

void editableObjectGUI_weldButton_onUncheck(s32 id) {
	if (g_app->m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::Other)
	{
		g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
		g_app->m_sdk->SetSelectVertexCallbacks(0, 0, 0, 0);
	}
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->DestroyTMPModelWithPoolAllocator();
}

void miEditableObject::OnWeld() {
	DestroyTMPModelWithPoolAllocator();
	CreateTMPModelWithPoolAllocator();

	auto mesh = &m_meshBuilderTmpModelPool->m_mesh;
	{
		auto c = mesh->m_first_vertex;
		auto c_base = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if (c_base->m_flags & c_base->flag_User1)
				c_base->m_flags ^= c_base->flag_User1;

			if (c == l)
				break;
			c = c->m_right;
			c_base = c_base->m_right;
		}
	}
	struct helpPair {
		helpPair() :
			m_vertex1(0),
			m_vertex2(0),
			m_baseVertex1(0),
			m_baseVertex2(0)
		{}
		helpPair(miVertex* v1, miVertex* v2, miVertex* bv1, miVertex* bv2):
			m_vertex1(v1),
			m_vertex2(v2),
			m_baseVertex1(bv1),
			m_baseVertex2(bv2)
		{}
		miVertex* m_vertex1;
		miVertex* m_vertex2;
		miVertex* m_baseVertex1;
		miVertex* m_baseVertex2;
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
			if ((base_c->m_flags & base_c->flag_User1) == 0 && base_c->m_flags & base_c->flag_isSelected)
			{
				base_c->m_flags |= base_c->flag_User1;

				newHelpVertex = 0;

				auto c2 = c->m_right;
				auto base_c2 = base_c->m_right;
				while (true)
				{
					if ((base_c2->m_flags & base_c2->flag_User1) == 0 && base_c2->m_flags & base_c2->flag_isSelected)
					{
						f32 d = base_c->m_position.distance(base_c2->m_position);
						if (d <= m_weldValue)
						{
							base_c2->m_flags |= base_c2->flag_User1;

							if (!newHelpVertex) {
								newHelpVertex = new helpVertex;
								vertices.push_back(newHelpVertex);
							}
							++newHelpVertex->m_num;

							newHelpVertex->m_pairs.push_back(helpPair(c,c2, base_c, base_c2));
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
		miBinarySearchTree<miPolygon*> bst;

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
					auto l2 = c2->m_left;
					while (true)
					{
						auto n2 = c2->m_right;
						miPolygon* p1 = 0;
						miPolygon* p2 = 0;

						c2->m_data.m_vertex1->m_position =
							c2->m_data.m_baseVertex1->m_position +
							c2->m_data.m_baseVertex2->m_position;
						c2->m_data.m_vertex1->m_position *= 0.5f;

						this->VertexTargetWeld(c2->m_data.m_vertex2, c2->m_data.m_vertex1, &p1, &p2);
						if (p1) bst.Add((uint64_t)p1, p1);
						if (p2) bst.Add((uint64_t)p2, p2);

						if (c2 == l2)
							break;
						c2 = n2;
					}
				}

				delete c->m_data;
				if (c == l)
					break;
				c = n;
			}
		}

		static miArray<miPolygon*> polygonsForDelete;
		polygonsForDelete.clear();
		bst.Get(&polygonsForDelete);
		for (u32 i = 0; i < polygonsForDelete.m_size; ++i)
		{
			DeletePolygon(polygonsForDelete.m_data[i]);
		}
	}

	_updateModel();
}