#include "miSDK.h"

#include "miplStd.h"
#include "miplPolygonObjectPlane.h"

const wchar_t* miplStd_plane_text_onSelectObject(miSceneObject* object) {
	return object->GetName().data();
}

int* miplStd_plane_sliderXSegments_onSelectObject(miSceneObject* object) {
	miplPolygonObjectPlane* o = (miplPolygonObjectPlane*)object;
	return &o->m_x_segments;
}
int* miplStd_plane_sliderYSegments_onSelectObject(miSceneObject* object) {
	miplPolygonObjectPlane* o = (miplPolygonObjectPlane*)object;
	return &o->m_y_segments;
}

void miplStd_plane_sliderXSegments_onValueChanged(miSceneObject* object, int* x) {
	miplPolygonObjectPlane* o = (miplPolygonObjectPlane*)object;
	o->m_x_segments = *x;
	o->_generate();
}
void miplStd_plane_sliderYSegments_onValueChanged(miSceneObject* object, int* y) {
	miplPolygonObjectPlane* o = (miplPolygonObjectPlane*)object;
	o->m_y_segments = *y;
	o->_generate();
}

void miplStd_plane_testbuttons_onClick(int id) {
	printf("Test button %i\n", id);
}
void miplStd_initGuiForPlane(miPluginGUI* gui) {
	float X = 0.f;
	float Y = 0.f;
	gui->AddText(v2f(X,Y), L"X segments: ", 0, 0);

	X = 60.f;
	gui->AddRangeSliderInt(v4f(X, Y, 100.f, 15.f), 1, 100, miplStd_plane_sliderXSegments_onSelectObject, miplStd_plane_sliderXSegments_onValueChanged, 0);
	
	X = 0.f;
	Y = 15.f;
	gui->AddText(v2f(X, Y), L"Y segments: ", 0, 0);

	X = 60.f;
	gui->AddRangeSliderInt(v4f(X, Y, 100.f, 15.f), 1, 100, miplStd_plane_sliderYSegments_onSelectObject, miplStd_plane_sliderYSegments_onValueChanged, 0);

	gui->AddText(v2f(0, 50), L"Object edit: ", 0, 0);
	gui->AddButton(v4f(70.f, 50.f, 100.f, 20.f), L"Button 1", 0, miplStd_plane_testbuttons_onClick, 0);
	
	gui->AddText(v2f(0, 50), L"Vertex edit: ", 0, miPluginGUI::Flag_ForVertexEditMode);
	gui->AddButton(v4f(70.f, 50.f, 100.f, 20.f), L"Button 2", 1, miplStd_plane_testbuttons_onClick, miPluginGUI::Flag_ForVertexEditMode);
	
	gui->AddText(v2f(0, 50), L"Edge edit: ", 0, miPluginGUI::Flag_ForEdgeEditMode);
	gui->AddButton(v4f(70.f, 50.f, 100.f, 20.f), L"Button 3", 2, miplStd_plane_testbuttons_onClick, miPluginGUI::Flag_ForEdgeEditMode);
	
	gui->AddText(v2f(0, 50), L"Polygon edit: ", 0, miPluginGUI::Flag_ForPolygonEditMode);
	gui->AddButton(v4f(70.f, 50.f, 100.f, 20.f), L"Button 4", 3, miplStd_plane_testbuttons_onClick, miPluginGUI::Flag_ForPolygonEditMode);
}

void miplPolygonObjectPlane_generate(
	miPolygonCreator* pc, 
	miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>> * mb,
	miViewportCameraType ct,
	const Aabb& aabb, 
	const v3f& firstPoint,
	v2f& size,
	unsigned int x_segments, 
	unsigned int y_segments)
{
	float uv_segment_size_h = 1.f / (float)y_segments;
	float uv_segment_size_w = 1.f / (float)x_segments;

	pc->Clear();

	float begin_x = 0.f;
	float begin_z = 0.f;

	float uv_begin_x = 0.f;
	float uv_begin_y = 1.f;

	float segment_size_h = 0.f;
	float segment_size_w = 0.f;

	switch (ct)
	{
	case miViewportCameraType::Perspective:
	case miViewportCameraType::Top:
		begin_x = aabb.m_min.x;
		begin_z = aabb.m_min.z;
		size.x = aabb.m_max.x - aabb.m_min.x;
		size.y = aabb.m_max.z - aabb.m_min.z;
		uv_begin_x = 0.f;
		uv_begin_y = 0.f;
		break;
	case miViewportCameraType::Bottom:
		begin_x = aabb.m_min.x;
		begin_z = aabb.m_min.z;
		size.x = aabb.m_max.x - aabb.m_min.x;
		size.y = aabb.m_max.z - aabb.m_min.z;
		uv_begin_x = 0.f;
		uv_begin_y = 1.f;
		break;
	case miViewportCameraType::Left:
		begin_x = aabb.m_min.y;
		begin_z = aabb.m_min.z;
		size.x = aabb.m_max.y - aabb.m_min.y;
		size.y = aabb.m_max.z - aabb.m_min.z;
		uv_begin_x = 1.f;
		uv_begin_y = 1.f;
		break;
	case miViewportCameraType::Right:
		begin_x = aabb.m_max.y;
		begin_z = aabb.m_min.z;
		size.x = aabb.m_max.y - aabb.m_min.y;
		size.y = aabb.m_max.z - aabb.m_min.z;
		uv_begin_x = 0.f;
		uv_begin_y = 0.f;
		break;
	case miViewportCameraType::Front:
		begin_x = aabb.m_min.x;
		begin_z = aabb.m_min.y;
		size.x = aabb.m_max.x - aabb.m_min.x;
		size.y = aabb.m_max.y - aabb.m_min.y;
		uv_begin_x = 0.f;
		uv_begin_y = 1.f;
		break;
	case miViewportCameraType::Back:
		begin_x = aabb.m_min.y;
		begin_z = aabb.m_min.x;
		size.x = aabb.m_max.y - aabb.m_min.y;
		size.y = aabb.m_max.x - aabb.m_min.x;
		uv_begin_x = 1.f;
		uv_begin_y = 1.f;
		break;
	}

	v4f aabbcenter;
	aabb.center(aabbcenter);

//	mb->m_position = aabbcenter;

	segment_size_h = size.y / (float)y_segments;
	segment_size_w = size.x / (float)x_segments;

	for (unsigned int h_i = 0; h_i < y_segments; ++h_i)
	{
		for (unsigned int w_i = 0; w_i < x_segments; ++w_i)
		{
			switch (ct)
			{
			case miViewportCameraType::Perspective:
			case miViewportCameraType::Top: {
				pc->Add(
					v3f(begin_x - aabbcenter.x, firstPoint.y, begin_z - aabbcenter.z),
					true,
					false,
					v3f(0.f, 1.f, 0.f),
					v2f(uv_begin_x, uv_begin_y));

				pc->Add(
					v3f(begin_x - aabbcenter.x, firstPoint.y, begin_z + segment_size_h - aabbcenter.z),
					true,
					false,
					v3f(0.f, 1.f, 0.f),
					v2f(uv_begin_x, uv_begin_y + uv_segment_size_h));

				pc->Add(
					v3f(begin_x + segment_size_w - aabbcenter.x, firstPoint.y, begin_z + segment_size_h - aabbcenter.z),
					true,
					false,
					v3f(0.f, 1.f, 0.f),
					v2f(uv_begin_x + uv_segment_size_w, uv_begin_y + uv_segment_size_h));

				pc->Add(
					v3f(begin_x + segment_size_w - aabbcenter.x, firstPoint.y, begin_z - aabbcenter.z),
					true,
					false,
					v3f(0.f, 1.f, 0.f),
					v2f(uv_begin_x + uv_segment_size_w, uv_begin_y));

				mb->AddPolygon(pc, false);
				pc->Clear();

				begin_x += segment_size_w;
				uv_begin_x += uv_segment_size_w;

				if (begin_x > size.x + aabb.m_max.x)
				{
					begin_x = aabb.m_min.x;
					uv_begin_x = 0;
				}
			}break;
			case miViewportCameraType::Bottom: {
				pc->Add(
					v3f(begin_x + segment_size_w - aabbcenter.x, firstPoint.y, begin_z - aabbcenter.z),
					true,
					false,
					v3f(0.f, -1.f, 0.f),
					v2f(uv_begin_x + uv_segment_size_w, uv_begin_y));

				pc->Add(
					v3f(begin_x + segment_size_w - aabbcenter.x, firstPoint.y, begin_z + segment_size_h - aabbcenter.z),
					true,
					false,
					v3f(0.f, -1.f, 0.f),
					v2f(uv_begin_x + uv_segment_size_w, uv_begin_y - uv_segment_size_h));
				
				pc->Add(
					v3f(begin_x - aabbcenter.x, firstPoint.y, begin_z + segment_size_h - aabbcenter.z),
					true,
					false,
					v3f(0.f, -1.f, 0.f),
					v2f(uv_begin_x, uv_begin_y - uv_segment_size_h));

				pc->Add(
					v3f(begin_x - aabbcenter.x, firstPoint.y, begin_z - aabbcenter.z),
					true,
					false,
					v3f(0.f, -1.f, 0.f),
					v2f(uv_begin_x, uv_begin_y));
				
				mb->AddPolygon(pc, false);
				pc->Clear();

				begin_x += segment_size_w;
				uv_begin_x += uv_segment_size_w;

				if (begin_x > size.x + aabb.m_max.x)
				{
					begin_x = aabb.m_min.x;
					uv_begin_x = 0.f;
				}
			}break;
			case miViewportCameraType::Left: {
				pc->Add(
					v3f(firstPoint.x, begin_x - aabbcenter.y, begin_z + segment_size_h - aabbcenter.z),
					true,
					false,
					v3f(1.f, 0.f, 0.f),
					v2f(uv_begin_y - uv_segment_size_h, uv_begin_x));
				
				pc->Add(
					v3f(firstPoint.x, begin_x - aabbcenter.y, begin_z - aabbcenter.z),
					true,
					false,
					v3f(1.f, 0.f, 0.f),
					v2f(uv_begin_y, uv_begin_x));

				pc->Add(
					v3f(firstPoint.x, begin_x + segment_size_w - aabbcenter.y, begin_z - aabbcenter.z),
					true,
					false,
					v3f(1.f, 0.f, 0.f),
					v2f(uv_begin_y, uv_begin_x - uv_segment_size_w));

				pc->Add(
					v3f(firstPoint.x, begin_x + segment_size_w - aabbcenter.y, begin_z + segment_size_h - aabbcenter.z),
					true,
					false,
					v3f(1.f, 0.f, 0.f),
					v2f(uv_begin_y - uv_segment_size_h, uv_begin_x - uv_segment_size_w));				

				mb->AddPolygon(pc, false);
				pc->Clear();

				begin_x += segment_size_w;
				uv_begin_x -= uv_segment_size_w;

				if (begin_x > size.x + aabb.m_max.x)
				{
					begin_x = aabb.m_min.x;
					uv_begin_x = 1;
				}
			}break;
			case miViewportCameraType::Right: {
				pc->Add(
					v3f(firstPoint.x, begin_x - aabbcenter.y, begin_z + segment_size_h - aabbcenter.z),
					true,
					false,
					v3f(-1.f, 0.f, 0.f),
					v2f(uv_begin_y + uv_segment_size_h, uv_begin_x));

				pc->Add(
					v3f(firstPoint.x, begin_x - aabbcenter.y, begin_z - aabbcenter.z),
					true,
					false,
					v3f(-1.f, 0.f, 0.f),
					v2f(uv_begin_y, uv_begin_x));

				pc->Add(
					v3f(firstPoint.x, begin_x - segment_size_w - aabbcenter.y, begin_z - aabbcenter.z),
					true,
					false,
					v3f(-1.f, 0.f, 0.f),
					v2f(uv_begin_y, uv_begin_x + uv_segment_size_w));

				pc->Add(
					v3f(firstPoint.x, begin_x - segment_size_w - aabbcenter.y, begin_z + segment_size_h - aabbcenter.z),
					true,
					false,
					v3f(-1.f, 0.f, 0.f),
					v2f(uv_begin_y + uv_segment_size_h, uv_begin_x + uv_segment_size_w));

				mb->AddPolygon(pc, false);
				pc->Clear();

				begin_x -= segment_size_w;
				uv_begin_x += uv_segment_size_w;

				if (begin_x <= aabb.m_min.y)
				{
					begin_x = aabb.m_max.y;
					uv_begin_x = 0;
				}
			}break;
			case miViewportCameraType::Front: {
				pc->Add(
					v3f(begin_x - aabbcenter.x, begin_z + segment_size_h - aabbcenter.y, firstPoint.z),
					true,
					false,
					v3f(0.f, 0.f, 1.f),
					v2f(uv_begin_x, uv_begin_y - uv_segment_size_h));

				pc->Add(
					v3f(begin_x - aabbcenter.x, begin_z - aabbcenter.y, firstPoint.z),
					true,
					false,
					v3f(0.f, 0.f, 1.f),
					v2f(uv_begin_x, uv_begin_y));

				pc->Add(
					v3f(begin_x + segment_size_w - aabbcenter.x, begin_z - aabbcenter.y, firstPoint.z),
					true,
					false,
					v3f(0.f, 0.f, 1.f),
					v2f(uv_begin_x + uv_segment_size_w, uv_begin_y));

				pc->Add(
					v3f(begin_x + segment_size_w - aabbcenter.x, begin_z + segment_size_h - aabbcenter.y, firstPoint.z),
					true,
					false,
					v3f(0.f, 0.f, 1.f),
					v2f(uv_begin_x + uv_segment_size_w, uv_begin_y - uv_segment_size_h));

				mb->AddPolygon(pc, false);
				pc->Clear();

				begin_x += segment_size_w;
				uv_begin_x += uv_segment_size_w;

				if (begin_x > size.x + aabb.m_max.x)
				{
					begin_x = aabb.m_min.x;
					uv_begin_x = 0;
				}
			}break;
			case miViewportCameraType::Back: {
				pc->Add(
					v3f(begin_z + segment_size_h - aabbcenter.x, begin_x - aabbcenter.y, firstPoint.z),
					true,
					false,
					v3f(0.f, 0.f, -1.f),
					v2f(uv_begin_y - uv_segment_size_h, uv_begin_x));

				pc->Add(
					v3f(begin_z - aabbcenter.x, begin_x - aabbcenter.y, firstPoint.z),
					true,
					false,
					v3f(0.f, 0.f, -1.f),
					v2f(uv_begin_y, uv_begin_x));

				pc->Add(
					v3f(begin_z - aabbcenter.x, begin_x + segment_size_w - aabbcenter.y, firstPoint.z),
					true,
					false,
					v3f(0.f, 0.f, -1.f),
					v2f(uv_begin_y, uv_begin_x - uv_segment_size_w));

				pc->Add(
					v3f(begin_z + segment_size_h - aabbcenter.x, begin_x + segment_size_w - aabbcenter.y, firstPoint.z),
					true,
					false,
					v3f(0.f, 0.f, -1.f),
					v2f(uv_begin_y - uv_segment_size_h, uv_begin_x - uv_segment_size_w));

				mb->AddPolygon(pc, false);
				pc->Clear();

				begin_x += segment_size_w;
				uv_begin_x -= uv_segment_size_w;

				if (begin_x > size.x + aabb.m_max.x)
				{
					begin_x = aabb.m_min.y;
					uv_begin_x = 1;
				}
			}break;
			}
		}

		switch (ct)
		{
		case miViewportCameraType::Perspective:
		case miViewportCameraType::Top:
			begin_z += segment_size_h;
			begin_x = aabb.m_min.x;
			uv_begin_y += uv_segment_size_h;
			uv_begin_x = 0;
			break;
		case miViewportCameraType::Bottom:
			begin_z += segment_size_h;
			begin_x = aabb.m_min.x;
			uv_begin_y -= uv_segment_size_h;
			uv_begin_x = 0.f;
			break;
		case miViewportCameraType::Left:
			begin_z += segment_size_h;
			begin_x = aabb.m_min.y;
			uv_begin_y -= uv_segment_size_h;
			uv_begin_x = 1;
			break;
		case miViewportCameraType::Right:
			begin_z += segment_size_h;
			begin_x = aabb.m_max.y;
			uv_begin_y += uv_segment_size_h;
			uv_begin_x = 0;
			break;
		case miViewportCameraType::Front:
			begin_z += segment_size_h;
			begin_x = aabb.m_min.x;
			uv_begin_y -= uv_segment_size_h;
			uv_begin_x = 0;
			break;
		case miViewportCameraType::Back:
			begin_z += segment_size_h;
			begin_x = aabb.m_min.y;
			uv_begin_y -= uv_segment_size_h;
			uv_begin_x = 1;
			break;
		}
	}
}

miplPolygonObjectPlane::miplPolygonObjectPlane(miSDK* sdk, miPlugin* p) {
	m_objectTypeName = "mesh_plane";

	m_sdk = sdk;
	m_plugin = p;
	m_pluginImpl = (miplStd*)p;
	m_isLMBDown = false;
	m_viewportCameraType = miViewportCameraType::Perspective;
	m_needToCreateNewGPUBuffers = true;

	m_polygonCreator = new miPolygonCreator;
	m_visualObject_polygon = m_sdk->CreateVisualObject(this, miVisualObjectType::Polygon);
	//m_visualObject_vertex = m_sdk->CreateVisualObject(this, miVisualObjectType::Vertex);
	m_visualObject_edge = m_sdk->CreateVisualObject(this, miVisualObjectType::Edge);
	m_meshBuilder = 0;

	m_flags |= miSceneObjectFlag_CanConvertToEditableObject;

	m_x_segments = 1;
	m_y_segments = 1;
}

miplPolygonObjectPlane::~miplPolygonObjectPlane() {
	if (m_polygonCreator) delete m_polygonCreator;
	if (m_meshBuilder) delete m_meshBuilder;
	if (m_visualObject_polygon) miDestroy(m_visualObject_polygon);
	//if (m_visualObject_vertex) miDestroy(m_visualObject_vertex);
	if (m_visualObject_edge) miDestroy(m_visualObject_edge);
	//printf("destroy\n");
}

int miplPolygonObjectPlane::GetVisualObjectCount() {
	return 2;
}
miVisualObject* miplPolygonObjectPlane::GetVisualObject(int i) {
	switch (i)
	{
	default:
	case 0: return m_visualObject_polygon; break;
	case 1: return m_visualObject_edge; break;
	//case 2: return m_visualObject_vertex; break;
	}
	return m_visualObject_polygon;
}

int miplPolygonObjectPlane::GetMeshCount() {
	return 1;
}
miMesh* miplPolygonObjectPlane::GetMesh(int) {
	return m_meshBuilder->m_mesh;
}


void miplPolygonObjectPlane::OnUpdate(float dt) {
}

void miplPolygonObjectPlane::OnDrawUV() {

}

void miplPolygonObjectPlane::OnDraw(miViewportDrawMode dm, miEditMode em, float dt) {
	if (dm == miViewportDrawMode::Material
		|| dm == miViewportDrawMode::MaterialWireframe)
	{
		if (m_visualObject_polygon) m_visualObject_polygon->Draw(false);
	}

	if ((dm == miViewportDrawMode::Wireframe || dm == miViewportDrawMode::MaterialWireframe)
		|| (m_isSelected && em == miEditMode::Edge)
		|| (m_isSelected && em == miEditMode::Polygon)
		)
	{
		if (m_visualObject_edge) m_visualObject_edge->Draw(false);
	}
}

miPlugin* miplPolygonObjectPlane::GetPlugin() {
	return m_plugin;
}

void miplPolygonObjectPlane::OnCreationLMBDown() {
	m_firstPoint = m_sdk->GetCursorPosition3DFirstClick();
	m_isLMBDown = true;
}
void miplPolygonObjectPlane::OnCreationLMBUp() {
	bool isOk = true;

	if (m_size.x < 0.1f) isOk = false;
	if (m_size.y < 0.1f) isOk = false;

	if (isOk)
	{
		m_pluginImpl->_dropNewObject();
	}
	else
	{
		m_sdk->RemoveObjectFromScene(this);
	}
	m_isLMBDown = false;
}

void miplPolygonObjectPlane::_generate() {
	m_meshBuilder->DeleteMesh();
	m_meshBuilder->Begin();

	miplPolygonObjectPlane_generate(
		m_polygonCreator,
		m_meshBuilder,
		m_viewportCameraType,
		m_creationAabb, m_firstPoint, 
		m_size,
		m_x_segments, 
		m_y_segments);
	
	m_meshBuilder->End();

	if (m_meshBuilder->m_mesh->m_first_polygon)
	{
		m_visualObject_polygon->CreateNewGPUModels(m_meshBuilder->m_mesh);
		m_visualObject_edge->CreateNewGPUModels(m_meshBuilder->m_mesh);
		//m_visualObject_vertex->CreateNewGPUModels(&m_meshBuilder->m_mesh);

		// object can contain a lot of m_visualObject
		// build aabb here
		//m_aabb = m_visualObject->GetAabb();
		this->UpdateTransform(); // for m_globalPosition;
		this->UpdateAabb();

		m_needToCreateNewGPUBuffers = false;
	}
}

void miplPolygonObjectPlane::OnCreationEnd() {
}
void miplPolygonObjectPlane::OnCreationMouseMove() {
	if (!m_polygonCreator) return;
	if (!m_meshBuilder) return;
	if (!m_isLMBDown) return;

	m_secondPoint = m_sdk->GetCursorPosition3D();

	m_creationAabb.reset();
	m_creationAabb.add(m_firstPoint);
	m_creationAabb.add(m_secondPoint);
	m_creationAabb.center(m_localPosition);

	m_viewportCameraType = m_sdk->GetActiveViewportCameraType();

	_generate();
}

void miplPolygonObjectPlane::OnCreation(miPluginGUI* gui) {
	assert(gui);
	m_gui = gui;
	OnCreationMouseMove();

	m_meshBuilder = new miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>>
		(40000,80400,40401); // 200x200
	m_sdk->AddObjectToScene(this, L"Plane");
}

void miplPolygonObjectPlane::DeleteSelectedObjects(miEditMode em) {
	switch (em)
	{
	case miEditMode::Vertex:
		break;
	case miEditMode::Edge:
		break;
	case miEditMode::Polygon:
		break;
	case miEditMode::Object:
	default:
		break;
	}
}

void miplPolygonObjectPlane::OnConvertToEditableObject() {
}

void miplPolygonObjectPlane::SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	miSceneObject::SelectSingle(em, km, sf);
	switch (em)
	{
	default:
	case miEditMode::Object:
	break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

void miplPolygonObjectPlane::Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
	miSceneObject::Select(em, km, sf);
	switch (em)
	{
	default:
	case miEditMode::Object:
	break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

void miplPolygonObjectPlane::SelectAll(miEditMode em) {
	miSceneObject::SelectAll(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

void miplPolygonObjectPlane::DeselectAll(miEditMode em) {
	miSceneObject::DeselectAll(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

void miplPolygonObjectPlane::InvertSelection(miEditMode em) {
	miSceneObject::InvertSelection(em);
	switch (em)
	{
	default:
	case miEditMode::Object:
		break;
	case miEditMode::Vertex:
	case miEditMode::Edge:
	case miEditMode::Polygon:
		/*for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
		}*/
		break;
	}
}

void miplPolygonObjectPlane::RebuildVisualObjects(bool) {
}