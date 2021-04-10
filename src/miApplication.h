#ifndef _MI_APP_H_
#define _MI_APP_H_

#include "yy.h"
#include "yy_window.h"
#include "yy_input.h"
#include "math\math.h"

class miGUIManager;
struct miViewport;

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

	miViewport * m_viewport;
	miViewport * m_activeViewport;

	bool m_isCursorInWindow;
	bool m_isCursorInGUI;
	bool m_isCursorMove;

	// надо определить первый клик в зоне вьюпорта. если был то true. потом двигать камеру и объекты
	// только если m_isViewportInFocus == true;
	bool m_isViewportInFocus;
	miKeyboardModifier  m_keyboardModifier;
	void _updateKeyboardModifier();

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
	friend void window_onActivate(yyWindow* window);
	friend void log_writeToFile(const char* message);

};

#endif