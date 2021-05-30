#ifndef _MI_SHRTCTMNGR_H_
#define _MI_SHRTCTMNGR_H_

enum class miShortcutCommandType : u32 {
	viewport_cameraReset,
	viewport_cameraMoveToSelection,
	viewport_viewPerspective,
	viewport_viewTop,
	viewport_viewBottom,
	viewport_viewLeft,
	viewport_viewRight,
	viewport_viewFront,
	viewport_viewBack,
	viewport_toggleGrid,
	viewport_toggleFullView,
	viewport_dmMaterial,
	viewport_dmMaterialWireframe,
	viewport_dmWireframe,
	viewport_toggleDMMaterial,
	viewport_toggleDMWireframe,
	transfromMode_NoTransform,
	transfromMode_Move,
	transfromMode_Scale,
	transfromMode_Rotate,

	count
};

struct miShortcutCommand {
	miShortcutCommand() {
		m_type = miShortcutCommandType::count;
		m_keyMod = miKeyboardModifier::None;
		m_key = yyKey::END;
	}
	miShortcutCommandType m_type;
	yyStringW m_text; // Z
	yyStringW m_name; // "viewport_cameraMoveToSelection"
	miKeyboardModifier m_keyMod;
	yyKey m_key;
};

class miShortcutManager
{
	miShortcutCommand m_commands[(u32)miShortcutCommandType::count];
public:
	miShortcutManager();
	~miShortcutManager();

	void SetCommand(miShortcutCommandType, const wchar_t* commandname, miKeyboardModifier, yyKey);
	bool IsShortcutActive(miShortcutCommandType);
	const wchar_t* GetText(miShortcutCommandType);
};


#endif