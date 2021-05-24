#include "miApplication.h"
#include "miGUIManager.h"

extern miApplication * g_app;
miGUIManager* g_guiManager = 0;
f32 g_guiButtonFadeSpeed = 8.0f;
f32 g_guiBGOpacity = 0.3f;
f32 g_guiButtonMinimumOpacity = 0.1f;

bool gui_textInput_onChar(wchar_t c) {
	if(c >= 0 && c <= 0x1f)
		return false;
	return true;
}
void gui_textInput_onEnter(yyGUIElement* elem, s32 m_id) {
	yyGUITextInput* ti = (yyGUITextInput*)elem;
	if (ti->m_textElement->m_text.size())
	{
		if (g_app->m_selectedObjects.m_size == 1)
		{
			if (g_app->m_selectedObjects.m_data[0]->GetName() == miString(ti->m_textElement->m_text.data()))
				return;

			auto newName = g_app->GetFreeName(ti->m_textElement->m_text.data());
			g_app->m_selectedObjects.m_data[0]->SetName(newName.data());
		}
	}
	g_app->update_selected_objects_array();
}
void gui_textInput_onEscape(yyGUIElement* elem, s32 m_id) {
	yyGUITextInput* ti = (yyGUITextInput*)elem;
	ti->DeleteAll();
	g_app->update_selected_objects_array();
}
void gui_textInput_onLMB(yyGUIElement* elem, s32 m_id) {
	yyGUITextInput* ti = (yyGUITextInput*)elem;
	ti->SelectAll();
}

void gui_mainMenu_buttons_onDraw(yyGUIElement* elem, s32 m_id) {
	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
	yyGUIButton * butt = (yyGUIButton *)elem;
	miGUIMainMenuItem* item = (miGUIMainMenuItem*)elem->m_userData;
	auto op = butt->GetOpacity(0);
	if (g_guiManager->m_isMainMenuInCursor)
	{
		op += v;
		if (op > 1.0f)
			op = 1.0f;
		butt->SetOpacity(op, 0);
		
		/*item->m_text->m_color.m_data[3] += v;
		if (item->m_text->m_color.m_data[3] > 1.f)
			item->m_text->m_color.m_data[3] = 1.f;*/
	}
	else
	{
		op -= v;
		if (op < g_guiButtonMinimumOpacity)
			op = g_guiButtonMinimumOpacity;
		butt->SetOpacity(op, 0);
		butt->SetOpacity(op, 1);
		butt->SetOpacity(op, 2);

		//item->m_text->m_color.m_data[3] -= v;
		//if (item->m_text->m_color.m_data[3] < 0.f)
		//	item->m_text->m_color.m_data[3] = 0.f;
	}
}
void gui_mainMenu_buttonOnMouseEnter(yyGUIElement* elem, s32 m_id) {
	g_guiManager->m_isMainMenuInCursor = true;
	g_guiManager->m_mainMenu_group->m_sensorRectInPixels = g_guiManager->m_mainMenu_group_actRect_Active;
}
void gui_mainMenu_groupOnMouseInRect(yyGUIElement* elem, s32 m_id) {
	g_guiManager->m_isMainMenuInCursor = true;
	g_guiManager->m_mainMenu_group->m_sensorRectInPixels = g_guiManager->m_mainMenu_group_actRect_Active;
	for (u16 i = 0, sz = g_guiManager->m_mainMenu_items.size(); i < sz; ++i)
	{
		auto item = g_guiManager->m_mainMenu_items.at(i);
		item->m_button->m_sensorRectInPixels = item->m_activeArea_Active;
	}
}
void gui_mainMenu_groupOnMouseLeave(yyGUIElement* elem, s32 m_id) {
	g_guiManager->m_isMainMenuInCursor = false;
	g_guiManager->m_mainMenu_group->m_sensorRectInPixels = g_guiManager->m_mainMenu_group_actRect_noActive;
	for (u16 i = 0, sz = g_guiManager->m_mainMenu_items.size(); i < sz; ++i)
	{
		auto item = g_guiManager->m_mainMenu_items.at(i);
		item->m_button->m_sensorRectInPixels = item->m_activeArea_noActive;
	}
}


void gui_mainMenu_backgroundPB_onDraw_hide(yyGUIElement* elem, s32 m_id) {
	elem->IgnoreInput(true);
	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
	elem->m_color.m_data[3] -= v;
	if (elem->m_color.m_data[3] < 0.0f)
	{
		elem->m_color.m_data[3] = 0.0f;
	}
	
	g_guiManager->m_mainMenu_windowBackgroundPB->m_color.m_data[3] -= v;
	if (g_guiManager->m_mainMenu_windowBackgroundPB->m_color.m_data[3] < 0.0f)
	{
		g_guiManager->m_mainMenu_windowBackgroundPB->m_color.m_data[3] = 0.0f;
		g_guiManager->m_mainMenu_windowBackgroundPB->SetVisible(false);
	}

	if (elem->m_color.m_data[3] == 0.f && g_guiManager->m_mainMenu_windowBackgroundPB->m_color.m_data[3] == 0.0f)
	{
		elem->SetVisible(false);
		elem->m_onDraw = 0;
	}
}
void gui_mainMenu_backgroundPB_onDraw_show(yyGUIElement* elem, s32 m_id) {
	elem->IgnoreInput(false);
	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
	elem->m_color.m_data[3] += v;
	if (elem->m_color.m_data[3] > g_guiBGOpacity)
	{
		elem->m_color.m_data[3] = g_guiBGOpacity;
	}

	g_guiManager->m_mainMenu_windowBackgroundPB->m_color.m_data[3] += v;
	if (g_guiManager->m_mainMenu_windowBackgroundPB->m_color.m_data[3] > 1.0f)
	{
		g_guiManager->m_mainMenu_windowBackgroundPB->m_color.m_data[3] = 1.0f;
	}

	if (elem->m_color.m_data[3] == g_guiBGOpacity &&
		g_guiManager->m_mainMenu_windowBackgroundPB->m_color.m_data[3] == 1.0f)
	{
		elem->m_onDraw = 0;
	}
}
void gui_mainMenu_buttonOnClick(yyGUIElement* elem, s32 m_id) {
	//yyLogWriteInfo("[%i]\n", m_id);
	g_guiManager->m_selectedMenuItemID = m_id;
	for (u16 i = 0, sz = g_guiManager->m_mainMenu_items.size(); i < sz; ++i)
	{
		auto item = g_guiManager->m_mainMenu_items.at(i);
		item->m_button->SetColor(ColorWhite, 0);
	}
	((yyGUIButton*)elem)->SetColor(ColorYellow, 0);
	g_guiManager->m_mainMenu_backgroundPB->m_onDraw = gui_mainMenu_backgroundPB_onDraw_show;
	g_guiManager->m_mainMenu_backgroundPB->SetVisible(true);
	g_guiManager->m_mainMenu_windowBackgroundPB->SetVisible(true);
	g_guiManager->ShowMenu(m_id);
}

void gui_mainMenu_backgroundPB_onClick(yyGUIElement* elem, s32 m_id) {
	g_guiManager->m_mainMenu_backgroundPB->m_onDraw = gui_mainMenu_backgroundPB_onDraw_hide;
	
	g_guiManager->m_selectedMenuItemID  = -1;
	for (u16 i = 0, sz = g_guiManager->m_mainMenu_items.size(); i < sz; ++i)
	{
		auto item = g_guiManager->m_mainMenu_items.at(i);
		item->m_button->SetColor(ColorWhite, 0);
	}
	g_guiManager->HideMenu();

	g_guiManager->m_isMainMenuInCursor = false;
	g_guiManager->m_mainMenu_group->m_onMouseLeave(g_guiManager->m_mainMenu_group, g_guiManager->m_mainMenu_group->m_id);
	//gui_mainMenu_groupOnMouseLeave(g_guiManager->m_mainMenu_backgroundPB, g_guiManager->m_mainMenu_backgroundPB->m_id);
}
//void gui_mainMenu_backgroundPB_onRebuildSetRects(yyGUIElement* elem, s32 m_id) {
//	auto window = g_app->GetWindowMain();
//	elem->m_sensorRectInPixels.set(0.f, 0.f, window->m_currentSize.x, window->m_currentSize.y);
//	elem->m_buildRectInPixels = elem->m_sensorRectInPixels;
//	elem->m_clipRectInPixels = elem->m_sensorRectInPixels;
//	elem->m_buildRectInPixelsCreation = elem->m_sensorRectInPixels;
//}
//void gui_mainMenu_buttonOnDraw_moreOpacity(yyGUIElement* elem, s32 m_id) {
//	auto button = (yyGUIButton*)elem;
//	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
//	button->SetOpacity(button->m_color.m_data[3] + v, 0);
//	if (elem->m_color.m_data[3] > 1.f)
//	{
//		elem->m_color.m_data[3] = 1.f;
//		elem->m_onDraw = 0;
//	}
//}
//void gui_mainMenu_buttonOnDraw_lessOpacity(yyGUIElement* elem, s32 m_id) {
//	auto button = (yyGUIButton*)elem;
//	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
//	button->SetOpacity(button->m_color.m_data[3] - v, 0);
//	if (elem->m_color.m_data[3] < 0.25f)
//	{
//		elem->m_color.m_data[3] = 0.25f;
//		elem->m_onDraw = 0;
//	}
//}
//void gui_mainMenu_textOnDraw_Active(yyGUIElement* elem, s32 m_id) {
//	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
//
//	if (m_id == g_guiManager->m_selectedMenuItemID || m_id == g_guiManager->m_hoveredMenuItemID)
//	{
//		elem->m_color.m_data[2] -= v;
//		if (elem->m_color.m_data[2] < 0.f)
//			elem->m_color.m_data[2] = 0.f;
//
//		elem->m_offset.x += v * 3.5f;
//		if (elem->m_offset.x > 3.f)
//			elem->m_offset.x = 3.f;
//	}
//	else
//	{
//		elem->m_color.m_data[2] += v;
//		if (elem->m_color.m_data[2] > 1.f)
//			elem->m_color.m_data[2] = 1.f;
//
//		elem->m_offset.x -= v * 3.5f;
//		if (elem->m_offset.x < 0.f)
//			elem->m_offset.x = 0.f;
//	}
//
//	if (elem->m_color.m_data[3] < 1.f)
//		elem->m_color.m_data[3] += v;
//
//	if (elem->m_color.m_data[2] == 0.f && elem->m_color.m_data[3] == 1.f)
//		elem->m_onDraw = 0;
//}
//void gui_mainMenu_textOnDraw_noActive(yyGUIElement* elem, s32 m_id) {
//	auto v = g_guiButtonFadeSpeed * g_app->m_dt;
//
//	elem->m_color.m_data[2] += v;
//	if (elem->m_color.m_data[2] > 1.f)
//		elem->m_color.m_data[2] = 1.f;
//
//	if (!g_guiManager->m_isMainMenuInCursor)
//	{
//		elem->m_color.m_data[3] -= v;
//		if (elem->m_color.m_data[3] < 0.f)
//			elem->m_color.m_data[3] = 0.f;
//	}
//
//	elem->m_offset.x -= v;
//	if (elem->m_offset.x < 0.f)
//		elem->m_offset.x = 0.f;
//
//	if (elem->m_color.m_data[2] == 1.f && elem->m_color.m_data[3] == 0.f)
//		elem->m_onDraw = 0;
//}
//void gui_mainMenu_buttonOnMouseEnter(yyGUIElement* elem, s32 m_id) {
//	miGUIMainMenuItem* item = (miGUIMainMenuItem*)elem->m_userData;
//	g_guiManager->m_hoveredMenuItemID = m_id;
//}
//void gui_mainMenu_buttonOnMouseLeave(yyGUIElement* elem, s32 m_id) {
//	miGUIMainMenuItem* item = (miGUIMainMenuItem*)elem->m_userData;
//}
//
//
//void gui_mainMenu_groupOnMouseLeave(yyGUIElement* elem, s32 m_id) {
//	g_guiManager->m_hoveredMenuItemID = -1;
//
//	if (g_guiManager->m_isMainMenuActive)
//		return;
//	g_guiManager->m_isMainMenuInCursor = false;
//	for (u16 i = 0, sz = g_guiManager->m_mainMenu_items.size(); i < sz; ++i)
//	{
//		auto item = g_guiManager->m_mainMenu_items.at(i);
//		item->m_button->m_onDraw = gui_mainMenu_buttonOnDraw_lessOpacity;
//
//		item->m_button->m_sensorRectInPixels = item->m_activeArea_noActive;
//		item->m_text->m_onDraw = gui_mainMenu_textOnDraw_noActive;
//	}
//	g_guiManager->m_mainMenu_group->m_sensorRectInPixels = g_guiManager->m_mainMenu_group_actRect_noActive;
//}
void gui_addButton_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->ShowPopupAtCursor(&g_app->m_popup_NewObject);
}

miGUIManager::miGUIManager(){
	m_selectedMenuItemID = -1;
	m_hoveredMenuItemID = -1;

	m_button_add = 0;
	m_textInput_rename = 0;
		 
	m_isMainMenuInCursor = false;
	m_isMainMenuActive = false;
	g_guiManager = this;
	m_gpu = yyGetVideoDriverAPI();

	m_fontDefault = yyGUILoadFont("../res/fonts/Noto/notosans.txt");
	if (!m_fontDefault)
		YY_PRINT_FAILED;

	auto window = g_app->GetWindowMain();

	m_mainMenu_drawGroup = yyGUICreateDrawGroup();

	m_mainMenu_group = yyGUICreateGroup(v4f(), 0, m_mainMenu_drawGroup);
	m_mainMenu_group->m_onMouseInRect = gui_mainMenu_groupOnMouseInRect;
	m_mainMenu_group->m_onMouseLeave = gui_mainMenu_groupOnMouseLeave;
	
	m_mainMenu_backgroundPB = yyGUICreatePictureBox(v4f(0.f, 0.f, (f32)window->m_creationSize.x, (f32)window->m_creationSize.y),
		yyGetTextureFromCache("../res/gui/black.dds"), -1, m_mainMenu_drawGroup, 0);
	m_mainMenu_backgroundPB->IgnoreInput(true);
	m_mainMenu_backgroundPB->m_color.m_data[3] = 0.f;
	m_mainMenu_backgroundPB->SetVisible(false);
	m_mainMenu_backgroundPB->m_onClick = gui_mainMenu_backgroundPB_onClick;
	m_mainMenu_backgroundPB->m_useProportionalScaling = true;
	//m_mainMenu_backgroundPB->m_onRebuildSetRects = gui_mainMenu_backgroundPB_onRebuildSetRects;

	m_mainMenu_windowBackgroundPB = yyGUICreatePictureBox(v4f(100.f, 100.f, (f32)(window->m_creationSize.x-100), (f32)(window->m_creationSize.y-100)),
		yyGetTextureFromCache("../res/gui/white.dds"), -1, m_mainMenu_drawGroup, 0);;
	m_mainMenu_windowBackgroundPB->m_color = ColorDarkGray;
	m_mainMenu_windowBackgroundPB->m_color.m_data[3] = 0.f;
	m_mainMenu_windowBackgroundPB->SetVisible(false);
	//m_mainMenu_windowBackgroundPB->m_align = yyGUIElement::AlignCenter;

	float button_add_size = 30.f;
	m_button_add = yyGUICreateButton(v4f(
		23.f,
		0.f,
		23.f + button_add_size,
		miViewportTopIndent 
	), 0, -1, 0, 0 );
	if (m_button_add)
	{
		m_button_add->SetText(L"Add", m_fontDefault, false);
		m_button_add->SetColor(ColorDimGray, 0);
		m_button_add->SetColor(ColorDarkGrey, 1);
		m_button_add->SetColor(ColorGrey, 2);
		m_button_add->m_textColor = ColorWhite;
		m_button_add->m_textColorHover = ColorWhite;
		m_button_add->m_textColorPress = ColorWhite;
		m_button_add->m_isAnimated = true;
		m_button_add->m_onClick = gui_addButton_onClick;
	}

	m_textInput_rename = yyGUICreateTextInput(
		v4f(
			window->m_creationSize.x - miViewportRightIndent,
			0.f,
			window->m_creationSize.x,
			15.f
		),
		m_fontDefault,
		L"",
		0);
	m_textInput_rename->m_align = m_textInput_rename->AlignRightTop;
	m_textInput_rename->UseDefaultText(L"Object name", yyColor(0.813f));
	m_textInput_rename->m_onCharacter = gui_textInput_onChar;
	m_textInput_rename->m_onEnter = gui_textInput_onEnter;
	m_textInput_rename->m_onEscape = gui_textInput_onEscape;
	//m_textInput_rename->m_onClickLMB = gui_textInput_onLMB; // need other callback like onActivate
	/*m_textInput->m_bgColor.set(1.f, 0.f, 0.f, 0.1f);
	m_textInput->m_bgColorHover.set(0.f, 1.f, 0.f, 0.1f);
	m_textInput->m_bgColorActive.set(0.f, 0.f, 1.f, 0.1f);*/

	auto menu_file = _addMainMenuItem(L"File", 
		v4f(0.f, 0.f, 23.f, 23.f), v4f(24.f, 0.f, 47.f, 23.f), v4f(48.f, 0.f, 71.f, 23.f),
		g_buttonID_File, gui_mainMenu_buttonOnClick);
	menu_file->addButton(L"New scene", v4f(0.f, 0.f, 100.f, 30.f), g_buttonID_File_NewScene, m_mainMenu_drawGroup);

	auto menu_settings = _addMainMenuItem(L"Settings",
		v4f(0.f, 24.f, 23.f, 47.f), v4f(24.f, 24.f, 47.f, 47.f), v4f(0.f, 0.f, 0.f, 0.f),
		g_buttonID_Settings, gui_mainMenu_buttonOnClick);
	/*_addMainMenuItem(L"Help",
		v4i(0, 48, 23, 71), v4i(24, 48, 47, 71), v4i(0, 0, 0, 0),
		g_buttonID_Settings + 1, gui_mainMenu_buttonOnClick);*/
}

miGUIManager::~miGUIManager(){
	for (u16 i = 0, sz = m_mainMenu_items.size(); i < sz; ++i)
	{
		delete m_mainMenu_items.at(i);
	}
}

void miGUIManager::HideMenu() {
	m_isMainMenuActive = false;
	//m_mainMenu_windowBackgroundPB->SetVisible(false);
	//m_mainMenu_backgroundPB->SetVisible(false);
	for (u16 i = 0, sz = m_mainMenu_menus.size(); i < sz; ++i)
	{
		auto menu = m_mainMenu_menus[i];
		for (u16 i2 = 0, sz2 = menu->m_buttons.size(); i2 < sz2; ++i2)
		{
			menu->m_buttons[i2]->SetVisible(false);
		}			
	}
}
void miGUIManager::ShowMenu(s32 buttonID) {
	HideMenu();
	yyGUIDrawGroupMoveFront(m_mainMenu_drawGroup);
	m_isMainMenuActive = true;
	m_mainMenu_windowBackgroundPB->SetVisible(true);
	m_mainMenu_backgroundPB->SetVisible(true);
	for (u16 i = 0, sz = m_mainMenu_menus.size(); i < sz; ++i)
	{
		auto menu = m_mainMenu_menus[i];
		if (menu->m_mainButtonID != buttonID)
			continue;
		for (u16 i2 = 0, sz2 = menu->m_buttons.size(); i2 < sz2; ++i2)
		{
			menu->m_buttons[i2]->SetVisible(true);
		}
	}
}

void miGUIMainMenuMenuGroup_onClick(yyGUIElement* elem, s32 m_id) {

}
void miGUIMainMenuMenuGroup::addButton(const wchar_t* text, const v4f& rect, s32 id, yyGUIDrawGroup* dg)
{
	yyGUIButton* newButton = yyGUICreateButton(rect, 0, id, dg, 0);
	newButton->m_isAnimated = true;
	newButton->SetText(text, g_guiManager->m_fontDefault, false);
	newButton->SetColor(yyColor(127, 127, 127, 255), 0);
	newButton->SetColor(yyColor(211, 211, 211, 255), 1);
	newButton->SetVisible(false);
	newButton->SetParent(g_guiManager->m_mainMenu_windowBackgroundPB);
	newButton->m_onClick = miGUIMainMenuMenuGroup_onClick;
	m_buttons.push_back(newButton);
}

miGUIMainMenuMenuGroup* miGUIManager::_addMainMenuItem(const wchar_t* text,
	//const v4f& buildRect, 
	const v4f& uvregion1, const v4f& uvregion2, const v4f& uvregion3,
	s32 id, yyGUICallback onClick) {
	v4f reg = uvregion1;

	static f32 y = 0.f;

	f32 w = (f32)(uvregion1.z - uvregion1.x);
	f32 h = (f32)(uvregion1.w - uvregion1.y);

	v4f buildRect;
	buildRect.y = y;
	buildRect.z = w;
	buildRect.w = y + h;

	y += h;

	auto newButton = yyGUICreateButton(buildRect,
		yyGetTextureFromCache("../res/gui/icons.png"), -1, m_mainMenu_drawGroup, &reg);
	newButton->m_id = id;
	reg = uvregion2;
	newButton->SetMouseHoverTexture(yyGetTextureFromCache("../res/gui/icons.png"), &reg);
	newButton->SetOpacity(g_guiButtonMinimumOpacity, 0);
	newButton->SetOpacity(g_guiButtonMinimumOpacity, 1);
	newButton->SetOpacity(g_guiButtonMinimumOpacity, 2);
	newButton->m_onClick = onClick;
	newButton->m_isAnimated = true;
	newButton->m_onDraw = gui_mainMenu_buttons_onDraw;
	newButton->m_onMouseEnter = gui_mainMenu_buttonOnMouseEnter;
	//newButton->m_onMouseLeave = gui_mainMenu_buttonOnMouseLeave;

//	reg = uvregion3;
//	newButton->SetMouseClickTexture(yyGetTextureResource("../res/gui/icons.png", false, false, true), &reg);
	

	miGUIMainMenuItem * newItem = new miGUIMainMenuItem;
	newItem->m_button = newButton;
	/*newItem->m_text = yyGUICreateText(v2f(buildRect.x + w, buildRect.y + (h * 0.3f)), m_fontDefault, text, m_mainMenu_drawGroup);
	newItem->m_text->IgnoreInput(true);
	newItem->m_text->m_color.m_data[3] = 0.f;
	newItem->m_text->m_id = id;*/

	newItem->m_button->m_userData = newItem;
	//newItem->m_text->m_userData = newItem;
	
	//auto text_w = newItem->m_text->m_buildRectInPixels.z - newItem->m_text->m_buildRectInPixels.x;
	
	if (buildRect.z > m_mainMenu_group->m_sensorRectInPixels.z)
		m_mainMenu_group->m_sensorRectInPixels.z = buildRect.z ;

	if (buildRect.w > m_mainMenu_group->m_sensorRectInPixels.w)
		m_mainMenu_group->m_sensorRectInPixels.w = buildRect.w;

	m_mainMenu_group->SetBuildRect(m_mainMenu_group->m_sensorRectInPixels);
	
	m_mainMenu_group_actRect_noActive = m_mainMenu_group->m_sensorRectInPixels;
	
	m_mainMenu_group_actRect_Active.x = m_mainMenu_group_actRect_noActive.x;
	m_mainMenu_group_actRect_Active.y = m_mainMenu_group_actRect_noActive.y;
	m_mainMenu_group_actRect_Active.w = m_mainMenu_group_actRect_noActive.w;

	/*if (text_w + w > m_mainMenu_group_actRect_Active.z)
		m_mainMenu_group_actRect_Active.z = text_w + w + 20.f;*/


	//newItem->m_onClick = onClick;
	newItem->m_activeArea_noActive = newButton->m_sensorRectInPixels;
	newItem->m_activeArea_Active = newItem->m_activeArea_noActive;
	m_mainMenu_items.push_back(newItem);

	for (u16 i = 0, sz = m_mainMenu_items.size(); i < sz; ++i)
	{
		auto item = m_mainMenu_items.at(i);
		if (m_mainMenu_group_actRect_Active.z > item->m_activeArea_Active.z)
			item->m_activeArea_Active.z = m_mainMenu_group_actRect_Active.z;
	}

	auto newMenuGroup = new miGUIMainMenuMenuGroup(id);
	m_mainMenu_menus.push_back(newMenuGroup);
	return newMenuGroup;
}

yyGUIFont* miGUIManager::GetFont(miGUIManager::Font f) {
	switch (f)
	{
	case miGUIManager::Default:
	default:
		return m_fontDefault;
		break;
	}
}