#ifndef _MI_PLUGINGUIIMPL_H_
#define _MI_PLUGINGUIIMPL_H_

class miPluginGUIImpl : public miPluginGUI
{
	miPluginGUIType m_type;
	yyGUIDrawGroup* m_gui_drawGroup;
	yyGUIGroup* m_gui_group;

	miBinarySearchTree<yyGUIButtonGroup*> m_buttonGroup;

	struct element_info
	{
		element_info() {
			m_textInput_onSelectObject_text = 0;
			m_slider_onSelectObject_slider_i = 0;
			m_slider_onSelectObject_slider_f = 0;
			m_slider_onValueChanged_slider_i = 0;
			m_slider_onValueChanged_slider_f = 0;
			m_checkbox_onClick = 0;
			m_button_onClick = 0;
			m_buttonAsCheckBox_onClick = 0;
			m_onCheck = 0;
			m_onUncheck = 0;
			m_flags = 0;
		}
		~element_info() {
		}

		yyGUIElement* m_element;
		const wchar_t* (*m_textInput_onSelectObject_text)(miSceneObject*);
		int*   (*m_slider_onSelectObject_slider_i)(miSceneObject*);
		float* (*m_slider_onSelectObject_slider_f)(miSceneObject*);
		void   (*m_slider_onValueChanged_slider_i)(miSceneObject*, int);
		void   (*m_slider_onValueChanged_slider_f)(miSceneObject*, float);

		void(*m_checkbox_onClick)(bool isChecked);
		void(*m_button_onClick)(s32 id);
		void(*m_buttonAsCheckBox_onClick)(s32 id, bool isChecked);
		void(*m_onCheck)(s32);
		void(*m_onUncheck)(s32);

		u32 m_flags;
	};

	yyArraySimple<element_info*> m_gui_elements;

	void _init(miPluginGUIType);

	friend void miPluginGUIImpl_buttonAsCheckbox_onCheck(yyGUIElement* elem, s32 m_id);
	friend void miPluginGUIImpl_buttonAsCheckbox_onUncheck(yyGUIElement* elem, s32 m_id);
	friend void miPluginGUIImpl_buttonAsCheckbox_onClick(yyGUIElement* elem, s32 m_id);
	friend void miPluginGUIImpl_button_onClick(yyGUIElement* elem, s32 m_id);
	friend void miPluginGUIImpl_checkBox_onClick(yyGUIElement* elem, s32 m_id);
	friend void miPluginGUIImpl_slider_onValueChanged(yyGUIRangeSlider* slider);
	friend void miPluginGUIImpl_gui_group_onRebuildSetRects(yyGUIElement* elem, s32 m_id);
	friend class miSDKImpl;
public:
	miPluginGUIImpl();
	virtual ~miPluginGUIImpl();

	virtual void AddText(const v2f& position, const wchar_t* text, const wchar_t* (*onSelectObject)(miSceneObject*), u32 flags) override;
	virtual void AddRangeSliderInt(const v4f& positionSize, int minimum, int maximum, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int), u32 flags) override;
	virtual void AddRangeSliderFloat(const v4f& positionSize, float minimum, float maximum, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float), u32 flags) override;
	virtual void AddRangeSliderIntNoLimit(const v4f& positionSize, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int), u32 flags) override;
	virtual void AddRangeSliderFloatNoLimit(const v4f& positionSize, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float), u32 flags) override;
	virtual void AddCheckBox(const v2f& position, const wchar_t* text, void(*onClick)(bool isChecked), bool isChecked, u32 flags) override;
	virtual void AddButton(const v4f& positionSize, const wchar_t* text, s32 id, void(*onClick)(s32), u32 flags) override;
	virtual void AddButtonAsCheckbox(const v4f& positionSize, const wchar_t* text, s32 id, void(*onClick)(s32, bool), 
		void(*onCheck)(s32), void(*onUncheck)(s32), s32 buttonGroupIndex, u32 flags) override;
	virtual void UncheckButtonGroup(s32 buttonGroupIndex) override;

	virtual void Show(bool);

	void OnSelectObject(miSceneObject*);
};

#endif