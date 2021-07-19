#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"

extern miApplication * g_app;

s32 g_SelectButtonID_More = 0;
s32 g_SelectButtonID_Less = 1;
const s32 g_SelectButtonID_ConnectVerts1 = 2;
const s32 g_SelectButtonID_BreakVerts = 3;

void editableObjectGUI_selectButtons_onClick(s32 id);
void editableObjectGUI_selectEdgeLoop_onClick(s32 id);
void editableObjectGUI_selectEdgeRing_onClick(s32 id);

void editableObjectGUI_tgweldButton_onClick(s32 id, bool isChecked);
void editableObjectGUI_tgweldButton_onCheck(s32 id);
void editableObjectGUI_tgweldButton_onUncheck(s32 id);
void editableObjectGUI_movetoButton_onClick(s32 id, bool isChecked);
void editableObjectGUI_movetoButton_onCheck(s32 id);
void editableObjectGUI_movetoButton_onUncheck(s32 id);

void editableObjectGUI_weldButton_onClick(s32 id, bool isChecked);
void editableObjectGUI_weldButtonOK_onClick(s32 id);
void editableObjectGUI_weldButton_onCheck(s32 id);
void editableObjectGUI_weldButton_onUncheck(s32 id);
float* editableObjectGUI_weldRange_onSelectObject(miSceneObject* obj);
void editableObjectGUI_weldRange_onValueChanged(miSceneObject*, float*);

void editableObjectGUI_chamferButton_onClick(s32 id, bool isChecked);
void editableObjectGUI_chamferButton_onCheck(s32 id);
void editableObjectGUI_chamferButton_onUncheck(s32 id);
void editableObjectGUI_chamferRange_onValueChanged(miSceneObject* obj, float*);
float* editableObjectGUI_chamferRange_onSelectObject(miSceneObject* obj);
void editableObjectGUI_chamferButtonOK_onClick(s32 id);
void editableObjectGUI_chamferCheckBox_onClick(bool);

void editableObjectGUI_attachButton_onClick(s32 id, bool isChecked) {
	g_app->m_sdk->SetTransformMode(miTransformMode::NoTransform);
}
bool editableObjectGUI_attachButton_onIsGoodObject(miSceneObject* inObject) {
	// selected object must be only 1 object, miEditableObject
	auto selObject = (miEditableObject*)g_app->m_selectedObjects.m_data[0];

	if (inObject->GetPlugin() == selObject->GetPlugin())
	{
		if(inObject->GetTypeForPlugin() == miApplicationPlugin::m_objectType_editableObject)
			return true;
	}
	else
	{
		auto flags = inObject->GetFlags();
		if (flags & miSceneObjectFlag::miSceneObjectFlag_CanConvertToEditableObject)
			return true;
	}
	return false;
}
void editableObjectGUI_attachButton_onSelect(miSceneObject* inObject) {
	//printf("editableObjectGUI_attachButton_onSelect\n");
	auto newObject = g_app->ConvertObjectToEditableObject(inObject);
	if (newObject)
	{
		auto selObject = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
		selObject->AttachObject(newObject);
		g_app->DeleteObject(newObject);
	}
}
void editableObjectGUI_attachButton_onUncheck(s32 id) {
	if (id == 1)
	{
		if (g_app->m_sdk->GetCursorBehaviorMode() == miCursorBehaviorMode::SelectObject)
		{
			g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::CommonMode);
			g_app->m_sdk->SetPickObjectCallbacks(0, 0, 0);
		}
	}
	//printf("on uncheck %i\n", id);
}
void editableObjectGUI_attachButton_onCancel() {
//	printf("editableObjectGUI_attachButton_onCancel\n");

	// it must be only 1 object
	auto selObject = (miEditableObject*)g_app->m_selectedObjects.m_data[0];
	auto gui = selObject->GetGui();
	gui->UncheckButtonGroup(1);
	editableObjectGUI_attachButton_onUncheck(1);
}
void editableObjectGUI_attachButton_onCheck(s32 id) {
	if (id == 1)
	{
		g_app->m_sdk->SetCursorBehaviorMode(miCursorBehaviorMode::SelectObject);
		g_app->m_sdk->SetPickObjectCallbacks(
			editableObjectGUI_attachButton_onIsGoodObject,
			editableObjectGUI_attachButton_onSelect,
			editableObjectGUI_attachButton_onCancel);
	}
}


void editableObjectGUI_editButton_onClick(s32 id) {
	if (g_app->m_selectedObjects.m_size != 1)
		return;

	auto object = g_app->m_selectedObjects.m_data[0];
	if (object->GetPlugin() != g_app->m_pluginForApp)
		return;

	if (object->GetTypeForPlugin() != miApplicationPlugin::m_objectType_editableObject)
		return;

	miEditableObject* editableObject = (miEditableObject*)object;
	switch (id)
	{
	default:
		break;
	case g_SelectButtonID_ConnectVerts1:
		editableObject->VertexConnect();
		break;
	case g_SelectButtonID_BreakVerts:
		editableObject->VertexBreak();
		break;
	}
}

void miApplication::_initEditableObjectGUI() {
	m_pluginGuiForEditableObject = (miPluginGUIImpl*)m_sdk->CreatePluginGUI(miPluginGUIType::ObjectParams);

	m_pluginGuiForEditableObject->AddButtonAsCheckbox(
		v4f(0.f, 0.f, 50.f, 15.f), 
		L"Attach", 
		1, 
		editableObjectGUI_attachButton_onClick, 
		editableObjectGUI_attachButton_onCheck,
		editableObjectGUI_attachButton_onUncheck,
		1,
		0);
	/*m_pluginGuiForEditableObject->AddButtonAsCheckbox(
		v4f(55.f, 0.f, 50.f, 15.f),
		L"Attach 2",
		2,
		editableObjectGUI_attachButton_onClick,
		editableObjectGUI_attachButton_onCheck,
		editableObjectGUI_attachButton_onUncheck,
		1,
		0);*/

	m_pluginGuiForEditableObject->AddText(v2f(0.f, 0.f), L"Select:", 0,
		miPluginGUI::Flag_ForEdgeEditMode |
		miPluginGUI::Flag_ForVertexEditMode |
		miPluginGUI::Flag_ForPolygonEditMode);
	f32 y = 0.f;
	m_pluginGuiForEditableObject->AddButton(v4f(50.f, y, 50.f, 15.f), L"More", g_SelectButtonID_More,
		editableObjectGUI_selectButtons_onClick,
		miPluginGUI::Flag_ForEdgeEditMode |
		miPluginGUI::Flag_ForVertexEditMode |
		miPluginGUI::Flag_ForPolygonEditMode);
	m_pluginGuiForEditableObject->AddButton(v4f(105.f, y, 50.f, 15.f), L"Less", g_SelectButtonID_Less,
		editableObjectGUI_selectButtons_onClick,
		miPluginGUI::Flag_ForEdgeEditMode |
		miPluginGUI::Flag_ForVertexEditMode |
		miPluginGUI::Flag_ForPolygonEditMode);
	y += 15.f + 3.f;

	m_pluginGuiForEditableObject->AddText(v2f(0.f, y), L"Edit:", 0, 
		miPluginGUI::Flag_ForVertexEditMode);
	m_pluginGuiForEditableObject->AddButton(v4f(50.f, y, 50.f, 15.f), L"Connect", g_SelectButtonID_ConnectVerts1,
		editableObjectGUI_editButton_onClick, miPluginGUI::Flag_ForVertexEditMode );
	y += 15.f;
	m_pluginGuiForEditableObject->AddButton(v4f(50.f, y, 50.f, 15.f), L"Break", g_SelectButtonID_BreakVerts,
		editableObjectGUI_editButton_onClick, miPluginGUI::Flag_ForVertexEditMode);

	y += 20.f;
	m_pluginGuiForEditableObject->AddButtonAsCheckbox(v4f(50.f, y, 70.f, 15.f), L"Target weld",
		-1,
		editableObjectGUI_tgweldButton_onClick,
		editableObjectGUI_tgweldButton_onCheck,
		editableObjectGUI_tgweldButton_onUncheck,
		1, 
		miPluginGUI::Flag_ForVertexEditMode);
	m_pluginGuiForEditableObject->AddButtonAsCheckbox(v4f(120.f, y, 50.f, 15.f), L"Move To",
		-1,
		editableObjectGUI_movetoButton_onClick,
		editableObjectGUI_movetoButton_onCheck,
		editableObjectGUI_movetoButton_onUncheck,
		1,
		miPluginGUI::Flag_ForVertexEditMode);

	y += 18.f;
	m_pluginGuiForEditableObject->AddButtonAsCheckbox(v4f(50.f, y, 40.f, 15.f), L"Weld",
		-1,
		editableObjectGUI_weldButton_onClick,
		editableObjectGUI_weldButton_onCheck,
		editableObjectGUI_weldButton_onUncheck,
		1,
		miPluginGUI::Flag_ForVertexEditMode);
	m_pluginGuiForEditableObject->AddRangeSliderFloatNoLimit(v4f(90.f, y, 70.f, 15.f),
		editableObjectGUI_weldRange_onSelectObject,
		editableObjectGUI_weldRange_onValueChanged,
		miPluginGUI::Flag_ForVertexEditMode,
		0.01);
	m_pluginGuiForEditableObject->AddButton(v4f(160.f, y, 40.f, 15.f), L"OK",
		-1,
		editableObjectGUI_weldButtonOK_onClick,
		miPluginGUI::Flag_ForVertexEditMode);
	
	y += 20.f;
	m_pluginGuiForEditableObject->AddButtonAsCheckbox(v4f(50.f, y, 40.f, 15.f), L"Chamfer",
		-1,
		editableObjectGUI_chamferButton_onClick,
		editableObjectGUI_chamferButton_onCheck,
		editableObjectGUI_chamferButton_onUncheck,
		1,
		miPluginGUI::Flag_ForVertexEditMode);
	m_pluginGuiForEditableObject->AddRangeSliderFloatNoLimit(v4f(90.f, y, 70.f, 15.f),
		editableObjectGUI_chamferRange_onSelectObject,
		editableObjectGUI_chamferRange_onValueChanged,
		miPluginGUI::Flag_ForVertexEditMode,
		0.01);
	m_pluginGuiForEditableObject->AddButton(v4f(160.f, y, 40.f, 15.f), L"OK",
		-1,
		editableObjectGUI_chamferButtonOK_onClick,
		miPluginGUI::Flag_ForVertexEditMode);
	y += 17.f;
	m_pluginGuiForEditableObject->AddCheckBox(v2f(50.f, y), L"Chamfer: add polygons",
		editableObjectGUI_chamferCheckBox_onClick,
		true,
		miPluginGUI::Flag_ForVertexEditMode);

	y = 18.f;
	m_pluginGuiForEditableObject->AddButton(v4f(50.f, y, 50.f, 15.f), L"Ring", -1,
		editableObjectGUI_selectEdgeRing_onClick,
		miPluginGUI::Flag_ForEdgeEditMode );
	m_pluginGuiForEditableObject->AddButton(v4f(105.f, y, 50.f, 15.f), L"Loop", -1,
		editableObjectGUI_selectEdgeLoop_onClick,
		miPluginGUI::Flag_ForEdgeEditMode );
	y += 15.f + 3.f;
}

miEditableObject::miEditableObject(miSDK* sdk, miPlugin*) {
	m_addPolygonsWhenChamfer = true;
	m_chamferValue = 0.1f;
	m_isWeld = false;
	m_isChamfer = false;
	m_weldValue = 0.1f;
	m_sdk = sdk;
	m_visualObject_polygon = m_sdk->CreateVisualObject(this, miVisualObjectType::Polygon);
	m_visualObject_vertex = m_sdk->CreateVisualObject(this, miVisualObjectType::Vertex);
	m_visualObject_edge = m_sdk->CreateVisualObject(this, miVisualObjectType::Edge);
	m_flags = 0;
	//m_meshBuilder = 0;

	m_meshBuilderTmpModelPool = 0;
	/*m_meshTmp = 0;
	m_allocatorPoolPolygon = 0;
	m_allocatorPoolEdge = 0;
	m_allocatorPoolVertex = 0;*/

	m_allocatorPolygon = new miDefaultAllocator<miPolygon>(0);
	m_allocatorEdge = new miDefaultAllocator<miEdge>(0);
	m_allocatorVertex = new miDefaultAllocator<miVertex>(0);

	m_mesh = miCreate<miMesh>();
	m_gui = (miPluginGUI*)g_app->m_pluginGuiForEditableObject;
	m_plugin = g_app->m_pluginForApp;
	m_typeForPlugin = miApplicationPlugin::m_objectType_editableObject;
}

miEditableObject::~miEditableObject() {
	if (m_allocatorPolygon)delete m_allocatorPolygon;
	if (m_allocatorEdge)delete m_allocatorEdge;
	if (m_allocatorVertex)delete m_allocatorVertex;
	
	/*if (m_allocatorPoolPolygon)delete m_allocatorPoolPolygon;
	if (m_allocatorPoolEdge)delete m_allocatorPoolEdge;
	if (m_allocatorPoolVertex)delete m_allocatorPoolVertex;*/
	DestroyTMPModelWithPoolAllocator();

	if (m_visualObject_polygon) miDestroy(m_visualObject_polygon);
	if (m_visualObject_vertex) miDestroy(m_visualObject_vertex);
	if (m_visualObject_edge) miDestroy(m_visualObject_edge);
}

void miEditableObject::_createMeshFromTMPMesh() {
	_destroyMesh();
	m_mesh = miCreate<miMesh>();
	g_app->m_sdk->AppendMesh(m_mesh, m_meshBuilderTmpModelPool->m_mesh);
}

void miEditableObject::CreateTMPModelWithPoolAllocator( s32 pc, s32 ec, s32 vc) {
	if (m_meshBuilderTmpModelPool)
		DestroyTMPModelWithPoolAllocator();


	m_meshBuilderTmpModelPool = new
		miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>>
		(pc, ec, vc);
	
		g_app->m_sdk->AppendMesh(m_meshBuilderTmpModelPool, m_mesh);

	RebuildVisualObjects(false);
}
void miEditableObject::DestroyTMPModelWithPoolAllocator() {
	if (!m_meshBuilderTmpModelPool)
		return;
	delete m_meshBuilderTmpModelPool;
	m_meshBuilderTmpModelPool = 0;

	/*if(rebuildVisualObjects)
		RebuildVisualObjects(false);*/
}

void miEditableObject::_destroyMesh() {
	if (!m_mesh) return;

	if (m_mesh->m_first_edge)
	{
		auto e = m_mesh->m_first_edge;
		auto last = e->m_left;
		while (true) {
			auto next = e->m_right;
			e->~miEdge();
			m_allocatorEdge->Deallocate(e);
			if (e == last)
				break;
			e = next;
		}
		m_mesh->m_first_edge = 0;
	}

	if (m_mesh->m_first_polygon)
	{
		auto p = m_mesh->m_first_polygon;
		auto last = p->m_left;
		while (true) {
			auto next = p->m_right;
			p->~miPolygon();
			m_allocatorPolygon->Deallocate(p);
			if (p == last)
				break;
			p = next;
		}
	}


	if (m_mesh->m_first_vertex)
	{
		auto v = m_mesh->m_first_vertex;
		auto last = v->m_left;
		while (true) {
			auto next = v->m_right;
			v->~miVertex();
			m_allocatorVertex->Deallocate(v);
			if (v == last)
				break;
			v = next;
		}
	}
	miDestroy(m_mesh);
	m_mesh = 0;
}

void miEditableObject::OnDraw(miViewportDrawMode dm, miEditMode em, float dt) {
	if (dm == miViewportDrawMode::Material
		|| dm == miViewportDrawMode::MaterialWireframe)
	{

		if (m_visualObject_polygon) m_visualObject_polygon->Draw();
	}

	if (em == miEditMode::Polygon)
	{
		if (m_visualObject_polygon && m_isSelected) m_visualObject_polygon->Draw();
	}

	if ((dm == miViewportDrawMode::Wireframe || dm == miViewportDrawMode::MaterialWireframe)
		|| (m_isSelected && em == miEditMode::Edge)
		|| (m_isSelected && em == miEditMode::Polygon)
		)
	{
		if (m_visualObject_edge) m_visualObject_edge->Draw();
	}

	if (em == miEditMode::Vertex && m_isSelected)
	{
		if (m_visualObject_vertex) m_visualObject_vertex->Draw();
	}
}

void miEditableObject::OnUpdate(float dt) {
}
void miEditableObject::OnCreation(miPluginGUI*) {
}
void miEditableObject::OnCreationLMBDown() {
}
void miEditableObject::OnCreationLMBUp() {
}
void miEditableObject::OnCreationMouseMove() {
}
void miEditableObject::OnCreationEnd() {
}


miPlugin* miEditableObject::GetPlugin() { 
	return m_plugin; 
}

int miEditableObject::GetVisualObjectCount() { 
	return 3;
}

int miEditableObject::GetMeshCount() {
	return 1;
}
miMesh* miEditableObject::GetMesh(int) {
	return m_mesh;
}

miVisualObject* miEditableObject::GetVisualObject(int i){ 
	switch (i)
	{
	default:
	case 0: return m_visualObject_polygon; break;
	case 1: return m_visualObject_edge; break;
	case 2: return m_visualObject_vertex; break;
	}
	return m_visualObject_polygon;
}

void miEditableObject::DeleteSelectedObjects(miEditMode em) {
	yyArray<miPolygon*> polygonsForDelete;
	polygonsForDelete.reserve(0xffff);
	polygonsForDelete.setAddMemoryValue(0xffff);

	miBinarySearchTree<miPolygon*> uniquePolygons;
	switch (em)
	{
	case miEditMode::Vertex:
	{
		auto c = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & c->flag_isSelected)
			{
				auto cp = c->m_polygons.m_head;
				auto lp = cp->m_left;
				while (true)
				{
					uniquePolygons.Add((uint64_t)cp->m_data, cp->m_data);
					if (cp == lp)
						break;
					cp = cp->m_right;
				}
			}
			if (c == l)
				break;
			c = c->m_right;
		}
	}break;
	case miEditMode::Edge:
	{
		auto c = m_mesh->m_first_edge;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & c->flag_isSelected)
			{
				if(c->m_polygon1)
					uniquePolygons.Add((uint64_t)c->m_polygon1, c->m_polygon1);
				if(c->m_polygon2)
					uniquePolygons.Add((uint64_t)c->m_polygon2, c->m_polygon2);
			}
			if (c == l)
				break;
			c = c->m_right;
		}
	}break;
	case miEditMode::Polygon:
	{
		auto c = m_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & c->flag_isSelected)
				uniquePolygons.Add((uint64_t)c, c);
			if (c == l)
				break;
			c = c->m_right;
		}
	}break;
	case miEditMode::Object:
	default:
		break;
	}

	uniquePolygons.Get(&polygonsForDelete);

	yyLogWriteInfo("Delete [%u] polygons\n", polygonsForDelete.size());
	for (u32 i = 0, sz = polygonsForDelete.size(); i < sz; ++i)
	{
		DeletePolygon(polygonsForDelete[i]);
	}
	this->_updateModel(true, true);

	auto voc = GetVisualObjectCount();
	for (int i = 0; i < voc; ++i)
	{
		GetVisualObject(i)->CreateNewGPUModels(m_mesh);
	}
}

void miEditableObject::DeletePolygon(miPolygon* _polygon) {
	static yyArraySimple<miVertex*> vertsForDelete;
	//static yyArraySimple<miEdge*> edgesForDelete;
	vertsForDelete.clear();
	//edgesForDelete.clear();
	
	auto mesh = m_meshBuilderTmpModelPool ? m_meshBuilderTmpModelPool->m_mesh : m_mesh;

	// check all verts of this polygons
	{
		auto c = _polygon->m_verts.m_head;
		auto l = c->m_left;
		while (true)
		{
			// remove polygon from list
			c->m_data1->m_polygons.erase_first(_polygon);
			// if there was last polygon, then add this vertex in to array
			if (!c->m_data1->m_polygons.m_head)
				vertsForDelete.push_back(c->m_data1);

			if (c == l)
				break;
			c = c->m_right;
		}
	}

	// check edges
	//{
	//	auto c = _polygon->m_edges.m_head;
	//	auto l = c->m_left;
	//	while (true)
	//	{
	//		if (c->m_data->m_polygon1 == _polygon)
	//			c->m_data->m_polygon1 = 0;
	//		if (c->m_data->m_polygon2 == _polygon)
	//			c->m_data->m_polygon2 = 0;

	//		if (c->m_data->m_polygon1 == 0 && c->m_data->m_polygon2 == 0)
	//		{
	//			edgesForDelete.push_back(c->m_data);
	//			// remove this edge from lists in verts
	//			c->m_data->m_vertex1->m_edges.erase_first(c->m_data);
	//			c->m_data->m_vertex2->m_edges.erase_first(c->m_data);
	//		}

	//		if (c == l)
	//			break;
	//		c = c->m_right;
	//	}
	//}

	{
		auto l = _polygon->m_left;
		auto r = _polygon->m_right;
		l->m_right = r;
		r->m_left = l;

		if (mesh->m_first_polygon == _polygon)
			mesh->m_first_polygon = r;
		if (mesh->m_first_polygon == _polygon)
			mesh->m_first_polygon = 0;
	}

	//printf("Verts for delete %u\n", vertsForDelete.m_size);
	//printf("Edges for delete %u\n", edgesForDelete.m_size);

	for (u32 i = 0; i < vertsForDelete.m_size; ++i)
	{
		auto c = vertsForDelete.m_data[i];

		auto l = c->m_left;
		auto r = c->m_right;
		l->m_right = r;
		r->m_left = l;

		if (mesh->m_first_vertex == c)
			mesh->m_first_vertex = r;
		if (mesh->m_first_vertex == c)
			mesh->m_first_vertex = 0;

		c->~miVertex();

		if (m_meshBuilderTmpModelPool)
			m_meshBuilderTmpModelPool->m_allocatorVertex->Deallocate(c);
		else
			m_allocatorVertex->Deallocate(c);
	}

	/*for (u32 i = 0; i < edgesForDelete.m_size; ++i)
	{
		auto c = edgesForDelete.m_data[i];

		auto l = c->m_left;
		auto r = c->m_right;
		l->m_right = r;
		r->m_left = l;

		if (mesh->m_first_edge == c)
			mesh->m_first_edge = r;
		if (mesh->m_first_edge == c)
			mesh->m_first_edge = 0;

		c->~miEdge();

		if (m_meshBuilderTmpModelPool)
			m_meshBuilderTmpModelPool->m_allocatorEdge->Deallocate(c);
		else
			m_allocatorEdge->Deallocate(c);
	}*/

	_polygon->~miPolygon();

	if (m_meshBuilderTmpModelPool)
		m_meshBuilderTmpModelPool->m_allocatorPolygon->Deallocate(_polygon);
	else
		m_allocatorPolygon->Deallocate(_polygon);
}

void miEditableObject::OnConvertToEditableObject() {

}

bool miEditableObject::IsEdgeMouseHover(miSelectionFrust* sf) {
	auto current_edge = m_mesh->m_first_edge;
	if (!current_edge)
		return false;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto last_edge = current_edge->m_left;

	while (true) {
		if (sf->LineInFrust(
			math::mul(current_edge->m_vertex1->m_position, M) + *position,
			math::mul(current_edge->m_vertex2->m_position, M) + *position))
			return true;

		if (current_edge == last_edge)
			break;
		current_edge = current_edge->m_right;
	}
	return false;
}

miVertex* miEditableObject::IsVertexMouseHover(miSelectionFrust* sf) {
	auto current_vertex = m_mesh->m_first_vertex;
	if (!current_vertex)
		return 0;

	Mat4 M = this->GetWorldMatrix()->getBasis();
	auto position = this->GetGlobalPosition();

	auto last_vertex = current_vertex->m_left;

	while (true) {
		if (sf->PointInFrust(math::mul(current_vertex->m_position, M) + *position))
			return current_vertex;

		if (current_vertex == last_vertex)
			break;
		current_vertex = current_vertex->m_right;
	}
	return 0;
}

void miEditableObject::_transformMove(const v3f& move_delta, bool isCancel) {
	for (u32 i = 0; i < m_vertsForTransform.m_size; ++i)
	{
		if (isCancel)
			m_vertsForTransform.m_data[i].m_first->m_position = m_vertsForTransform.m_data[i].m_second;
		else
			m_vertsForTransform.m_data[i].m_first->m_position += move_delta;
	}
}

void miEditableObject::_transformScale(Mat4* S, const v3f& selectionCenter, bool isCancel) {
	auto M = *S;
	Mat4 W = m_rotationOnlyMatrix;

	auto WT = W;
	WT.transpose();

	auto WI = W;
	WI.invert();

	M = M * W;
	M = WT * M;

	auto C = selectionCenter;
	auto W2 = m_worldMatrix;
	W2.invert();
	C = math::mul(C, W2);

	for (u32 i = 0; i < m_vertsForTransform.m_size; ++i)
	{
		if (isCancel)
			m_vertsForTransform.m_data[i].m_first->m_position = m_vertsForTransform.m_data[i].m_second;
		else
			m_vertsForTransform.m_data[i].m_first->m_position = math::mulBasis(m_vertsForTransform.m_data[i].m_second - C, M) + C;
	}
}

void miEditableObject::_transformRotate(Mat4* R, const v3f& selectionCenter, bool isCancel) {
	_transformScale(R, selectionCenter, isCancel);
}

void miEditableObject::OnTransformEdge(
	miTransformMode tm, 
	const v3f& move_delta, 
	Mat4* S,
	const v3f& C,
	bool isCancel) 
{
	switch (tm)
	{
	case miTransformMode::Move:
		_transformMove(move_delta, isCancel);
		break;
	case miTransformMode::Scale:
		_transformScale(S, C, isCancel);
		break;
	case miTransformMode::Rotate:
		_transformRotate(S, C, isCancel);
		break;
	case miTransformMode::NoTransform:
	default:
		break;
	}
	CallVisualObjectOnTransform();
}

void miEditableObject::OnTransformVertex(
	miTransformMode tm, 
	const v3f& move_delta, 
	Mat4* S,
	const v3f& C,
	bool isCancel) 
{
	switch (tm)
	{
	case miTransformMode::Move:
		_transformMove(move_delta, isCancel);
	break;
	case miTransformMode::Scale:
		_transformScale(S, C, isCancel);
	break;
	case miTransformMode::Rotate:
		_transformRotate(S, C, isCancel);
		break;
	case miTransformMode::NoTransform:
	default:
		break;
	}
	CallVisualObjectOnTransform();
}

//void miEditableObject::CallVisualObjectOnTransform() {
//	auto voc = GetVisualObjectCount();
//	for (int i = 0; i < voc; ++i)
//	{
//		auto vo = GetVisualObject(i);
//		vo->OnTransform();
//	}
//}
//
//void miEditableObject::CallVisualObjectOnSelect() {
//	switch (g_app->m_editMode)
//	{
//	case miEditMode::Vertex:
//	case miEditMode::Edge:
//	case miEditMode::Polygon:
//			auto voc = GetVisualObjectCount();
//			for (int o = 0; o < voc; ++o)
//			{
//				GetVisualObject(o)->OnSelect(g_app->m_editMode);
//			}
//		break;
//	case miEditMode::Object:
//	default:
//		break;
//	}
//}

void miEditableObject::OnTransformPolygon(
	miTransformMode tm, 
	const v3f& move_delta, 
	Mat4* S,
	const v3f& C,
	bool isCancel) 
{
	switch (tm)
	{
	case miTransformMode::Move:
		_transformMove(move_delta, isCancel);
		break;
	case miTransformMode::Scale:
		_transformScale(S, C, isCancel);
		break;
	case miTransformMode::Rotate:
		_transformRotate(S, C, isCancel);
		break;
	case miTransformMode::NoTransform:
	default:
		break;
	}
	CallVisualObjectOnTransform();
}

void miEditableObject::RebuildVisualObjects(bool onlyEditMode) {
	auto mesh = m_meshBuilderTmpModelPool ? m_meshBuilderTmpModelPool->m_mesh : m_mesh;
	if (onlyEditMode)
	{
		auto em = g_app->GetEditMode();
		switch (em)
		{
		default:
		case miEditMode::Object:
			break;
		case miEditMode::Vertex:
			m_visualObject_vertex->CreateNewGPUModels(mesh);
			break;
		case miEditMode::Edge:
			m_visualObject_edge->CreateNewGPUModels(mesh);
			break;
		case miEditMode::Polygon:
			m_visualObject_polygon->CreateNewGPUModels(mesh);
			break;
		}
	}
	else
	{
		auto voc = GetVisualObjectCount();
		for (int i = 0; i < voc; ++i)
		{
			GetVisualObject(i)->CreateNewGPUModels(mesh);
		}
	}
}

void miEditableObject::OnSetEditMode(miEditMode em) {
	_updateVertsForTransformArray(em);
}

void miEditableObject::OnEndTransform(miEditMode em) {
	_updateVertsForTransformArray(em);
}

void miEditableObject::_updateVertsForTransformArray(miEditMode em) {
	m_vertsForTransform.clear();
	switch (em)
	{
	case miEditMode::Vertex:
	{
		auto c = m_mesh->m_first_vertex;
		if (!c)
			return;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & c->flag_isSelected)
				m_vertsForTransform.push_back(miPair<miVertex*, v3f>(c, c->m_position));

			if (c == l)
				break;
			c = c->m_right;
		}
	}break;
	case miEditMode::Edge:
	{
		auto c = m_mesh->m_first_edge;
		if (!c)
			return;
		miBinarySearchTree<miPair<miVertex*, v3f>> bst;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miEdge::flag_isSelected)
			{
				bst.Add((uint64_t)c->m_vertex1, miPair<miVertex*, v3f>(c->m_vertex1, c->m_vertex1->m_position));
				bst.Add((uint64_t)c->m_vertex2, miPair<miVertex*, v3f>(c->m_vertex2, c->m_vertex2->m_position));
			}

			if (c == l)
				break;
			c = c->m_right;
		}
		bst.Get(&m_vertsForTransform);

	}break;
	case miEditMode::Polygon:
	{
		auto c = m_mesh->m_first_polygon;
		if (!c)
			return;
		miBinarySearchTree<miPair<miVertex*, v3f>> bst;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miPolygon::flag_isSelected)
			{
				auto cv = c->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					bst.Add((uint64_t)cv->m_data1, miPair<miVertex*, v3f>(cv->m_data1, cv->m_data1->m_position));

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}

			if (c == l)
				break;
			c = c->m_right;
		}
		bst.Get(&m_vertsForTransform);
	}break;
	case miEditMode::Object:
	default:
		break;
	}
	//printf("VERTS FOR TRANSFORM: %u\n", m_vertsForTransform.m_size);
}

void miEditableObject::VertexConnect() {
	miArray<miListNode2<miVertex*, v2f>*> vertsForNewpolygon;

	/*for (s32 o = 0, osz = GetMeshCount(); o < osz; ++o)
	{
		auto mesh = GetMesh(o);*/
		auto mesh = m_mesh;
		auto cp = mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto np = cp->m_right;
			vertsForNewpolygon.clear();

			u32 vertexCount = 0;
			{// remove miVertex::flag_User1
				auto currentVertex = cp->m_verts.m_head;
				auto lastVertex = currentVertex->m_left;
				while (true) {
					if (currentVertex->m_data1->m_flags & miVertex::flag_User1)
						currentVertex->m_data1->m_flags ^= miVertex::flag_User1;
					++vertexCount;
					if (currentVertex == lastVertex)
						break;
					currentVertex = currentVertex->m_right;
				}
			}

			miPolygon* newPolygon = 0;
			bool isStarted = false;
			auto currentVertex = cp->m_verts.m_head;
			auto lastVertex = currentVertex->m_left;
			while (true) {
				if (currentVertex->m_data1->m_flags & miVertex::flag_isSelected)
				{
					if (!isStarted) {
						isStarted = true;
						lastVertex = currentVertex->m_left;
					}
				}

				if (isStarted)
				{
					if ((currentVertex->m_data1->m_flags & miVertex::flag_User1) == 0)
					{
						vertsForNewpolygon.push_back(currentVertex);
					}

					if (currentVertex->m_data1->m_flags & miVertex::flag_isSelected)
					{
						if (vertsForNewpolygon.m_size > 2)
						{
							newPolygon = m_allocatorPolygon->Allocate();

							for (u32 i = 0, l = vertsForNewpolygon.m_size - 1; i < vertsForNewpolygon.m_size; ++i)
							{
								auto v = vertsForNewpolygon.m_data[i];

								if (i != 0 && i != l) {
									v->m_data1->m_flags |= miVertex::flag_User1;
									--vertexCount;
								}
								
								newPolygon->m_verts.push_back(v->m_data1, v->m_data2);
								v->m_data1->m_polygons.push_back(newPolygon);

								if (v == lastVertex)
									lastVertex = vertsForNewpolygon.m_data[0];
							}

							cp->m_left->m_right = newPolygon;
							newPolygon->m_left = cp->m_left;
							cp->m_left = newPolygon;
							newPolygon->m_right = cp;

							vertsForNewpolygon.clear();
							vertsForNewpolygon.push_back(currentVertex);

							if (vertexCount == 2)
								break;
						}
					}
				}

				if (currentVertex->m_right == lastVertex)
				{
					if (!isStarted)
						break;
					else if (isStarted && !newPolygon)
						break;
				}

				currentVertex = currentVertex->m_right;
			}

			if (newPolygon)
			{
				// need to delete old polygon
				DeletePolygon(cp);

				/*mesh->_delete_edges(m_allocatorEdge);
				mesh->CreateEdges(m_allocatorPolygon, m_allocatorEdge, m_allocatorVertex);*/
			}

			if (cp == lp)
				break;
			cp = np;
		}
	//}

	mesh->_delete_edges(m_allocatorEdge);
	mesh->CreateEdges(m_allocatorPolygon, m_allocatorEdge, m_allocatorVertex);

	RebuildVisualObjects(false);
	UpdateCounts();
}

void miEditableObject::VertexBreak() {
	miArray<miVertex*> verts;
	verts.reserve(0x1000);
	{
		auto c = m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			if (c->m_flags & miVertex::flag_isSelected)
			{
				auto p1 = c->m_polygons.m_head;
				auto p2 = c->m_polygons.m_head->m_left;
				if (p1 != p2)
					verts.push_back(c);
			}
			if (c == l)
				break;
			c = c->m_right;
		}
	}
	if (!verts.m_size)
		return;

	miArray<miPolygon*> removeThisPolygons;
	for (u32 i = 0; i < verts.m_size; ++i)
	{
		removeThisPolygons.clear();

		auto v = verts.m_data[i];
		auto cp = v->m_polygons.m_head;
		auto lp = cp->m_left;
		auto fp = cp;
		while (true)
		{
			if (cp != fp)
			{
				miVertex* newVertex = m_allocatorVertex->Allocate();
				newVertex->CopyData(v);
				newVertex->m_left = v;
				newVertex->m_right = v->m_right;
				v->m_right->m_left = newVertex;
				v->m_right = newVertex;

				//auto vNode = v->m_polygons.m_head->m_data->FindVertex(v);
				auto vNode = cp->m_data->FindVertex(v);

				cp->m_data->m_verts.replace(v, newVertex, vNode->m_data2);
				newVertex->m_polygons.push_back(cp->m_data);

				removeThisPolygons.push_back(cp->m_data);
			}

			if (cp == lp)
				break;
			cp = cp->m_right;
		}

		for (u32 o = 0; o < removeThisPolygons.m_size; ++o)
		{
			v->m_polygons.erase_first(removeThisPolygons.m_data[o]);
		}
	}

	m_mesh->_delete_edges(m_allocatorEdge);
	m_mesh->CreateEdges(m_allocatorPolygon, m_allocatorEdge, m_allocatorVertex);
	_updateVertsForTransformArray(miEditMode::Vertex);
	RebuildVisualObjects(false);
	UpdateCounts();
	g_app->_callVisualObjectOnSelect();
}

void miEditableObject::AttachObject(miEditableObject* otherObject) {
	{
		auto c = otherObject->m_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			auto n = c->m_right;
			m_mesh->_add_polygon_to_list(c);

			if (c == l)
				break;
			c = n;
		}
	}
	{
		auto c = otherObject->m_mesh->m_first_edge;
		auto l = c->m_left;
		while (true)
		{
			auto n = c->m_right;
			m_mesh->_add_edge_to_list(c);

			if (c == l)
				break;
			c = n;
		}
	}
	{
		auto TIM = otherObject->m_rotationScaleMatrix;
		TIM.invert();
		TIM.transpose();

		auto IM = m_rotationScaleMatrix;
		IM.invert();

		auto c = otherObject->m_mesh->m_first_vertex;
		auto l = c->m_left;
		while (true)
		{
			auto n = c->m_right;
			m_mesh->_add_vertex_to_list(c);

			v3f norm(c->m_normal[0], c->m_normal[1], c->m_normal[2]);
			norm = math::mul(norm, TIM);
			c->m_normal[0] = norm.x;
			c->m_normal[1] = norm.y;
			c->m_normal[2] = norm.z;

			c->m_position = math::mulBasis(c->m_position, otherObject->m_rotationScaleMatrix)
				+ otherObject->m_globalPosition - m_globalPosition;

			c->m_position = math::mul(c->m_position, IM);

			if (c == l)
				break;
			c = n;
		}
	}

	otherObject->m_mesh->m_first_polygon = 0;
	otherObject->m_mesh->m_first_edge = 0;
	otherObject->m_mesh->m_first_vertex = 0;
	
	auto voc = GetVisualObjectCount();
	for (int i = 0; i < voc; ++i)
	{
		auto vo = GetVisualObject(i);
		vo->UpdateAabb();
	}
	RebuildVisualObjects(false);
	UpdateAabb();

	g_app->_updateObjectsOnSceneArray();
	g_app->_transformObjectsApply();
	g_app->UpdateSceneAabb();
	g_app->UpdateSelectionAabb();
	g_app->_updateIsVertexEdgePolygonSelected();

	UpdateCounts();
}

void miEditableObject::UpdateCounts() {
	m_vertexCount = 0;
	m_edgeCount = 0;
	m_polygonCount = 0;
	{
		auto c = m_mesh->m_first_vertex;
		if (c)
		{
			auto l = c->m_left;
			while (true)
			{
				++m_vertexCount;
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}
	{
		auto c = m_mesh->m_first_edge;
		if (c)
		{
			auto l = c->m_left;
			while (true)
			{
				++m_edgeCount;
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}
	{
		auto c = m_mesh->m_first_polygon;
		if (c)
		{
			auto l = c->m_left;
			while (true)
			{
				++m_polygonCount;
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}
	//printf("V: %u E: %u P: %u\n", m_vertexCount, m_edgeCount, m_polygonCount);
}

u32 miEditableObject::GetVertexCount() {
	return m_vertexCount;
}

u32 miEditableObject::GetEdgeCount() {
	return m_edgeCount;
}

u32 miEditableObject::GetPolygonCount() {
	return m_polygonCount;
}

void miEditableObject::_createMeshFromTMPMesh_meshBuilder(bool saveSelection, bool weldSelected) {
	_destroyMesh();

	miSDKImporterHelper importeHelper;

	importeHelper.m_meshBuilder->Begin();

	auto pool_mesh = m_meshBuilderTmpModelPool->m_mesh;
	{
		auto c = pool_mesh->m_first_polygon;
		auto l = c->m_left;
		while (true)
		{
			importeHelper.m_polygonCreator.Clear();

			auto cv = c->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				bool select = false;
				if (saveSelection)
				{
					if (cv->m_data1->m_flags & miVertex::flag_isSelected)
						select = true;
				}

				bool weld = true;
				if (weldSelected)
				{
					weld = false;
					if (cv->m_data1->m_flags & miVertex::flag_isSelected)
						weld = true;
				}

				importeHelper.m_polygonCreator.Add(
					cv->m_data1->m_position,
					weld,
					select,
					v3f(cv->m_data1->m_normal[0], cv->m_data1->m_normal[1], cv->m_data1->m_normal[2]),
					cv->m_data2);

				if (cv == lv)
					break;
				cv = cv->m_right;
			}

			importeHelper.m_meshBuilder->AddPolygon(&importeHelper.m_polygonCreator, false, false);

			if (c == l)
				break;
			c = c->m_right;
		}

		importeHelper.m_meshBuilder->End();
	}
	m_mesh = importeHelper.m_meshBuilder->m_mesh;
	importeHelper.m_meshBuilder->m_mesh = 0;
}