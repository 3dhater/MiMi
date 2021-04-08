#ifndef _MI_APP_H_
#define _MI_APP_H_

#include "yy.h"
#include "yy_window.h"
#include "yy_input.h"
#include "math\math.h"

class miGUIManager;
struct miViewport;

class miApplication
{
	yyInputContext * m_inputContext;
	yyEngineContext* m_engineContext;
	yyWindow*        m_window;
	yyVideoDriverAPI* m_gpu;


	miGUIManager* m_GUIManager;

	miViewport * m_viewport;

public:
	miApplication();
	~miApplication();
	
	f32 m_dt;

	bool Init(const char* videoDriver);
	void MainLoop();
	void DrawViewports();
	yyWindow* GetWindowMain();

	friend void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click);
	friend void updateInputContext();
	friend void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character);
	friend void log_writeToFile(const char* message);
};

#endif