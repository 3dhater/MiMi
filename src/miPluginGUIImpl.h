#ifndef _MI_PLUGINGUIIMPL_H_
#define _MI_PLUGINGUIIMPL_H_

class miPluginGUIImpl : public miPluginGUI
{
	miPluginGUIType m_type;
	yyGUIDrawGroup* m_gui_drawGroup;
	yyGUIGroup* m_gui_group;
	
	struct element_info
	{
		element_info() {
			m_textInput_onSelectObject_text = 0;
			m_slider_onSelectObject_slider_i = 0;
			m_slider_onSelectObject_slider_f = 0;
			m_slider_onValueChanged_slider_i = 0;
			m_slider_onValueChanged_slider_f = 0;
			m_checkbox_onClick = 0;
		}

		yyGUIElement* m_element;
		const wchar_t* (*m_textInput_onSelectObject_text)(miSceneObject*);
		int*   (*m_slider_onSelectObject_slider_i)(miSceneObject*);
		float* (*m_slider_onSelectObject_slider_f)(miSceneObject*);
		void   (*m_slider_onValueChanged_slider_i)(miSceneObject*, int);
		void   (*m_slider_onValueChanged_slider_f)(miSceneObject*, float);

		void(*m_checkbox_onClick)(bool isChecked);
	};

	yyArray<element_info> m_gui_elements;

	void _init(miPluginGUIType);

	friend void miPluginGUIImpl_checkBox_onClick(yyGUIElement* elem, s32 m_id);
	friend void miPluginGUIImpl_slider_onValueChanged(yyGUIRangeSlider* slider);
	friend void miPluginGUIImpl_gui_group_onRebuildSetRects(yyGUIElement* elem, s32 m_id);
	friend class miSDKImpl;
public:
	miPluginGUIImpl();
	virtual ~miPluginGUIImpl();

	virtual void AddText(const v2f& position, const wchar_t* text, const wchar_t* (*onSelectObject)(miSceneObject*));
	virtual void AddRangeSliderInt(const v4f& rect, int minimum, int maximum, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int));
	virtual void AddRangeSliderFloat(const v4f& rect, float minimum, float maximum, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float));
	virtual void AddRangeSliderIntNoLimit(const v4f& rect, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int));
	virtual void AddRangeSliderFloatNoLimit(const v4f& rect, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float));
	virtual void AddCheckBox(const v2f& position, const wchar_t* text, void(*onClick)(bool isChecked), bool isChecked);

	virtual void Show(bool);

	void OnSelectObject(miSceneObject*);
};

#endif