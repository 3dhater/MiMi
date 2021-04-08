#ifndef _MI_GUIMANAGER_H_
#define _MI_GUIMANAGER_H_

#include "yy_gui.h"

struct miGUIMainMenuItem 
{
	miGUIMainMenuItem() {
		m_button = 0;
		m_text = 0;
		//m_onClick = 0;
	}
	~miGUIMainMenuItem() {
	}
	yyGUIButton* m_button;
	yyGUIText* m_text;
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

	v4f m_mainMenu_group_actRect_noActive;
	v4f m_mainMenu_group_actRect_Active;
	yyGUIGroup* m_mainMenu_group;
	yyGUIPictureBox* m_mainMenu_backgroundPB;
	yyGUIPictureBox* m_mainMenu_windowBackgroundPB;

	yyGUIDrawGroup* m_mainMenu_drawGroup;
	bool m_isMainMenuInCursor;
	bool m_isMainMenuActive;
	s32 m_selectedMenuItemID;
	s32 m_hoveredMenuItemID;
	//yyGUIButton* m_mainMenu_button;
	yyArraySmall<miGUIMainMenuItem*> m_mainMenu_items;
	
	yyArraySmall<miGUIMainMenuMenuGroup*> m_mainMenu_menus;
	miGUIMainMenuMenuGroup* _addMainMenuItem(const wchar_t* text, //const v4f& buildRect, 
		const v4i& uvregion1, const v4i& uvregion2, const v4i& uvregion3,
		s32 id, yyGUICallback onClick);

public:
	miGUIManager();
	~miGUIManager();

	void ShowMenu(s32);
	void HideMenu();

	friend void gui_mainMenu_buttonOnMouseEnter(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_groupOnMouseInRect(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_groupOnMouseLeave(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_textOnDraw_noActive(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_textOnDraw_Active(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_buttonOnClick(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_backgroundPB_onClick(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_backgroundPB_onDraw_hide(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_backgroundPB_onDraw_show(yyGUIElement* elem, s32 m_id);

	friend struct miGUIMainMenuMenuGroup;
};

#endif