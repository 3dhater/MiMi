#ifndef _MI_POPUP_H_
#define _MI_POPUP_H_

struct miPopup;

typedef void(*miPopupCallback)(miPopup* popup, s32 m_id);

struct miPopupInfo
{
	miPopupInfo();
	~miPopupInfo();

	HMENU m_hPopupMenu;
	//yyStringW m_text;
	
	yyArraySmall<miPopupInfo*> m_subMenus;
	miPopupInfo* CreateSubMenu(const wchar_t* text);
	
	void AddItem(const wchar_t*, u32 id, const wchar_t* shortcut);
	void AddSeparator();
	void Show(s32 x, s32 y);
};

struct miPopup
{
	miPopup();
	~miPopup();

	miPopupInfo* m_menu;
	
	void AddItem(const wchar_t*, u32 id, const wchar_t* shortcut);
	void AddSeparator();
	void Show(s32 x, s32 y);
};

#endif