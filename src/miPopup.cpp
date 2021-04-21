#include "miApplication.h"
#include "miPopup.h"

extern miApplication * g_app;

miPopup::miPopup() {
	m_hPopupMenu = CreatePopupMenu();
}

miPopup::~miPopup() {
	DestroyMenu(m_hPopupMenu);
}

void miPopup::AddSeparator() {
	AppendMenu(m_hPopupMenu, MF_SEPARATOR, 0, 0);
}

void miPopup::AddItem(const wchar_t* text, u32 id, const wchar_t* shortcut) {
	m_text = text;
	if (shortcut)
	{
		m_text += "\t";
		m_text += shortcut;
	}
	AppendMenu(m_hPopupMenu, MF_BYPOSITION | MF_STRING, id, m_text.data());
}

void miPopup::Show(s32 x, s32 y) {
	SetForegroundWindow(g_app->GetWindowMain()->m_hWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(g_app->GetWindowMain()->m_hWnd, &pt);
	TrackPopupMenu(m_hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, g_app->GetWindowMain()->m_hWnd, NULL);
}