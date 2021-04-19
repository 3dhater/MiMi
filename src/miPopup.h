#ifndef _MI_POPUP_H_
#define _MI_POPUP_H_

struct miPopup;

typedef void(*miPopupCallback)(miPopup* popup, s32 m_id);

struct miPopup
{
	miPopup();
	~miPopup();

	HMENU m_hPopupMenu;

	void AddItem(const wchar_t*, u32 id);
	void Show(s32 x, s32 y);
};

#endif