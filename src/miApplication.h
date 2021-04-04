#ifndef _MI_APP_H_
#define _MI_APP_H_

#include "yy.h"
#include "yy_window.h"
#include "yy_input.h"

class miGUIManager;

class miApplication
{
	yyInputContext * m_inputContext;
	yyEngineContext* m_engineContext;
	yyWindow*        m_window;
	yyVideoDriverAPI* m_gpu;


	miGUIManager* m_GUIManager;

public:
	miApplication();
	~miApplication();
	
	f32 m_dt;

	bool Init(const char* videoDriver);
	void MainLoop();

	friend void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click);
	friend void updateInputContext();
	friend void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character);
	friend void log_writeToFile(const char* message);
};

#endif