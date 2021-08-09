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
	m_isDrawAabbs = false;
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
	m_drawMode = miViewportDrawMode::MaterialWireframe;
	m_drawGrid = true;
	m_index = 0;
	m_isCursorInRect = false;
	m_gui_text_vpName = 0;
	m_gui_button_resetCamera = 0;
	m_activeCamera = 0;
	//m_gui_button_viewport = 0;

	m_camera[Camera_Perspective] = new miViewportCamera(this, miViewportCameraType::Perspective);
	m_camera[Camera_Top] = new miViewportCamera(this, miViewportCameraType::Top);
	m_camera[Camera_UV] = new miViewportCamera(this, miViewportCameraType::UV);
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
	case miViewportCameraType::UV:
		m_activeCamera = m_camera[Camera_UV];
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
	m_activeCamera->Reset();

	switch (vct)
	{
	case miViewportCameraType::Perspective:
	case miViewportCameraType::Top:
	case miViewportCameraType::UV:
		m_rayTestTiangles[0].v1 = v4f(-999999.f, 0.f, -999999.f, 0.f);
		m_rayTestTiangles[0].v2 = v4f(-999999.f, 0.f, 999999.f, 0.f);
		m_rayTestTiangles[0].v3 = v4f(999999.f,0.f, 999999.f, 0.f);
		m_rayTestTiangles[0].update();
		m_rayTestTiangles[1].v1 = v4f(-999999.f, 0.f, -999999.f, 0.f);
		m_rayTestTiangles[1].v2 = v4f(999999.f, 0.f, 999999.f, 0.f);
		m_rayTestTiangles[1].v3 = v4f(999999.f, 0.f, -999999.f, 0.f);
		m_rayTestTiangles[1].update();
		break;
	case miViewportCameraType::Left:
		break;
	case miViewportCameraType::Right:
		break;
	case miViewportCameraType::Bottom:
		break;
	case miViewportCameraType::Front:
		break;
	case miViewportCameraType::Back:
		break;
	default:
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


	if (m_cameraType != miViewportCameraType::UV)
	{

		m_gui_text_vpName = yyGUICreateText(v2f(vpNamePosX, 2.f), g_app->m_GUIManager->GetFont(miGUIManager::Font::Default),
			L"T", 0);
		m_gui_text_vpName->m_align = m_gui_text_vpName->AlignLeftTop;
		m_gui_text_vpName->IgnoreInput(false);
		//m_gui_text_vpName->m_onClick = miViewport_onClick_viewportName;
		m_gui_text_vpName->m_ignoreSetCursorInGUI = false;
		m_gui_text_vpName->m_userData = this;
		m_gui_text_vpName->m_onClick = miViewport_onClick_viewport;
		m_gui_group->AddElement(m_gui_text_vpName);

		f32 buttonPositionX = 70.f;
		/*if (m_creationRect.x == 0.f)
			buttonPositionX += miViewportLeftIndent;*/

		f32 buttonWidth = 17.f;

		v4f region(0.f, 72.f, 16.f, 88.f);
		m_gui_button_resetCamera = yyGUICreateButton(v4f(buttonPositionX, 2.f, buttonPositionX + buttonWidth, 19.f),
			yyGetTextureFromCache("../res/gui/icons.png"), m_index, 0, &region);
		m_gui_button_resetCamera->m_isAnimated = true;
		region.set(24.f, 72.f, 40.f, 88.f);
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
	}

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
	if (m_cameraType == miViewportCameraType::UV)
	{
		m_activeCamera = m_camera[Camera_UV];
		return;
	}

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
	case miViewportCameraType::UV:
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
	UpdateAspect();
	/*m_gui_group->m_activeAreaRect = m_currentRect;
	m_gui_group->m_activeAreaRect_global = m_currentRect;
	m_gui_group->Rebuild();*/
	

	/*printf("%f %f %f %f - %f\n", 
		m_currentRect.x, m_currentRect.y, m_currentRect.z, m_currentRect.w, windowSizeX_1);*/
}

void miViewport::UpdateAspect() {
	if(m_activeCamera)
		m_activeCamera->m_aspect = m_currentRectSize.x / m_currentRectSize.y;
}

void miViewport::_frustum_cull(miSceneObject* o) {
	if (o != g_app->m_rootObject)
	{
		v4f center;
		o->GetAABBTransformed()->center(center);
		auto r = o->GetAABBTransformed()->radius();
		if (m_activeCamera->m_frust.PointInFrustum(*o->GetGlobalPosition()) || m_activeCamera->m_frust.PointInFrustum(center))
		{
			m_visibleObjects.push_back(o);
		}
		else if (m_activeCamera->m_frust.SphereInFrustum(r+r, center))
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

void miViewport::OnDrawUV()
{
	static yyMaterial m;
	m.m_baseColor.set(1.f,0.f,1.f,1.f);
	m.m_type = yyMaterialType::Simple;
	yySetMaterial(0);

	g_app->m_gpu->SetTexture(0, g_app->m_UVPlaneTexture);
	if (g_app->m_selectedObjects.m_size)
	{
		if (g_app->m_selectedObjects.m_data[0]->m_material)
		{
			if (g_app->m_selectedObjects.m_data[0]->m_material->m_first->m_maps[0].m_GPUTexture)
			{
				g_app->m_gpu->SetTexture(0, (yyResource*)g_app->m_selectedObjects.m_data[0]->m_material->m_first->m_maps[0].m_GPUTexture);
			}
		}
	}

	m_gpu->UseDepth(false);
	auto W = Mat4();
	auto WVP = m_activeCamera->m_projectionMatrix* m_activeCamera->m_viewMatrix * W;
	yySetMatrix(yyMatrixType::WorldViewProjection, &WVP);
	yySetMatrix(yyMatrixType::World, &W);
	m_gpu->SetModel(g_app->m_UVPlaneModel);
	m_gpu->Draw();

	m_gpu->UseDepth(false);
	m_gpu->DrawLine3D(v4f(), v4f(1.f, 0.f, 0.f, 0.f), ColorRed);
	m_gpu->DrawLine3D(v4f(), v4f(0.f, 0.f, 1.f, 0.f), ColorLime);

	m_gpu->UseDepth(true);

	bool isUVGizmo = false;
	for (u32 i = 0; i < g_app->m_selectedObjects.m_size; ++i)
	{
		auto object = g_app->m_selectedObjects.m_data[i];
		
		object->OnDrawUV();

		if (object->IsUVSelected())
			isUVGizmo = true;
	}

	m_gpu->UseDepth(false);
	if (isUVGizmo)
	{
		f32 zoom = 1.f * m_activeCamera->m_positionPlatform.w;
		f32 indent = 0.018f * zoom;

		//m_UVAabbMoveOffset
		v4f aabbMin = g_app->m_UVAabbOnClick.m_min;
		v4f aabbMax = g_app->m_UVAabbOnClick.m_max;

		auto p1 = v4f(aabbMin.x - indent, 0.f, aabbMin.z - indent, 0.f); // LT
		auto p2 = v4f(aabbMax.x + indent, 0.f, aabbMin.z - indent, 0.f); // RT
		auto p3 = v4f(aabbMin.x - indent, 0.f, aabbMax.z + indent, 0.f); // LB
		auto p4 = v4f(aabbMax.x + indent, 0.f, aabbMax.z + indent, 0.f); // RB

		auto p1_2 = (p1 + p2) * 0.5f; // Top
		auto p1_3 = (p1 + p3) * 0.5f; // Left
		auto p3_4 = (p3 + p4) * 0.5f; // Bottom
		auto p2_4 = (p2 + p4) * 0.5f; // Right
		auto pCenter = (p1 + p4) * 0.5f;

		auto dp1 = v4f(g_app->m_UVAabb.m_min.x - indent, 0.f, g_app->m_UVAabb.m_min.z - indent, 0.f); // LT
		auto dp2 = v4f(g_app->m_UVAabb.m_max.x + indent, 0.f, g_app->m_UVAabb.m_min.z - indent, 0.f); // RT
		auto dp3 = v4f(g_app->m_UVAabb.m_min.x - indent, 0.f, g_app->m_UVAabb.m_max.z + indent, 0.f); // LB
		auto dp4 = v4f(g_app->m_UVAabb.m_max.x + indent, 0.f, g_app->m_UVAabb.m_max.z + indent, 0.f); // RB

		auto dp1_2 = (dp1 + dp2) * 0.5f; // Top
		auto dp1_3 = (dp1 + dp3) * 0.5f; // Left
		auto dp3_4 = (dp3 + dp4) * 0.5f; // Bottom
		auto dp2_4 = (dp2 + dp4) * 0.5f; // Right
		auto dpCenter = (dp1 + dp4) * 0.5f;

		v4f moveOffset(g_app->m_UVAabbMoveOffset.x, 0.f, g_app->m_UVAabbMoveOffset.y, 0.f);

		m_gpu->DrawLine3D(dp1 + moveOffset, dp2 + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1 + moveOffset, dp3 + moveOffset, ColorRed);
	
		m_gpu->DrawLine3D(dp3 + moveOffset, dp4 + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp2 + moveOffset, dp4 + moveOffset, ColorRed);


		f32 cornersSize = 0.003f * zoom;
		m_gpu->DrawLine3D(dp1 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dp1 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		m_gpu->DrawLine3D(dp1_2 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1_2 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1_2 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1_2 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1_2 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1_2 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1_2 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dp1_2 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		m_gpu->DrawLine3D(dp2 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp2 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp2 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp2 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp2 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp2 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp2 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dp2 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		m_gpu->DrawLine3D(dp3 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp3 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp3 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp3 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp3 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp3 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp3 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dp3 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		m_gpu->DrawLine3D(dp1_3 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1_3 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1_3 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1_3 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1_3 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp1_3 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp1_3 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dp1_3 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		m_gpu->DrawLine3D(dp4 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp4 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp4 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp4 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp4 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp4 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp4 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dp4 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		m_gpu->DrawLine3D(dp3_4 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp3_4 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp3_4 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp3_4 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp3_4 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp3_4 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp3_4 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dp3_4 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		m_gpu->DrawLine3D(dp2_4 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp2_4 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp2_4 - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp2_4 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp2_4 + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dp2_4 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dp2_4 + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dp2_4 + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		m_gpu->DrawLine3D(dpCenter - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dpCenter + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dpCenter - v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dpCenter + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dpCenter + v4f(-cornersSize, 0.f, cornersSize, 0.f) + moveOffset, dpCenter + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);
		m_gpu->DrawLine3D(dpCenter + v4f(cornersSize, 0.f, -cornersSize, 0.f) + moveOffset, dpCenter + v4f(cornersSize, 0.f, cornersSize, 0.f) + moveOffset, ColorRed);

		if (g_app->m_selectionFrust->PointInFrust(p1))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::SizeNWSE]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::LeftTop;
				g_app->OnGizmoUVClick();
			}
		}
		else if (g_app->m_selectionFrust->PointInFrust(p2))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::SizeNESW]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::RightTop;
				g_app->OnGizmoUVClick();
			}
		}
		else if (g_app->m_selectionFrust->PointInFrust(p3))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::SizeNESW]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::LeftBottom;
				g_app->OnGizmoUVClick();
			}
		}
		else if (g_app->m_selectionFrust->PointInFrust(p4))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::SizeNWSE]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::RightBottom;
				g_app->OnGizmoUVClick();
			}
		}
		else if (g_app->m_selectionFrust->PointInFrust(p1_2))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::Rotate]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::Top;
				g_app->OnGizmoUVClick();
			}
		}
		else if (g_app->m_selectionFrust->PointInFrust(p1_3))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::Rotate]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::Left;
				g_app->OnGizmoUVClick();
			}
		}
		else if (g_app->m_selectionFrust->PointInFrust(p3_4))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::Rotate]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::Bottom;
				g_app->OnGizmoUVClick();
			}
		}
		else if (g_app->m_selectionFrust->PointInFrust(p2_4))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::Rotate]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::Right;
				g_app->OnGizmoUVClick();
			}
		}
		else if (g_app->m_selectionFrust->PointInFrust(pCenter))
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::Size]);
			if (g_app->m_inputContext->m_isLMBDown)
			{
				g_app->m_gizmoModeUV = miGizmoUVMode::Center;
				g_app->OnGizmoUVClick();
			}
		}
		else
		{
			yySetCursor(yyCursorType::Arrow, g_app->m_cursors[(u32)miCursorType::Arrow]);
			g_app->m_gizmoModeUV = miGizmoUVMode::NoTransform;
		}

		if (g_app->m_gizmoModeUV != miGizmoUVMode::NoTransform)
		{
			g_app->m_gizmoMode = miGizmoMode::Other;
		}
	}
}

void miViewport::OnDraw() {
	m_activeCamera->Update();
	yySetEyePosition(&m_activeCamera->m_positionCamera);


	m_gpu->UseDepth(false);
	m_gpu->DrawRectangle(m_currentRect, g_app->m_color_windowClearColor, g_app->m_color_viewportColor);

	m_gpu->SetScissorRect(m_currentRect, g_app->m_window, 0);
	m_gpu->SetViewport(m_currentRect.x, m_currentRect.y, m_currentRectSize.x, m_currentRectSize.y, g_app->m_window, 0);

	yySetMatrix(yyMatrixType::View, &m_activeCamera->m_viewMatrix);
	yySetMatrix(yyMatrixType::Projection, &m_activeCamera->m_projectionMatrix);
	yySetMatrix(yyMatrixType::ViewProjection,
		&m_activeCamera->m_viewProjectionMatrix);
	
	// Прежде чем рисовать скорее всего лучше сделать сортировку и всё сохранить в массив
	//g_app->m_currentViewportDrawCamera = m_activeCamera;
	g_app->m_currentViewportDraw = this;

	if (m_cameraType == miViewportCameraType::UV)
	{
		OnDrawUV();
	}
	else
	{
		/*for (auto & r : g_app->g_rays)
		{
			m_gpu->DrawLine3D(math::v4f_to_v4f(r.m_origin), math::v4f_to_v4f(r.m_end), ColorWhite);
		}*/

		if (g_app->m_isClickAndDrag)
		{
			m_gpu->DrawLine3D(g_app->m_cursorLMBClickPosition3D, g_app->m_cursorPosition3D, ColorWhite);
		}

		if (m_drawGrid)
		{
			_drawGrid();
		}

		

		g_app->m_gpu->UseDepth(true);

		m_visibleObjects.clear();
		_frustum_cull(g_app->m_rootObject);

		//printf("%i\n", (s32)m_visibleObjects.m_size);

		g_app->m_gpu->UseDepth(true);

		

		if (m_visibleObjects.m_size)
		{
			_drawSelectedObjectFrame();
			_drawScene();
			if (m_isDrawAabbs)
				g_app->DrawAabb(g_app->m_sceneAabb, v4f(1.f), v3f());
		}
	}
	
	/*m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_left[0], g_app->m_selectionFrust->m_data.m_left[1], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_left[1], g_app->m_selectionFrust->m_data.m_left[2], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_left[2], g_app->m_selectionFrust->m_data.m_left[3], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_left[3], g_app->m_selectionFrust->m_data.m_left[0], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_right[0], g_app->m_selectionFrust->m_data.m_right[1], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_right[1], g_app->m_selectionFrust->m_data.m_right[2], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_right[2], g_app->m_selectionFrust->m_data.m_right[3], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_right[3], g_app->m_selectionFrust->m_data.m_right[0], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_top[0], g_app->m_selectionFrust->m_data.m_top[1], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_top[1], g_app->m_selectionFrust->m_data.m_top[2], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_top[2], g_app->m_selectionFrust->m_data.m_top[3], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_top[3], g_app->m_selectionFrust->m_data.m_top[0], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_bottom[0], g_app->m_selectionFrust->m_data.m_bottom[1], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_bottom[1], g_app->m_selectionFrust->m_data.m_bottom[2], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_bottom[2], g_app->m_selectionFrust->m_data.m_bottom[3], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_bottom[3], g_app->m_selectionFrust->m_data.m_bottom[0], ColorWhite);
	m_gpu->DrawLine3D(g_app->m_selectionFrust->m_data.m_BackC, g_app->m_selectionFrust->m_data.m_FrontC, ColorWhite);*/
}

void miViewport::ToggleDrawAABB() {
	m_isDrawAabbs = m_isDrawAabbs ? false : true;
}

void miViewport::_drawScene() {
	for (u32 i = 0; i < m_visibleObjects.m_size; ++i)
	{
		auto object = m_visibleObjects.m_data[i];
		auto object_position = object->GetGlobalPosition();
		auto object_position_v4f = *object_position;

		object->m_worldViewProjection = m_activeCamera->m_projectionMatrix* m_activeCamera->m_viewMatrix* object->m_worldMatrix;
		object->OnUpdate(g_app->m_dt);
		
		m_gpu->UseDepth(true);
		object->OnDraw(m_drawMode, g_app->m_editMode, g_app->m_dt);
		
		if (object->IsSelected())
		{
			if (m_isDrawAabbs)
				g_app->DrawAabb(*object->GetAABBTransformed(), *object->GetEdgeColor(), v3f());
		}
	}

	switch (g_app->m_editMode)
	{
	case miEditMode::Object:
		if (g_app->m_selectedObjects.m_size)
			g_app->m_gizmo->Draw(this);
		break;
	default:
		if (g_app->m_isVertexEdgePolygonSelected)
			g_app->m_gizmo->Draw(this);
		break;
	}

}

void miViewport::SetDrawMode(miViewportDrawMode dm) {
	m_drawMode = dm;
}

void miViewport::SetDrawGrid(bool v) {
	m_drawGrid = v;
}

void miViewport::ToggleDrawModeMaterial() {
	static bool is_materail_mode = false;
	if (m_drawMode == miViewportDrawMode::Wireframe)
	{
		if (is_materail_mode)
		{
			m_drawMode = miViewportDrawMode::Material;
			is_materail_mode = false;
		}
		else
			m_drawMode = miViewportDrawMode::MaterialWireframe;
	}
	else if (m_drawMode == miViewportDrawMode::MaterialWireframe)
		m_drawMode = miViewportDrawMode::Wireframe;
	else if (m_drawMode == miViewportDrawMode::Material)
	{
		m_drawMode = miViewportDrawMode::Wireframe;
		is_materail_mode = true;
	}
	//kkDrawAll();
}
void miViewport::ToggleDrawModeWireframe() {
	if (m_drawMode == miViewportDrawMode::Material)
		m_drawMode = miViewportDrawMode::MaterialWireframe;
	else if (m_drawMode == miViewportDrawMode::MaterialWireframe)
		m_drawMode = miViewportDrawMode::Material;
	//kkDrawAll();
}

v4f miViewport::GetCursorRayHitPosition(const v2f& cursorPosition) {
	yyRay ray;
	g_app->m_sdk->GetRayFromScreen(&ray, 
		cursorPosition,
		m_currentRect,
		m_activeCamera->m_viewProjectionInvertMatrix);
	
	v4f ip;
	
	bool isObject = false;
	
	/*
		check objects on scene here
	*/



	if (!isObject)
	{
		f32 T = 0.f;
		f32 U = 0.f;
		f32 V = 0.f;
		f32 W = 0.f;
		switch (m_cameraType)
		{
		default:
		case miViewportCameraType::Top:
		case miViewportCameraType::UV:
		case miViewportCameraType::Perspective:
			// another way
			if (!m_rayTestTiangles[0].rayTest_MT(ray, true, T, U, V, W))
			{
				m_rayTestTiangles[1].rayTest_MT(ray, true, T, U, V, W);
			}
			//ray.planeIntersection(v4f(0.f,0.f,0.f,1.f), v4f(0.f,1.f,0.f,1.f), ip);
			break;
		case miViewportCameraType::Bottom:
			ray.planeIntersection(v4f(0.f, 0.f, 0.f, 1.f), v4f(0.f, -1.f, 0.f, 1.f), T);
			break;
		case miViewportCameraType::Left:
			ray.planeIntersection(v4f(0.f, 0.f, 0.f, 1.f), v4f(-1.f, 0.f, 0.f, 1.f), T);
			break;
		case miViewportCameraType::Right:
			ray.planeIntersection(v4f(0.f, 0.f, 0.f, 1.f), v4f(1.f, 0.f, 0.f, 1.f), T);
			break;
		case miViewportCameraType::Front:
			ray.planeIntersection(v4f(0.f, 0.f, 0.f, 1.f), v4f(0.f, 0.f, 1.f, 1.f), T);
			break;
		case miViewportCameraType::Back:
			ray.planeIntersection(v4f(0.f, 0.f, 0.f, 1.f), v4f(0.f, 0.f, -1.f, 1.f), T);
			break;
		}
		ray.getIntersectionPoint(T, ip);

	}

	return ip;
}

void miViewport::_drawGrid() {
	yySetMatrix(yyMatrixType::World, &g_emptyMatrix);

	static Mat4 WVP;
	WVP = m_activeCamera->m_projectionMatrix * m_activeCamera->m_viewMatrix * g_emptyMatrix;
	yySetMatrix(yyMatrixType::WorldViewProjection, &WVP);

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
		bool front = ((m_activeCamera->m_rotationPlatform.y < math::PIPI) &&
			(m_activeCamera->m_rotationPlatform.y > math::PI));
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

	//m_gpu->DrawLine3D(v4f(), v4f(99999.f, 0.f, 0.f, 0.f), ColorRed);
	//m_gpu->DrawLine3D(v4f(), v4f(0.f, 99999.f, 0.f, 0.f), ColorBlue);
	//m_gpu->DrawLine3D(v4f(), v4f(0.f, 0.f, 99999.f, 0.f), ColorLime);

	yySetMaterial(&g_app->m_gridModelMaterial);
	m_gpu->UseDepth(true);
	m_gpu->Draw();
}

void miViewport::_drawSelectedObjectFrame() {
	f32 frameSizeX = 0.f;
	f32 frameSizeY = 0.f;
	f32 frameSizeZ = 0.f;
	f32 frameIndentX = 0.f;
	f32 frameIndentY = 0.f;
	f32 frameIndentZ = 0.f;

	for (u32 i = 0; i < g_app->m_selectedObjects.m_size; ++i)
	{
		auto obj = g_app->m_selectedObjects.m_data[i];

		auto aabb = *obj->GetAABBTransformed();
		aabb.m_max.w = 0.f;
		aabb.m_min.w = 0.f;

		if (g_app->m_editMode == miEditMode::Object)
		{
			aabb.m_max += g_app->m_gizmo->m_var_move;
			aabb.m_min += g_app->m_gizmo->m_var_move;
		}

		frameSizeX = (aabb.m_max.x - aabb.m_min.x)*0.2f;
		frameSizeY = (aabb.m_max.y - aabb.m_min.y)*0.2f;
		frameSizeZ = (aabb.m_max.z - aabb.m_min.z)*0.2f;
		//frameSize /= 12.f;
		frameIndentX = frameSizeX * 0.2f;
		frameIndentY = frameSizeY * 0.2f;
		frameIndentZ = frameSizeZ * 0.2f;

		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_min.z + frameSizeZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y + frameSizeY, aabb.m_min.z - frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_min.x + frameSizeX, aabb.m_min.y - frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), ColorLightGray);

		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_max.z - frameSizeZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y - frameSizeY, aabb.m_max.z + frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_max.x - frameSizeX, aabb.m_max.y + frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), ColorLightGray);

		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_min.z + frameSizeZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y - frameSizeY, aabb.m_min.z - frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_max.x - frameSizeX, aabb.m_max.y + frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), ColorLightGray);

		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y + frameSizeY, aabb.m_max.z + frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_min.x + frameSizeX, aabb.m_min.y - frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_min.x - frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_max.z - frameSizeZ, 0.f), ColorLightGray);

		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_min.x + frameSizeX, aabb.m_max.y + frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y - frameSizeY, aabb.m_min.z - frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_min.z + frameSizeZ, 0.f), ColorLightGray);

		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_min.x + frameSizeX, aabb.m_max.y + frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y - frameSizeY, aabb.m_max.z + frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_min.x - frameIndentX, aabb.m_max.y + frameIndentY, aabb.m_max.z - frameSizeZ, 0.f), ColorLightGray);

		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_max.x - frameSizeX, aabb.m_min.y - frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y + frameSizeY, aabb.m_min.z - frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_min.z - frameIndentZ, 0.f), v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_min.z + frameSizeZ, 0.f), ColorLightGray);

		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_max.x - frameSizeX, aabb.m_min.y - frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y + frameSizeY, aabb.m_max.z + frameIndentZ, 0.f), ColorLightGray);
		m_gpu->DrawLine3D(v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_max.z + frameIndentZ, 0.f), v4f(aabb.m_max.x + frameIndentX, aabb.m_min.y - frameIndentY, aabb.m_max.z - frameSizeZ, 0.f), ColorLightGray);
	}
}