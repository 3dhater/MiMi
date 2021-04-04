#include "miApplication.h"
#include "miGUIManager.h"

extern miApplication * g_app;
miGUIManager* g_guiManager = 0;
f32 g_guiButtonFadeSpeed = 4.0f;

void gui_mainMenu_buttonOnClick(yyGUIElement* elem, s32 m_id) {
	yyLogWriteInfo("[%i]\n", m_id);
//	((yyGUIButton*)elem)->SetColor(ColorBlue);
}

//void gui_mainMenu_textOnDraw_moreOpacity(yyGUIElement* elem, s32 m_id) {
//	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
//	elem->m_color.m_data[3] += v;
//	if (elem->m_color.m_data[3] > 1.f)
//	{
//		elem->m_color.m_data[3] = 1.f;
//		elem->m_onDraw = 0;
//	}
//}
void gui_mainMenu_buttonOnDraw_moreOpacity(yyGUIElement* elem, s32 m_id) {
	auto button = (yyGUIButton*)elem;
	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
	button->SetOpacity(button->m_color.m_data[3] + v, 0);
	if (elem->m_color.m_data[3] > 1.f)
	{
		elem->m_color.m_data[3] = 1.f;
		elem->m_onDraw = 0;
	}
}
void gui_mainMenu_buttonOnDraw_lessOpacity(yyGUIElement* elem, s32 m_id) {
	auto button = (yyGUIButton*)elem;
	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
	button->SetOpacity(button->m_color.m_data[3] - v, 0);
	if (elem->m_color.m_data[3] < 0.25f)
	{
		elem->m_color.m_data[3] = 0.25f;
		elem->m_onDraw = 0;
	}
}
//void gui_mainMenu_textOnDraw_lessOpacity(yyGUIElement* elem, s32 m_id) {
//	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
//	elem->m_color.m_data[3] -= v;
//	if (elem->m_color.m_data[3] < 0.f)
//	{
//		elem->m_color.m_data[3] = 0.f;
//		elem->m_onDraw = 0;
//	}
//}
void gui_mainMenu_textOnDraw_Active(yyGUIElement* elem, s32 m_id) {
	auto v = g_guiButtonFadeSpeed * g_app->m_dt;

	if (m_id == g_guiManager->GetSelectedMainMenuItemID())
	{
		elem->m_color.m_data[2] -= v;
		if (elem->m_color.m_data[2] < 0.f)
			elem->m_color.m_data[2] = 0.f;

		elem->m_offset.x += v * 3.5f;
		if (elem->m_offset.x > 3.f)
			elem->m_offset.x = 3.f;
	}
	else
	{
		elem->m_color.m_data[2] += v;
		if (elem->m_color.m_data[2] > 1.f)
			elem->m_color.m_data[2] = 1.f;

		elem->m_offset.x -= v * 3.5f;
		if (elem->m_offset.x < 0.f)
			elem->m_offset.x = 0.f;
	}

	if (elem->m_color.m_data[3] < 1.f)
		elem->m_color.m_data[3] += v;

	if (elem->m_color.m_data[2] == 0.f && elem->m_color.m_data[3] == 1.f)
		elem->m_onDraw = 0;
}
void gui_mainMenu_textOnDraw_noActive(yyGUIElement* elem, s32 m_id) {
	auto v = g_guiButtonFadeSpeed * g_app->m_dt;

	elem->m_color.m_data[2] += v;
	if (elem->m_color.m_data[2] > 1.f)
		elem->m_color.m_data[2] = 1.f;

	if (!g_guiManager->m_isMainMenuActive)
	{
		elem->m_color.m_data[3] -= v;
		if (elem->m_color.m_data[3] < 0.f)
			elem->m_color.m_data[3] = 0.f;
	}

	elem->m_offset.x -= v;
	if (elem->m_offset.x < 0.f)
		elem->m_offset.x = 0.f;

	if (elem->m_color.m_data[2] == 1.f && elem->m_color.m_data[3] == 0.f)
		elem->m_onDraw = 0;
}
void gui_mainMenu_buttonOnMouseEnter(yyGUIElement* elem, s32 m_id) {
	g_guiManager->SetSelectedMainMenuItemID(m_id);
	miGUIMainMenuItem* item = (miGUIMainMenuItem*)elem->m_userData;
}
void gui_mainMenu_buttonOnMouseLeave(yyGUIElement* elem, s32 m_id) {
	miGUIMainMenuItem* item = (miGUIMainMenuItem*)elem->m_userData;
}


void gui_mainMenu_groupOnMouseLeave(yyGUIElement* elem, s32 m_id) {
	g_guiManager->SetSelectedMainMenuItemID(-1);
	g_guiManager->m_isMainMenuActive = false;
	for (u16 i = 0, sz = g_guiManager->m_mainMenu_items->size(); i < sz; ++i)
	{
		auto item = g_guiManager->m_mainMenu_items->at(i);
		item->m_button->m_onDraw = gui_mainMenu_buttonOnDraw_lessOpacity;
		//item->m_text->m_onDraw = gui_mainMenu_textOnDraw_lessOpacity;

		item->m_button->m_activeAreaRect_global = item->m_activeArea_noActive;
		item->m_text->m_onDraw = gui_mainMenu_textOnDraw_noActive;
	}
	g_guiManager->m_mainMenu_group->m_activeAreaRect_global = g_guiManager->m_mainMenu_group_actRect_noActive;
}
void gui_mainMenu_groupOnMouseInRect(yyGUIElement* elem, s32 m_id) {
	g_guiManager->m_isMainMenuActive = true;
	for (u16 i = 0, sz = g_guiManager->m_mainMenu_items->size(); i < sz; ++i)
	{
		auto item = g_guiManager->m_mainMenu_items->at(i);
		if (item->m_button->m_color.m_data[3] < 1.f)
			item->m_button->m_onDraw = gui_mainMenu_buttonOnDraw_moreOpacity;
		/*if (item->m_text->m_color.m_data[3] < 1.f)
		{
			item->m_text->m_onDraw = gui_mainMenu_textOnDraw_moreOpacity;
		}*/
		item->m_button->m_activeAreaRect_global = item->m_activeArea_Active;
		item->m_text->m_onDraw = gui_mainMenu_textOnDraw_Active;
	}
	g_guiManager->m_mainMenu_group->m_activeAreaRect_global = g_guiManager->m_mainMenu_group_actRect_Active;
}

miGUIManager::miGUIManager(){
	//m_mainMenuMaxWidth = 0.f;
	m_selectedMenuItemID = -1;
	m_isMainMenuActive = false;
	g_guiManager = this;
	m_gpu = yyGetVideoDriverAPI();

	m_fontDefault = yyGUILoadFont("../res/fonts/Noto/notosans.txt");
	if (!m_fontDefault)
		YY_PRINT_FAILED;

	m_mainMenu_drawGroup = yyGUICreateDrawGroup();
	m_mainMenu_group = yyGUICreateGroup(v4f(), 0, m_mainMenu_drawGroup);
	m_mainMenu_group->m_onMouseInRect = gui_mainMenu_groupOnMouseInRect;
	m_mainMenu_group->m_onMouseLeave = gui_mainMenu_groupOnMouseLeave;
	m_mainMenu_items = new yyArraySmall<miGUIMainMenuItem*>;

	_addMainMenuItem(L"Main menu", 
		v4i(0, 0, 23, 23), v4i(24, 0, 47, 23), v4i(48, 0, 71, 23), 
		g_buttonID_MainMenu, gui_mainMenu_buttonOnClick);
	_addMainMenuItem(L"Settings",
		v4i(0, 24, 23, 47), v4i(24, 24, 47, 47), v4i(0, 0, 0, 0),
		g_buttonID_Settings, gui_mainMenu_buttonOnClick);
	_addMainMenuItem(L"Help",
		v4i(0, 48, 23, 71), v4i(24, 48, 47, 71), v4i(0, 0, 0, 0),
		g_buttonID_Settings + 1, gui_mainMenu_buttonOnClick);
}

miGUIManager::~miGUIManager(){
	for (u16 i = 0, sz = m_mainMenu_items->size(); i < sz; ++i)
	{
		delete m_mainMenu_items->at(i);
	}
	delete m_mainMenu_items;
}

void miGUIManager::_addMainMenuItem(const wchar_t* text, 
	//const v4f& buildRect, 
	const v4i& uvregion1, const v4i& uvregion2, const v4i& uvregion3,
	s32 id, yyGUICallback onClick) {
	v4i reg = uvregion1;

	static f32 y = 0.f;

	f32 w = uvregion1.z - uvregion1.x;
	f32 h = uvregion1.w - uvregion1.y;

	v4f buildRect;
	buildRect.y = y;
	buildRect.z = w;
	buildRect.w = y + h;

	y += h;

	auto newButton = yyGUICreateButton(buildRect,
		yyGetTextureResource("../res/gui/icons.png", false, false, true), -1, m_mainMenu_drawGroup, &reg);
	newButton->m_id = id;
	reg = uvregion2;
	newButton->SetMouseHoverTexture(yyGetTextureResource("../res/gui/icons.png", false, false, true), &reg);
	newButton->SetOpacity(0.25f, 0);
	newButton->SetOpacity(0.25f, 1);
	newButton->SetOpacity(0.25f, 2);
	newButton->m_onClick = onClick;
	newButton->m_isAnimated = true;
	newButton->m_onMouseEnter = gui_mainMenu_buttonOnMouseEnter;
	newButton->m_onMouseLeave = gui_mainMenu_buttonOnMouseLeave;
//	reg = uvregion3;
//	newButton->SetMouseClickTexture(yyGetTextureResource("../res/gui/icons.png", false, false, true), &reg);
	

	miGUIMainMenuItem * newItem = new miGUIMainMenuItem;
	newItem->m_button = newButton;
	newItem->m_text = yyGUICreateText(v2f(buildRect.x + w, buildRect.y + (h * 0.3f)), m_fontDefault, text, m_mainMenu_drawGroup);
	newItem->m_text->IgnoreInput(true);
	newItem->m_text->m_color.m_data[3] = 0.f;
	newItem->m_text->m_id = id;

	newItem->m_button->m_userData = newItem;
	newItem->m_text->m_userData = newItem;
	
	auto text_w = newItem->m_text->m_buildingRect.z - newItem->m_text->m_buildingRect.x;
	
	if (buildRect.z > m_mainMenu_group->m_activeAreaRect.z)
		m_mainMenu_group->m_activeAreaRect.z = buildRect.z ;

	if (buildRect.w > m_mainMenu_group->m_activeAreaRect.w)
		m_mainMenu_group->m_activeAreaRect.w = buildRect.w;
	
	m_mainMenu_group_actRect_noActive = m_mainMenu_group->m_activeAreaRect;
	
	m_mainMenu_group_actRect_Active.x = m_mainMenu_group_actRect_noActive.x;
	m_mainMenu_group_actRect_Active.y = m_mainMenu_group_actRect_noActive.y;
	m_mainMenu_group_actRect_Active.w = m_mainMenu_group_actRect_noActive.w;

	if (text_w + w > m_mainMenu_group_actRect_Active.z)
		m_mainMenu_group_actRect_Active.z = text_w + w + 20.f;

	m_mainMenu_group->m_activeAreaRect_global = m_mainMenu_group->m_activeAreaRect;

	//newItem->m_onClick = onClick;
	newItem->m_activeArea_noActive = newButton->m_activeAreaRect;
	newItem->m_activeArea_Active = newItem->m_activeArea_noActive;
	m_mainMenu_items->push_back(newItem);

	for (u16 i = 0, sz = m_mainMenu_items->size(); i < sz; ++i)
	{
		auto item = m_mainMenu_items->at(i);
		if (m_mainMenu_group_actRect_Active.z > item->m_activeArea_Active.z)
			item->m_activeArea_Active.z = m_mainMenu_group_actRect_Active.z;
	}
}

void miGUIManager::SetSelectedMainMenuItemID(s32 v) {
	m_selectedMenuItemID = v;
}

s32 miGUIManager::GetSelectedMainMenuItemID() {
	return m_selectedMenuItemID;
}
