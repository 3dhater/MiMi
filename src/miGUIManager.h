#ifndef _MI_GUIMANAGER_H_
#define _MI_GUIMANAGER_H_

#include "yy_gui.h"

//struct miGUIShowMenuInfo

struct miGUIMainMenuItem 
{
	miGUIMainMenuItem() {
		m_button = 0;
		//m_text = 0;
		//m_onClick = 0;
	}
	~miGUIMainMenuItem() {
	}
	yyGUIButton* m_button;
	//yyGUIText* m_text;
	v4f m_activeArea_noActive;
	v4f m_activeArea_Active;
	//yyGUICallback m_onClick;
};

struct miGUIMainMenuMenuGroup
{
	miGUIMainMenuMenuGroup(s32 mainButtonID) {
		m_mainButtonID = mainButtonID;
	};
	~miGUIMainMenuMenuGroup() {};

	void addButton(const wchar_t* text, const v4f& rect, s32 id, yyGUIDrawGroup* dg);

	yyArraySmall<yyGUIButton*> m_buttons;
	s32 m_mainButtonID;
};

#define g_buttonID_File 0
#define g_buttonID_File_NewScene 2
#define g_buttonID_Settings 1

class miGUIManager
{
	yyVideoDriverAPI* m_gpu;

	yyGUIFont* m_fontDefault;

	f32 m_mainMenu_Y;
	miPluginGUIImpl* m_activePluginGUI;

	yyGUIDrawGroup* m_mainMenu_drawGroup;
	bool m_isMainMenuActive;
	s32 m_hoveredMenuItemID;
	
	yyArraySmall<miGUIMainMenuMenuGroup*> m_mainMenu_menus;
	miGUIMainMenuMenuGroup* _addMainMenuItem(const wchar_t* text, //const v4f& buildRect, 
		const v4f& uvregion1, const v4f& uvregion2, const v4f& uvregion3,
		s32 id, yyGUICallback onClick);

public:
	miGUIManager();
	~miGUIManager();

	enum Font {
		Default
	};

	yyGUIFont* GetFont(Font);
	void ShowMenu(s32);
	void HideMenu();

	void ShowImportMenu(miPluginGUI* gui);

	friend struct miGUIMainMenuMenuGroup;
	bool m_isMainMenuInCursor;
	yyGUIGroup* m_mainMenu_group;
	v4f m_mainMenu_group_actRect_noActive;
	v4f m_mainMenu_group_actRect_Active;
	yyArraySmall<miGUIMainMenuItem*> m_mainMenu_items;
	s32 m_selectedMenuItemID;
	yyGUIPictureBox* m_mainMenu_backgroundPB;
	yyGUIPictureBox* m_mainMenu_windowBackgroundPB;
	
	yyGUIButton*  m_button_add;
	yyGUIButton*  m_button_import;
	yyGUITextInput* m_textInput_rename;
	
	yyGUIButton*  m_button_importWindow_import;
	
	yyGUIButton*  m_button_selectByName;

	void UpdateTransformModeButtons();
	yyGUIButtonGroup* m_buttonGroup_transformMode;
	yyGUIButton*  m_button_transformModeNoTransform;
	yyGUIButton*  m_button_transformModeMove;
	yyGUIButton*  m_button_transformModeScale;
	yyGUIButton*  m_button_transformModeRotate;
	yyGUIButton*  m_button_transformModeScaleLocal;
	yyGUIButton*  m_button_transformModeRotateLocal;
};

#endif