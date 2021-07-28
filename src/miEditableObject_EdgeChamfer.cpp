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
		helpStruct() :m_newVertex(0) {}
		helpStruct(miVertex* v) :m_newVertex(v) {}
		miVertex* m_newVertex;
	};

	// for creating 4 angles polygons
	struct helpStructPolygon1
	{
		helpStructPolygon1(){
			m_v1 = 0;
			m_v2 = 0;
			m_v3 = 0;
			m_v4 = 0;

			m_edge = 0;
		}
		helpStructPolygon1(miListNode3<miVertex*, v2f, v3f>* v1, miListNode3<miVertex*, v2f, v3f>* v2, miListNode3<miVertex*, v2f, v3f>* v3, miListNode3<miVertex*, v2f, v3f>* v4) :
			m_v1(v1),
			m_v2(v2),
			m_v3(v3),
			m_v4(v4)
		{
			m_edge = 0;
		}

		miListNode3<miVertex*,v2f,v3f>* m_v1;
		miListNode3<miVertex*, v2f, v3f>* m_v2;
		miListNode3<miVertex*, v2f, v3f>* m_v3;
		miListNode3<miVertex*, v2f, v3f>* m_v4;

		miEdge* m_edge;
	};
	miBinarySearchTree<helpStructPolygon1> newPolygons; // key is selected edge
	//miArray<helpStructPolygon1> newPolygons;

	// find selected edges in polygons
	if (m_mesh->m_first_polygon)
	{
		auto c = mesh->m_first_polygon;
		auto c_old = m_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			miBinarySearchTree<helpStruct> hs;

			auto ce = c->m_edges.m_head;
			auto ce_old = c_old->m_edges.m_head;
			auto le = ce->m_left;
			while (true)
			{
				if (ce->m_data->m_flags & miEdge::flag_isSelected)
				{
					{
						// sorting for vertices in edge doing using memory address
						// and vertices in edges in m_mesh and in tmp mesh can mismatch
						if (ce_old->m_data->m_vertex1->m_position != ce->m_data->m_vertex1->m_position)
						{
							auto o = ce_old->m_data->m_vertex1;
							ce_old->m_data->m_vertex1 = ce_old->m_data->m_vertex2;
							ce_old->m_data->m_vertex2 = o;
						}
					}

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

					helpStructPolygon1 hsp1;
					if (!newPolygons.Get((uint64_t)ce->m_data, hsp1))
					{
						auto vn1 = c->m_verts.find(v1);
						auto vn2 = c->m_verts.find(v2);

						//if (vn1->m_right == vn2)
						//{
							hsp1.m_v1 = vn1;
							hsp1.m_v2 = vn2;
						//}
						//else
						//{
						//	hsp1.m_v1 = vn2;
						//	hsp1.m_v2 = vn1;
						//}

						hsp1.m_edge = ce_old->m_data;

						newPolygons.Add((uint64_t)ce->m_data, hsp1);
					}
					else
					{
						auto vn1 = c->m_verts.find(v1);
						auto vn2 = c->m_verts.find(v2);

					//	if (vn1->m_right == vn2)
					//	{
							hsp1.m_v3 = vn1;
							hsp1.m_v4 = vn2;
					//	}
					//	else
					//	{
					//		hsp1.m_v3 = vn2;
					//		hsp1.m_v4 = vn1;
					//	}
						
						newPolygons.Add((uint64_t)ce->m_data, hsp1);
					}
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

	// now `corners` that have one vertex of selected edge
	if (m_mesh->m_first_polygon)
	{
		auto c = mesh->m_first_polygon;
		auto c_old = m_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			auto cv = c->m_verts.m_head;
			auto cv_old = c_old->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				auto nv = cv->m_right;
				auto nv_old = cv_old->m_right;

				// check if this vertex is part of some selected edge
				bool isTrue = false;
				{
					auto ce = cv->m_data1->m_edges.m_head;
					if (ce)
					{
						auto le = ce->m_left;
						while (true)
						{
							if (ce->m_data->m_flags & miEdge::flag_isSelected)
							{
								isTrue = true;
								break;
							}
							if (ce == le)break;
							ce = ce->m_right;
						}
					}
				}

				// if true, check selected edges in this polygon
				if (isTrue)
				{
					auto ce = c->m_edges.m_head;
					auto le = ce->m_left;
					while (true)
					{
						if (ce->m_data->m_flags & miEdge::flag_isSelected)
						{
							if (ce->m_data->m_vertex1 == cv->m_data1
								|| ce->m_data->m_vertex2 == cv->m_data1)
							{
								// this vertex part of edge
								isTrue = false;
								break;
							}
						}
						if (ce == le)break;
						ce = ce->m_right;
					}
				}

				// if still true then this is vertex what I need
				if (isTrue)
				{
					// calculate directions
					v3f dir1, dir2;
					{
						dir1 = cv->m_right->m_data1->m_position - cv->m_data1->m_position;
						dir2 = cv->m_left->m_data1->m_position - cv->m_data1->m_position;
						dir1.normalize2();
						dir2.normalize2();
					}

					// create 2 new vertices
					miVertex* v1 = m_meshBuilderTmpModelPool->m_allocatorVertex->Allocate();
					miVertex* v2 = m_meshBuilderTmpModelPool->m_allocatorVertex->Allocate();

					v1->m_position = cv_old->m_data1->m_position;
					v2->m_position = cv_old->m_data1->m_position;

					v1->m_position += m_edgeChamferValue * dir1;
					v2->m_position += m_edgeChamferValue * dir2;

					v1->m_polygons.push_back(c);
					v2->m_polygons.push_back(c);

					mesh->_add_vertex_to_list(v1);
					mesh->_add_vertex_to_list(v2);

					auto vNode = c->m_verts.find(cv->m_data1);
					c->m_verts.insert_after(vNode->m_data1, v1, vNode->m_data2, vNode->m_data3);
					c->m_verts.insert_before(vNode->m_data1, v2, vNode->m_data2, vNode->m_data3);
				}


				if (cv == lv)break;
				cv = nv;
				cv_old = nv_old;
			}

			if (c == l)
				break;
			c = c->m_right;
			c_old = c_old->m_right;
		}
	}

	//// create polygons from edges
	//{
	//	miArray<helpStructPolygon1> arr;
	//	arr.reserve(0x1000);
	//	newPolygons.Get(&arr);
	//	for (u32 i = 0; i < arr.m_size; ++i)
	//	{
	//		auto & hsp1 = arr.m_data[i];

	//		if (!hsp1.m_v1 || !hsp1.m_v2 || !hsp1.m_v3 || !hsp1.m_v4)
	//			continue;

	//		miPolygon * newPolygon = m_meshBuilderTmpModelPool->m_allocatorPolygon->Allocate();
	//		
	//		newPolygon->m_verts.push_back(hsp1.m_v1->m_data1, hsp1.m_v1->m_data2, hsp1.m_v1->m_data3);
	//		newPolygon->m_verts.push_back(hsp1.m_v2->m_data1, hsp1.m_v2->m_data2, hsp1.m_v2->m_data3);
	//		//newPolygon->m_verts.push_back(hsp1.m_v4->m_data1, hsp1.m_v4->m_data2, hsp1.m_v4->m_data3);
	//		//newPolygon->m_verts.push_back(hsp1.m_v3->m_data1, hsp1.m_v3->m_data2, hsp1.m_v3->m_data3);
	//		{
	//			/*v3f d1 = hsp1.m_v4->m_data1->m_position - hsp1.m_v3->m_data1->m_position;
	//			v3f d2 = hsp1.m_v2->m_data1->m_position - hsp1.m_v1->m_data1->m_position;
	//			d1.normalize2();
	//			d2.normalize2();*/
	//			
	//			yyRay r;
	//			r.m_origin = hsp1.m_v4->m_data1->m_position;
	//			r.m_end = hsp1.m_v3->m_data1->m_position;

	//			auto d = r.distanceToLine(hsp1.m_v2->m_data1->m_position, hsp1.m_v1->m_data1->m_position);

	//			bool good = true;

	//			if (d < 0.1f)
	//				good = false;
	//			
	//			if (good)
	//			{
	//				r.m_origin = hsp1.m_v4->m_data1->m_position;
	//				r.m_end = hsp1.m_v1->m_data1->m_position;
	//				d = r.distanceToLine(hsp1.m_v3->m_data1->m_position, hsp1.m_v2->m_data1->m_position);
	//				if (d < 0.1f)
	//					good = false;
	//			}

	//			if (good)
	//			{
	//				newPolygon->m_verts.push_back(hsp1.m_v3->m_data1, hsp1.m_v3->m_data2, hsp1.m_v3->m_data3);
	//				newPolygon->m_verts.push_back(hsp1.m_v4->m_data1, hsp1.m_v4->m_data2, hsp1.m_v4->m_data3);
	//			}
	//			else
	//			{
	//				newPolygon->m_verts.push_back(hsp1.m_v4->m_data1, hsp1.m_v4->m_data2, hsp1.m_v4->m_data3);
	//				newPolygon->m_verts.push_back(hsp1.m_v3->m_data1, hsp1.m_v3->m_data2, hsp1.m_v3->m_data3);
	//			}
	//		}

	//		newPolygon->CalculateNormal();
	//		mesh->_add_polygon_to_list(newPolygon);

	//		// Check normal. 
	//		v3f edgeNormal;
	//		if (hsp1.m_edge->m_polygon1)
	//		{
	//			hsp1.m_edge->m_polygon1->CalculateNormal();
	//			edgeNormal += hsp1.m_edge->m_polygon1->GetFaceNormal();
	//		}
	//		if (hsp1.m_edge->m_polygon2)
	//		{
	//			hsp1.m_edge->m_polygon2->CalculateNormal();
	//			edgeNormal += hsp1.m_edge->m_polygon2->GetFaceNormal();
	//		}
	//		edgeNormal.normalize2();

	//		if (edgeNormal.dot(newPolygon->GetFaceNormal()) <= 0.f)
	//		{
	//			newPolygon->Flip();
	//			newPolygon->CalculateNormal();
	//		}
	//	}
	//}

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

