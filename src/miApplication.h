#ifndef _MI_APP_H_
#define _MI_APP_H_

#include "yy.h"
#include "yy_window.h"
#include "yy_input.h"
#include "yy_material.h"
#include "math\math.h"

#include "miViewport.h"
#include "miPopup.h"

#define miViewportBordeSize 1.f

#define miCommandID_CameraReset 1
#define miCommandID_CameraMoveToSelection 2
#define miCommandID_ViewportViewPerspective 3
#define miCommandID_ViewportViewFront 4
#define miCommandID_ViewportViewBack 5
#define miCommandID_ViewportViewTop 6
#define miCommandID_ViewportViewBottom 7
#define miCommandID_ViewportViewLeft 8
#define miCommandID_ViewportViewRight 9
#define miCommandID_ViewportToggleFullView 10
#define miCommandID_ViewportToggleGrid 11
#define miCommandID_ViewportDrawMaterial 12
#define miCommandID_ViewportDrawMaterialWireframe 13
#define miCommandID_ViewportDrawWireframe 14

class miGUIManager;
class miShortcutManager;

enum class miKeyboardModifier : u32
{
	None,
	Ctrl,
	Alt,
	Shift,
	ShiftAlt,
	ShiftCtrl,
	ShiftCtrlAlt,
	CtrlAlt,

	END
};

class miApplication
{
	yyInputContext * m_inputContext;
	yyEngineContext* m_engineContext;
	yyWindow*        m_window;
	yyVideoDriverAPI* m_gpu;


	miGUIManager* m_GUIManager;
	miShortcutManager* m_shortcutManager;

	yyResource* m_gridModel_perspective1;
	yyResource* m_gridModel_perspective2;
	yyResource* m_gridModel_top1;
	yyResource* m_gridModel_top2;
	yyResource* m_gridModel_top1_10;
	yyResource* m_gridModel_top2_10;
	yyResource* m_gridModel_top1_100;
	yyResource* m_gridModel_top2_100;
	yyResource* m_gridModel_front1;
	yyResource* m_gridModel_front2;
	yyResource* m_gridModel_front1_10;
	yyResource* m_gridModel_front2_10;
	yyResource* m_gridModel_front1_100;
	yyResource* m_gridModel_front2_100;
	yyResource* m_gridModel_left1;
	yyResource* m_gridModel_left2;
	yyResource* m_gridModel_left1_10;
	yyResource* m_gridModel_left2_10;
	yyResource* m_gridModel_left1_100;
	yyResource* m_gridModel_left2_100;
	yyMaterial m_gridModelMaterial;
	void _initGrid();


	yyColor m_color_windowClearColor;
	yyColor m_color_viewportBorder;

	bool m_isCursorInWindow;
	bool m_isCursorInGUI;
	bool m_isCursorMove;

	// надо определить первый клик в зоне вьюпорта. если был то true. потом двигать камеру и объекты
	// только если m_isViewportInFocus == true;
	bool m_isViewportInFocus;
	miKeyboardModifier  m_keyboardModifier;
	void _updateKeyboardModifier();
	void _callViewportOnSize();


	miViewportLayout* m_activeViewportLayout;
	miViewportLayout* m_previousViewportLayout; // save here when Alt + W
	miViewportLayout* m_viewportLayouts[miViewportLayout_Count];
	void _initViewports();


public:
	miApplication();
	~miApplication();
	
	f32 m_dt;

	bool Init(const char* videoDriver);
	void MainLoop();
	void UpdateViewports();
	void ProcessShortcuts();
	void DrawViewports();
	yyWindow* GetWindowMain();

	miViewport* m_popupViewport; // set this when click on button whith popup menu
	miPopup m_popup_ViewportCamera;
	miPopup m_popup_ViewportParameters;
	void ShowPopupAtCursor(miPopup* popup);
	
	void CommandCameraReset(miViewport* vp);
	void CommandCameraMoveToSelection(miViewport* vp);
	void CommandViewportChangeView(miViewport* vp, miViewportCameraType);
	void CommandViewportToggleFullView(miViewport* vp);
	void CommandViewportToggleGrid(miViewport* vp);
	void CommandViewportSetDrawMode(miViewport* vp, miViewport::DrawMode);


	friend struct miViewportCamera;
	friend struct miViewport;
	friend class miShortcutManager;
	friend void window_onActivate(yyWindow* window);
	friend void log_writeToFile(const char* message);

};

#endif