#ifndef _MI_APP_H_
#define _MI_APP_H_

#include "yy.h"
#include "yy_window.h"
#include "yy_input.h"
#include "yy_material.h"
#include "math\math.h"

#include "miViewport.h"

class miGUIManager;

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

	yyResource* m_gridModel_perspective1;
	yyResource* m_gridModel_perspective2;
	yyMaterial m_gridModelMaterial;
	void _initGrid();


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
	void DrawViewports();
	yyWindow* GetWindowMain();


	friend struct miViewportCamera;
	friend struct miViewport;
	friend void window_onActivate(yyWindow* window);
	friend void log_writeToFile(const char* message);

};

#endif