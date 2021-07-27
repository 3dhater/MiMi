#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void editableObjectGUI_edgeChamferButton_onCheck(s32 id);
void editableObjectGUI_edgeChamferButton_onCancel();
void editableObjectGUI_edgeChamferButton_onUncheck(s32 id);

void editableObjectGUI_edgeChamferCheckBox_onClick(bool isChecked) {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->m_addPolygonsWhenEdgeChamfer = isChecked;
	if (object->m_isEdgeChamfer)
		object->OnEdgeChamfer();
}

void editableObjectGUI_edgeChamferButtonOK_onClick(s32 id) {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->OnEdgeChamferApply();
	editableObjectGUI_edgeChamferButton_onCancel();
}

float* editableObjectGUI_edgeChamferRange_onSelectObject(miSceneObject* obj) {
	if (obj->GetPlugin() != g_app->m_pluginForApp)
		return 0;

	if (obj->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return 0;

	return &((miEditableObject*)obj)->m_edgeChamferValue;
}

void editableObjectGUI_edgeChamferRange_onValueChanged(miSceneObject* obj, float* fptr) {
	if (obj->GetPlugin() != g_app->m_pluginForApp)
		return;

	if (obj->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return;
	auto object = (miEditableObject*)obj;
	if (object->m_isEdgeChamfer)
	{
		if (*fptr < 0.f)
			*fptr = 0.f;
		object->OnEdgeChamfer();
	}
}

void editableObjectGUI_edgeChamferButton_onSelect(miEditMode em)
{
	switch (em)
	{
	case miEditMode::Edge: {
		editableObjectGUI_edgeChamferButton_onCheck(-1);
	}break;
	case miEditMode::Vertex:
	case miEditMode::Polygon:
	case miEditMode::Object:
	default:
		break;
	}
}

void editableObjectGUI_edgeChamferButton_onCancel() {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	auto gui = object->GetGui();
	gui->UncheckButtonGroup(1);
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
	g_app->m_sdk->SetSelectObjectCallbacks(0);
	editableObjectGUI_edgeChamferButton_onUncheck(-1);
}

void editableObjectGUI_edgeChamferButton_onClick(s32 id, bool isChecked) {
	g_app->m_sdk->SetTransformMode(miTransformMode::NoTransform);
}

void editableObjectGUI_edgeChamferButton_onCheck(s32 id) {
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::Other);
	g_app->m_sdk->SetPickVertexCallbacks(
		0, 0, 0,
		editableObjectGUI_edgeChamferButton_onCancel);
	g_app->m_sdk->SetSelectObjectCallbacks(editableObjectGUI_edgeChamferButton_onSelect);

	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->OnEdgeChamfer();
}

void editableObjectGUI_edgeChamferButton_onUncheck(s32 id) {
	if (g_app->m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::Other)
	{
		g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
		g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
		g_app->m_sdk->SetSelectObjectCallbacks(0);
	}
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->m_isEdgeChamfer = false;
	object->DestroyTMPModelWithPoolAllocator();
	object->_updateModel(false);
}

void miEditableObject::OnEdgeChamfer() {
	m_isEdgeChamfer = true;

	this->UpdateCounts();

	DestroyTMPModelWithPoolAllocator();
	CreateTMPModelWithPoolAllocator(this->GetPolygonCount() * 5, this->GetEdgeCount() * 5, this->GetVertexCount() * 5);
	auto mesh = m_meshBuilderTmpModelPool->m_mesh;

	miBinarySearchTree<miVertex*> deleteVertices;

	struct helpStruct
	{
		helpStruct():m_newVertex(0){}
		helpStruct(miVertex* v):m_newVertex(v) {}
		miVertex* m_newVertex;
	};

	if (m_mesh->m_first_polygon)
	{
		auto c = mesh->m_first_polygon;
		auto c_old = m_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			miBinarySearchTree<helpStruct> hs;

			// find selected edge in this polygon
			auto ce = c->m_edges.m_head;
			auto ce_old = c_old->m_edges.m_head;
			auto le = ce->m_left;
			while (true)
			{
				if (ce->m_data->m_flags & miEdge::flag_isSelected)
				{
					v3f dir1, dir2;
					{
						auto vNode1 = c_old->m_verts.find(ce_old->m_data->m_vertex1);
						auto vNode2 = c_old->m_verts.find(ce_old->m_data->m_vertex2);
						if (vNode1->m_right == vNode2)
							dir1 = vNode1->m_left->m_data1->m_position - vNode1->m_data1->m_position;
						else
							dir1 = vNode1->m_right->m_data1->m_position - vNode1->m_data1->m_position;

						if (vNode2->m_right == vNode1)
							dir2 = vNode2->m_left->m_data1->m_position - vNode2->m_data1->m_position;
						else
							dir2 = vNode2->m_right->m_data1->m_position - vNode2->m_data1->m_position;
						dir1.normalize2();
						dir2.normalize2();
					}

					miVertex* v1 = 0;
					miVertex* v2 = 0;

					helpStruct h1;
					if (hs.Get((uint64_t)ce->m_data->m_vertex1, h1))
					{
						v1 = h1.m_newVertex;
					}
					else
					{
						v1 = m_meshBuilderTmpModelPool->m_allocatorVertex->Allocate();
						v1->m_position = ce->m_data->m_vertex1->m_position;
						h1.m_newVertex = v1;
						hs.Add((uint64_t)ce->m_data->m_vertex1, h1);

						v1->m_polygons.push_back(c);
						mesh->_add_vertex_to_list(v1);
						
						auto vNode1 = c->m_verts.find(ce->m_data->m_vertex1);
						auto vNode2 = c->m_verts.find(ce->m_data->m_vertex2);
						
						if(vNode1->m_right == vNode2)
							c->m_verts.insert_before(vNode1->m_data1, v1, vNode1->m_data2, vNode1->m_data3);
						else
							c->m_verts.insert_after(vNode1->m_data1, v1, vNode1->m_data2, vNode1->m_data3);
					
						deleteVertices.Add((uint64_t)ce->m_data->m_vertex1, ce->m_data->m_vertex1);
					}

					helpStruct h2;
					if (hs.Get((uint64_t)ce->m_data->m_vertex2, h2))
					{
						v2 = h2.m_newVertex;
					}
					else
					{
						v2 = m_meshBuilderTmpModelPool->m_allocatorVertex->Allocate();
						v2->m_position = ce->m_data->m_vertex2->m_position;
						h2.m_newVertex = v2;
						hs.Add((uint64_t)ce->m_data->m_vertex2, h2);

						v2->m_polygons.push_back(c);
						mesh->_add_vertex_to_list(v2);
						
						auto vNode1 = c->m_verts.find(ce->m_data->m_vertex1);
						auto vNode2 = c->m_verts.find(ce->m_data->m_vertex2);

						if (vNode2->m_right == vNode1)
							c->m_verts.insert_before(vNode2->m_data1, v2, vNode2->m_data2, vNode2->m_data3);
						else
							c->m_verts.insert_after(vNode2->m_data1, v2, vNode2->m_data2, vNode2->m_data3);
						
						deleteVertices.Add((uint64_t)ce->m_data->m_vertex2, ce->m_data->m_vertex2);
					}
					
					v1->m_position += m_edgeChamferValue * dir1;
					v2->m_position += m_edgeChamferValue * dir2;

				}
				if (ce == le)
					break;
				ce = ce->m_right;
				ce_old = ce_old->m_right;
			}

			if (c == l)
				break;
			c = c->m_right;
			c_old = c_old->m_right;
		}
	}

	{
		miArray<miVertex*> arr;
		arr.reserve(0x1000);
		deleteVertices.Get(&arr);
		for (u32 i = 0; i < arr.m_size; ++i)
		{
			auto v = arr.m_data[i];
			auto c = v->m_polygons.m_head;
			auto l = c->m_left;
			while (true)
			{
				c->m_data->m_verts.erase_first(v);
				if (c == l)break;
				c = c->m_right;
			}

			mesh->_remove_vertex_from_list(v);
			m_meshBuilderTmpModelPool->m_allocatorVertex->Deallocate(v);
		}
	}
	_updateModel();
}

void miEditableObject::OnEdgeChamferApply() {
	if (!m_isEdgeChamfer)
		return;
	m_isEdgeChamfer = false;

	_createMeshFromTMPMesh_meshBuilder(true, false);
	
	this->DestroyTMPModelWithPoolAllocator();
	_updateModel();
}

