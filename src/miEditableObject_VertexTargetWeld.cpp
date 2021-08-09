#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

bool editableObjectGUI_tgweldButton_onIsGoodVertex(miSceneObject* o, miVertex* v) {
	return o == (miEditableObject*)g_app->m_selectedObjects.m_data[0];
}
void editableObjectGUI_tgweldButton_onSelectFirst(miSceneObject* o, miVertex* v) {}
void editableObjectGUI_tgweldButton_onSelectSecond(miSceneObject* o, miVertex* v1, miVertex* v2) {
	//printf("%u %u\n", v1, v2);
	if (o != g_app->m_selectedObjects.m_data[0])
		return;

	if (v1 == v2)
		return;

	miPolygon* p1 = 0;
	miPolygon* p2 = 0;

	auto selObject = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	if (selObject->VertexTargetWeld(v1, v2, &p1, &p2))
	{
		if (p1) selObject->DeletePolygon(p1);
		if (p2) selObject->DeletePolygon(p2);
		selObject->_updateModel(false);
	}
}
void editableObjectGUI_tgweldButton_onCancel(){
	auto selObject = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	auto gui = selObject->GetGui();
	gui->UncheckButtonGroup(1);
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
}
void editableObjectGUI_tgweldButton_onClick(s32 id, bool isChecked) {
	g_app->m_sdk->SetTransformMode(miTransformMode::NoTransform);
}
void editableObjectGUI_tgweldButton_onCheck(s32 id) {
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::SelectVertex);
	g_app->m_sdk->SetPickVertexCallbacks(
		editableObjectGUI_tgweldButton_onIsGoodVertex,
		editableObjectGUI_tgweldButton_onSelectFirst,
		editableObjectGUI_tgweldButton_onSelectSecond,
		editableObjectGUI_tgweldButton_onCancel);
}
void editableObjectGUI_tgweldButton_onUncheck(s32 id) {
	if (g_app->m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::SelectVertex)
	{
		g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
		g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
	}
}

bool miEditableObject::VertexTargetWeld(miVertex* v1, miVertex* v2, miPolygon** polygonForDelete1, miPolygon** polygonForDelete2) {
	bool isOnSameEdge = false;
	{
		auto e1_c = v1->m_edges.m_head;
		auto e1_l = e1_c->m_left;
		while (true) 
		{
			auto e2_c = v2->m_edges.m_head;
			auto e2_l = e2_c->m_left;
			while (true)
			{
				if (e1_c->m_data == e2_c->m_data)
				{
					isOnSameEdge = true;
					goto end;
				}

				if (e2_c == e2_l)
					break;
				e2_c = e2_c->m_right;
			}

			if (e1_c == e1_l)
				break;
			e1_c = e1_c->m_right;
		}
	}

end:;

	bool success = false;
	if (isOnSameEdge)
	{
		auto cp = v1->m_polygons.m_head;
		auto lp = cp->m_left;
		while (true)
		{
			miListNode<miPolygon::_vertex_data>* UV_v1 = 0;

			// if polygon contain v2 then remove v1 from this polygon
			// else replace v1 to v2 and add this polygon to v2
			bool isContainV2 = false;
			{
				auto cv = cp->m_data->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					if (cv->m_data.m_vertex == v1)
						UV_v1 = cv;

					if (cv->m_data.m_vertex == v2)
					{
						//UV_v2 = cuv;
						isContainV2 = true;
					}

					if (cv == lv)
						break;

					cv = cv->m_right;
				}
			}
			if (isContainV2)
			{
				cp->m_data->m_verts.erase_first(v1);

				// check if there are 2 vertices left
				u32 c = 0;
				auto cv = cp->m_data->m_verts.m_head;
				if (cv)
				{
					auto lv = cv->m_left;
					while (true)
					{
						c++;
						if (cv == lv)
							break;
						cv = cv->m_right;
					}
				}
				if (c < 3)
				{
					//printf("DELETE P\n");
					if (polygonForDelete1)
					{
						if (!*polygonForDelete1) *polygonForDelete1 = cp->m_data;
						else *polygonForDelete2 = cp->m_data;
					}
				}
			}
			else
			{
				// I need to find v2 in polygon that contain v2
				/*auto curP = v2->m_polygons.m_head;
				auto curV = curP->m_data->m_verts.m_head;
				auto lastV = curV->m_left;
				while (true)
				{
					if (curV->m_data1 == v2)
						break;
					if (curV == lastV)
						break;
					curV = curV->m_right;
				}*/
				auto vNode = v2->m_polygons.m_head->m_data->FindVertex(v2);

				cp->m_data->m_verts.replace(v1, miPolygon::_vertex_data( v2, vNode->m_data.m_uv, vNode->m_data.m_normal, 0));
				v2->m_polygons.push_back(cp->m_data);
			}


			if (cp == lp)
				break;
			cp = cp->m_right;
		}

		success = true;
	}
	else // they on different edges, and each edge have only 1 polygon
	{
		bool v1OnEdge = v1->IsOnEdge();
		bool v2OnEdge = v2->IsOnEdge();

		if (v1OnEdge && v2OnEdge)
		{
			// replase v1->v2
			auto cp = v1->m_polygons.m_head;
			auto lp = cp->m_left;
			while (true)
			{
				auto vNode = cp->m_data->FindVertex(v1);
				cp->m_data->m_verts.replace(v1, miPolygon::_vertex_data( v2, vNode->m_data.m_uv, vNode->m_data.m_normal, 0));
				v2->m_polygons.push_back(cp->m_data);

				if (cp == lp)
					break;
				cp = cp->m_right;
			}
			success = true;
		}

	}


	if (success)
	{
		auto mesh = m_meshBuilderTmpModelPool ? m_meshBuilderTmpModelPool->m_mesh : m_mesh;

		auto l = v1->m_left;
		auto r = v1->m_right;
		l->m_right = r;
		r->m_left = l;

		if(m_meshBuilderTmpModelPool)
			m_meshBuilderTmpModelPool->m_allocatorVertex->Deallocate(v1);
		else
			m_allocatorVertex->Deallocate(v1);

		if (v1 == mesh->m_first_vertex)
			mesh->m_first_vertex = r;
		if (v1 == mesh->m_first_vertex)
			mesh->m_first_vertex = 0;
	}

	return success;
}

void miEditableObject::VertexMoveTo(miVertex* v1, miVertex* v2) {
	v1->m_position = v2->m_position;

	if (v2->m_polygons.m_head)
	{
		v2f UV;
		{
			auto c = v2->m_polygons.m_head;
			auto l = c->m_left;
			while (true)
			{
				auto vnode = c->m_data->FindVertex(v2);
				if (vnode) 
				{
					UV = vnode->m_data.m_uv;
					break;
				}
				if (c == l)
					break;
				c = c->m_right;
			}
		}
		if (v1->m_polygons.m_head)
		{
			auto c = v1->m_polygons.m_head;
			auto l = c->m_left;
			while (true)
			{
				auto vnode = c->m_data->FindVertex(v1);
				if (vnode)
					vnode->m_data.m_uv = UV;
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}

}

void miEditableObject::_updateModel(bool onlyEdge, bool updateCounts) {
	if (m_meshBuilderTmpModelPool)
	{
		m_meshBuilderTmpModelPool->m_mesh->_delete_edges(m_meshBuilderTmpModelPool->m_allocatorEdge);
		m_meshBuilderTmpModelPool->m_mesh->CreateEdges(m_meshBuilderTmpModelPool->m_allocatorPolygon, m_meshBuilderTmpModelPool->m_allocatorEdge, m_meshBuilderTmpModelPool->m_allocatorVertex);
	}
	else
	{
		m_mesh->_delete_edges(m_allocatorEdge);
		m_mesh->CreateEdges(m_allocatorPolygon, m_allocatorEdge, m_allocatorVertex);
	}

	if (onlyEdge)
		return;

	RebuildVisualObjects(false);

	if(updateCounts)
		m_mesh->UpdateCounts();
}

bool editableObjectGUI_movetoButton_onIsGoodVertex(miSceneObject* o, miVertex* v) {
	return o == (miEditableObject*)g_app->m_selectedObjects.m_data[0];
}
void editableObjectGUI_movetoButton_onSelectFirst(miSceneObject* o, miVertex* v) {}
void editableObjectGUI_movetoButton_onSelectSecond(miSceneObject* o, miVertex* v1, miVertex* v2) {
	//printf("%u %u\n", v1, v2);
	if (o != g_app->m_selectedObjects.m_data[0])
		return;

	if (v1 == v2)
		return;

	auto selObject = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	selObject->VertexMoveTo(v1,v2);
	selObject->_updateModel(false);
}
void editableObjectGUI_movetoButton_onCancel() {
	auto selObject = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	auto gui = selObject->GetGui();
	gui->UncheckButtonGroup(1);
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
}
void editableObjectGUI_movetoButton_onClick(s32 id, bool isChecked) {
	g_app->m_sdk->SetTransformMode(miTransformMode::NoTransform);
}
void editableObjectGUI_movetoButton_onCheck(s32 id) {
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::SelectVertex);
	g_app->m_sdk->SetPickVertexCallbacks(
		editableObjectGUI_movetoButton_onIsGoodVertex,
		editableObjectGUI_movetoButton_onSelectFirst,
		editableObjectGUI_movetoButton_onSelectSecond,
		editableObjectGUI_movetoButton_onCancel);
}
void editableObjectGUI_movetoButton_onUncheck(s32 id) {
	if (g_app->m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::SelectVertex)
	{
		g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
		g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
	}
}