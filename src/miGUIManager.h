#ifndef _MI_GUIMANAGER_H_
#define _MI_GUIMANAGER_H_

#include "yy_gui.h"

struct miGUIMainMenuItem {
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

template<class miGUIMainMenuItem>
class yyArraySmall;

#define g_buttonID_MainMenu 0
#define g_buttonID_Settings 1

class miGUIManager
{
	yyVideoDriverAPI* m_gpu;

	yyGUIFont* m_fontDefault;

	v4f m_mainMenu_group_actRect_noActive;
	v4f m_mainMenu_group_actRect_Active;
	yyGUIGroup* m_mainMenu_group;

	yyGUIDrawGroup* m_mainMenu_drawGroup;
	bool m_isMainMenuActive;
	s32 m_selectedMenuItemID;
	//yyGUIButton* m_mainMenu_button;
	yyArraySmall<miGUIMainMenuItem*>* m_mainMenu_items;
	
	//f32 m_mainMenuMaxWidth;

	void _addMainMenuItem(const wchar_t* text, //const v4f& buildRect, 
		const v4i& uvregion1, const v4i& uvregion2, const v4i& uvregion3,
		s32 id, yyGUICallback onClick);

public:
	miGUIManager();
	~miGUIManager();

	void SetSelectedMainMenuItemID(s32);
	s32 GetSelectedMainMenuItemID();


	friend void gui_mainMenu_groupOnMouseInRect(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_groupOnMouseLeave(yyGUIElement* elem, s32 m_id);
	friend void gui_mainMenu_textOnDraw_noActive(yyGUIElement* elem, s32 m_id);
};

#endif