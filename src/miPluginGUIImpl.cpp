#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miPluginGUIImpl.h"

extern miApplication * g_app;

void miPluginGUIImpl_gui_group_onRebuildSetRects(yyGUIElement* elem, s32 m_id) {
	miPluginGUIImpl * gui = (miPluginGUIImpl *)elem->m_userData;

	switch (gui->m_type)
	{
	case miPluginGUIType::ObjectParams:
		gui->m_gui_group->m_buildRectInPixels.x = g_app->GetWindowMain()->m_currentSize.x - miViewportRightIndent;
		gui->m_gui_group->m_buildRectInPixels.y = miViewportTopIndent + 15.f;
		gui->m_gui_group->m_buildRectInPixels.z = g_app->GetWindowMain()->m_currentSize.x;
		gui->m_gui_group->m_buildRectInPixels.w = g_app->GetWindowMain()->m_currentSize.y;
		break;
	default:
		break;
	}

	gui->m_gui_group->m_clipRectInPixels = gui->m_gui_group->m_buildRectInPixels;
	gui->m_gui_group->m_sensorRectInPixels = gui->m_gui_group->m_buildRectInPixels;
}
void miPluginGUIImpl_gui_group_onMouseInRect(yyGUIElement* elem, s32 m_id) {
	printf("in");
}

miPluginGUIImpl::miPluginGUIImpl()
{
	m_type = miPluginGUIType::ObjectParams;
	m_gui_drawGroup = 0;
}

miPluginGUIImpl::~miPluginGUIImpl(){
	if (m_gui_drawGroup)
		yyGUIDeleteDrawGroup(m_gui_drawGroup);
}

void miPluginGUIImpl::_init(miPluginGUIType t) {
	m_gui_drawGroup = yyGUICreateDrawGroup();
	m_gui_drawGroup->SetInput(true);
	m_gui_drawGroup->SetDraw(true);
	m_type = t;

	m_gui_group = yyGUICreateGroup(v4f(), -1, m_gui_drawGroup);
	m_gui_group->m_align = m_gui_group->AlignRightTop;
	m_gui_group->m_userData = this;
	m_gui_group->m_onMouseInRect = miPluginGUIImpl_gui_group_onMouseInRect;
	m_gui_group->m_onRebuildSetRects = miPluginGUIImpl_gui_group_onRebuildSetRects;
}

void miPluginGUIImpl::AddText(
	const miVec2& position, 
	const wchar_t* text, 
	const wchar_t* (*onSelectObject)(miSceneObject*)) 
{


}