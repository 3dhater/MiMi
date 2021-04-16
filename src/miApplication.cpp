#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "yy_color.h"
#include "yy_gui.h"
#include "yy_model.h"

miApplication * g_app = 0;
Mat4 g_emptyMatrix;

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
	m_activeViewportLayout = 0;
	m_previousViewportLayout = 0;
	m_keyboardModifier = miKeyboardModifier::None;
	m_isViewportInFocus = false;
	m_gridModel_perspective1 = 0;
	m_gridModel_perspective2 = 0;
	m_gridModel_top1 = 0;
	m_gridModel_top2 = 0;
	m_gridModel_top1_10 = 0;
	m_gridModel_top2_10 = 0;
	m_gridModel_top1_100 = 0;
	m_gridModel_top2_100 = 0;
	m_gridModel_front1 = 0;
	m_gridModel_front2 = 0;
	m_gridModel_front1_10 = 0;
	m_gridModel_front2_10 = 0;
	m_gridModel_front1_100 = 0;
	m_gridModel_front2_100 = 0;
	m_gridModel_left1 = 0;
	m_gridModel_left2 = 0;
	m_gridModel_left1_10 = 0;
	m_gridModel_left2_10 = 0;
	m_gridModel_left1_100 = 0;
	m_gridModel_left2_100 = 0;

	for (s32 i = 0; i < miViewportLayout_Count; ++i)
	{
		m_viewportLayouts[i] = 0;
	}
}

miApplication::~miApplication() {
	for (s32 i = 0; i < miViewportLayout_Count; ++i)
	{
		if(m_viewportLayouts[i])
			delete m_viewportLayouts[i];
	}
	if (m_gridModel_left1) m_gpu->DeleteModel(m_gridModel_left1);
	if (m_gridModel_left2) m_gpu->DeleteModel(m_gridModel_left2);
	if (m_gridModel_left1_10) m_gpu->DeleteModel(m_gridModel_left1_10);
	if (m_gridModel_left2_10) m_gpu->DeleteModel(m_gridModel_left2_10);
	if (m_gridModel_left1_100) m_gpu->DeleteModel(m_gridModel_left1_100);
	if (m_gridModel_left2_100) m_gpu->DeleteModel(m_gridModel_left2_100);
	if (m_gridModel_front1) m_gpu->DeleteModel(m_gridModel_front1);
	if (m_gridModel_front2) m_gpu->DeleteModel(m_gridModel_front2);
	if (m_gridModel_front1_10) m_gpu->DeleteModel(m_gridModel_front1_10);
	if (m_gridModel_front2_10) m_gpu->DeleteModel(m_gridModel_front2_10);
	if (m_gridModel_front1_100) m_gpu->DeleteModel(m_gridModel_front1_100);
	if (m_gridModel_front2_100) m_gpu->DeleteModel(m_gridModel_front2_100);
	if (m_gridModel_top1) m_gpu->DeleteModel(m_gridModel_top1);
	if (m_gridModel_top2) m_gpu->DeleteModel(m_gridModel_top2);
	if (m_gridModel_top1_10) m_gpu->DeleteModel(m_gridModel_top1_10);
	if (m_gridModel_top2_10) m_gpu->DeleteModel(m_gridModel_top2_10);
	if (m_gridModel_top1_100) m_gpu->DeleteModel(m_gridModel_top1_100);
	if (m_gridModel_top2_100) m_gpu->DeleteModel(m_gridModel_top2_100);
	if (m_gridModel_perspective1) m_gpu->DeleteModel(m_gridModel_perspective1);
	if (m_gridModel_perspective2) m_gpu->DeleteModel(m_gridModel_perspective2);
	if (m_GUIManager) delete m_GUIManager;
	if (m_window) yyDestroy(m_window);
	if (m_engineContext) yyDestroy(m_engineContext);
	if (m_inputContext) yyDestroy(m_inputContext);
}

void miApplication::_initViewports() {
	for (s32 i = 0; i < miViewportLayout_Count; ++i)
	{
		m_viewportLayouts[i] = new miViewportLayout;
		switch (i)
		{
		default:
			YY_PRINT_FAILED;
			break;
		case miViewportLayout_Full:
			m_viewportLayouts[i]->Add(v4f(0.f, 0.f, 1.f, 1.f), miViewportCameraType::Perspective);
			break;
		case miViewportLayout_Standart:
			m_viewportLayouts[i]->Add(v4f(0.f, 0.f, 0.5f, 0.5f), miViewportCameraType::Top);
			m_viewportLayouts[i]->Add(v4f(0.5f, 0.f, 1.0f, 0.5f), miViewportCameraType::Left);
			m_viewportLayouts[i]->Add(v4f(0.f, 0.5f, 0.5f, 1.0f), miViewportCameraType::Front);
			m_viewportLayouts[i]->Add(v4f(0.5f, 0.5f, 1.0f, 1.0f), miViewportCameraType::Perspective);
			break;
		}
	}

	m_activeViewportLayout = m_viewportLayouts[miViewportLayout_Standart];
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

	m_color_viewportBorder = ColorYellow;

	m_color_windowClearColor.setAsByteRed(118);
	m_color_windowClearColor.setAsByteGreen(118);
	m_color_windowClearColor.setAsByteBlue(118);
	m_gpu->SetClearColor(m_color_windowClearColor.m_data[0], m_color_windowClearColor.m_data[1], m_color_windowClearColor.m_data[2], 1.f);
	m_window->SetTitle(m_gpu->GetVideoDriverName());
	
	_initGrid();

	m_GUIManager = new miGUIManager;

	_initViewports();

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
					_callViewportOnSize();
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
		for (u16 i = 0, sz = m_activeViewportLayout->m_viewports.size(); i < sz; ++i)
		{
			auto viewport = m_activeViewportLayout->m_viewports[i];

			viewport->m_isCursorInRect =
				math::pointInRect(m_inputContext->m_cursorCoords.x, m_inputContext->m_cursorCoords.y,
					viewport->m_currentRect);

			if (viewport->m_isCursorInRect)
			{
				if(m_inputContext->m_wheelDelta)
					viewport->m_activeCamera->Zoom();

				if (m_inputContext->m_isLMBDown
					|| m_inputContext->m_isMMBDown
					|| m_inputContext->m_isRMBDown
					|| m_inputContext->m_isX1MBDown
					|| m_inputContext->m_isX2MBDown)
				{
					if (m_activeViewportLayout->m_activeViewport != viewport)
					{
						m_activeViewportLayout->m_activeViewport = viewport;
						return;
					}
				}
			}
		}
	}

	if (m_isCursorMove && m_isViewportInFocus)
	{
		if (m_inputContext->m_isMMBHold)
		{
			switch (m_keyboardModifier)
			{
			default:
				m_activeViewportLayout->m_activeViewport->m_activeCamera->PanMove();
				break;
			case miKeyboardModifier::Alt:
				m_activeViewportLayout->m_activeViewport->m_activeCamera->Rotate();
				break;
			case miKeyboardModifier::CtrlAlt:
				m_activeViewportLayout->m_activeViewport->m_activeCamera->ChangeFOV();
				break;
			case miKeyboardModifier::ShiftCtrlAlt:
				m_activeViewportLayout->m_activeViewport->m_activeCamera->RotateZ();
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
	for (u16 i = 0, sz = m_activeViewportLayout->m_viewports.size(); i < sz; ++i)
	{
		m_gpu->SetScissorRect(v4f(0.f, 0.f, m_window->m_currentSize.x, m_window->m_currentSize.y), m_window);
		m_gpu->SetViewport(0.f, 0.f, m_window->m_currentSize.x, m_window->m_currentSize.y, m_window);

		auto viewport = m_activeViewportLayout->m_viewports[i];
		if (viewport == m_activeViewportLayout->m_activeViewport)
		{
			v4f rect = viewport->m_currentRect;
			rect.x -= miViewportBordeSize;
			rect.y -= miViewportBordeSize;
			rect.z += miViewportBordeSize;
			rect.w += miViewportBordeSize;
			m_gpu->UseDepth(false);
			m_gpu->DrawRectangle(rect, m_color_viewportBorder, m_color_viewportBorder);
		}
		viewport->OnDraw(m_gpu);
	}

	m_gpu->SetScissorRect(v4f(0.f,0.f, m_window->m_currentSize.x, m_window->m_currentSize.y), m_window);
	m_gpu->SetViewport(0.f, 0.f, m_window->m_currentSize.x, m_window->m_currentSize.y, m_window);
}

void miApplication::_callViewportOnSize() {
	for (u16 i = 0, sz = m_activeViewportLayout->m_viewports.size(); i < sz; ++i)
	{
		m_activeViewportLayout->m_viewports[i]->OnWindowSize();
	}
}