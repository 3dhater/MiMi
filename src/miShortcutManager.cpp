#include "miApplication.h"
#include "miShortcutManager.h"

extern miApplication * g_app;

miShortcutManager::miShortcutManager() {
	SetCommand(miShortcutCommandType::viewport_cameraReset, L"viewport_cameraReset", miKeyboardModifier::None, yyKey::END);
	SetCommand(miShortcutCommandType::viewport_cameraMoveToSelection, L"viewport_cameraMoveToSelection", miKeyboardModifier::None, yyKey::K_Z);
	SetCommand(miShortcutCommandType::viewport_viewPerspective, L"viewport_viewPerspective", miKeyboardModifier::None, yyKey::K_P);
	SetCommand(miShortcutCommandType::viewport_viewTop, L"viewport_viewTop", miKeyboardModifier::None, yyKey::K_T);
	SetCommand(miShortcutCommandType::viewport_viewBottom, L"viewport_viewBottom", miKeyboardModifier::None, yyKey::END);
	SetCommand(miShortcutCommandType::viewport_viewLeft, L"viewport_viewLeft", miKeyboardModifier::None, yyKey::K_L);
	SetCommand(miShortcutCommandType::viewport_viewRight, L"viewport_viewRight", miKeyboardModifier::None, yyKey::END);
	SetCommand(miShortcutCommandType::viewport_viewFront, L"viewport_viewFront", miKeyboardModifier::None, yyKey::K_F);
	SetCommand(miShortcutCommandType::viewport_viewBack, L"viewport_viewBack", miKeyboardModifier::None, yyKey::END);
	SetCommand(miShortcutCommandType::viewport_toggleGrid, L"viewport_toggleGrid", miKeyboardModifier::None, yyKey::K_G);
	SetCommand(miShortcutCommandType::viewport_toggleFullView, L"viewport_toggleFullView", miKeyboardModifier::Alt, yyKey::K_W);
	SetCommand(miShortcutCommandType::viewport_dmMaterial, L"viewport_dmMaterial", miKeyboardModifier::None, yyKey::END);
	SetCommand(miShortcutCommandType::viewport_dmMaterialWireframe, L"viewport_dmMaterialWireframe", miKeyboardModifier::None, yyKey::END);
	SetCommand(miShortcutCommandType::viewport_dmWireframe, L"viewport_dmWireframe", miKeyboardModifier::None, yyKey::END);
	SetCommand(miShortcutCommandType::viewport_toggleDMMaterial, L"viewport_toggleDMMaterial", miKeyboardModifier::None, yyKey::K_F1);
	SetCommand(miShortcutCommandType::viewport_toggleDMWireframe, L"viewport_toggleDMWireframe", miKeyboardModifier::None, yyKey::K_F2);
	SetCommand(miShortcutCommandType::viewport_toggleDrawAABB, L"viewport_toggleDrawAABB", miKeyboardModifier::None, yyKey::END);
	SetCommand(miShortcutCommandType::transfromMode_NoTransform, L"transfromMode_NoTransform", miKeyboardModifier::None, yyKey::K_Q);
	SetCommand(miShortcutCommandType::transfromMode_Move, L"transfromMode_Move", miKeyboardModifier::None, yyKey::K_W);
	SetCommand(miShortcutCommandType::transfromMode_Scale, L"transfromMode_Scale", miKeyboardModifier::None, yyKey::K_E);
	SetCommand(miShortcutCommandType::transfromMode_Rotate, L"transfromMode_Rotate", miKeyboardModifier::None, yyKey::K_R);
	SetCommand(miShortcutCommandType::select_selectAll, L"select_selectAll", miKeyboardModifier::Ctrl, yyKey::K_A);
	SetCommand(miShortcutCommandType::select_deselectAll, L"select_deselectAll", miKeyboardModifier::Ctrl, yyKey::K_D);
	SetCommand(miShortcutCommandType::select_invertSelection, L"select_invertSelection", miKeyboardModifier::Ctrl, yyKey::K_I);
	SetCommand(miShortcutCommandType::editMode_Vertex, L"editMode_Vertex", miKeyboardModifier::None, yyKey::K_1);
	SetCommand(miShortcutCommandType::editMode_Edge, L"editMode_Edge", miKeyboardModifier::None, yyKey::K_2);
	SetCommand(miShortcutCommandType::editMode_Polygon, L"editMode_Polygon", miKeyboardModifier::None, yyKey::K_3);
}

miShortcutManager::~miShortcutManager() {
}

bool miShortcutManager::IsShortcutActive(miShortcutCommandType type) {
	auto & command = m_commands[(u32)type];
	if (command.m_key == yyKey::END)
		return false;

	if(command.m_keyMod != g_app->m_keyboardModifier)
		return false;

	return g_app->m_inputContext->IsKeyHit(command.m_key);
}

void miShortcutManager::SetCommand(miShortcutCommandType type, const wchar_t* commandname, 
	miKeyboardModifier kmod, yyKey k) {
	m_commands[(u32)type].m_type = type;
	m_commands[(u32)type].m_name = commandname;
	m_commands[(u32)type].m_key = k;
	m_commands[(u32)type].m_keyMod = kmod;
	m_commands[(u32)type].m_text.clear();

	if (k != yyKey::END)
	{
		switch (kmod)
		{
		case miKeyboardModifier::None:break;
		case miKeyboardModifier::Ctrl:m_commands[(u32)type].m_text += L"Ctrl+";break;
		case miKeyboardModifier::Alt:m_commands[(u32)type].m_text += L"Alt+";break;
		case miKeyboardModifier::Shift:m_commands[(u32)type].m_text += L"Shift+";break;
		case miKeyboardModifier::ShiftAlt:m_commands[(u32)type].m_text += L"Shift+Alt+";break;
		case miKeyboardModifier::ShiftCtrl:m_commands[(u32)type].m_text += L"Shift+Ctrl+";break;
		case miKeyboardModifier::ShiftCtrlAlt:m_commands[(u32)type].m_text += L"Shift+Ctrl+Alt+";break;
		case miKeyboardModifier::CtrlAlt:m_commands[(u32)type].m_text += L"Ctrl+Alt+";break;
		case miKeyboardModifier::END:break;
		default:break;
		}

		switch (k)
		{
		case yyKey::K_PAUSE:m_commands[(u32)type].m_text += L"Pause";break;
		case yyKey::K_SPACE:m_commands[(u32)type].m_text += L"Space";break;
		case yyKey::K_PGUP:m_commands[(u32)type].m_text += L"PageUp";break;
		case yyKey::K_PGDOWN:m_commands[(u32)type].m_text += L"PageDown";break;
		case yyKey::K_END:m_commands[(u32)type].m_text += L"End";break;
		case yyKey::K_HOME:m_commands[(u32)type].m_text += L"Home";break;
		case yyKey::K_LEFT:m_commands[(u32)type].m_text += L"Left";break;
		case yyKey::K_UP:m_commands[(u32)type].m_text += L"Up";break;
		case yyKey::K_RIGHT:m_commands[(u32)type].m_text += L"Right";break;
		case yyKey::K_DOWN:m_commands[(u32)type].m_text += L"Down";break;
		case yyKey::K_PRTSCR:m_commands[(u32)type].m_text += L"PrtScr";break;
		case yyKey::K_INSERT:m_commands[(u32)type].m_text += L"Insert";break;
		case yyKey::K_0:m_commands[(u32)type].m_text += L"0";break;
		case yyKey::K_1:m_commands[(u32)type].m_text += L"1";break;
		case yyKey::K_2:m_commands[(u32)type].m_text += L"2";break;
		case yyKey::K_3:m_commands[(u32)type].m_text += L"3";break;
		case yyKey::K_4:m_commands[(u32)type].m_text += L"4";break;
		case yyKey::K_5:m_commands[(u32)type].m_text += L"5";break;
		case yyKey::K_6:m_commands[(u32)type].m_text += L"6";break;
		case yyKey::K_7:m_commands[(u32)type].m_text += L"7";break;
		case yyKey::K_8:m_commands[(u32)type].m_text += L"8";break;
		case yyKey::K_9:m_commands[(u32)type].m_text += L"9";break;
		case yyKey::K_A:m_commands[(u32)type].m_text += L"A";break;
		case yyKey::K_B:m_commands[(u32)type].m_text += L"B";break;
		case yyKey::K_C:m_commands[(u32)type].m_text += L"C";break;
		case yyKey::K_D:m_commands[(u32)type].m_text += L"D";break;
		case yyKey::K_E:m_commands[(u32)type].m_text += L"E";break;
		case yyKey::K_F:m_commands[(u32)type].m_text += L"F";break;
		case yyKey::K_G:m_commands[(u32)type].m_text += L"G";break;
		case yyKey::K_H:m_commands[(u32)type].m_text += L"H";break;
		case yyKey::K_I:m_commands[(u32)type].m_text += L"I";break;
		case yyKey::K_J:m_commands[(u32)type].m_text += L"J";break;
		case yyKey::K_K:m_commands[(u32)type].m_text += L"K";break;
		case yyKey::K_L:m_commands[(u32)type].m_text += L"L";break;
		case yyKey::K_M:m_commands[(u32)type].m_text += L"M";break;
		case yyKey::K_N:m_commands[(u32)type].m_text += L"N";break;
		case yyKey::K_O:m_commands[(u32)type].m_text += L"O";break;
		case yyKey::K_P:m_commands[(u32)type].m_text += L"P";break;
		case yyKey::K_Q:m_commands[(u32)type].m_text += L"Q";break;
		case yyKey::K_R:m_commands[(u32)type].m_text += L"R";break;
		case yyKey::K_S:m_commands[(u32)type].m_text += L"S";break;
		case yyKey::K_T:m_commands[(u32)type].m_text += L"T";break;
		case yyKey::K_U:m_commands[(u32)type].m_text += L"U";break;
		case yyKey::K_V:m_commands[(u32)type].m_text += L"V";break;
		case yyKey::K_W:m_commands[(u32)type].m_text += L"W";break;
		case yyKey::K_X:m_commands[(u32)type].m_text += L"X";break;
		case yyKey::K_Y:m_commands[(u32)type].m_text += L"Y";break;
		case yyKey::K_Z:m_commands[(u32)type].m_text += L"Z";break;
		case yyKey::K_NUM_0:m_commands[(u32)type].m_text += L"Num0";break;
		case yyKey::K_NUM_1:m_commands[(u32)type].m_text += L"Num1";break;
		case yyKey::K_NUM_2:m_commands[(u32)type].m_text += L"Num2";break;
		case yyKey::K_NUM_3:m_commands[(u32)type].m_text += L"Num3";break;
		case yyKey::K_NUM_4:m_commands[(u32)type].m_text += L"Num4";break;
		case yyKey::K_NUM_5:m_commands[(u32)type].m_text += L"Num5";break;
		case yyKey::K_NUM_6:m_commands[(u32)type].m_text += L"Num6";break;
		case yyKey::K_NUM_7:m_commands[(u32)type].m_text += L"Num7";break;
		case yyKey::K_NUM_8:m_commands[(u32)type].m_text += L"Num8";break;
		case yyKey::K_NUM_9:m_commands[(u32)type].m_text += L"Num9";break;
		case yyKey::K_NUM_MUL:m_commands[(u32)type].m_text += L"Num*";break;
		case yyKey::K_NUM_ADD:m_commands[(u32)type].m_text += L"Num+";break;
		case yyKey::K_NUM_SUB:m_commands[(u32)type].m_text += L"Num-";break;
		case yyKey::K_NUM_DOT:m_commands[(u32)type].m_text += L"Num.";break;
		case yyKey::K_NUM_SLASH:m_commands[(u32)type].m_text += L"Num/";break;
		case yyKey::K_F1:m_commands[(u32)type].m_text += L"F1";break;
		case yyKey::K_F2:m_commands[(u32)type].m_text += L"F2";break;
		case yyKey::K_F3:m_commands[(u32)type].m_text += L"F3";break;
		case yyKey::K_F4:m_commands[(u32)type].m_text += L"F4";break;
		case yyKey::K_F5:m_commands[(u32)type].m_text += L"F5";break;
		case yyKey::K_F6:m_commands[(u32)type].m_text += L"F6";break;
		case yyKey::K_F7:m_commands[(u32)type].m_text += L"F7";break;
		case yyKey::K_F8:m_commands[(u32)type].m_text += L"F8";break;
		case yyKey::K_F9:m_commands[(u32)type].m_text += L"F9";break;
		case yyKey::K_F10:m_commands[(u32)type].m_text += L"F10";break;
		case yyKey::K_F11:m_commands[(u32)type].m_text += L"F11";break;
		case yyKey::K_F12:m_commands[(u32)type].m_text += L"F12";break;
		case yyKey::K_COLON:m_commands[(u32)type].m_text += L";";break;
		case yyKey::K_ADD:m_commands[(u32)type].m_text += L"=";break;
		case yyKey::K_COMMA:m_commands[(u32)type].m_text += L",";break;
		case yyKey::K_SUB:m_commands[(u32)type].m_text += L"-";break;
		case yyKey::K_DOT:m_commands[(u32)type].m_text += L".";break;
		case yyKey::K_SLASH:m_commands[(u32)type].m_text += L"/";break;
		case yyKey::K_TILDE:m_commands[(u32)type].m_text += L"`";break;
		case yyKey::K_FIGURE_OPEN:m_commands[(u32)type].m_text += L"[";break;
		case yyKey::K_FIGURE_CLOSE:m_commands[(u32)type].m_text += L"]";break;
		case yyKey::K_QUOTE:m_commands[(u32)type].m_text += L"'";break;
		default:break;
		}
	}
}

const wchar_t* miShortcutManager::GetText(miShortcutCommandType type) {
	return m_commands[(u32)type].m_text.data();
}