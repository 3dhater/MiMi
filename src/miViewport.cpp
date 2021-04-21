#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "miViewportCamera.h"
#include "miShortcutManager.h"

extern miApplication * g_app;
extern Mat4 g_emptyMatrix;

miViewport::miViewport(miViewportCameraType vct){
	m_gpu = yyGetVideoDriverAPI();

	m_drawMode = this->Draw_Material;
	m_drawGrid = true;
	m_index = 0;
	m_isCursorInRect = false;
	m_gui_text_vpName = 0;
	m_gui_button_resetCamera = 0;
	m_gui_button_viewport = 0;

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

	m_creationRect = v4f(0.f, 0.f, 1.f, 1.f);
	m_currentRect = m_creationRect;
	m_currentRectSize = v2f(800.f, 600.f);
	m_cameraType = vct;
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
	m_gui_group->SetRectsFromBuildRect();

	m_gui_text_vpName = yyGUICreateText(v2f(10.f, 2.f), g_app->m_GUIManager->GetFont(miGUIManager::Font::Default),
		L"T", 0);
	m_gui_text_vpName->m_align = m_gui_text_vpName->AlignLeftTop;
	m_gui_text_vpName->IgnoreInput(true);
	//m_gui_text_vpName->m_onClick = miViewport_onClick_viewportName;
	m_gui_text_vpName->m_ignoreSetCursorInGUI = true;
	m_gui_text_vpName->m_userData = this;
	m_gui_group->AddElement(m_gui_text_vpName);

	f32 buttonPositionX = 70.f;
	f32 buttonWidth = 17.f;

	v4i region(0,72,16,88);
	m_gui_button_resetCamera = yyGUICreateButton(v4f(buttonPositionX, 2.f, buttonPositionX + buttonWidth, 19.f),
		yyGetTextureResource("../res/gui/icons.png", false, false, true), m_index, 0, &region);
	m_gui_button_resetCamera->m_isAnimated = true;
	region.set(24,72,40,88);
	m_gui_button_resetCamera->SetMouseHoverTexture(yyGetTextureResource("../res/gui/icons.png", false, false, true), &region);
	m_gui_group->AddElement(m_gui_button_resetCamera);
	m_gui_button_resetCamera->SetOpacity(0.19f, 0);
	m_gui_button_resetCamera->SetOpacity(0.19f, 1);
	m_gui_button_resetCamera->SetOpacity(0.19f, 2);
	m_gui_button_resetCamera->m_onClick = miViewport_onClick_cameraReset;
	m_gui_button_resetCamera->m_userData = this;

	buttonPositionX += buttonWidth;

	region.set(0, 96, 16, 112);
	m_gui_button_viewport = yyGUICreateButton(v4f(buttonPositionX, 2.f, buttonPositionX + buttonWidth, 19.f),
		yyGetTextureResource("../res/gui/icons.png", false, false, true), m_index, 0, &region);
	m_gui_button_viewport->m_isAnimated = true;
	region.set(24, 96, 40, 112);
	m_gui_button_viewport->SetMouseHoverTexture(yyGetTextureResource("../res/gui/icons.png", false, false, true), &region);
	m_gui_group->AddElement(m_gui_button_viewport);
	m_gui_button_viewport->SetOpacity(0.19f, 0);
	m_gui_button_viewport->SetOpacity(0.19f, 1);
	m_gui_button_viewport->SetOpacity(0.19f, 2);
	m_gui_button_viewport->m_onClick = miViewport_onClick_viewport;
	m_gui_button_viewport->m_userData = this;
	
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


	m_currentRectSize.x = m_currentRect.z - m_currentRect.x;
	m_currentRectSize.y = m_currentRect.w - m_currentRect.y;
	m_activeCamera->m_aspect = m_currentRectSize.x / m_currentRectSize.y;

	m_currentRect.x += miViewportBordeSize;
	m_currentRect.y += miViewportBordeSize;
	m_currentRect.z -= miViewportBordeSize;
	m_currentRect.w -= miViewportBordeSize;

	/*m_gui_group->m_activeAreaRect = m_currentRect;
	m_gui_group->m_activeAreaRect_global = m_currentRect;
	m_gui_group->Rebuild();*/
	

	/*printf("%f %f %f %f - %f\n", 
		m_currentRect.x, m_currentRect.y, m_currentRect.z, m_currentRect.w, windowSizeX_1);*/
}

void miViewport::OnDraw() {
	m_activeCamera->Update();

	m_gpu->UseDepth(false);
	m_gpu->DrawRectangle(m_currentRect, g_app->m_color_windowClearColor, g_app->m_color_windowClearColor);

	m_gpu->SetScissorRect(m_currentRect, g_app->m_window);
	m_gpu->SetViewport(m_currentRect.x, m_currentRect.y, m_currentRectSize.x, m_currentRectSize.y, g_app->m_window);

	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::View, m_activeCamera->m_viewMatrix);
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, m_activeCamera->m_projectionMatrix);
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection,
		m_activeCamera->m_projectionMatrix * m_activeCamera->m_viewMatrix);

	/*m_gpu->DrawLine3D(v4f(-10.f, 0.f, 0.f, 0.f), v4f(10.f, 0.f, 0.f, 0.f), ColorRed);
	m_gpu->DrawLine3D(v4f(0.f, -10.f, 0.f, 0.f), v4f(0.f, 10.f, 0.f, 0.f), ColorGreen);
	m_gpu->DrawLine3D(v4f(0.f, 0.f, -10.f, 0.f), v4f(0.f, 0.f, 10.f, 0.f), ColorBlue);*/

	if (m_drawGrid)
	{
		_drawGrid();
	}
}

void miViewport::SetDrawMode(DrawMode dm) {
	m_drawMode = dm;
}

void miViewport::SetDrawGrid(bool v) {
	m_drawGrid = v;
}

void miViewport::_drawGrid() {
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, g_emptyMatrix);
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection,
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
		bool front = ((m_activeCamera->m_rotationPlatform.y < -math::PIHalf) &&
			(m_activeCamera->m_rotationPlatform.y > -math::PIPlusHalf));

		if (m_activeCamera->m_positionPlatform.w < 40.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_left1) : m_gpu->SetModel(g_app->m_gridModel_left2);
		else if (m_activeCamera->m_positionPlatform.w < 140.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_left1_10) : m_gpu->SetModel(g_app->m_gridModel_left2_10);
		else
			front ? m_gpu->SetModel(g_app->m_gridModel_left1_100) : m_gpu->SetModel(g_app->m_gridModel_left2_100);
	}break;
	case miViewportCameraType::Right: {
		bool front = ((m_activeCamera->m_rotationPlatform.y > math::PIHalf) &&
			(m_activeCamera->m_rotationPlatform.y < math::PIPlusHalf));

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
		bool front = ((m_activeCamera->m_rotationPlatform.y > math::PIHalf) &&
			(m_activeCamera->m_rotationPlatform.y < math::PIPlusHalf));

		if (m_activeCamera->m_positionPlatform.w < 40.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_front1) : m_gpu->SetModel(g_app->m_gridModel_front2);
		else if (m_activeCamera->m_positionPlatform.w < 140.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_front1_10) : m_gpu->SetModel(g_app->m_gridModel_front2_10);
		else
			front ? m_gpu->SetModel(g_app->m_gridModel_front1_100) : m_gpu->SetModel(g_app->m_gridModel_front2_100);
	}break;
	case miViewportCameraType::Back: {
		bool front = ((m_activeCamera->m_rotationPlatform.y > -math::PIHalf) &&
			(m_activeCamera->m_rotationPlatform.y < math::PIHalf));

		if (m_activeCamera->m_positionPlatform.w < 40.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_front1) : m_gpu->SetModel(g_app->m_gridModel_front2);
		else if (m_activeCamera->m_positionPlatform.w < 140.f)
			front ? m_gpu->SetModel(g_app->m_gridModel_front1_10) : m_gpu->SetModel(g_app->m_gridModel_front2_10);
		else
			front ? m_gpu->SetModel(g_app->m_gridModel_front1_100) : m_gpu->SetModel(g_app->m_gridModel_front2_100);
	}break;
	}

	m_gpu->SetMaterial(&g_app->m_gridModelMaterial);
	m_gpu->UseDepth(true);
	m_gpu->Draw();
}

