#include "miApplication.h"
#include "miGUIManager.h"
#include "miPluginGUIImpl.h"

extern miApplication * g_app;
miGUIManager* g_guiManager = 0;
f32 g_guiButtonFadeSpeed = 8.0f;
f32 g_guiBGOpacity = 0.3f;
f32 g_guiButtonMinimumOpacity = 0.1f;
v4f g_guiWindowBackgroundPBRect;

void gui_editorTypeCombo_onSelect(yyGUIComboBox*, yyGUIComboBoxItem* item) {
	if (item == g_guiManager->m_editorTypeComboItem_3D)
	{
		g_app->SetEditorType(miEditorType::_3D);
	}
	else if (item == g_guiManager->m_editorTypeComboItem_UV)
	{
		g_app->SetEditorType(miEditorType::UV);
	}
}

void gui_group_commonParams_range_Position(yyGUIRangeSlider* slider) {
	if (g_app->m_selectedObjects.m_size > 1)
	{
		auto p = g_guiManager->m_commonParams_range_Position_many - 
			g_app->m_gizmo->m_position + g_app->m_gizmo->m_var_move;

		for (u32 i = 0; i < g_app->m_selectedObjects.m_size; ++i)
		{
			auto pos = g_app->m_selectedObjects.m_data[i]->GetLocalPosition();
			*pos += p;
		}
		g_app->m_gizmo->m_position = g_guiManager->m_commonParams_range_Position_many;
	}
	for (u32 i = 0; i < g_app->m_selectedObjects.m_size; ++i)
	{
		g_app->m_selectedObjects.m_data[i]->UpdateTransform();
		g_app->m_selectedObjects.m_data[i]->UpdateAabb();
	}
	g_app->UpdateSceneAabb();
	g_app->UpdateSelectionAabb();
}
void gui_buttonTransformModeNoTransform_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetTransformMode(miTransformMode::NoTransform, false);
}
void gui_buttonTransformModeMove_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetTransformMode(miTransformMode::Move, false);
}
void gui_buttonTransformModeScale_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetTransformMode(miTransformMode::Scale, false);
}
void gui_buttonTransformModeRotate_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetTransformMode(miTransformMode::Rotate, false);
}
void gui_buttonTransformModeScaleLocal_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetTransformMode(miTransformMode::Scale, true);
}
void gui_buttonTransformModeRotateLocal_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetTransformMode(miTransformMode::Rotate, true);
}
void gui_buttonEditModeVertex_onClick(yyGUIElement* elem, s32 m_id) {
	yyGUIButton* b = (yyGUIButton*)elem;
	switch (m_id)
	{
	default:
	case 0:
		b->m_isChecked ? g_app->SetEditMode(miEditMode::Vertex) : g_app->SetEditMode(miEditMode::Object);
		break;
	case 1:
		b->m_isChecked ? g_app->SetEditMode(miEditMode::Edge) : g_app->SetEditMode(miEditMode::Object);
		break;
	case 2:
		b->m_isChecked ? g_app->SetEditMode(miEditMode::Polygon) : g_app->SetEditMode(miEditMode::Object);
		break;
	}
}
void gui_buttonObjectCommonParams_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetObjectParametersMode(miObjectParametersMode::CommonParameters);
}
void gui_buttonObjectObjectParams_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetObjectParametersMode(miObjectParametersMode::ObjectParameters);
}
void gui_buttonUV_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetObjectParametersMode(miObjectParametersMode::UV);
}
void gui_buttonMaterials_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->SetObjectParametersMode(miObjectParametersMode::Materials);
}
void gui_buttonMaterialsAdd_onRelease(yyGUIElement* elem, s32 m_id) {
	g_app->MaterialCreate();
}
void gui_buttonMaterialsDelete_onRelease(yyGUIElement* elem, s32 m_id) {
	g_guiManager->DeleteSelectedMaterial();
}
void gui_buttonUvMakePlanar_onRelease(yyGUIElement* elem, s32 m_id) {
	g_app->UvMakePlanar(false);
}
void gui_buttonUvFlatten_onRelease(yyGUIElement* elem, s32 m_id) {
	g_app->UvFlattenMapping();
}
void gui_buttonUvMakePlanarScreen_onRelease(yyGUIElement* elem, s32 m_id) {
	g_app->UvMakePlanar(true);
}
void gui_buttonUvOpenEditor_onRelease(yyGUIElement* elem, s32 m_id) {
	g_app->SetEditorType(miEditorType::UV);
	g_guiManager->m_editorTypeCombo->SelectItem(g_guiManager->m_editorTypeComboItem_UV);
}
void gui_buttonMaterialsAssign_onRelease(yyGUIElement* elem, s32 m_id) {
	g_guiManager->AssignSelectedMaterial();
}
void gui_buttonMaterialsLoadImage_onRelease(yyGUIElement* elem, s32 m_id) {
	g_guiManager->LoadNewImageForMaterial();
}
void gui_buttonMaterialsDeleteImage_onRelease(yyGUIElement* elem, s32 m_id) {
	g_guiManager->DeleteImageFromMaterial();
}
void gui_lbMaterials_onSelect(yyGUIListBox*, yyGUIListBoxItem* item) {
	g_guiManager->UpdateMaterialMapPictureBox();
	wprintf(L"%s\n", item->GetText());
}
void gui_lbMaps_onSelect(yyGUIListBox*, yyGUIListBoxItem*) {
	g_guiManager->UpdateMaterialMapPictureBox();
}

bool gui_textInput_onChar(wchar_t c) {
	if(c >= 0 && c <= 0x1f)
		return false;
	return true;
}
void gui_rename_onEnter(yyGUIElement* elem, s32 m_id) {
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
	g_app->UpdateSelectedObjectsArray();
}
void gui_rename_onEscape(yyGUIElement* elem, s32 m_id) {
	yyGUITextInput* ti = (yyGUITextInput*)elem;
	ti->DeleteAll();
	g_app->UpdateSelectedObjectsArray();
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
	// must be command/event g_guiManager->ShowMenu(m_id);
	static s32 _id = m_id;
	yyEvent e;
	e.m_type = yyEventType::User;
	e.m_event_user.m_id = miEventId_ShowMainMenu;
	e.m_event_user.m_data = &_id;
	yyAddEvent(e, false);
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
	if(g_guiManager->m_mainMenu_group->m_onMouseLeave)
		g_guiManager->m_mainMenu_group->m_onMouseLeave(g_guiManager->m_mainMenu_group, g_guiManager->m_mainMenu_group->m_id);
	//gui_mainMenu_groupOnMouseLeave(g_guiManager->m_mainMenu_backgroundPB, g_guiManager->m_mainMenu_backgroundPB->m_id);
}
void gui_addButton_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->ShowPopupAtCursor(&g_app->m_popup_NewObject);
}
void gui_importButton_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->ShowPopupAtCursor(&g_app->m_popup_Importers);
}
void gui_exportButton_onClick(yyGUIElement* elem, s32 m_id) {
	g_app->ShowPopupAtCursor(&g_app->m_popup_Exporters);
}

void gui_renameMaterial_onEnter(yyGUIElement* elem, s32 m_id) {
	printf("OnEnter\n");
	yyGUITextInput* ti = (yyGUITextInput*)elem;
	if (ti->m_textElement->m_text.size())
	{
		for (u32 i = 0, sz = g_guiManager->m_gui_listbox_materials->GetItemsCount(); i < sz; ++i)
		{
			auto item = g_guiManager->m_gui_listbox_materials->GetItem(i);
			if (item->IsSelected())
			{
				miMaterial* mat = (miMaterial*)item->GetUserData();
				g_app->MaterialRename(mat, ti->m_textElement->m_text.data());
				//item->SetText(mat->m_name.data());
				break;
			}
		}
	}
}

miGUIManager::miGUIManager(){
	m_editorTypeCombo = 0;
	m_gui_pictureBox_map = 0;
	m_default_value_float = 0.f;
	m_buttonGroup_transformMode = new yyGUIButtonGroup;
	m_buttonGroup_rightSide = new yyGUIButtonGroup;
	m_mainMenu_Y = 0.f;
	m_button_selectByName = 0;
	m_button_importWindow_import = 0;
	m_button_exportWindow_export = 0;
	m_selectedMenuItemID = -1;
	m_hoveredMenuItemID = -1;
	m_activePluginGUI = 0;
	m_button_import = 0;
	m_button_export = 0;
	m_button_add = 0;
	m_textInput_rename = 0;
	m_button_transformModeNoTransform = 0;
	m_button_transformModeMove = 0;
	m_button_transformModeScale = 0;
	m_button_transformModeRotate = 0;
	m_button_transformModeScaleLocal = 0;
	m_button_transformModeRotateLocal = 0;
	m_button_editModeVertex = 0;
	m_button_editModeEdge = 0;
	m_button_editModePolygon = 0;
	m_button_objectCommonParams = 0;
	m_button_objectObjectParams = 0;
	m_gui_group_commonParams_range_PositionX = 0;
	m_gui_group_commonParams_range_PositionY = 0;
	m_gui_group_commonParams_range_PositionZ = 0;

	m_button_materials = 0;
	m_button_UV = 0;

	m_isMainMenuInCursor = false;
	m_isMainMenuActive = false;
	g_guiManager = this;
	m_gpu = yyGetVideoDriverAPI();

	
	m_fontDefault = yyGUILoadFont("../res/fonts/Noto/notosans.txt");
	if (!m_fontDefault)
		YY_PRINT_FAILED;

	auto window = g_app->GetWindowMain();

	m_mainMenu_drawGroup = yyGUICreateDrawGroup();
	m_mainMenu_drawGroup->SetDraw(false);
	m_mainMenu_drawGroup->SetInput(false);

	m_mainMenu_group = yyGUICreateGroup(v4f(), 0, m_mainMenu_drawGroup);
	//m_mainMenu_group->m_onMouseInRect = gui_mainMenu_groupOnMouseInRect;
	//m_mainMenu_group->m_onMouseLeave = gui_mainMenu_groupOnMouseLeave;
	
	m_mainMenu_backgroundPB = yyGUICreatePictureBox(v4f(0.f, 0.f, (f32)window->m_creationSize.x, (f32)window->m_creationSize.y),
		yyGetTexture(L"../res/gui/black.dds"), -1, m_mainMenu_drawGroup, 0);
	m_mainMenu_backgroundPB->IgnoreInput(true);
	m_mainMenu_backgroundPB->m_color.m_data[3] = 0.f;
	//m_mainMenu_backgroundPB->SetVisible(false);
	m_mainMenu_backgroundPB->m_onClick = gui_mainMenu_backgroundPB_onClick;
	m_mainMenu_backgroundPB->m_useProportionalScaling = true;
	//m_mainMenu_backgroundPB->m_onRebuildSetRects = gui_mainMenu_backgroundPB_onRebuildSetRects;

	g_guiWindowBackgroundPBRect = v4f(24.f, 0.f, (f32)(window->m_creationSize.x - 100), (f32)(window->m_creationSize.y - 100));
	m_mainMenu_windowBackgroundPB = yyGUICreatePictureBox(g_guiWindowBackgroundPBRect,
		yyGetTexture(L"../res/gui/white.dds"), -1, m_mainMenu_drawGroup, 0);;
	m_mainMenu_windowBackgroundPB->m_color.set(0.43f);
	m_mainMenu_windowBackgroundPB->m_color.m_data[3] = 0.f;
	//m_mainMenu_windowBackgroundPB->SetVisible(false);
	//m_mainMenu_windowBackgroundPB->m_align = yyGUIElement::AlignCenter;

	float button_add_size = 30.f;
	float X = 23.f;
	m_button_add = yyGUICreateButton(v4f(
		X,
		0.f,
		X + button_add_size,
		miViewportTopIndent 
	), 0, -1, 0, 0 );
	if (m_button_add)
	{
		m_button_add->SetText(L"Add", m_fontDefault, false);
		m_button_add->SetColor(g_app->m_color_windowClearColor, 0);
		m_button_add->SetColor(ColorDarkGrey, 1);
		m_button_add->SetColor(ColorGrey, 2);
		m_button_add->m_textColor = ColorWhite;
		m_button_add->m_textColorHover = ColorWhite;
		m_button_add->m_textColorPress = ColorWhite;
		m_button_add->m_isAnimated = true;
		m_button_add->m_onClick = gui_addButton_onClick;
	}

	X += button_add_size;

	float button_import_size = 55.f;
	m_button_import = yyGUICreateButton(v4f(
		X,
		0.f,
		X + button_import_size,
		miViewportTopIndent
	), 0, -1, 0, 0);
	if (m_button_import)
	{
		m_button_import->SetText(L"Import", m_fontDefault, false);
		m_button_import->SetColor(g_app->m_color_windowClearColor, 0);
		m_button_import->SetColor(ColorDarkGrey, 1);
		m_button_import->SetColor(ColorGrey, 2);
		m_button_import->m_textColor = ColorWhite;
		m_button_import->m_textColorHover = ColorWhite;
		m_button_import->m_textColorPress = ColorWhite;
		m_button_import->m_isAnimated = true;
		m_button_import->m_onClick = gui_importButton_onClick;
	}
	X += button_import_size;

	f32 button_export_size = 55.f;
	m_button_export = yyGUICreateButton(v4f(
		X,
		0.f,
		X + button_export_size,
		miViewportTopIndent
	), 0, -1, 0, 0);
	if (m_button_export)
	{
		m_button_export->SetText(L"Export", m_fontDefault, false);
		m_button_export->SetColor(g_app->m_color_windowClearColor, 0);
		m_button_export->SetColor(ColorDarkGrey, 1);
		m_button_export->SetColor(ColorGrey, 2);
		m_button_export->m_textColor = ColorWhite;
		m_button_export->m_textColorHover = ColorWhite;
		m_button_export->m_textColorPress = ColorWhite;
		m_button_export->m_isAnimated = true;
		m_button_export->m_onClick = gui_exportButton_onClick;
	}

	X += button_import_size;
	m_editorTypeCombo = yyGUICreateComboBox(v2f(X, 0.f), 50.f, m_fontDefault, 0);
	m_editorTypeComboItem_3D = m_editorTypeCombo->AddItem(L"3D");
	m_editorTypeComboItem_UV = m_editorTypeCombo->AddItem(L"UV");
	m_editorTypeCombo->SelectItem(m_editorTypeComboItem_3D);
	m_editorTypeCombo->m_onSelect = gui_editorTypeCombo_onSelect;


	m_button_importWindow_import = yyGUICreateButton(v4f(
		g_guiWindowBackgroundPBRect.x,
		g_guiWindowBackgroundPBRect.w - 30.f,
		g_guiWindowBackgroundPBRect.x + 130.f,
		g_guiWindowBackgroundPBRect.w
	), 0, -1, m_mainMenu_drawGroup, 0);
	m_button_importWindow_import->SetText(L"Import", m_fontDefault, false);
	m_button_importWindow_import->m_textColor.set(0.9f);
	m_button_importWindow_import->m_textColorHover.set(1.0f);
	m_button_importWindow_import->m_textColorPress.set(0.6f);
	m_button_importWindow_import->m_isAnimated = true;
	m_button_importWindow_import->SetVisible(false);
	m_button_importWindow_import->m_bgColor = yyColor(0.3f, 0.3f, 0.9f, 1.f);
	m_button_importWindow_import->m_bgColorHover = yyColor(0.4f, 0.4f, 0.9f, 1.f);
	m_button_importWindow_import->m_bgColorPress = yyColor(0.2f, 0.2f, 0.9f, 1.f);

	m_button_exportWindow_export = yyGUICreateButton(v4f(
		g_guiWindowBackgroundPBRect.x,
		g_guiWindowBackgroundPBRect.w - 30.f,
		g_guiWindowBackgroundPBRect.x + 130.f,
		g_guiWindowBackgroundPBRect.w
	), 0, -1, m_mainMenu_drawGroup, 0);
	m_button_exportWindow_export->SetText(L"Export", m_fontDefault, false);
	m_button_exportWindow_export->m_textColor.set(0.9f);
	m_button_exportWindow_export->m_textColorHover.set(1.0f);
	m_button_exportWindow_export->m_textColorPress.set(0.6f);
	m_button_exportWindow_export->m_isAnimated = true;
	m_button_exportWindow_export->SetVisible(false);
	m_button_exportWindow_export->m_bgColor = yyColor(0.3f, 0.3f, 0.9f, 1.f);
	m_button_exportWindow_export->m_bgColorHover = yyColor(0.4f, 0.4f, 0.9f, 1.f);
	m_button_exportWindow_export->m_bgColorPress = yyColor(0.2f, 0.2f, 0.9f, 1.f);


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
	m_textInput_rename->m_onEnter = gui_rename_onEnter;
	m_textInput_rename->m_onEscape = gui_rename_onEscape;
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

	{
		v4f uvregion1(0.f,96.f,23.f,119.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_selectByName = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_selectByName->m_useBackground = true;
		m_button_selectByName->m_isAnimated = true;
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(24.f, 96.f, 47.f, 119.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_transformModeNoTransform = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_transformModeNoTransform->m_onClick = gui_buttonTransformModeNoTransform_onClick;
		m_button_transformModeNoTransform->m_useBackground = true;
		m_button_transformModeNoTransform->m_isAnimated = true;
		m_button_transformModeNoTransform->m_useAsCheckbox = true;
		m_button_transformModeNoTransform->m_isChecked = true;
		m_button_transformModeNoTransform->m_buttonGroup = m_buttonGroup_transformMode;
		m_buttonGroup_transformMode->m_buttons.push_back(m_button_transformModeNoTransform);
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(48.f, 96.f, 71.f, 119.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_transformModeMove = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_transformModeMove->m_onClick = gui_buttonTransformModeMove_onClick;
		m_button_transformModeMove->m_useBackground = true;
		m_button_transformModeMove->m_isAnimated = true;
		m_button_transformModeMove->m_useAsCheckbox = true;
		m_button_transformModeMove->m_buttonGroup = m_buttonGroup_transformMode;
		m_buttonGroup_transformMode->m_buttons.push_back(m_button_transformModeMove);
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(72.f, 96.f, 95.f, 119.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_transformModeScale = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_transformModeScale->m_onClick = gui_buttonTransformModeScale_onClick;
		m_button_transformModeScale->m_useBackground = true;
		m_button_transformModeScale->m_isAnimated = true;
		m_button_transformModeScale->m_useAsCheckbox = true;
		m_button_transformModeScale->m_buttonGroup = m_buttonGroup_transformMode;
		m_buttonGroup_transformMode->m_buttons.push_back(m_button_transformModeScale);
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(96.f, 96.f, 119.f, 119.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_transformModeRotate = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_transformModeRotate->m_onClick = gui_buttonTransformModeRotate_onClick;
		m_button_transformModeRotate->m_useBackground = true;
		m_button_transformModeRotate->m_isAnimated = true;
		m_button_transformModeRotate->m_useAsCheckbox = true;
		m_button_transformModeRotate->m_buttonGroup = m_buttonGroup_transformMode;
		m_buttonGroup_transformMode->m_buttons.push_back(m_button_transformModeRotate);
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(120.f, 96.f, 143.f, 119.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_transformModeScaleLocal = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_transformModeScaleLocal->m_onClick = gui_buttonTransformModeScaleLocal_onClick;
		m_button_transformModeScaleLocal->m_useBackground = true;
		m_button_transformModeScaleLocal->m_isAnimated = true;
		m_button_transformModeScaleLocal->m_useAsCheckbox = true;
		m_button_transformModeScaleLocal->m_buttonGroup = m_buttonGroup_transformMode;
		m_buttonGroup_transformMode->m_buttons.push_back(m_button_transformModeScaleLocal);
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(144.f, 96.f, 167.f, 119.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_transformModeRotateLocal = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_transformModeRotateLocal->m_onClick = gui_buttonTransformModeRotateLocal_onClick;
		m_button_transformModeRotateLocal->m_useBackground = true;
		m_button_transformModeRotateLocal->m_isAnimated = true;
		m_button_transformModeRotateLocal->m_useAsCheckbox = true;
		m_button_transformModeRotateLocal->m_buttonGroup = m_buttonGroup_transformMode;
		m_buttonGroup_transformMode->m_buttons.push_back(m_button_transformModeRotateLocal);
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(0.f, 144.f, 23.f, 167.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_editModeVertex = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), 0, 0, &uvregion1);
		m_button_editModeVertex->m_onClick = gui_buttonEditModeVertex_onClick;
		m_button_editModeVertex->m_useBackground = true;
		m_button_editModeVertex->m_isAnimated = true;
		m_button_editModeVertex->m_useAsCheckbox = true;
		m_button_editModeVertex->m_bgColorPress = ColorYellow;
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(24.f, 144.f, 47.f, 167.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_editModeEdge = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), 1, 0, &uvregion1);
		m_button_editModeEdge->m_onClick = gui_buttonEditModeVertex_onClick;
		m_button_editModeEdge->m_useBackground = true;
		m_button_editModeEdge->m_isAnimated = true;
		m_button_editModeEdge->m_useAsCheckbox = true;
		m_button_editModeEdge->m_bgColorPress = ColorYellow;
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(48.f, 144.f, 71.f, 167.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_editModePolygon = yyGUICreateButton(v4f(
			0.f,
			m_mainMenu_Y,
			w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), 2, 0, &uvregion1);
		m_button_editModePolygon->m_onClick = gui_buttonEditModeVertex_onClick;
		m_button_editModePolygon->m_useBackground = true;
		m_button_editModePolygon->m_isAnimated = true;
		m_button_editModePolygon->m_useAsCheckbox = true;
		m_button_editModePolygon->m_bgColorPress = ColorYellow;
		m_mainMenu_Y += h;
	}

	// RIGHT SIDE
	f32 topIndent = 5.f;
	m_mainMenu_Y = miViewportTopIndent + topIndent;
	{
		v4f uvregion1(0.f, 120.f, 23.f, 143.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_objectCommonParams = yyGUICreateButton(v4f(
			window->m_creationSize.x - miViewportRightIndent,
			m_mainMenu_Y,
			window->m_creationSize.x - miViewportRightIndent + w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_objectCommonParams->m_align = yyGUIElement::AlignRightTop;
		m_button_objectCommonParams->m_onClick = gui_buttonObjectCommonParams_onClick;
		m_button_objectCommonParams->m_useBackground = true;
		m_button_objectCommonParams->m_isAnimated = true;
		m_button_objectCommonParams->m_useAsCheckbox = true;
		m_button_objectCommonParams->m_isChecked = true;
		m_button_objectCommonParams->m_buttonGroup = m_buttonGroup_rightSide;
		m_buttonGroup_rightSide->m_buttons.push_back(m_button_objectCommonParams);
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(24.f, 120.f, 47.f, 143.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_objectObjectParams = yyGUICreateButton(v4f(
			window->m_creationSize.x - miViewportRightIndent,
			m_mainMenu_Y,
			window->m_creationSize.x - miViewportRightIndent + w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_objectObjectParams->m_align = yyGUIElement::AlignRightTop;
		m_button_objectObjectParams->m_onClick = gui_buttonObjectObjectParams_onClick;
		m_button_objectObjectParams->m_useBackground = true;
		m_button_objectObjectParams->m_isAnimated = true;
		m_button_objectObjectParams->m_useAsCheckbox = true;
		m_button_objectObjectParams->m_buttonGroup = m_buttonGroup_rightSide;
		m_buttonGroup_rightSide->m_buttons.push_back(m_button_objectObjectParams);
		m_mainMenu_Y += h;
	}
	{
		v4f uvregion1(48.f, 120.f, 71.f, 143.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_materials = yyGUICreateButton(v4f(
			window->m_creationSize.x - miViewportRightIndent,
			m_mainMenu_Y,
			window->m_creationSize.x - miViewportRightIndent + w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_materials->m_align = yyGUIElement::AlignRightTop;
		m_button_materials->m_onClick = gui_buttonMaterials_onClick;
		m_button_materials->m_useBackground = true;
		m_button_materials->m_isAnimated = true;
		m_button_materials->m_useAsCheckbox = true;
		m_button_materials->m_buttonGroup = m_buttonGroup_rightSide;
		m_buttonGroup_rightSide->m_buttons.push_back(m_button_objectObjectParams);
		m_mainMenu_Y += h;
	}

	//m_button_UV
	{
		v4f uvregion1(72.f, 120.f, 95.f, 143.f);
		f32 w = (f32)(uvregion1.z - uvregion1.x);
		f32 h = (f32)(uvregion1.w - uvregion1.y);
		m_button_UV = yyGUICreateButton(v4f(
			window->m_creationSize.x - miViewportRightIndent,
			m_mainMenu_Y,
			window->m_creationSize.x - miViewportRightIndent + w,
			m_mainMenu_Y + h
		), yyGetTexture(L"../res/gui/icons.png"), -1, 0, &uvregion1);
		m_button_UV->m_align = yyGUIElement::AlignRightTop;
		m_button_UV->m_onClick = gui_buttonUV_onClick;
		m_button_UV->m_useBackground = true;
		m_button_UV->m_isAnimated = true;
		m_button_UV->m_useAsCheckbox = true;
		m_button_UV->m_buttonGroup = m_buttonGroup_rightSide;
		m_buttonGroup_rightSide->m_buttons.push_back(m_button_objectObjectParams);
		m_mainMenu_Y += h;
	}

	// COMMON PARAMS
	auto y = miViewportTopIndent + topIndent;
	m_gui_drawGroup_commonParams = yyGUICreateDrawGroup();
	m_gui_drawGroup_commonParams->SetDraw(true);
	m_gui_group_commonParams = yyGUICreateGroup(v4f(), -1, m_gui_drawGroup_commonParams);
	m_gui_group_commonParams->m_align = m_gui_group_commonParams->AlignRightTop;
	{
		m_gui_group_commonParams_text_Position = yyGUICreateText(v2f(
			window->m_creationSize.x - miViewportRightIndent + miRightSideButtonSize, y),
			m_fontDefault, L"Position XYZ:", m_gui_drawGroup_commonParams);
		m_gui_group_commonParams_text_Position->IgnoreInput(true);
		m_gui_group_commonParams->AddElement(m_gui_group_commonParams_text_Position);
	}
	{
		m_gui_group_commonParams_range_PositionX = yyGUICreateRangeSliderFloatNoLimit(
			v4f(
				window->m_creationSize.x - miViewportRightIndent + miRightSideButtonSize + 
				m_gui_group_commonParams_text_Position->m_sensorRectInPixels.z - m_gui_group_commonParams_text_Position->m_sensorRectInPixels.x,
				y,
				window->m_creationSize.x,
				y + 15.f
			),
			&m_default_value_float,
			false, m_gui_drawGroup_commonParams);
		m_gui_group_commonParams_range_PositionX->UseText(m_fontDefault);
		m_gui_group_commonParams_range_PositionX->m_valueMultiplerNormal = 0.1f;
		m_gui_group_commonParams_range_PositionX->m_valueMultiplerAlt = 0.01f;
		m_gui_group_commonParams_range_PositionX->m_onValueChanged = gui_group_commonParams_range_Position;
		m_gui_group_commonParams->AddElement(m_gui_group_commonParams_range_PositionX);
		y += 15.f;
	}
	{
		m_gui_group_commonParams_range_PositionY = yyGUICreateRangeSliderFloatNoLimit(
			v4f(
				window->m_creationSize.x - miViewportRightIndent + miRightSideButtonSize +
				m_gui_group_commonParams_text_Position->m_sensorRectInPixels.z - m_gui_group_commonParams_text_Position->m_sensorRectInPixels.x,
				y,
				window->m_creationSize.x,
				y + 15.f
			),
			&m_default_value_float,
			false, m_gui_drawGroup_commonParams);
		m_gui_group_commonParams_range_PositionY->UseText(m_fontDefault);
		m_gui_group_commonParams_range_PositionY->m_valueMultiplerNormal = 0.1f;
		m_gui_group_commonParams_range_PositionY->m_valueMultiplerAlt = 0.01f;
		m_gui_group_commonParams_range_PositionY->m_onValueChanged = gui_group_commonParams_range_Position;
		m_gui_group_commonParams->AddElement(m_gui_group_commonParams_range_PositionY);
		y += 15.f;
	}
	{
		m_gui_group_commonParams_range_PositionZ = yyGUICreateRangeSliderFloatNoLimit(
			v4f(
				window->m_creationSize.x - miViewportRightIndent + miRightSideButtonSize +
				m_gui_group_commonParams_text_Position->m_sensorRectInPixels.z - m_gui_group_commonParams_text_Position->m_sensorRectInPixels.x,
				y,
				window->m_creationSize.x,
				y + 15.f
			),
			&m_default_value_float,
			false, m_gui_drawGroup_commonParams);
		m_gui_group_commonParams_range_PositionZ->UseText(m_fontDefault);
		m_gui_group_commonParams_range_PositionZ->m_valueMultiplerNormal = 0.1f;
		m_gui_group_commonParams_range_PositionZ->m_valueMultiplerAlt = 0.01f;
		m_gui_group_commonParams_range_PositionZ->m_onValueChanged = gui_group_commonParams_range_Position;
		m_gui_group_commonParams->AddElement(m_gui_group_commonParams_range_PositionZ);
		y += 15.f;
	}

	// MATERIALS
	{
		v4f groupRect = v4f(
			window->m_creationSize.x - miViewportRightIndent + miRightSideButtonSize,
			miViewportTopIndent + topIndent,
			window->m_creationSize.x,
			window->m_creationSize.y);

		m_gui_drawGroup_materials = yyGUICreateDrawGroup();
		m_gui_drawGroup_materials->SetDraw(false);
		m_gui_drawGroup_materials->SetInput(false);
		m_gui_group_materials = yyGUICreateGroup(
			groupRect,
			-1, m_gui_drawGroup_materials);
		m_gui_group_materials->m_align = m_gui_group_materials->AlignRightTop;
		f32 y = 0.f;
		f32 x = 0;
		{
			auto t = yyGUICreateText(v2f(x, y),
				m_fontDefault, L"Materials:", m_gui_drawGroup_materials);
			t->IgnoreInput(true);
			m_gui_group_materials->AddElement(t);
			y += m_fontDefault->m_maxHeight;
		}


		f32 matLbSz = 150.f;
		m_gui_listbox_materials = yyGUICreateListBox(
			v4f(
				x,
				y, 
				x + miViewportRightIndent,
				y + matLbSz),
			m_fontDefault, m_gui_drawGroup_materials);
		m_gui_listbox_materials->m_onSelect = gui_lbMaterials_onSelect;
		m_gui_listbox_materials->m_onTextEnter = gui_renameMaterial_onEnter;
		m_gui_group_materials->AddElement(m_gui_listbox_materials);
		y += matLbSz;

		f32 addButtonY = 15.f;

		{
			auto addButton = yyGUICreateButton(
				v4f(x, y, x + 40.f, y + addButtonY),
				0, -1, m_gui_drawGroup_materials, 0);
			addButton->SetText(L"Add", m_fontDefault, false);
			addButton->m_isAnimated = true;
			addButton->m_onRelease = gui_buttonMaterialsAdd_onRelease;
			addButton->m_bgColor.set(0.5f);
			addButton->m_bgColorHover.set(0.65f);
			addButton->m_bgColorPress.set(0.35f);
addButton->m_textColor.set(0.95f);
addButton->m_textColorHover.set(1.f);
addButton->m_textColorPress.set(0.6f);
m_gui_group_materials->AddElement(addButton);
		}

		{
			x += groupRect.z - groupRect.x - 40.f;
			auto deleteButton = yyGUICreateButton(
				v4f(x, y, x + 40.f, y + addButtonY),
				0, -1, m_gui_drawGroup_materials, 0);
			deleteButton->SetText(L"Delete", m_fontDefault, false);
			deleteButton->m_isAnimated = true;
			deleteButton->m_onRelease = gui_buttonMaterialsDelete_onRelease;
			deleteButton->m_bgColor = ColorDarkRed;
			deleteButton->m_bgColorHover.set(0.65f);
			deleteButton->m_bgColorPress.set(0.35f);
			deleteButton->m_textColor.set(0.95f);
			deleteButton->m_textColorHover.set(1.f);
			deleteButton->m_textColorPress.set(0.6f);
			m_gui_group_materials->AddElement(deleteButton);
		}

		{
			x = 40.f;
			auto assignButton = yyGUICreateButton(
				v4f(x, y, groupRect.z - groupRect.x - 40.f, y + addButtonY),
				0, -1, m_gui_drawGroup_materials, 0);
			assignButton->SetText(L"Assign", m_fontDefault, false);
			assignButton->m_isAnimated = true;
			assignButton->m_onRelease = gui_buttonMaterialsAssign_onRelease;
			assignButton->m_bgColor.set(0.5f);
			assignButton->m_bgColorHover.set(0.65f);
			assignButton->m_bgColorPress.set(0.35f);
			assignButton->m_textColor.set(0.95f);
			assignButton->m_textColorHover.set(1.f);
			assignButton->m_textColorPress.set(0.6f);
			m_gui_group_materials->AddElement(assignButton);

			y += addButtonY;
		}

		x = 0.f;
		y += 10.f;
		{
			auto t = yyGUICreateText(v2f(x, y),
				m_fontDefault, L"Maps:", m_gui_drawGroup_materials);
			t->IgnoreInput(true);
			m_gui_group_materials->AddElement(t);
			y += m_fontDefault->m_maxHeight;
		}
		f32 mapsLbSz = 100.f;
		m_gui_listbox_maps = yyGUICreateListBox(
			v4f(
				x,
				y,
				x + miViewportRightIndent,
				y + mapsLbSz),
			m_fontDefault, m_gui_drawGroup_materials);
		m_gui_listbox_maps->m_onSelect = gui_lbMaps_onSelect;
		m_gui_listbox_maps->m_isEditable = false;
		m_gui_group_materials->AddElement(m_gui_listbox_maps);
		y += mapsLbSz;
		miString s;
		for (s32 i = 0; i < miMaterialMaxMaps; ++i)
		{
			s.clear();
			s = L"Map";
			s += i;
			if (i == 0) s = L"Diffuse";
			else if (i == 1) s = L"Normal/Bump";
			else if (i == 2) s = L"Specular Highlight";
			else if (i == 3) s = L"Specular Color";
			else if (i == 4) s = L"Ambient Color";
			else if (i == 5) s = L"Alpha";
			else if (i == 6) s = L"Displacement";
			else if (i == 7) s = L"Reflection";

			m_gui_listbox_maps->AddItem(s.data());
		}
		{
			f32 pbSize = miViewportRightIndent;
			m_gui_pictureBox_map = yyGUICreatePictureBox(
				v4f(x, y, x + pbSize, y + pbSize),
				g_app->m_blackTexture,
				-1,
				m_gui_drawGroup_materials,
				0);
			m_gui_group_materials->AddElement(m_gui_pictureBox_map);
			m_gui_pictureBox_map->IgnoreInput(true);
			y += pbSize;
		}
		{
			auto btn = yyGUICreateButton(
				v4f(x, y, x + 80.f, y + addButtonY),
				0, -1, m_gui_drawGroup_materials, 0);
			btn->SetText(L"Load Image", m_fontDefault, false);
			btn->m_isAnimated = true;
			btn->m_onRelease = gui_buttonMaterialsLoadImage_onRelease;
			btn->m_bgColor.set(0.5f);
			btn->m_bgColorHover.set(0.65f);
			btn->m_bgColorPress.set(0.35f);
			btn->m_textColor.set(0.95f);
			btn->m_textColorHover.set(1.f);
			btn->m_textColorPress.set(0.6f);
			m_gui_group_materials->AddElement(btn);
		}

		{
			x += groupRect.z - groupRect.x - 40.f;
			auto btn = yyGUICreateButton(
				v4f(x, y, x + 40.f, y + addButtonY),
				0, -1, m_gui_drawGroup_materials, 0);
			btn->SetText(L"Delete", m_fontDefault, false);
			btn->m_isAnimated = true;
			btn->m_onRelease = gui_buttonMaterialsDeleteImage_onRelease;
			btn->m_bgColor = ColorDarkRed;
			btn->m_bgColorHover.set(0.65f);
			btn->m_bgColorPress.set(0.35f);
			btn->m_textColor.set(0.95f);
			btn->m_textColorHover.set(1.f);
			btn->m_textColorPress.set(0.6f);
			m_gui_group_materials->AddElement(btn);
		}
	}
	
	// UV
	{
		//m_gui_drawGroup_UV
		v4f groupRect = v4f(
			window->m_creationSize.x - miViewportRightIndent + miRightSideButtonSize,
			miViewportTopIndent + topIndent,
			window->m_creationSize.x,
			window->m_creationSize.y);

		m_gui_drawGroup_UV = yyGUICreateDrawGroup();
		m_gui_drawGroup_UV->SetDraw(false);
		m_gui_drawGroup_UV->SetInput(false);

		auto currDrawGroup = m_gui_drawGroup_UV;

		m_gui_group_UV = yyGUICreateGroup(
			groupRect,
			-1, currDrawGroup);
		m_gui_group_UV->m_align = m_gui_group_UV->AlignRightTop;
		f32 y = 0.f;
		f32 x = 0;
		{
			auto t = yyGUICreateText(v2f(x, y),
				m_fontDefault, L"UV Editor:", currDrawGroup);
			t->IgnoreInput(true);
			m_gui_group_UV->AddElement(t);
			y += m_fontDefault->m_maxHeight;
		}

		f32 btnSzY = 20.f;
		{
			x += 10.f; ;
			auto openEditorButton = yyGUICreateButton(
				v4f(x, y, groupRect.z - groupRect.x - 10.f, y + btnSzY),
				0, -1, currDrawGroup, 0);
			openEditorButton->m_bgColor.set(0.5f);
			openEditorButton->m_bgColorHover.set(0.65f);
			openEditorButton->m_bgColorPress.set(0.35f);
			openEditorButton->m_textColor.set(0.95f);
			openEditorButton->m_textColorHover.set(1.f);
			openEditorButton->m_textColorPress.set(0.6f);
			openEditorButton->SetText(L"Open UV editor", m_fontDefault, false);
			openEditorButton->m_isAnimated = true;
			openEditorButton->m_onRelease = gui_buttonUvOpenEditor_onRelease;
			m_gui_group_UV->AddElement(openEditorButton);
		}

		y += btnSzY + 20.f;

		btnSzY = 15.f;
		{
			auto btn = yyGUICreateButton(
				v4f(x, y, groupRect.z - groupRect.x - 10.f, y + btnSzY),
				0, -1, currDrawGroup, 0);
			btn->m_bgColor.set(0.5f);
			btn->m_bgColorHover.set(0.65f);
			btn->m_bgColorPress.set(0.35f);
			btn->m_textColor.set(0.95f);
			btn->m_textColorHover.set(1.f);
			btn->m_textColorPress.set(0.6f);
			btn->SetText(L"Make planar", m_fontDefault, false);
			btn->m_isAnimated = true;
			btn->m_onRelease = gui_buttonUvMakePlanar_onRelease;
			m_gui_group_UV->AddElement(btn);
		}
		y += btnSzY + 3.f;

		btnSzY = 15.f;
		{
			auto btn = yyGUICreateButton(
				v4f(x, y, groupRect.z - groupRect.x - 10.f, y + btnSzY),
				0, -1, currDrawGroup, 0);
			btn->m_bgColor.set(0.5f);
			btn->m_bgColorHover.set(0.65f);
			btn->m_bgColorPress.set(0.35f);
			btn->m_textColor.set(0.95f);
			btn->m_textColorHover.set(1.f);
			btn->m_textColorPress.set(0.6f);
			btn->SetText(L"Make planar (screen)", m_fontDefault, false);
			btn->m_isAnimated = true;
			btn->m_onRelease = gui_buttonUvMakePlanarScreen_onRelease;
			m_gui_group_UV->AddElement(btn);
		}
		y += btnSzY + 11.f;

		btnSzY = 15.f;
		{
			auto btn = yyGUICreateButton(
				v4f(x, y, groupRect.z - groupRect.x - 10.f, y + btnSzY),
				0, -1, currDrawGroup, 0);
			btn->m_bgColor.set(0.5f);
			btn->m_bgColorHover.set(0.65f);
			btn->m_bgColorPress.set(0.35f);
			btn->m_textColor.set(0.95f);
			btn->m_textColorHover.set(1.f);
			btn->m_textColorPress.set(0.6f);
			btn->SetText(L"Flatten Mapping", m_fontDefault, false);
			btn->m_isAnimated = true;
			btn->m_onRelease = gui_buttonUvFlatten_onRelease;
			m_gui_group_UV->AddElement(btn);
		}
		y += btnSzY + 3.f;
	}
}

miGUIManager::~miGUIManager(){
	if (m_gui_drawGroup_commonParams)
		yyGUIDeleteDrawGroup(m_gui_drawGroup_commonParams);

	if (m_buttonGroup_transformMode)
		delete m_buttonGroup_transformMode;

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

	if (m_activePluginGUI)
	{
		m_activePluginGUI->Show(false);
		m_activePluginGUI = 0;
		m_button_importWindow_import->SetVisible(false);
		m_button_exportWindow_export->SetVisible(false);
	}
	
	m_mainMenu_drawGroup->SetDraw(false);
	m_mainMenu_drawGroup->SetInput(false);
}
void miGUIManager::ShowMenu(s32 buttonID) {
	HideMenu();

	m_mainMenu_drawGroup->SetDraw(true);
	m_mainMenu_drawGroup->SetInput(true);

	yyGUIDrawGroupMoveFront(m_mainMenu_drawGroup);
	m_isMainMenuActive = true;
	//m_mainMenu_windowBackgroundPB->SetVisible(true);
	//m_mainMenu_backgroundPB->SetVisible(true);
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
	switch (m_id)
	{
	case g_buttonID_File_NewScene:
	default:
	//	printf("new\n");
		break;
	}
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

	f32 w = (f32)(uvregion1.z - uvregion1.x);
	f32 h = (f32)(uvregion1.w - uvregion1.y);

	v4f buildRect;
	buildRect.y = m_mainMenu_Y;
	buildRect.z = w;
	buildRect.w = m_mainMenu_Y + h;

	m_mainMenu_Y += h;

	auto newButton = yyGUICreateButton(buildRect,
		yyGetTexture(L"../res/gui/icons.png"), -1, 0, &reg);
	newButton->m_id = id;
	reg = uvregion2;
	newButton->SetMouseHoverTexture(yyGetTexture(L"../res/gui/icons.png"), &reg);
	//newButton->SetOpacity(g_guiButtonMinimumOpacity, 0);
	//newButton->SetOpacity(g_guiButtonMinimumOpacity, 1);
	//newButton->SetOpacity(g_guiButtonMinimumOpacity, 2);
	newButton->m_onClick = onClick;
	newButton->m_isAnimated = true;
	//newButton->m_onDraw = gui_mainMenu_buttons_onDraw;
	//newButton->m_onMouseEnter = gui_mainMenu_buttonOnMouseEnter;
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

void miGUIManager_button_importWindow_import_onClick(yyGUIElement* elem, s32 m_id) {
	g_guiManager->HideMenu();
	g_app->OnImport_openDialog();
}

void miGUIManager_button_exportWindow_export_onClick(yyGUIElement* elem, s32 m_id) {
	g_guiManager->HideMenu();
	g_app->OnExport_openDialog();
}

void miGUIManager::ShowExportMenu(miPluginGUI* gui) {
	m_mainMenu_backgroundPB->m_onDraw = gui_mainMenu_backgroundPB_onDraw_show;
	m_mainMenu_drawGroup->SetDraw(true);
	m_mainMenu_drawGroup->SetInput(true);
	m_mainMenu_drawGroup->MoveFront();
	m_button_exportWindow_export->SetVisible(true);
	m_button_exportWindow_export->m_onRelease = miGUIManager_button_exportWindow_export_onClick;

	m_activePluginGUI = (miPluginGUIImpl*)gui;
	m_activePluginGUI->Show(true);
}

void miGUIManager::ShowImportMenu(miPluginGUI* gui) {
	m_mainMenu_backgroundPB->m_onDraw = gui_mainMenu_backgroundPB_onDraw_show;
	m_mainMenu_drawGroup->SetDraw(true);
	m_mainMenu_drawGroup->SetInput(true);
	m_mainMenu_drawGroup->MoveFront();
	m_button_importWindow_import->SetVisible(true);
	m_button_importWindow_import->m_onRelease = miGUIManager_button_importWindow_import_onClick;

	m_activePluginGUI = (miPluginGUIImpl*)gui;
	m_activePluginGUI->Show(true);
}

void miGUIManager::UpdateTransformModeButtons() {
	for (u32 i = 0, sz = m_buttonGroup_transformMode->m_buttons.size(); i < sz; ++i)
	{
		m_buttonGroup_transformMode->m_buttons[i]->m_isChecked = false;
	}
	switch (g_app->m_transformMode)
	{
	default:
	case miTransformMode::NoTransform:
		m_button_transformModeNoTransform->m_isChecked = true;
		break;
	case miTransformMode::Move:
		m_button_transformModeMove->m_isChecked = true;
		break;
	case miTransformMode::Scale:
		m_button_transformModeScale->m_isChecked = true;
		break;
	case miTransformMode::Rotate:
		m_button_transformModeRotate->m_isChecked = true;
		break;
	}
}

void miGUIManager::SetCommonParamsRangePosition() {
	//printf("SET\n");
	m_gui_group_commonParams_range_PositionX->m_ptr_f = &m_default_value_float;
	m_gui_group_commonParams_range_PositionY->m_ptr_f = &m_default_value_float;
	m_gui_group_commonParams_range_PositionZ->m_ptr_f = &m_default_value_float;
	if (g_app->m_selectedObjects.m_size == 1)
	{
		auto pos = g_app->m_selectedObjects.m_data[0]->GetLocalPosition();
		m_gui_group_commonParams_range_PositionX->m_ptr_f = &pos->x;
		m_gui_group_commonParams_range_PositionY->m_ptr_f = &pos->y;
		m_gui_group_commonParams_range_PositionZ->m_ptr_f = &pos->z;
	}
	else
	{
		m_commonParams_range_Position_many = g_app->m_gizmo->m_position + g_app->m_gizmo->m_var_move;
		m_gui_group_commonParams_range_PositionX->m_ptr_f = &m_commonParams_range_Position_many.x;
		m_gui_group_commonParams_range_PositionY->m_ptr_f = &m_commonParams_range_Position_many.y;
		m_gui_group_commonParams_range_PositionZ->m_ptr_f = &m_commonParams_range_Position_many.z;
	}
}

void miGUIManager::OnNewMaterial(miMaterial* m) {
	auto item = m_gui_listbox_materials->AddItem(m->m_name.data());
	item->SetUserData(m);
	m_gui_listbox_materials->SelectItem(item);
	UpdateMaterialMapPictureBox();
}

void miGUIManager::AssignSelectedMaterial() {
	for (u32 i = 0, sz = m_gui_listbox_materials->GetItemsCount(); i < sz; ++i)
	{
		auto item = m_gui_listbox_materials->GetItem(i);
		if (item->IsSelected())
		{
			miMaterial* mat = (miMaterial*)item->GetUserData();
			
			for (u32 o2 = 0; o2 < g_app->m_selectedObjects.m_size; ++o2)
			{
				g_app->MaterialAssign(mat, g_app->m_selectedObjects.m_data[o2]);
			}
			break;
		}
	}
}

void miGUIManager::DeleteImageFromMaterial() {
	for (u32 i = 0, sz = m_gui_listbox_materials->GetItemsCount(); i < sz; ++i)
	{
		auto item = m_gui_listbox_materials->GetItem(i);
		if (item->IsSelected())
		{
			miMaterial* mat = (miMaterial*)item->GetUserData();
			for (u32 o = 0; o < g_app->m_materials.m_size; ++o)
			{
				auto pair = g_app->m_materials.m_data[o];
				if (mat == pair->m_first)
				{
					for (u32 k = 0; k < m_gui_listbox_maps->GetItemsCount(); ++k)
					{
						auto item2 = m_gui_listbox_maps->GetItem(k);
						if (item2->IsSelected())
						{
							if (pair->m_first->m_maps[k].m_GPUTexture)
							{
								g_app->MaterialDeleteTexture((yyGPUTexture*)pair->m_first->m_maps[k].m_GPUTexture);
								pair->m_first->m_maps[k].m_GPUTexture = 0;
							}
							break;
						}
					}
				}
			}
			break;
		}
	}
	UpdateMaterialMapPictureBox();
}

void miGUIManager::DeleteSelectedMaterial() {
	yyGUIListBoxItem* lastItem = 0;
	yyGUIListBoxItem* nextItem = 0;
	for (u32 i = 0, sz = m_gui_listbox_materials->GetItemsCount(); i < sz; ++i)
	{
		if( i + 1  < sz)
			nextItem = m_gui_listbox_materials->GetItem(i + 1);

		auto item = m_gui_listbox_materials->GetItem(i);
		if (item->IsSelected())
		{
			miMaterial* mat = (miMaterial*)item->GetUserData();
			for (u32 o = 0; o < g_app->m_materials.m_size; ++o)
			{
				auto pair = g_app->m_materials.m_data[o];
				if (mat == pair->m_first)
				{
					pair->m_second = 0;

					// delete all textures 
					for (u32 k = 0; k < m_gui_listbox_maps->GetItemsCount(); ++k)
					{
						if (pair->m_first->m_maps[k].m_GPUTexture)
						{
							g_app->MaterialDeleteTexture((yyGPUTexture*)pair->m_first->m_maps[k].m_GPUTexture);
							pair->m_first->m_maps[k].m_GPUTexture = 0;
						}
					}
				}
			}

			m_gui_listbox_materials->DeleteItem(item);

			if (lastItem)
				m_gui_listbox_materials->SelectItem(lastItem);
			else if (nextItem)
				m_gui_listbox_materials->SelectItem(nextItem);
			m_gui_listbox_materials->Rebuild();
			break;
		}

		lastItem = item;
	}
	UpdateMaterialMapPictureBox();
}

void miGUIManager::LoadNewImageForMaterial() {
	for (u32 i = 0, sz = m_gui_listbox_materials->GetItemsCount(); i < sz; ++i)
	{
		auto item = m_gui_listbox_materials->GetItem(i);
		if (item->IsSelected())
		{
			miMaterial* mat = (miMaterial*)item->GetUserData();

			for (u32 o = 0; o < miMaterialMaxMaps; ++o)
			{
				if (m_gui_listbox_maps->GetItem(o)->IsSelected())
				{
					yyStringA supportedExtensions;
					yyGetImageExtensions(&supportedExtensions);
					auto path = yyOpenFileDialog("Texture file", "Select", supportedExtensions.data(), "Supported files");
					if (path)
					{
						auto * map = &mat->m_maps[o];
						map->m_texturePath = path->data();
						{
							if(map->m_GPUTexture)
								g_app->MaterialDeleteTexture((yyGPUTexture*)map->m_GPUTexture);

							map->m_GPUTexture = g_app->MaterialGetTexture(map->m_texturePath.data());
							m_gui_pictureBox_map->SetTexture((yyGPUTexture*)map->m_GPUTexture);
							m_gui_pictureBox_map->Rebuild();
						}
						yyDestroy(path);
					}
					break;
				}
			}
			break;
		}
	}
}

void miGUIManager::UpdateMaterialMapPictureBox() {
	m_gui_pictureBox_map->m_texture = g_app->m_blackTexture;
	for (u32 i = 0, sz = m_gui_listbox_materials->GetItemsCount(); i < sz; ++i)
	{
		auto item = m_gui_listbox_materials->GetItem(i);
		if (item->IsSelected())
		{
			miMaterial* mat = (miMaterial*)item->GetUserData();

			for (u32 o = 0; o < miMaterialMaxMaps; ++o)
			{
				if (m_gui_listbox_maps->GetItem(o)->IsSelected())
				{
					auto * map = &mat->m_maps[o];
					if(map->m_GPUTexture)
						m_gui_pictureBox_map->m_texture = (yyGPUTexture*)map->m_GPUTexture;
					return;
				}
			}
		}
	}
}