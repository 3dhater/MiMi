#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "miViewportCamera.h"
#include "miShortcutManager.h"
#include "miSelectionFrust.h"
#include "miSDKImpl.h"
#include "miRootObject.h"

extern miApplication * g_app;
extern Mat4 g_emptyMatrix;

miViewport::miViewport(miViewportCameraType vct, const v4f& rect1_0){
	m_creationRect = rect1_0;

	m_isOnLeftBorder = false;
	m_isOnRightBorder = false;
	m_isOnTopBorder = false;
	m_isOnBottomBorder = false;

	if (m_creationRect.x == 0.f) m_isOnLeftBorder = true;
	if (m_creationRect.y == 0.f) m_isOnTopBorder = true;
	if (m_creationRect.z == 1.f) m_isOnRightBorder = true;
	if (m_creationRect.w == 1.f) m_isOnBottomBorder = true;

	m_gpu = yyGetVideoDriverAPI();
	m_gui_group = 0;
	m_drawMode = this->Draw_MaterialWireframe;
	m_drawGrid = true;
	m_index = 0;
	m_isCursorInRect = false;
	m_gui_text_vpName = 0;
	m_gui_button_resetCamera = 0;
	//m_gui_button_viewport = 0;

	m_camera[Camera_Perspective] = new miViewportCamera(this, miViewportCameraType::Perspective);
	m_camera[Camera_Top] = new miViewportCamera(this, miViewportCameraType::Top);
	m_camera[Camera_Bottom] = new miViewportCamera(this, miViewportCameraType::Bottom);
	m_camera[Camera_Front] = new miViewportCamera(this, miViewportCameraType::Front);
	m_camera[Camera_Back] = new miViewportCamera(this, miViewportCameraType::Back);
	m_camera[Camera_Left] = new miViewportCamera(this, miViewportCameraType::Left);
	m_camera[Camera_Right] = new miViewportCamera(this, miViewportCameraType::Right);

	switch (vct)
	{
	default:
	case miViewportCameraType::Perspective:
		m_activeCamera = m_camera[Camera_Perspective];
		break;
	case miViewportCameraType::Left:
		m_activeCamera = m_camera[Camera_Left];
		break;
	case miViewportCameraType::Right:
		m_activeCamera = m_camera[Camera_Right];
		break;
	case miViewportCameraType::Top:
		m_activeCamera = m_camera[Camera_Top];
		break;
	case miViewportCameraType::Bottom:
		m_activeCamera = m_camera[Camera_Bottom];
		break;
	case miViewportCameraType::Front:
		m_activeCamera = m_camera[Camera_Front];
		break;
	case miViewportCameraType::Back:
		m_activeCamera = m_camera[Camera_Back];
		break;
	}

	m_currentRect = m_creationRect;
	m_currentRectSize = v2f(800.f, 600.f);
	m_cameraType = vct;
	Init();
	OnWindowSize();
}

miViewport::~miViewport(){
	/*if (m_gui_group)
		yyGUIDeleteElement(m_gui_group);*/

	for (s32 i = 0; i < Camera_count_; ++i)
	{
		if (m_camera[i]) delete m_camera[i];
	}
}

void miViewport::Copy(miViewport* other) {
	this->SetCameraType(other->m_cameraType);
	this->SetDrawGrid(other->m_drawGrid);
	this->SetDrawMode(other->m_drawMode);
	m_activeCamera->Copy(other->m_activeCamera);
}

//void miViewport_onClick_viewportName(yyGUIElement* elem, s32 m_id) {
//	miViewport* vp = (miViewport*)elem->m_userData;
//	g_app->m_popupViewport = vp;
//	g_app->ShowPopupAtCursor(&g_app->m_popup_ViewportTypes);
//}
void miViewport_onClick_cameraReset(yyGUIElement* elem, s32 m_id) {
	miViewport* vp = (miViewport*)elem->m_userData;
	g_app->m_popupViewport = vp;
	g_app->ShowPopupAtCursor(&g_app->m_popup_ViewportCamera);
}
void miViewport_onClick_viewport(yyGUIElement* elem, s32 m_id) {
	miViewport* vp = (miViewport*)elem->m_userData;
	g_app->m_popupViewport = vp;
	g_app->ShowPopupAtCursor(&g_app->m_popup_ViewportParameters);
}
void miViewport_onRebuildSetRects_gui_group(yyGUIElement* elem, s32 m_id) {
	yyGUIGroup* group = (yyGUIGroup*)elem;
	miViewport* vp = (miViewport*)elem->m_userData;
	if (vp->m_isOnLeftBorder)
	{
		vp->m_gui_group->m_buildRectInPixels.x = miViewportLeftIndent;
		vp->m_gui_group->m_clipRectInPixels.x = miViewportLeftIndent;
		vp->m_gui_group->m_sensorRectInPixels.x = miViewportLeftIndent;
	}
	if (vp->m_isOnRightBorder)
	{
		vp->m_gui_group->m_buildRectInPixels.z -= miViewportRightIndent;
		vp->m_gui_group->m_clipRectInPixels.z -= miViewportRightIndent;
		vp->m_gui_group->m_sensorRectInPixels.z -= miViewportRightIndent;
	}
	if (vp->m_isOnTopBorder)
	{
		vp->m_gui_group->m_buildRectInPixels.y = miViewportTopIndent;
		vp->m_gui_group->m_clipRectInPixels.y = miViewportTopIndent;
		vp->m_gui_group->m_sensorRectInPixels.y = miViewportTopIndent;
	}
	if (vp->m_isOnBottomBorder)
	{
		vp->m_gui_group->m_buildRectInPixels.w -= miViewportBottomIndent;
		vp->m_gui_group->m_clipRectInPixels.w -= miViewportBottomIndent;
		vp->m_gui_group->m_sensorRectInPixels.w -= miViewportBottomIndent;
	}
	/*printf("%f %f %f %f\n", vp->m_gui_group->m_buildRectInPixels.x,
		vp->m_gui_group->m_buildRectInPixels.y, vp->m_gui_group->m_buildRectInPixels.z,
		vp->m_gui_group->m_buildRectInPixels.w);*/
}

void onMouseInRect(yyGUIElement* elem, s32 m_id) {
	miViewport* vp = (miViewport*)elem->m_userData;
	//printf("VP: %i\n", vp->m_index);
}
void miViewport::Init() {
	m_gui_group = yyGUICreateGroup(v4f(), -1, 0);
	m_gui_group->IgnoreInput(true);
	m_gui_group->m_ignoreSetCursorInGUI = true;
	m_gui_group->m_useProportionalScaling = true;
//	m_gui_group->m_onMouseInRect = onMouseInRect;
	m_gui_group->m_userData = this;
	m_gui_group->m_buildRect = m_creationRect;
	m_gui_group->m_onRebuildSetRects = miViewport_onRebuildSetRects_gui_group;
	m_gui_group->SetRectsFromBuildRect();

	f32 vpNamePosX = 10.f;
	/*if (m_creationRect.x == 0.f)
		vpNamePosX += miViewportLeftIndent;*/

	m_gui_text_vpName = yyGUICreateText(v2f(vpNamePosX, 2.f), g_app->m_GUIManager->GetFont(miGUIManager::Font::Default),
		L"T", 0);
	m_gui_text_vpName->m_align = m_gui_text_vpName->AlignLeftTop;
	m_gui_text_vpName->IgnoreInput(false);
	//m_gui_text_vpName->m_onClick = miViewport_onClick_viewportName;
	m_gui_text_vpName->m_ignoreSetCursorInGUI = false;
	m_gui_text_vpName->m_userData = this;
	m_gui_text_vpName->m_onClick = miViewport_onClick_viewport;
	m_gui_group->AddElement(m_gui_text_vpName);

	f32 buttonPositionX = 70.f ;
	/*if (m_creationRect.x == 0.f)
		buttonPositionX += miViewportLeftIndent;*/

	f32 buttonWidth = 17.f;

	v4f region(0.f,72.f,16.f,88.f);
	m_gui_button_resetCamera = yyGUICreateButton(v4f(buttonPositionX, 2.f, buttonPositionX + buttonWidth, 19.f),
		yyGetTextureFromCache("../res/gui/icons.png"), m_index, 0, &region);
	m_gui_button_resetCamera->m_isAnimated = true;
	region.set(24.f,72.f,40.f,88.f);
	m_gui_button_resetCamera->SetMouseHoverTexture(yyGetTextureFromCache("../res/gui/icons.png"), &region);
	m_gui_group->AddElement(m_gui_button_resetCamera);
	m_gui_button_resetCamera->SetOpacity(0.19f, 0);
	m_gui_button_resetCamera->SetOpacity(0.19f, 1);
	m_gui_button_resetCamera->SetOpacity(0.19f, 2);
	m_gui_button_resetCamera->m_onClick = miViewport_onClick_cameraReset;
	m_gui_button_resetCamera->m_userData = this;

	buttonPositionX += buttonWidth;

	/*region.set(0.f, 96.f, 16.f, 112.f);
	m_gui_button_viewport = yyGUICreateButton(v4f(buttonPositionX, 2.f, buttonPositionX + buttonWidth, 19.f),
		yyGetTextureFromCache("../res/gui/icons.png"), m_index, 0, &region);
	m_gui_button_viewport->m_isAnimated = true;
	region.set(24.f, 96.f, 40.f, 112.f);
	m_gui_button_viewport->SetMouseHoverTexture(yyGetTextureFromCache("../res/gui/icons.png"), &region);
	m_gui_group->AddElement(m_gui_button_viewport);
	m_gui_button_viewport->SetOpacity(0.19f, 0);
	m_gui_button_viewport->SetOpacity(0.19f, 1);
	m_gui_button_viewport->SetOpacity(0.19f, 2);
	m_gui_button_viewport->m_onClick = miViewport_onClick_viewport;
	m_gui_button_viewport->m_userData = this;*/
	
	buttonPositionX += buttonWidth;

	SetCameraType(m_cameraType);
}

void miViewport::SetViewportName(const wchar_t* name) {
	static yyStringW n;
	n = name;
	if (m_gui_text_vpName)
	{
		if (m_gui_text_vpName->m_text != n)
			m_gui_text_vpName->SetText(name);
	}
}

void miViewport::SetCameraType(miViewportCameraType ct) {
	m_cameraType = ct;
	switch (ct)
	{
	case miViewportCameraType::Perspective:
		SetViewportName(L"Perspective");
		break;
	case miViewportCameraType::Left:
		SetViewportName(L"Left");
		break;
	case miViewportCameraType::Right:
		SetViewportName(L"Right");
		break;
	case miViewportCameraType::Top:
		SetViewportName(L"Top");
		break;
	case miViewportCameraType::Bottom:
		SetViewportName(L"Bottom");
		break;
	case miViewportCameraType::Front:
		SetViewportName(L"Front");
		break;
	case miViewportCameraType::Back:
		SetViewportName(L"Back");
		break;
	default:
		SetViewportName(L"Orthogonal");
		break;
	}

	switch (ct)
	{
	default:
	case miViewportCameraType::Perspective:
		m_activeCamera = m_camera[Camera_Perspective];
		break;
	case miViewportCameraType::Left:
		m_activeCamera = m_camera[Camera_Left];
		break;
	case miViewportCameraType::Right:
		m_activeCamera = m_camera[Camera_Right];
		break;
	case miViewportCameraType::Top:
		m_activeCamera = m_camera[Camera_Top];
		break;
	case miViewportCameraType::Bottom:
		m_activeCamera = m_camera[Camera_Bottom];
		break;
	case miViewportCameraType::Front:
		m_activeCamera = m_camera[Camera_Front];
		break;
	case miViewportCameraType::Back:
		m_activeCamera = m_camera[Camera_Back];
		break;
	}
}

void miViewport::HideGUI() {
	m_gui_group->SetVisible(false);
}

void miViewport::ShowGUI() {
	m_gui_group->SetVisible(true);
}

void miViewport::OnWindowSize() {
	f32 windowSizeX_1 = 1.f / g_app->m_window->m_currentSize.x;
	f32 windowSizeY_1 = 1.f / g_app->m_window->m_currentSize.y;

	m_currentRect = m_creationRect;
	
	if (m_creationRect.x > 0.f) m_currentRect.x = m_creationRect.x / windowSizeX_1;
	if (m_creationRect.z > 0.f) m_currentRect.z = m_creationRect.z / windowSizeX_1;
	if (m_creationRect.y > 0.f) m_currentRect.y = m_creationRect.y / windowSizeY_1;
	if (m_creationRect.w > 0.f) m_currentRect.w = m_creationRect.w / windowSizeY_1;


	

	m_currentRect.x += miViewportBorderSize;
	m_currentRect.y += miViewportBorderSize;
	m_currentRect.z -= miViewportBorderSize;
	m_currentRect.w -= miViewportBorderSize;

	if (m_isOnLeftBorder) m_currentRect.x += miViewportLeftIndent;
	if (m_isOnRightBorder) m_currentRect.z -= miViewportRightIndent;
	if (m_isOnTopBorder) m_currentRect.y += miViewportTopIndent;
	if (m_isOnBottomBorder) m_currentRect.w -= miViewportBottomIndent;

	m_currentRectSize.x = m_currentRect.z - m_currentRect.x;
	m_currentRectSize.y = m_currentRect.w - m_currentRect.y;
	m_activeCamera->m_aspect = m_currentRectSize.x / m_currentRectSize.y;
	/*m_gui_group->m_activeAreaRect = m_currentRect;
	m_gui_group->m_activeAreaRect_global = m_currentRect;
	m_gui_group->Rebuild();*/
	

	/*printf("%f %f %f %f - %f\n", 
		m_currentRect.x, m_currentRect.y, m_currentRect.z, m_currentRect.w, windowSizeX_1);*/
}

void miViewport::_frustum_cull(miSceneObject* o) {
	if (o != g_app->m_rootObject)
	{
		if (m_activeCamera->m_frust.PointInFrustum(*o->GetGlobalPosition()))
		{
			m_visibleObjects.push_back(o);
		}
		else if (m_activeCamera->m_frust.SphereInFrustum(o->GetAABB()->radius(), *o->GetGlobalPosition()))
		{
			m_visibleObjects.push_back(o);
		}
	}


	auto node = o->GetChildren()->m_head;
	if (node)
	{
		auto last = node->m_left;
		while (true) {
			_frustum_cull(node->m_data);

			if (node == last)
				break;

			node = node->m_right;
		}
	}
}

void miViewport::OnDraw() {
	m_activeCamera->Update();
	yySetEyePosition(m_activeCamera->m_positionCamera);


	m_gpu->UseDepth(false);
	m_gpu->DrawRectangle(m_currentRect, g_app->m_color_windowClearColor, g_app->m_color_windowClearColor);

	m_gpu->SetScissorRect(m_currentRect, g_app->m_window);
	m_gpu->SetViewport(m_currentRect.x, m_currentRect.y, m_currentRectSize.x, m_currentRectSize.y, g_app->m_window);


	yySetMatrix(yyMatrixType::View, m_activeCamera->m_viewMatrix);
	yySetMatrix(yyMatrixType::Projection, m_activeCamera->m_projectionMatrix);
	yySetMatrix(yyMatrixType::ViewProjection,
		m_activeCamera->m_viewProjectionMatrix);

	/*for (auto & r : g_app->g_rays)
	{
		m_gpu->DrawLine3D(math::miVec4_to_v4f(r.m_origin), math::miVec4_to_v4f(r.m_end), ColorWhite);
	}*/

	/*m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_left[0]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_left[1]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_left[1]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_left[2]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_left[2]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_left[3]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_left[3]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_left[0]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_right[0]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_right[1]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_right[1]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_right[2]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_right[2]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_right[3]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_right[3]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_right[0]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_top[0]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_top[1]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_top[1]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_top[2]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_top[2]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_top[3]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_top[3]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_top[0]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_bottom[0]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_bottom[1]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_bottom[1]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_bottom[2]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_bottom[2]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_bottom[3]), ColorWhite);
	m_gpu->DrawLine3D(mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_bottom[3]), mimath::miVec4_to_v4f(g_app->m_selectionFrust->m_bottom[0]), ColorWhite);*/
	
	if (g_app->m_isClickAndDrag)
	{
		m_gpu->DrawLine3D(g_app->m_cursorLMBClickPosition3D, g_app->m_cursorPosition3D, ColorWhite);
	}


	if (m_drawGrid)
	{
		_drawGrid();
	}

	// Прежде чем рисовать скорее всего лучше сделать сортировку и всё сохранить в массив
	//g_app->m_currentViewportDrawCamera = m_activeCamera;
	g_app->m_currentViewportDraw = this;

	g_app->m_gpu->UseDepth(true);

	m_visibleObjects.clear();
	_frustum_cull(g_app->m_rootObject);

	//printf("%i\n", (s32)m_visibleObjects.m_size);

	g_app->m_gpu->UseDepth(true);

	if(m_visibleObjects.m_size)
		_drawScene();
}

void miViewport::_drawScene() {

	static f32 a = 0.f;

	a += 0.001f;

	if (a > 10.f)
		a = 0.f;

	miQuaternion q(miVec4(0.f, a, 0.f, 0.f));

	for (u32 i = 0; i < m_visibleObjects.m_size; ++i)
	{
		auto object = m_visibleObjects.m_data[i];

		//object->GetLocalPosition()->set(a, 0.f, 0.f, 0.f);


		auto rm = object->GetRotationMatrix();
		//rm->setRotation(q);

		auto taabb = object->GetAABBTransformed();
		taabb->transform(object->GetAABB(), rm, object->GetGlobalPosition());

		object->UpdateTransform();
		object->OnUpdate(g_app->m_dt);
		object->OnDraw();

		_drawAabb(*object->GetAABBTransformed(), *object->GetEdgeColor());
	}
	//g_app->UpdateSceneAabb();
}

void miViewport::SetDrawMode(DrawMode dm) {
	m_drawMode = dm;
}

void miViewport::SetDrawGrid(bool v) {
	m_drawGrid = v;
}

v4f miViewport::GetCursorRayHitPosition(const v2f& cursorPosition) {
	miRay ray;
	g_app->m_sdk->GetRayFromScreen(&ray, 
		mimath::v2f_to_miVec2(cursorPosition),
		mimath::v4f_to_miVec4(m_currentRect),
		mimath::Mat4_to_miMatrix(m_activeCamera->m_viewProjectionInvertMatrix));
	
	v4f v;
	miVec4 ip;
	
	bool isObject = false;
	
	/*
		check objects on scene here
	*/



	if (!isObject)
	{
		switch (m_cameraType)
		{
		default:
		case miViewportCameraType::Top:
		case miViewportCameraType::Perspective:
			ray.planeIntersection(miVec4(0.f,0.f,0.f,1.f), miVec4(0.f,1.f,0.f,1.f), ip);
			break;
		case miViewportCameraType::Bottom:
			ray.planeIntersection(miVec4(0.f, 0.f, 0.f, 1.f), miVec4(0.f, -1.f, 0.f, 1.f), ip);
			break;
		case miViewportCameraType::Left:
			ray.planeIntersection(miVec4(0.f, 0.f, 0.f, 1.f), miVec4(-1.f, 0.f, 0.f, 1.f), ip);
			break;
		case miViewportCameraType::Right:
			ray.planeIntersection(miVec4(0.f, 0.f, 0.f, 1.f), miVec4(1.f, 0.f, 0.f, 1.f), ip);
			break;
		case miViewportCameraType::Front:
			ray.planeIntersection(miVec4(0.f, 0.f, 0.f, 1.f), miVec4(0.f, 0.f, 1.f, 1.f), ip);
			break;
		case miViewportCameraType::Back:
			ray.planeIntersection(miVec4(0.f, 0.f, 0.f, 1.f), miVec4(0.f, 0.f, -1.f, 1.f), ip);
			break;
		}
	}

	v = mimath::miVec4_to_v4f(ip);

	return v;
}

void miViewport::_drawAabb(const miAabb& aabb, const miVec4& _color) {
	auto & p1 = aabb.m_min;
	auto & p2 = aabb.m_max;

	yyColor color;
	color.m_data[0] = _color.x;
	color.m_data[1] = _color.y;
	color.m_data[2] = _color.z;
	color.m_data[3] = 1.f;

	color = ColorWhite;

	v4f positionOffset;

	v4f v1 = mimath::miVec4_to_v4f(p1);
	v4f v2 = mimath::miVec4_to_v4f(p2);

	v4f v3(p1.x, p1.y, p2.z, 1.f);
	v4f v4(p2.x, p1.y, p1.z, 1.f);
	v4f v5(p1.x, p2.y, p1.z, 1.f);
	v4f v6(p1.x, p2.y, p2.z, 1.f);
	v4f v7(p2.x, p1.y, p2.z, 1.f);
	v4f v8(p2.x, p2.y, p1.z, 1.f);

	m_gpu->DrawLine3D(v1 + positionOffset, v4 + positionOffset, color);
	m_gpu->DrawLine3D(v5 + positionOffset, v8 + positionOffset, color);
	m_gpu->DrawLine3D(v1 + positionOffset, v5 + positionOffset, color);
	m_gpu->DrawLine3D(v4 + positionOffset, v8 + positionOffset, color);
	m_gpu->DrawLine3D(v3 + positionOffset, v7 + positionOffset, color);
	m_gpu->DrawLine3D(v6 + positionOffset, v2 + positionOffset, color);
	m_gpu->DrawLine3D(v3 + positionOffset, v6 + positionOffset, color);
	m_gpu->DrawLine3D(v7 + positionOffset, v2 + positionOffset, color);
	m_gpu->DrawLine3D(v2 + positionOffset, v8 + positionOffset, color);
	m_gpu->DrawLine3D(v4 + positionOffset, v7 + positionOffset, color);
	m_gpu->DrawLine3D(v5 + positionOffset, v6 + positionOffset, color);
	m_gpu->DrawLine3D(v1 + positionOffset, v3 + positionOffset, color);
}

void miViewport::_drawGrid() {
	yySetMatrix(yyMatrixType::World, g_emptyMatrix);
	yySetMatrix(yyMatrixType::WorldViewProjection,
		m_activeCamera->m_projectionMatrix * m_activeCamera->m_viewMatrix * g_emptyMatrix);

	bool isCameraLowerThanWorld = false;
	if (m_activeCamera->m_positionCamera.y < 0.f)
		isCameraLowerThanWorld = true;

	switch (m_cameraType)
	{
	default:
	case miViewportCameraType::Perspective:
		if (isCameraLowerThanWorld)
			m_gpu->SetModel(g_app->m_gridModel_perspective2);
		else
			m_gpu->SetModel(g_app->m_gridModel_perspective1);
		break;
	case miViewportCameraType::Left: {
		bool front = ((m_activeCamera->m_rotationPlatform.y < 0.f) &&
			(m_activeCamera->m_rotationPlatform.y > -math::PI));

		if (m_activeCamera->m_positionPlatform.w < 40.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_left1) : m_gpu->SetModel(g_app->m_gridModel_left2);
		else if (m_activeCamera->m_positionPlatform.w < 140.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_left1_10) : m_gpu->SetModel(g_app->m_gridModel_left2_10);
		else
			front ? m_gpu->SetModel(g_app->m_gridModel_left1_100) : m_gpu->SetModel(g_app->m_gridModel_left2_100);
	}break;
	case miViewportCameraType::Right: {
		bool front = ((m_activeCamera->m_rotationPlatform.y > 0.f) &&
			(m_activeCamera->m_rotationPlatform.y < math::PI));

		if (m_activeCamera->m_positionPlatform.w < 40.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_left1) : m_gpu->SetModel(g_app->m_gridModel_left2);
		else if (m_activeCamera->m_positionPlatform.w < 140.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_left1_10) : m_gpu->SetModel(g_app->m_gridModel_left2_10);
		else
			front ? m_gpu->SetModel(g_app->m_gridModel_left1_100) : m_gpu->SetModel(g_app->m_gridModel_left2_100);
	}break;
	case miViewportCameraType::Bottom:
	case miViewportCameraType::Top:
		if (m_activeCamera->m_positionPlatform.w < 40.f)
			isCameraLowerThanWorld ? m_gpu->SetModel(g_app->m_gridModel_top2) : m_gpu->SetModel(g_app->m_gridModel_top1);
		else if (m_activeCamera->m_positionPlatform.w < 140.f)
			isCameraLowerThanWorld ? m_gpu->SetModel(g_app->m_gridModel_top2_10) : m_gpu->SetModel(g_app->m_gridModel_top1_10);
		else
			isCameraLowerThanWorld ? m_gpu->SetModel(g_app->m_gridModel_top2_100) : m_gpu->SetModel(g_app->m_gridModel_top1_100);
		break;
	case miViewportCameraType::Front: {
		bool front = ((m_activeCamera->m_rotationPlatform.y < math::PIHalf) &&
			(m_activeCamera->m_rotationPlatform.y > -math::PIPlusHalf));

		if (m_activeCamera->m_positionPlatform.w < 40.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_front1) : m_gpu->SetModel(g_app->m_gridModel_front2);
		else if (m_activeCamera->m_positionPlatform.w < 140.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_front1_10) : m_gpu->SetModel(g_app->m_gridModel_front2_10);
		else
			front ? m_gpu->SetModel(g_app->m_gridModel_front1_100) : m_gpu->SetModel(g_app->m_gridModel_front2_100);
	}break;
	case miViewportCameraType::Back: {
		//printf("%f\n", m_activeCamera->m_rotationPlatform.y);
		bool front = ((m_activeCamera->m_rotationPlatform.y > math::PIHalf) &&
			(m_activeCamera->m_rotationPlatform.y < math::PI + math::PIHalf));

		if (m_activeCamera->m_positionPlatform.w < 40.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_front1) : m_gpu->SetModel(g_app->m_gridModel_front2);
		else if (m_activeCamera->m_positionPlatform.w < 140.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_front1_10) : m_gpu->SetModel(g_app->m_gridModel_front2_10);
		else
			front ? m_gpu->SetModel(g_app->m_gridModel_front1_100) : m_gpu->SetModel(g_app->m_gridModel_front2_100);
	}break;
	}

	yySetMaterial(g_app->m_gridModelMaterial);
	m_gpu->UseDepth(true);
	m_gpu->Draw();
}

