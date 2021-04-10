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

void window_callbackOnSize(yyWindow* window) {
}

void window_onActivate(yyWindow* window) {
	switch (g_app->m_keyboardModifier) {
	case miKeyboardModifier::None:
	case miKeyboardModifier::Ctrl:
	case miKeyboardModifier::Shift:
	case miKeyboardModifier::ShiftCtrl:
	case miKeyboardModifier::END:
	default:
		break;
	case miKeyboardModifier::Alt:
	case miKeyboardModifier::ShiftAlt:
	case miKeyboardModifier::ShiftCtrlAlt:
	case miKeyboardModifier::CtrlAlt:
	{
		g_app->m_keyboardModifier = miKeyboardModifier::None;
	}
	break;
	}
	g_app->m_isViewportInFocus = false;
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
	m_keyboardModifier = miKeyboardModifier::None;
	m_isViewportInFocus = false;
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
	m_window->m_onSize = window_callbackOnSize;
	m_window->m_onMaximize = window_callbackOnSize;
	m_window->m_onRestore = window_callbackOnSize;
	m_window->m_onActivate = window_onActivate;

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
	m_activeViewport = m_viewport;

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

	while (yyRun(&m_dt))
	{
		_updateKeyboardModifier();
		m_isCursorMove = (m_inputContext->m_mouseDelta.x != 0.f) || (m_inputContext->m_mouseDelta.y != 0.f);
	

		m_isCursorInWindow = false;

		if (math::pointInRect(m_inputContext->m_cursorCoords.x, m_inputContext->m_cursorCoords.y,
			v4f(0.f, 0.f, m_window->m_currentSize.x, m_window->m_currentSize.y)))
		{
			m_isCursorInWindow = true;
		}

		++fps_counter;
		fps_timer += m_dt;

		if (fps_timer > 1.f)
		{
			fps_timer = 0.f;
			fps = fps_counter;
			fps_counter = 0;
		}
		
		m_isCursorInGUI = yyGUIUpdate(m_dt);

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
			break;
		case yySystemState::Run:
		{
			UpdateViewports();

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

void miApplication::_updateKeyboardModifier() {
	s32 ctrl_shift_alt = 0;
	if (m_inputContext->IsKeyHold(yyKey::K_LALT) || m_inputContext->IsKeyHold(yyKey::K_RALT))
		ctrl_shift_alt |= 1;
	if (m_inputContext->IsKeyHold(yyKey::K_LSHIFT) || m_inputContext->IsKeyHold(yyKey::K_RSHIFT))
		ctrl_shift_alt |= 2;
	if (m_inputContext->IsKeyHold(yyKey::K_LCTRL) || m_inputContext->IsKeyHold(yyKey::K_RCTRL))
		ctrl_shift_alt |= 4;

	switch (ctrl_shift_alt)
	{
	default:
	case 0:  m_keyboardModifier = miKeyboardModifier::None;          break;
	case 1:  m_keyboardModifier = miKeyboardModifier::Alt;           break;
	case 2:  m_keyboardModifier = miKeyboardModifier::Shift;         break;
	case 3:  m_keyboardModifier = miKeyboardModifier::ShiftAlt;      break;
	case 4:  m_keyboardModifier = miKeyboardModifier::Ctrl;          break;
	case 5:  m_keyboardModifier = miKeyboardModifier::CtrlAlt;       break;
	case 6:  m_keyboardModifier = miKeyboardModifier::ShiftCtrl;     break;
	case 7:  m_keyboardModifier = miKeyboardModifier::ShiftCtrlAlt;  break;
	}
}

void miApplication::UpdateViewports() {
	if (!m_isCursorInGUI)
	{
		if(m_inputContext->m_wheelDelta)
			m_activeViewport->m_activeCamera->Zoom();
	}

	if (m_isCursorMove && m_isViewportInFocus)
	{
		if (m_inputContext->m_isMMBHold)
		{
			switch (m_keyboardModifier)
			{
			default:
				m_activeViewport->m_activeCamera->PanMove();
				break;
			case miKeyboardModifier::Alt:
				m_activeViewport->m_activeCamera->Rotate();
				break;
			case miKeyboardModifier::CtrlAlt:
				m_activeViewport->m_activeCamera->ChangeFOV();
				break;
			case miKeyboardModifier::ShiftCtrlAlt:
				m_activeViewport->m_activeCamera->RotateZ();
				break;
			}
		}
	}


	if (m_isCursorInWindow && !m_isCursorInGUI)
	{
		if (m_inputContext->m_isMMBDown || m_inputContext->m_isLMBDown)
		{
			m_isViewportInFocus = true;
		}
	}

	if (m_inputContext->m_isLMBUp || m_inputContext->m_isMMBUp)
	{
		m_isViewportInFocus = false;
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
