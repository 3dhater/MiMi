#ifndef _MI_PLUGINGUIIMPL_H_
#define _MI_PLUGINGUIIMPL_H_

class miPluginGUIImpl : public miPluginGUI
{
	miPluginGUIType m_type;
	yyGUIDrawGroup* m_gui_drawGroup;
	yyGUIGroup* m_gui_group;
	
	void _init(miPluginGUIType);

	friend void miPluginGUIImpl_gui_group_onRebuildSetRects(yyGUIElement* elem, s32 m_id);
	friend class miSDKImpl;
public:
	miPluginGUIImpl();
	virtual ~miPluginGUIImpl();

	virtual void AddText(const miVec2& position, const wchar_t* text, const wchar_t* (*onSelectObject)(miSceneObject*));
};

#endif