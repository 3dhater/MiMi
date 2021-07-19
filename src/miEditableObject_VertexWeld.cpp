#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void editableObjectGUI_weldButton_onCancel();
void editableObjectGUI_weldButton_onUncheck(s32 id);

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
	if (object->m_isWeld)
	{
		if (*fptr < 0.f)
			*fptr = 0.f;
		object->OnWeld(false);
	}
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
	/*object->DestroyTMPModelWithPoolAllocator();
	object->m_isWeld = false;*/
	editableObjectGUI_weldButton_onUncheck(-1);
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
	object->OnWeld(true);
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
	object->m_isWeld = false;
	object->_updateModel(false);
}

void miEditableObject::OnWeld(bool createNewTMPModel) {
	//return;

	if (!m_isWeld)
	{
		m_isWeld = true;
	}

	if (createNewTMPModel)
	{
		DestroyTMPModelWithPoolAllocator();
		CreateTMPModelWithPoolAllocator(GetPolygonCount(), GetEdgeCount(), GetVertexCount());
	}
	

	auto mesh = m_meshBuilderTmpModelPool->m_mesh;

	// easy to go through array than through list
	static miArray<miVertex*> varr_base;
	static miArray<miVertex*> varr;
	varr_base.reserve(0x2000);
	varr_base.clear();
	varr.reserve(0x2000);
	varr.clear();

	{
		auto c = mesh->m_first_vertex;
		auto c_base = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			varr_base.push_back(c_base);
			varr.push_back(c);

			if (c_base->m_flags & c_base->flag_User1) c_base->m_flags ^= c_base->flag_User1;
			if (c_base->m_flags & c_base->flag_User2) c_base->m_flags ^= c_base->flag_User2;
			if (c->m_flags & c->flag_User1) c->m_flags ^= c->flag_User1;
			if (c->m_flags & c->flag_User2) c->m_flags ^= c->flag_User2;
			
			c->m_position = c_base->m_position;

			if (c == l)
				break;
			c = c->m_right;
			c_base = c_base->m_right;
		}
	}
	{
		auto c = mesh->m_first_polygon;
		auto c_base = m_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			auto cv = c->m_verts.m_head;
			auto cv_base = c_base->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				cv->m_data2 = cv_base->m_data2;
				if (cv == lv)
					break;
				cv = cv->m_right;
				cv_base = cv_base->m_right;
			}

			if (c == l)
				break;
			c = c->m_right;
			c_base = c_base->m_right;
		}
	}

	for (u32 i = 0; i < varr.m_size; ++i)
	{
		if ((varr.m_data[i]->m_flags & miVertex::flag_User1) == 0 && varr.m_data[i]->m_flags & miVertex::flag_isSelected)
		{
			for (u32 i2 = i; i2 < varr.m_size; ++i2)
			{
				if ((varr.m_data[i2]->m_flags & miVertex::flag_User1) == 0 && varr.m_data[i2]->m_flags & miVertex::flag_isSelected)
				{
					f32 d = varr_base.m_data[i]->m_position.distance(varr_base.m_data[i2]->m_position);
					if (d <= m_weldValue)
					{
						varr.m_data[i2]->m_flags |= miVertex::flag_User1;
						this->VertexMoveTo(varr.m_data[i2], varr.m_data[i]);
					}
				}
			}
		}
	}


	_updateModel(false, false);
}

void miEditableObject::OnWeldApply() {
	if (!m_isWeld)
		return;
	m_isWeld = false;

	auto mesh = m_meshBuilderTmpModelPool->m_mesh;
	
	// 1. Delete all invisible polygons if they have at least one selected vertex
	// 2. Delete vertices(from polygon) with same position
	// 3.  'weld' all selected

	// 1.
	{
		auto currentPolygon = mesh->m_first_polygon;
		auto lastPolygon = currentPolygon->m_left;
		while (true)
		{
			auto nextPolygon = currentPolygon->m_right;

			if (!currentPolygon->IsVisible())
			{
				auto cv = currentPolygon->m_verts.m_head;
				auto lv = cv->m_left;
				while(true)
				{
					if (cv->m_data1->m_flags & miVertex::flag_isSelected)
					{
						this->DeletePolygon(currentPolygon);
						break;
					}

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}

			if (currentPolygon == lastPolygon)
				break;
			currentPolygon = nextPolygon;
		}
	}
	_updateModel(false, true);

	_destroyMesh();
	m_mesh = miCreate<miMesh>();
	g_app->m_sdk->AppendMesh(m_mesh, mesh);
	this->DestroyTMPModelWithPoolAllocator();

	this->VertexBreak();

	// 2.
	{
		auto currentPolygon = m_mesh->m_first_polygon;
		auto lastPolygon = currentPolygon->m_left;
		while (true)
		{
			auto nextPolygon = currentPolygon->m_right;
		begin:;
			auto currVertex = currentPolygon->m_verts.m_head;
			auto lastVertex = currVertex->m_left;
			while (true)
			{
				if (currVertex->m_data1->m_flags & miVertex::flag_isSelected)
				{
					if (currVertex->m_data1->m_flags & miVertex::flag_User1)
						currVertex->m_data1->m_flags ^= miVertex::flag_User1;

					auto currVertex2 = currVertex->m_right;
					auto lastVertex2 = currVertex2->m_left;
					while (true)
					{
						if (currVertex2->m_data1->m_flags & miVertex::flag_isSelected)
						{
							if (currVertex2->m_data1->m_flags & miVertex::flag_User1)
								currVertex2->m_data1->m_flags ^= miVertex::flag_User1;

							if (currVertex->m_data1 != currVertex2->m_data1)
							{
								if (currVertex->m_data1->m_position == currVertex2->m_data1->m_position)
								{
									currentPolygon->m_verts.erase_first(currVertex2->m_data1);
									currVertex2->m_data1->m_polygons.erase_first(currentPolygon);
									if (!currVertex2->m_data1->m_polygons.m_head)
									{
										m_mesh->_remove_vertex_from_list(currVertex2->m_data1);
										currVertex2->m_data1->~miVertex();
										m_allocatorVertex->Deallocate(currVertex2->m_data1);
										goto begin;
									}
								}
							}
						}

						if (currVertex2 == lastVertex2)
							break;
						currVertex2 = currVertex2->m_right;
					}
				}

				if (currVertex == lastVertex)
					break;
				currVertex = currVertex->m_right;
			}

			if (currentPolygon == lastPolygon)
				break;
			currentPolygon = nextPolygon;
		}
	}
	
	// 3.
	{
		// I need list of vertices in specific position
		std::unordered_map<std::string, miListNode2<miVertex*, v2f>*> weldMap;

		std::string vertsMapHash;
		auto _set_hash = [&](v3f* position)
		{
			vertsMapHash.clear();
			char * ptr = (char *)position->data();
			char bytes[13];
			bytes[0] = ptr[0];
			bytes[1] = ptr[1];
			bytes[2] = ptr[2];
			bytes[3] = ptr[3];
			bytes[4] = ptr[4];
			bytes[5] = ptr[5];
			bytes[6] = ptr[6];
			bytes[7] = ptr[7];
			bytes[8] = ptr[8];
			bytes[9] = ptr[9];
			bytes[10] = ptr[10];
			bytes[11] = ptr[11];
			if (bytes[0] == 0) bytes[0] = 1;
			if (bytes[1] == 0) bytes[1] = 1;
			if (bytes[2] == 0) bytes[2] = 1;
			if (bytes[3] == 0) bytes[3] = 1;
			if (bytes[4] == 0) bytes[4] = 1;
			if (bytes[5] == 0) bytes[5] = 1;
			if (bytes[6] == 0) bytes[6] = 1;
			if (bytes[7] == 0) bytes[7] = 1;
			if (bytes[8] == 0) bytes[8] = 1;
			if (bytes[9] == 0) bytes[9] = 1;
			if (bytes[10] == 0) bytes[10] = 1;
			if (bytes[11] == 0) bytes[11] = 1;
			bytes[12] = 0;
			vertsMapHash = bytes;
		};

		auto currentPolygon = m_mesh->m_first_polygon;
		auto lastPolygon = currentPolygon->m_left;
		while (true)
		{
			auto nextPolygon = currentPolygon->m_right;
			auto currVertex = currentPolygon->m_verts.m_head;
			auto lastVertex = currVertex->m_left;
			while (true)
			{
				if (currVertex->m_data1->m_flags & miVertex::flag_isSelected)
				{
					if (currVertex->m_data1->m_flags & miVertex::flag_User1)
						currVertex->m_data1->m_flags ^= miVertex::flag_User1;

					_set_hash(&currVertex->m_data1->m_position);
					auto it = weldMap.find(vertsMapHash);
					if (it == weldMap.end())
					{
						weldMap[vertsMapHash] = currVertex; // I need to know only one vertex in this position
					}
				}

				if (currVertex == lastVertex)
					break;
				currVertex = currVertex->m_right;
			}

			if (currentPolygon == lastPolygon)
				break;
			currentPolygon = nextPolygon;
		}

		{
			auto currentVertex = m_mesh->m_first_vertex;
			auto lastVertex = currentVertex->m_left;
			while (true)
			{
				auto nextVertex = currentVertex->m_right;

				if (currentVertex->m_flags & miVertex::flag_isSelected)
				{
					_set_hash(&currentVertex->m_position);
					auto iterator = weldMap.find(vertsMapHash);
					if (iterator != weldMap.end())
					{
						bool v1OnEdge = iterator->second->m_data1->IsOnEdge();

						if (currentVertex != iterator->second->m_data1
							&& v1OnEdge)
						{
							{
								// replace vertices in edges
								auto ce = currentVertex->m_edges.m_head;
								auto le = ce->m_left;
								while (true)
								{
									bool add = false;
									auto ne = ce->m_right;

									if (ce->m_data->m_vertex1 == currentVertex)
									{
										ce->m_data->m_vertex1 = iterator->second->m_data1;
										add = true;
									}
									else if (ce->m_data->m_vertex2 == currentVertex)
									{
										ce->m_data->m_vertex2 = iterator->second->m_data1;
										add = true;
									}

									if (add)
									{
										//check duplicates
										bool isDuplicate = false;
										auto ce2 = iterator->second->m_data1->m_edges.m_head;
										auto le2 = ce2->m_left;
										while (true)
										{
											if (ce2->m_data != ce->m_data)
											{
												auto v1_1 = ce2->m_data->m_vertex1;
												auto v2_1 = ce2->m_data->m_vertex2;
												if (v2_1 < v1_1)
												{
													v2_1 = ce2->m_data->m_vertex1;
													v1_1 = ce2->m_data->m_vertex2;
												}

												auto v1_2 = ce->m_data->m_vertex1;
												auto v2_2 = ce->m_data->m_vertex2;
												if (v2_2 < v1_2)
												{
													v2_2 = ce->m_data->m_vertex1;
													v1_2 = ce->m_data->m_vertex2;
												}

												if (v1_1 == v1_2 && v2_1 == v2_2)
												{
													isDuplicate = true;
													break;
												}
											}
											if (ce2 == le2)
												break;
											ce2 = ce2->m_right;
										}
										if (isDuplicate)
										{
											// change pointers for polygon
											miPolygon* ce_polygon = ce->m_data->m_polygon1;
											if (!ce_polygon)
												ce_polygon = ce->m_data->m_polygon2;

											ce_polygon->m_edges.replace(ce->m_data, ce2->m_data);

											if (!ce2->m_data->m_polygon1)
												ce2->m_data->m_polygon1 = ce_polygon;
											else if (!ce2->m_data->m_polygon2)
												ce2->m_data->m_polygon2 = ce_polygon;

											// delete edge
											m_mesh->_remove_edge_from_list(ce->m_data);
											ce->m_data->~miEdge();
											m_allocatorEdge->Deallocate(ce->m_data);
										//	printf("DUPLICATE\n");
										}
										else
										{
											iterator->second->m_data1->m_edges.push_back(ce->m_data);
										}
									}

									if (ce == le)
										break;
									ce = ne;
								}
							}

							auto cp = currentVertex->m_polygons.m_head;
							auto lp = cp->m_left;
							while (true)
							{
								bool v2OnEdge = currentVertex->IsOnEdge();
								if (v2OnEdge)
								{
									cp->m_data->m_verts.replace(currentVertex, iterator->second->m_data1, iterator->second->m_data2);
									iterator->second->m_data1->m_polygons.push_back(cp->m_data);
									currentVertex->m_polygons.erase_first(cp->m_data);
								}

								if (cp == lp)
									break;
								cp = cp->m_right;
							}

							{
								m_mesh->_remove_vertex_from_list(currentVertex);
								currentVertex->~miVertex();
								m_allocatorVertex->Deallocate(currentVertex);
							}

						}
					}
				}
				if (currentVertex == lastVertex)
					break;
				currentVertex = nextVertex;
			}
		}
	}


	_updateModel(false);
	DeselectAll(g_app->m_editMode);
}