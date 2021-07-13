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
	if (object->m_isWeld)
	{
		if (*fptr < 0.f)
			*fptr = 0.f;
		object->OnWeld();
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
	//return;

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

							this->VertexMoveTo(c2,c);
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


	_updateModel(false);
}

void miEditableObject::OnWeldApply() {
	if (!m_isWeld)
		return;
	m_isWeld = false;

	auto mesh = m_meshBuilderTmpModelPool->m_mesh;
	
	// 1. Delete all invisible polygons if they have at least one selected vertex
	// 2. Create some help structs/containers that contain all polygons and vertices 
	//     in same position.
	// 3. Remove from polygon that vertices, who have same position. Save only last vertex.
	// 4. Add to this polygons that vertites of other polygons, who have same position 
	//     with vertex of this polygon.


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
	
	_destroyMesh();
	m_mesh = miCreate<miMesh>();
	g_app->m_sdk->AppendMesh(m_mesh, mesh);
	this->DestroyTMPModelWithPoolAllocator();

	this->VertexBreak();

	// Delete vertices(from polygon) with same position
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

	// just target weld
	{
	begin2:;
		auto currVertex = m_mesh->m_first_vertex;
		auto lastVertex = currVertex->m_left;
		while (true)
		{
			if ((currVertex->m_flags & miVertex::flag_User1) == 0
				&& (currVertex->m_flags & miVertex::flag_isSelected))
			{

				auto currVertex2 = currVertex->m_right;
				auto lastVertex2 = currVertex2->m_left;
				while (true)
				{
					if (currVertex2->m_flags & miVertex::flag_isSelected)
					{
						if (currVertex != currVertex2)
						{
							if (currVertex->m_position == currVertex2->m_position)
							{
								miPolygon* p1 = 0;
								miPolygon* p2 = 0;
								this->VertexTargetWeld(currVertex2, currVertex, &p1, &p2);
								_updateModel(true);
								if (p1) printf("!!! p1\n");
								if (p2) printf("!!! p2\n");
								goto begin2;
							}
						}
					}

					if (currVertex2 == lastVertex2)
						break;
					currVertex2 = currVertex2->m_right;
				}
			}

			currVertex->m_flags |= miVertex::flag_User1;

			if (currVertex == lastVertex)
				break;
			currVertex = currVertex->m_right;
		}
	}

	//// 2.
	//{
	//	struct _node
	//	{
	//		miList<miListNode2<miVertex*,v2f>*> m_vertices;
	//	};
	//	miArray<_node> nodes;

	//	auto currentPolygon = mesh->m_first_polygon;
	//	auto lastPolygon = currentPolygon->m_left;
	//	while (true)
	//	{
	//		auto nextPolygon = currentPolygon->m_right;



	//		if (currentPolygon == lastPolygon)
	//			break;
	//		currentPolygon = nextPolygon;
	//	}
	//}

	//// 2.
	//{
	//	struct _city
	//	{
	//		_city(){}
	//		~_city(){
	//			for (u32 i = 0; i < m_brands.m_size; ++i){
	//				delete m_brands.m_data[i];
	//			}
	//		}

	//		struct _brand{
	//			_brand(){
	//				m_brand = 0;
	//			}
	//			
	//			~_brand(){}

	//			miArray<miVertex*> m_models;
	//			miPolygon* m_brand;
	//		};

	//		miArray<_brand*> m_brands;

	//		v3f m_cityName;
	//	};
	//	miArray<_city*> cities;
	//	cities.reserve(0x1000);

	//	auto cv = mesh->m_first_vertex;
	//	auto lv = cv->m_left;
	//	while (true)
	//	{
	//		if (cv->m_flags & miVertex::flag_isSelected)
	//		{
	//			// find city
	//			_city * city = 0;
	//			for (u32 i = 0; i < cities.m_size; ++i)
	//			{
	//				if (cities.m_data[i]->m_cityName == cv->m_position)
	//				{
	//					city = cities.m_data[i];
	//					break;
	//				}
	//			}

	//			// if not found then add new city
	//			if (!city)
	//			{
	//				city = new _city;
	//				city->m_cityName = cv->m_position;
	//				cities.push_back(city);
	//			}

	//			auto currVertexPolygon = cv->m_polygons.m_head;
	//			auto lastVertexPolygon = currVertexPolygon->m_left;
	//			while (true)
	//			{
	//				_city::_brand* brand = 0;

	//				// find brand or add new
	//				for (u32 i = 0; i < city->m_brands.m_size; ++i)
	//				{
	//					auto currBrand = city->m_brands.m_data[i];
	//					if (currVertexPolygon->m_data == currBrand->m_brand)
	//					{
	//						brand = currBrand;
	//						break;
	//					}
	//				}
	//				if (!brand)
	//				{
	//					brand = new _city::_brand;
	//					brand->m_brand = currVertexPolygon->m_data;
	//					city->m_brands.push_back(brand);
	//				}
	//				
	//				// add model of this brand
	//				brand->m_models.push_back(cv);

	//				if (currVertexPolygon == lastVertexPolygon)
	//					break;
	//				currVertexPolygon = currVertexPolygon->m_right;
	//			}

	//		}

	//		if (cv == lv)
	//			break;
	//		cv = cv->m_right;
	//	}

	//	// 3. 
	//	for (u32 i = 0; i < cities.m_size; ++i)
	//	{
	//		auto city = cities.m_data[i];
	//		
	//		for (u32 i2 = 0; i2 < city->m_brands.m_size; ++i2)
	//		{
	//			auto brand = city->m_brands.m_data[i2];
	//			
	//			// every brand in this city must have only one model
	//			if (brand->m_models.m_size > 1)
	//			{
	//				for (u32 i3 = 1; i3 < brand->m_models.m_size; ++i3)
	//				{
	//					auto model = brand->m_models.m_data[i3];

	//					// remove this model...
	//					brand->m_brand->m_verts.erase_first(model);
	//					model->m_polygons.erase_first(brand->m_brand);
	//					if (!model->m_polygons.m_head)
	//					{
	//						mesh->_remove_vertex_from_list(model);
	//						model->~miVertex();
	//						m_meshBuilderTmpModelPool->m_allocatorVertex->Deallocate(model);
	//					}
	//				}
	//			}
	//		}
	//		
	//	}

	//	// 4.
	//	_updateModel(true);
	//	for (u32 i = 0; i < cities.m_size; ++i)
	//	{
	//		auto city = cities.m_data[i];
	//		//printf("City %u\n", i);

	//		auto brand1 = city->m_brands.m_data[0];

	//		for (u32 i2 = 1; i2 < city->m_brands.m_size; ++i2)
	//		{
	//			auto brand2 = city->m_brands.m_data[i2];
	//		
	//			//printf("%f\n", brand2->m_models.m_data[0]->m_position.x);
	//			/*miPolygon* p1 = 0;
	//			miPolygon* p2 = 0;
	//			this->VertexTargetWeld(brand2->m_models.m_data[0], brand1->m_models.m_data[0], &p1, &p2);
	//			if (p1) printf("p1!\n");
	//			if (p2) printf("p2!\n");

	//			_updateModel(false);*/

	//			/*printf("Brand %u\n", i2);

	//			for (u32 i3 = 0; i3 < brand2->m_models.m_size; ++i3)
	//			{
	//				printf("Model %u\n", i3);
	//			}*/

	//			/*brand1->m_models.m_data[0]->m_polygons.push_back(brand2->m_brand);
	//				
	//			brand2->m_brand->m_verts.replace(brand2->m_models.m_data[0], brand1->m_models.m_data[0], v2f());

	//			mesh->_remove_vertex_from_list(brand2->m_models.m_data[0]);
	//			brand2->m_models.m_data[0]->~miVertex();
	//			m_meshBuilderTmpModelPool->m_allocatorVertex->Deallocate(brand2->m_models.m_data[0]);*/
	//		}
	//	}


	//	for (u32 i = 0; i < cities.m_size; ++i)
	//	{
	//		delete cities.m_data[i];
	//	}
	//}
	

	/*_destroyMesh();
	m_mesh = miCreate<miMesh>();
	g_app->m_sdk->AppendMesh(m_mesh, mesh);
	this->DestroyTMPModelWithPoolAllocator();*/

	_updateModel(false);
	DeselectAll(g_app->m_editMode);
}