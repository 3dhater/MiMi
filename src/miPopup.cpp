#include "miApplication.h"
#include "miPopup.h"

extern miApplication * g_app;

miPopupInfo::miPopupInfo() {
	m_hPopupMenu = CreatePopupMenu();
}
miPopupInfo::~miPopupInfo() {
	for (u16 i = 0, sz = m_subMenus.size(); i < sz; ++i)
	{
		delete m_subMenus[i];
	}
	DestroyMenu(m_hPopupMenu);
}
miPopupInfo* miPopupInfo::CreateSubMenu(const wchar_t* text) {
	miPopupInfo* newSubMenu = new miPopupInfo;
	m_subMenus.push_back(newSubMenu);
	AppendMenu(m_hPopupMenu, MF_POPUP | MF_BYPOSITION | MF_STRING, (UINT_PTR)newSubMenu->m_hPopupMenu, text);
	return newSubMenu;
}

void miPopupInfo::AddSeparator() {
	AppendMenu(m_hPopupMenu, MF_SEPARATOR, 0, 0);
}

void miPopupInfo::AddItem(const wchar_t* _text, u32 id, const wchar_t* shortcut) {
	yyStringW text = _text;
	if (shortcut)
	{
		text += L"\t";
		text += shortcut;
	}
	AppendMenu(m_hPopupMenu, MF_BYPOSITION | MF_STRING, id, text.data());
}

void miPopupInfo::Show(s32 x, s32 y) {
	SetForegroundWindow(g_app->GetWindowMain()->m_hWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(g_app->GetWindowMain()->m_hWnd, &pt);
	TrackPopupMenu(m_hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, g_app->GetWindowMain()->m_hWnd, NULL);
}


miPopup::miPopup() {
	m_menu = new miPopupInfo;
}

miPopup::~miPopup() {
	if (m_menu) delete m_menu;
}
void miPopup::Show(s32 x, s32 y) {
	m_menu->Show(x, y);
}
void miPopup::AddItem(const wchar_t* t, u32 id, const wchar_t* shortcut) {
	m_menu->AddItem(t, id, shortcut);
}
void miPopup::AddSeparator() {
	m_menu->AddSeparator();
}