#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "yy_color.h"
#include "yy_gui.h"

miApplication * g_app = 0;

void log_writeToFile(const char* message) {
	auto l = strlen(message);
	if (l < 1) return;
	FILE * f = fopen("log.txt", "a");
	fwrite(message, 1, l, f);
	fclose(f);
}

void log_onError(const char* message) {
	fprintf(stderr, message);
	log_writeToFile(message);
}

void log_onInfo(const char* message) {
	fprintf(stdout, message);
	log_writeToFile(message);
}

void log_onWarning(const char* message) {
	fprintf(stderr, message);
	log_writeToFile(message);
}

void window_onCLose(yyWindow* window) {
	yyQuit();
}

void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click) {
	g_app->m_inputContext->m_cursorCoords.x = (f32)x;
	g_app->m_inputContext->m_cursorCoords.y = (f32)y;

	g_app->m_inputContext->m_mouseDelta.x = (f32)x - g_app->m_inputContext->m_cursorCoordsOld.x;
	g_app->m_inputContext->m_mouseDelta.y = (f32)y - g_app->m_inputContext->m_cursorCoordsOld.y;

	g_app->m_inputContext->m_cursorCoordsOld = g_app->m_inputContext->m_cursorCoords;

	if (click & yyWindow_mouseClickMask_LMB_DOWN)
	{
		g_app->m_inputContext->m_isLMBDown = true;
		g_app->m_inputContext->m_isLMBHold = true;
	}
	if (click & yyWindow_mouseClickMask_LMB_UP)
	{
		g_app->m_inputContext->m_isLMBHold = false;
		g_app->m_inputContext->m_isLMBUp = true;
	}
	if (click & yyWindow_mouseClickMask_LMB_DOUBLE)
	{
		g_app->m_inputContext->m_isLMBDbl = true;
	}

	if (click & yyWindow_mouseClickMask_RMB_DOWN)
	{
		g_app->m_inputContext->m_isRMBDown = true;
		g_app->m_inputContext->m_isRMBHold = true;
	}
	if (click & yyWindow_mouseClickMask_RMB_UP)
	{
		g_app->m_inputContext->m_isRMBHold = false;
	}
}

void window_callbackOnSize(yyWindow* window) {
}

void updateInputContext() {
	g_app->m_inputContext->m_isLMBDbl = false;
	g_app->m_inputContext->m_isLMBDown = false;
	g_app->m_inputContext->m_isRMBDown = false;
	g_app->m_inputContext->m_isLMBUp = false;
	g_app->m_inputContext->m_mouseDelta.x = 0.f;
	g_app->m_inputContext->m_mouseDelta.y = 0.f;
	g_app->m_inputContext->m_cursorCoordsForGUI = g_app->m_inputContext->m_cursorCoords;
	memset(g_app->m_inputContext->m_key_pressed, 0, sizeof(u8) * 256);
	memset(g_app->m_inputContext->m_key_hit, 0, sizeof(u8) * 256);
}

void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character) {
	if (isPress)
	{
		if (key < 256)
		{
			g_app->m_inputContext->m_key_hold[key] = 1;
			g_app->m_inputContext->m_key_hit[key] = 1;
		}
	}
	else
	{
		if (key < 256)
		{
			g_app->m_inputContext->m_key_hold[key] = 0;
			g_app->m_inputContext->m_key_pressed[key] = 1;
		}
	}
}

int main(int argc, char* argv[]) {

	yyPtr<miApplication> app = yyCreate<miApplication>();

	{
		const char * videoDriverType = "vdd3d11.dll"; // for example read name from .ini
		yyStringA videoDriverTypeStr = videoDriverType;
		for (int i = 0; i < argc; ++i)
		{
			if (strcmp(argv[i], "-vid") == 0)
			{
				++i;
				if (i < argc)
				{
					videoDriverTypeStr = argv[i];
				}
			}
		}

		if (!app.m_data->Init(videoDriverTypeStr.c_str())) {
			YY_PRINT_FAILED;
			return 1;
		}
	}

	app.m_data->MainLoop();

	return 0;
}

miApplication::miApplication() {
	m_inputContext = 0;
	m_engineContext = 0;
	m_window = 0;
	g_app = this;
	m_dt = 0.f;
	m_gpu = 0;
	m_GUIManager = 0;
	m_viewport = 0;
}

miApplication::~miApplication() {
	if (m_viewport) delete m_viewport;
	if (m_GUIManager) delete m_GUIManager;
	if (m_window) yyDestroy(m_window);
	if (m_engineContext) yyDestroy(m_engineContext);
	if (m_inputContext) yyDestroy(m_inputContext);
}

bool miApplication::Init(const char* videoDriver) {
	{
		FILE * f = fopen("log.txt", "w");
		fwrite(" ", 1, 1, f);
		fclose(f);
	}


	m_inputContext = yyCreate<yyInputContext>();
	m_engineContext = yyCreate<yyEngineContext>();
	m_engineContext->init(m_inputContext);

	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onWarning);

	m_window = yyCreate<yyWindow>();
	u32 windowStyle = 0;
	if (!m_window->init(800, 600, windowStyle))
	{
		YY_PRINT_FAILED;
		return false;
	}

	yySetMainWindow(m_window);
	m_window->m_onClose = window_onCLose;
	m_window->m_onMouseButton = window_callbackMouse;
	m_window->m_onKeyboard = window_callbackKeyboard;
	m_window->m_onSize = window_callbackOnSize;
	m_window->m_onMaximize = window_callbackOnSize;
	m_window->m_onRestore = window_callbackOnSize;

	if (!yyInitVideoDriver(videoDriver, m_window))
	{
		yyLogWriteWarning("Can't load video driver : %s\n", videoDriver);
		for (auto & entry : yy_fs::directory_iterator(yy_fs::current_path()))
		{
			auto path = entry.path();
			if (path.has_extension())
			{
				auto ex = path.extension();
				if (ex == ".dll" && yyIsValidVideoDriver(path.generic_string().c_str()))
				{
					yyLogWriteWarning("Trying to load video driver : %s\n", path.generic_string().c_str());

					if (yyInitVideoDriver(path.generic_string().c_str(), m_window))
					{
						goto vidOk;
					}
					else
					{
						yyLogWriteWarning("Can't load video driver : %s\n", path.generic_string().c_str());
					}
				}
			}
		}
		YY_PRINT_FAILED;
		return false;
	}

vidOk:

	m_gpu = yyGetVideoDriverAPI();
	m_gpu->UseVSync(true);
	yySetDefaultTexture(yyGetTextureResource("../res/gui/white.dds", false, false, true));

	yyColor clColor;
	clColor.setAsByteRed(118);
	clColor.setAsByteGreen(118);
	clColor.setAsByteBlue(118);
	m_gpu->SetClearColor(clColor.m_data[0], clColor.m_data[1], clColor.m_data[2], 1.f);
	m_window->SetTitle(m_gpu->GetVideoDriverName());

	m_GUIManager = new miGUIManager;

	m_viewport = new miViewport;

	yyGUIRebuild();

	return true;
}

yyWindow* miApplication::GetWindowMain() {
	return m_window;
}

void miApplication::MainLoop() {
	f32 fps_timer = 0.f;
	u32 fps = 0;
	u32 fps_counter = 0;

	yyEvent currentEvent;

	bool run = true;
	while (run)
	{
		static u64 t1 = 0;
		u64 t2 = yyGetTime();
		f32 m_tick = f32(t2 - t1);
		t1 = t2;
		m_dt = m_tick / 1000.f;


		++fps_counter;
		fps_timer += m_dt;

		if (fps_timer > 1.f)
		{
			fps_timer = 0.f;
			fps = fps_counter;
			fps_counter = 0;
		}

		updateInputContext();

#ifdef YY_PLATFORM_WINDOWS
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#else
#error For windows
#endif
		POINT cursorPoint;
		GetCursorPos(&cursorPoint);
		ScreenToClient(m_window->m_hWnd, &cursorPoint);
		m_inputContext->m_cursorCoords.x = (f32)cursorPoint.x;
		m_inputContext->m_cursorCoords.y = (f32)cursorPoint.y;

		yyGUIUpdate(m_dt);

		while (yyPollEvent(currentEvent))
		{
			switch (currentEvent.m_type)
			{
			default:
			case yyEventType::Engine:
				break;
			case yyEventType::System:
				break;
			case yyEventType::Window: {
				if (currentEvent.m_event_window.m_event == yyEvent_Window::size_changed) {
					yyGetVideoDriverAPI()->UpdateMainRenderTarget(m_window->m_currentSize,
						v2f(m_window->m_currentSize.x, m_window->m_currentSize.y));
					yyGUIRebuild();
				}
			}break;
			}
		}

		switch (*m_engineContext->m_state)
		{
		default:
			run = false;
			break;
		case yySystemState::Run:
		{
			m_gpu->BeginDraw();
			m_gpu->ClearAll();

			DrawViewports();

			yyGUIDrawAll();
			m_gpu->EndDraw();
			m_gpu->SwapBuffers();
		}
		}
	}
}

void miApplication::DrawViewports() {
	if (!m_viewport)
		return;
	m_viewport->m_activeCamera->Update();

	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::View, m_viewport->m_activeCamera->m_viewMatrix);
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, m_viewport->m_activeCamera->m_projectionMatrix);
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, 
		m_viewport->m_activeCamera->m_projectionMatrix * m_viewport->m_activeCamera->m_viewMatrix);
	m_gpu->DrawLine3D(v4f(-10.f, 0.f, 0.f, 0.f), v4f(10.f, 0.f, 0.f, 0.f), ColorRed);
	m_gpu->DrawLine3D(v4f(0.f, -10.f, 0.f, 0.f), v4f(0.f, 10.f, 0.f, 0.f), ColorGreen);
	m_gpu->DrawLine3D(v4f(0.f, 0.f, -10.f, 0.f), v4f(0.f, 0.f, 10.f, 0.f), ColorBlue);
	
}
