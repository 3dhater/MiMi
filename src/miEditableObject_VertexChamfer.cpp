#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

void editableObjectGUI_vertexChamferButton_onCheck(s32 id);
void editableObjectGUI_vertexChamferButton_onCancel();
void editableObjectGUI_vertexChamferButton_onUncheck(s32 id);

void editableObjectGUI_vertexChamferCheckBox_onClick(bool isChecked) {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->m_addPolygonsWhenVertexChamfer = isChecked;
	if (object->m_isVertexChamfer)
		object->OnVertexChamfer();
}

void editableObjectGUI_vertexChamferButtonOK_onClick(s32 id) {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->OnVertexChamferApply();
	editableObjectGUI_vertexChamferButton_onCancel();
}

float* editableObjectGUI_vertexChamferRange_onSelectObject(miSceneObject* obj) {
	if (obj->GetPlugin() != g_app->m_pluginForApp)
		return 0;

	if (obj->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return 0;

	return &((miEditableObject*)obj)->m_vertexChamferValue;
}

void editableObjectGUI_vertexChamferRange_onValueChanged(miSceneObject* obj, float* fptr) {
	if (obj->GetPlugin() != g_app->m_pluginForApp)
		return;

	if (obj->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return;
	auto object = (miEditableObject*)obj;
	if (object->m_isVertexChamfer)
	{
		if (*fptr < 0.f)
			*fptr = 0.f;
		object->OnVertexChamfer();
	}
}

void editableObjectGUI_vertexChamferButton_onSelect(miEditMode em)
{
	switch (em)
	{
	case miEditMode::Vertex: {
		editableObjectGUI_vertexChamferButton_onCheck(-1);
	}break;
	case miEditMode::Edge:
	case miEditMode::Polygon:
	case miEditMode::Object:
	default:
		break;
	}
}

void editableObjectGUI_vertexChamferButton_onCancel() {
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	auto gui = object->GetGui();
	gui->UncheckButtonGroup(1);
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
	g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
	g_app->m_sdk->SetSelectObjectCallbacks(0);
	/*object->DestroyTMPModelWithPoolAllocator();
	object->m_isChamfer = false;*/
	editableObjectGUI_vertexChamferButton_onUncheck(-1);
}

void editableObjectGUI_vertexChamferButton_onClick(s32 id, bool isChecked) {
	g_app->m_sdk->SetTransformMode(miTransformMode::NoTransform);
}

void editableObjectGUI_vertexChamferButton_onCheck(s32 id) {
	g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::Other);
	g_app->m_sdk->SetPickVertexCallbacks(
		0, 0, 0,
		editableObjectGUI_vertexChamferButton_onCancel);
	g_app->m_sdk->SetSelectObjectCallbacks(editableObjectGUI_vertexChamferButton_onSelect);

	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->OnVertexChamfer();
}

void editableObjectGUI_vertexChamferButton_onUncheck(s32 id) {
	if (g_app->m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::Other)
	{
		g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
		g_app->m_sdk->SetPickVertexCallbacks(0, 0, 0, 0);
		g_app->m_sdk->SetSelectObjectCallbacks(0);
	}
	auto object = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	object->m_isVertexChamfer = false;
	object->DestroyTMPModelWithPoolAllocator();
	object->_updateModel(false);
}

void miEditableObject::OnVertexChamfer() {
	static s32 pc = 0;
	static s32 ec = 0;
	static s32 vc = 0;


	// I need to know maximum numbers of vertices/edges/polygons
	// for poolAllocator
	if (!m_isVertexChamfer)
	{
		this->UpdateCounts();

		pc = 0;
		ec = 0;
		vc = 0;

		if (m_mesh->m_first_vertex)
		{
			auto c = m_mesh->m_first_vertex;
			auto l = c->m_left;
			while (true)
			{
				//later need to try to use only selected vertices

				s32 polygonCount = 0;
				if (c->m_polygons.m_head)
				{
					auto cp = c->m_polygons.m_head;
					auto lp = cp->m_left;
					while (true)
					{
						++polygonCount;
						if (cp == lp)
							break;
						cp = cp->m_right;
					}
					if (polygonCount)
					{
						if (polygonCount == 1)
						{
							++vc;
							++ec;
						}
						else
						{
							vc += polygonCount * 2 + 2;
							ec += polygonCount * 2 + 2;
							pc += 1;
						}
					}
				}
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}
	m_isVertexChamfer = true;

	DestroyTMPModelWithPoolAllocator();
	CreateTMPModelWithPoolAllocator(this->GetPolygonCount() + pc, this->GetEdgeCount() + ec, this->GetVertexCount() + vc);
	auto mesh = m_meshBuilderTmpModelPool->m_mesh;
	
	struct helpStruct
	{
		helpStruct(miPolygon* p, miVertex* v):m_p(p),m_v(v) {}
		miPolygon* m_p;
		miVertex*  m_v;
	};
	static miArray<helpStruct> removeVertFromPolygon;
	removeVertFromPolygon.clear();

	if (m_mesh->m_first_vertex)
	{
		auto c = m_mesh->m_first_vertex;
		auto c_new = mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miVertex::flag_isSelected)
			{
				v3f vertexNormal;
				// calculate vertex normal
				{
					auto cp = c->m_polygons.m_head;
					auto lp = cp->m_left;
					while (true)
					{
						// find this vertex in polygon
						auto cv = cp->m_data->m_verts.m_head;
						auto lv = cv->m_left;
						while (true)
						{
							if (cv->m_data.m_vertex == c)
							{
								auto e1 = cv->m_left->m_data.m_vertex->m_position - cv->m_data.m_vertex->m_position;
								auto e2 = cv->m_right->m_data.m_vertex->m_position - cv->m_data.m_vertex->m_position;
								auto n = e1.cross(e2);
								vertexNormal -= n;
								break;
							}
							if (cv == lv)
								break;
							cv = cv->m_right;
						}

						if (cp == lp)
							break;
						cp = cp->m_right;
					}
					vertexNormal.normalize2();
				}

				auto ce = c->m_edges.m_head;
				auto ce_new = c_new->m_edges.m_head;

				miPolygon* firstEdgePolygon1 = ce_new->m_data->m_polygon1;
				miPolygon* firstEdgePolygon2 = ce_new->m_data->m_polygon2;

				struct helpStruct2
				{
					helpStruct2(miVertex* v, const v2f& uv, const v3f& n):m_vertex(v),m_uv(uv),m_normal(n) {}

					miVertex* m_vertex;
					v2f m_uv;
					v3f m_normal;
				};
				static miArray<helpStruct2> vericesForNewpolygon;
				vericesForNewpolygon.clear();

				auto le = ce->m_left;
				while (true)
				{
					v3f dir;
					if (ce->m_data->m_vertex1 == c)
						dir = ce->m_data->m_vertex2->m_position - ce->m_data->m_vertex1->m_position;
					else
						dir = ce->m_data->m_vertex1->m_position - ce->m_data->m_vertex2->m_position;
					dir.normalize2();

					auto chamferValue = m_vertexChamferValue;

					f32 edgeLen = ce->m_data->m_vertex1->m_position.distance(ce->m_data->m_vertex2->m_position);
					f32 edgeLenHalf = edgeLen* 0.5f;

					if (chamferValue > edgeLen) chamferValue = edgeLen;

					if (ce->m_data->m_vertex1->m_flags & miVertex::flag_isSelected
						&& ce->m_data->m_vertex2->m_flags & miVertex::flag_isSelected)
					{
						if (chamferValue > edgeLenHalf) chamferValue = edgeLenHalf;
					}

					miVertex* vertex = 0;
					v2f uv;
					v3f normal;

					if (ce->m_data->m_polygon1)
					{
						auto fv = ce->m_data->m_polygon1->FindVertex(c);
						if (fv)
						{
							uv = fv->m_data.m_uv;
							normal = fv->m_data.m_normal;
						}
					}
					else if (ce->m_data->m_polygon2)
					{
						auto fv = ce->m_data->m_polygon2->FindVertex(c);
						if (fv)
						{
							uv = fv->m_data.m_uv;
							normal = fv->m_data.m_normal;
						}
					}

					// first edge for this vertex
					if (ce == c->m_edges.m_head)
					{
						vertex = c_new;
						vertex->m_polygons.clear();
						if(ce->m_data->m_polygon1)
							vertex->m_polygons.push_back(ce->m_data->m_polygon1);
						if (ce->m_data->m_polygon2)
							vertex->m_polygons.push_back(ce->m_data->m_polygon2);
					}
					else
					{
						// create new vertex
						vertex = m_meshBuilderTmpModelPool->m_allocatorVertex->Allocate();
						vertex->m_flags |= miVertex::flag_isSelected;
						
						vertex->m_right = c_new;
						vertex->m_left = c_new->m_left;
						c_new->m_left->m_right = vertex;
						c_new->m_left = vertex;

						// put vertex into polygon
						if (ce->m_data->m_polygon1)
						{
							vertex->m_polygons.push_back(ce_new->m_data->m_polygon1);
							
							auto pvc = ce->m_data->m_polygon1->m_verts.m_head;
							auto pvl = pvc->m_left;
							while (true)
							{
								// I need to find current vertex (c), in polygons vertices
								if (pvc->m_data.m_vertex == c)
								{
									// next to find second vertex of this edge
									miVertex* c2 = ce->m_data->m_vertex1;
									if (c == c2)
										c2 = ce->m_data->m_vertex2;

									if (pvc->m_right->m_data.m_vertex == c2)
									{
										ce_new->m_data->m_polygon1->m_verts.insert_after(c_new, miPolygon::_vertex_data(vertex, pvc->m_data.m_uv, pvc->m_data.m_normal, 0));
									}
									else
									{
										ce_new->m_data->m_polygon1->m_verts.insert_before(c_new, miPolygon::_vertex_data(vertex, pvc->m_data.m_uv, pvc->m_data.m_normal, 0));
									}
									break;
								}

								if (pvc == pvl)
									break;
								pvc = pvc->m_right;
							}

							if (ce_new->m_data->m_polygon1 != firstEdgePolygon1
								&& ce_new->m_data->m_polygon1 != firstEdgePolygon2)
							{
								removeVertFromPolygon.push_back(helpStruct(ce_new->m_data->m_polygon1, c_new));
							}
						}

						if (ce->m_data->m_polygon2)
						{
							vertex->m_polygons.push_back(ce_new->m_data->m_polygon2);

							auto pvc_new = ce_new->m_data->m_polygon2->m_verts.m_head;
							auto pvc = ce->m_data->m_polygon2->m_verts.m_head;
							auto pvl = pvc->m_left;
							while (true)
							{
								// I need to find current vertex (c), in polygons vertices
								if (pvc->m_data.m_vertex == c)
								{
									// next to find second vertex of this edge
									miVertex* c2 = ce->m_data->m_vertex1;
									if (c == ce->m_data->m_vertex1)
										c2 = ce->m_data->m_vertex2;

									if (pvc->m_right->m_data.m_vertex == c2)
									{
										ce_new->m_data->m_polygon2->m_verts.insert_after(c_new, miPolygon::_vertex_data( vertex, pvc->m_data.m_uv, pvc->m_data.m_normal, 0));
									}
									else
									{
										ce_new->m_data->m_polygon2->m_verts.insert_before(c_new, miPolygon::_vertex_data(vertex, pvc->m_data.m_uv, pvc->m_data.m_normal, 0));
									}
									break;
								}

								if (pvc == pvl)
									break;
								pvc = pvc->m_right;
								pvc_new = pvc_new->m_right;
							}

							if (ce_new->m_data->m_polygon2 != firstEdgePolygon1
								&& ce_new->m_data->m_polygon2 != firstEdgePolygon2)
							{
								removeVertFromPolygon.push_back(helpStruct(ce_new->m_data->m_polygon2, c_new));
							}
						}

					}
					if (vertex) 
					{
						vertex->m_position = c->m_position + chamferValue * dir;
						
						/*bool _addv = true;  ????????????????????????
						for (u32 i = 0; i < vericesForNewpolygon.m_size; ++i)
						{
							if (vericesForNewpolygon.m_data[i].m_vertex == vertex)
							{
								_addv = false;
								break;
							}
						}

						if(_addv)*/
							vericesForNewpolygon.push_back(helpStruct2(vertex,uv, normal));
					}

					if (ce == le)
						break;
					ce = ce->m_right;
					ce_new = ce_new->m_right;
				}

				if (vericesForNewpolygon.m_size > 2 && m_addPolygonsWhenVertexChamfer)
				{
					auto newPolygon = m_meshBuilderTmpModelPool->m_allocatorPolygon->Allocate();
					
					for (u32 i = vericesForNewpolygon.m_size - 1; i >= 0; )
					{
						auto & v = vericesForNewpolygon.m_data[i];
						newPolygon->m_verts.push_back(miPolygon::_vertex_data(v.m_vertex, v.m_uv, v.m_normal, 0));
						v.m_vertex->m_polygons.push_back(newPolygon);

						if (i==0)
							break;
						--i;
					}

					newPolygon->m_left = m_meshBuilderTmpModelPool->m_mesh->m_first_polygon->m_left;
					newPolygon->m_right = m_meshBuilderTmpModelPool->m_mesh->m_first_polygon;

					newPolygon->CalculateNormal();
					{
						auto n1 = newPolygon->m_verts.m_head->m_data.m_normal;
						auto n2 = vertexNormal;
						if (n1.dot(n2) < 0.f)
						{
							newPolygon->Flip();
							newPolygon->CalculateNormal();
						}
					}
					m_meshBuilderTmpModelPool->m_mesh->m_first_polygon->m_left->m_right = newPolygon;
					m_meshBuilderTmpModelPool->m_mesh->m_first_polygon->m_left = newPolygon;
				}
			}

			if (c == l)
				break;
			c = c->m_right;
			c_new = c_new->m_right;
		}
		//printf("\n\n");

	}

	for (u32 i = 0; i < removeVertFromPolygon.m_size; ++i)
	{
		removeVertFromPolygon.m_data[i].m_p->m_verts.erase_first(removeVertFromPolygon.m_data[i].m_v);
	}

	_updateModel();
}

void miEditableObject::OnVertexChamferApply() {
	if (!m_isVertexChamfer)
		return;
	m_isVertexChamfer = false;

	_createMeshFromTMPMesh_meshBuilder(true, false);
	
	this->DestroyTMPModelWithPoolAllocator();
	_updateModel();
}

