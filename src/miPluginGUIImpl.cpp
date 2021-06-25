#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miGUIManager.h"
#include "miPluginGUIImpl.h"

extern miApplication * g_app;
extern v4f g_guiWindowBackgroundPBRect;

void miPluginGUIImpl_gui_group_onRebuildSetRects(yyGUIElement* elem, s32 m_id) {
	miPluginGUIImpl * gui = (miPluginGUIImpl *)elem->m_userData;

	switch (gui->m_type)
	{
	case miPluginGUIType::ObjectParams:
		gui->m_gui_group->m_buildRectInPixels.x = g_app->GetWindowMain()->m_currentSize.x - miViewportRightIndent + miRightSideButtonSize;
		gui->m_gui_group->m_buildRectInPixels.y = miViewportTopIndent;
		gui->m_gui_group->m_buildRectInPixels.z = g_app->GetWindowMain()->m_currentSize.x;
		gui->m_gui_group->m_buildRectInPixels.w = g_app->GetWindowMain()->m_currentSize.y;
		break;
	case miPluginGUIType::ImportExportParams:
		gui->m_gui_group->m_buildRectInPixels = g_guiWindowBackgroundPBRect;
		break;
	default:
		break;
	}

	gui->m_gui_group->m_clipRectInPixels = gui->m_gui_group->m_buildRectInPixels;
	gui->m_gui_group->m_sensorRectInPixels = gui->m_gui_group->m_buildRectInPixels;
}
void miPluginGUIImpl_gui_group_onMouseInRect(yyGUIElement* elem, s32 m_id) {
	//printf("in");
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
	Show(false);
	m_type = t;

	m_gui_group = yyGUICreateGroup(v4f(), -1, m_gui_drawGroup);
	
	switch (t)
	{
	case miPluginGUIType::ObjectParams:
		m_gui_group->m_align = m_gui_group->AlignRightTop;
		break;
	case miPluginGUIType::ImportExportParams:
		m_gui_group->m_align = m_gui_group->AlignLeftTop;
		break;
	default:
		break;
	}

	m_gui_group->m_userData = this;
	m_gui_group->m_onMouseInRect = miPluginGUIImpl_gui_group_onMouseInRect;
	m_gui_group->m_onRebuildSetRects = miPluginGUIImpl_gui_group_onRebuildSetRects;
}

void miPluginGUIImpl::Show(bool show) {
	if (show)
	{
		for (u32 i = 0; i < m_gui_elements.m_size; ++i)
		{
			auto & e = m_gui_elements.m_data[i];
		
			if (e.m_flags & miPluginGUI::Flag_OnlyForObjectEditMode)
				e.m_element->SetVisible(g_app->m_editMode == miEditMode::Object);
			else if (e.m_flags & miPluginGUI::Flag_OnlyForVertexEditMode)
				e.m_element->SetVisible(g_app->m_editMode == miEditMode::Vertex);
			else if (e.m_flags & miPluginGUI::Flag_OnlyForEdgeEditMode)
				e.m_element->SetVisible(g_app->m_editMode == miEditMode::Edge);
			else if (e.m_flags & miPluginGUI::Flag_OnlyForPolygonEditMode)
				e.m_element->SetVisible(g_app->m_editMode == miEditMode::Polygon);
		}
	}

	m_gui_drawGroup->MoveFront();
	m_gui_drawGroup->SetInput(show);
	m_gui_drawGroup->SetDraw(show);
}

void miPluginGUIImpl::AddText(
	const v2f& position, 
	const wchar_t* text, 
	const wchar_t* (*onSelectObject)(miSceneObject*), 
	u32 flags)
{
	yyGUIText* gui_text = yyGUICreateText(position, g_app->m_GUIManager->GetFont(miGUIManager::Font::Default), text, m_gui_drawGroup);
	gui_text->IgnoreInput(true);
	m_gui_group->AddElement(gui_text);

	element_info ei;
	ei.m_element = gui_text;
	ei.m_textInput_onSelectObject_text = onSelectObject;
	ei.m_flags = flags;

	m_gui_elements.push_back(ei);
}

void miPluginGUIImpl_slider_onValueChanged(yyGUIRangeSlider* slider) {
	miPluginGUIImpl* gui = (miPluginGUIImpl*)slider->m_userData;
	for (u32 i = 0; i < gui->m_gui_elements.m_size; ++i)
	{
		auto & ge = gui->m_gui_elements.m_data[i];
		if (ge.m_element == slider)
		{
			switch (slider->m_sliderType)
			{
			case yyGUIRangeSliderType::Float:
			case yyGUIRangeSliderType::FloatLimits: {
				ge.m_slider_onValueChanged_slider_f(g_app->m_selectedObjects.m_data[0], *slider->m_ptr_f);
			}break;
			default:
			case yyGUIRangeSliderType::Int:
			case yyGUIRangeSliderType::IntLimits:{
				ge.m_slider_onValueChanged_slider_i(g_app->m_selectedObjects.m_data[0], *slider->m_ptr_i);
			}break;
			}
		}
	}
}
void miPluginGUIImpl::AddRangeSliderInt(
	const v4f& positionSize, 
	int minimum, 
	int maximum, 
	int* (*onSelectObject)(miSceneObject*), 
	void(*onValueChanged)(miSceneObject*, int), 
	u32 flags)
{
	static int default_int = 0;

	yyGUIRangeSlider* gui_slider = yyGUICreateRangeSliderInt(
		v4f(positionSize.x, positionSize.y, positionSize.x + positionSize.z, positionSize.y + positionSize.w),
		minimum, maximum, &default_int, false, m_gui_drawGroup);
	gui_slider->UseText(g_app->m_GUIManager->GetFont(miGUIManager::Font::Default));
	gui_slider->m_userData = this;
	gui_slider->m_onValueChanged = miPluginGUIImpl_slider_onValueChanged;
	m_gui_group->AddElement(gui_slider);

	element_info ei;
	ei.m_element = gui_slider;
	ei.m_slider_onSelectObject_slider_i = onSelectObject;
	ei.m_slider_onValueChanged_slider_i = onValueChanged;
	ei.m_flags = flags;

	m_gui_elements.push_back(ei);
}

void miPluginGUIImpl::AddRangeSliderFloat(
	const v4f& positionSize,
	float minimum, 
	float maximum, 
	float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float), 
	u32 flags)
{
	static f32 default_float = 0;

	yyGUIRangeSlider* gui_slider = yyGUICreateRangeSliderFloat(
		v4f(positionSize.x, positionSize.y, positionSize.x + positionSize.z, positionSize.y + positionSize.w), minimum, maximum, &default_float, false, m_gui_drawGroup);
	gui_slider->UseText(g_app->m_GUIManager->GetFont(miGUIManager::Font::Default));
	gui_slider->m_userData = this;
	gui_slider->m_onValueChanged = miPluginGUIImpl_slider_onValueChanged;
	m_gui_group->AddElement(gui_slider);

	element_info ei;
	ei.m_element = gui_slider;
	ei.m_slider_onSelectObject_slider_f = onSelectObject;
	ei.m_slider_onValueChanged_slider_f = onValueChanged;
	ei.m_flags = flags;

	m_gui_elements.push_back(ei);
}

void miPluginGUIImpl::AddRangeSliderIntNoLimit(
	const v4f& positionSize,
	int* (*onSelectObject)(miSceneObject*), 
	void(*onValueChanged)(miSceneObject*, int), 
	u32 flags)
{
	static int default_int = 0;

	yyGUIRangeSlider* gui_slider = yyGUICreateRangeSliderIntNoLimit(
		v4f(positionSize.x, positionSize.y, positionSize.x + positionSize.z, positionSize.y + positionSize.w), 
		&default_int, false, m_gui_drawGroup);
	gui_slider->UseText(g_app->m_GUIManager->GetFont(miGUIManager::Font::Default));
	gui_slider->m_userData = this;
	gui_slider->m_onValueChanged = miPluginGUIImpl_slider_onValueChanged;
	m_gui_group->AddElement(gui_slider);

	element_info ei;
	ei.m_element = gui_slider;
	ei.m_slider_onSelectObject_slider_i = onSelectObject;
	ei.m_slider_onValueChanged_slider_i = onValueChanged;
	ei.m_flags = flags;

	m_gui_elements.push_back(ei);
}

void miPluginGUIImpl::AddRangeSliderFloatNoLimit(
	const v4f& positionSize,
	float* (*onSelectObject)(miSceneObject*), 
	void(*onValueChanged)(miSceneObject*, float), 
	u32 flags)
{
	static f32 default_float = 0;

	yyGUIRangeSlider* gui_slider = yyGUICreateRangeSliderFloatNoLimit(
		v4f(positionSize.x, positionSize.y, positionSize.x + positionSize.z, positionSize.y + positionSize.w), 
		&default_float, false, m_gui_drawGroup);
	gui_slider->UseText(g_app->m_GUIManager->GetFont(miGUIManager::Font::Default));
	gui_slider->m_userData = this;
	gui_slider->m_onValueChanged = miPluginGUIImpl_slider_onValueChanged;
	m_gui_group->AddElement(gui_slider);

	element_info ei;
	ei.m_element = gui_slider;
	ei.m_slider_onSelectObject_slider_f = onSelectObject;
	ei.m_slider_onValueChanged_slider_f = onValueChanged;
	ei.m_flags = flags;

	m_gui_elements.push_back(ei);
}

void miPluginGUIImpl_checkBox_onClick(yyGUIElement* elem, s32 m_id) {
	miPluginGUIImpl* gui = (miPluginGUIImpl*)elem->m_userData;
	for (u32 i = 0; i < gui->m_gui_elements.m_size; ++i)
	{
		auto & ge = gui->m_gui_elements.m_data[i];
		if (ge.m_element == elem)
		{
			yyGUICheckBox* checkBox = (yyGUICheckBox*)elem;
			ge.m_checkbox_onClick(checkBox->m_isChecked);
			return;
		}
	}
}

void miPluginGUIImpl::AddCheckBox(
	const v2f& position, 
	const wchar_t* text, 
	void(*onClick)(bool isChecked), 
	bool isChecked, 
	u32 flags)
{
	yyGUICheckBox* checkBox = yyGUICreateCheckBox(position,
		yyGUICheckBoxType::Type1, g_app->m_GUIManager->GetFont(miGUIManager::Font::Default),
		text, m_gui_drawGroup);
	m_gui_group->AddElement(checkBox);
	m_gui_group->m_userData = this;

	checkBox->m_isChecked = isChecked;
	checkBox->m_userData = this;

	element_info ei;
	ei.m_element = checkBox;
	ei.m_checkbox_onClick = onClick;
	ei.m_flags = flags;

	m_gui_elements.push_back(ei);

	checkBox->m_onClick = miPluginGUIImpl_checkBox_onClick;
}

void miPluginGUIImpl_button_onClick(yyGUIElement* elem, s32 m_id) {
	miPluginGUIImpl* gui = (miPluginGUIImpl*)elem->m_userData;
	for (u32 i = 0; i < gui->m_gui_elements.m_size; ++i)
	{
		auto & ge = gui->m_gui_elements.m_data[i];
		if (ge.m_element == elem)
		{
			yyGUIButton* button = (yyGUIButton*)elem;
			ge.m_button_onClick(button->m_id);
			return;
		}
	}
}
void miPluginGUIImpl::AddButton(const v4f& positionSize, const wchar_t* text, s32 id, void(*onClick)(s32), u32 flags) {
	yyGUIButton* button = yyGUICreateButton(
		v4f(positionSize.x, positionSize.y, positionSize.x + positionSize.z, positionSize.y + positionSize.w),
		0, id, m_gui_drawGroup, 0);
	m_gui_group->AddElement(button);
	m_gui_group->m_userData = this;

	button->m_userData = this;
	button->SetText(text, g_app->m_GUIManager->GetFont(miGUIManager::Font::Default), true);
	button->m_bgColor.set(0.5f);
	button->m_bgColorHover.set(0.65f);
	button->m_bgColorPress.set(0.35f);
	button->m_textColor.set(0.95f);
	button->m_textColorHover.set(1.f);
	button->m_textColorPress.set(0.6f);

	element_info ei;
	ei.m_element = button;
	ei.m_button_onClick = onClick;
	ei.m_flags = flags;

	m_gui_elements.push_back(ei);

	button->m_onClick = miPluginGUIImpl_button_onClick;
}

void miPluginGUIImpl::OnSelectObject(miSceneObject* so) {
	for (u32 i = 0; i < m_gui_elements.m_size; ++i)
	{
		auto & e = m_gui_elements.m_data[i];
		switch (e.m_element->m_type)
		{
		case yyGUIElementType::Text: {
			yyGUIText* gui_text = (yyGUIText*)e.m_element;
			if (e.m_textInput_onSelectObject_text)
			{
				auto new_ptr = e.m_textInput_onSelectObject_text(so);
				if (new_ptr)
					gui_text->SetText(new_ptr);
			}
		}break;
		case yyGUIElementType::CheckBox: {
			yyGUICheckBox* gui_checkbox = (yyGUICheckBox*)e.m_element;
			if (e.m_checkbox_onClick)
			{
				e.m_checkbox_onClick(gui_checkbox->m_isChecked);
			}
		}break;
		case yyGUIElementType::RangeSlider: {
			yyGUIRangeSlider* gui_slider = (yyGUIRangeSlider*)e.m_element;
			switch (gui_slider->m_sliderType)
			{
			case yyGUIRangeSliderType::Float:
			case yyGUIRangeSliderType::FloatLimits: {
				if (e.m_slider_onSelectObject_slider_f)
				{
					auto new_ptr = e.m_slider_onSelectObject_slider_f(so);
					if (new_ptr)
					{
						gui_slider->m_ptr_f = new_ptr;
						gui_slider->UpdateText();
					}
				}
			}break;
			default:
			case yyGUIRangeSliderType::Int:
			case yyGUIRangeSliderType::IntLimits:
				if (e.m_slider_onSelectObject_slider_i)
				{
					auto new_ptr = e.m_slider_onSelectObject_slider_i(so);
					if (new_ptr)
					{
						gui_slider->m_ptr_i  = new_ptr;
						gui_slider->UpdateText();
					}
				}
				break;
			}
		}break;
		default:
			break;
		}
	}
}