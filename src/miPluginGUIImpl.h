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
			m_onSelectObject_text = 0;
			m_onSelectObject_slider_i = 0;
			m_onSelectObject_slider_f = 0;
			m_onValueChanged_slider_i = 0;
			m_onValueChanged_slider_f = 0;
		}

		yyGUIElement* m_element;
		const wchar_t* (*m_onSelectObject_text)(miSceneObject*);
		int* (*m_onSelectObject_slider_i)(miSceneObject*);
		float* (*m_onSelectObject_slider_f)(miSceneObject*);
		void(*m_onValueChanged_slider_i)(miSceneObject*, int);
		void(*m_onValueChanged_slider_f)(miSceneObject*, float);
	};

	yyArray<element_info> m_gui_elements;

	void _init(miPluginGUIType);

	friend void miPluginGUIImpl_slider_onValueChanged(yyGUIRangeSlider* slider);
	friend void miPluginGUIImpl_gui_group_onRebuildSetRects(yyGUIElement* elem, s32 m_id);
	friend class miSDKImpl;
public:
	miPluginGUIImpl();
	virtual ~miPluginGUIImpl();

	virtual void AddText(const miVec2& position, const wchar_t* text, const wchar_t* (*onSelectObject)(miSceneObject*));
	virtual void AddRangeSliderInt(const miVec4& rect, int minimum, int maximum, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int));
	virtual void AddRangeSliderFloat(const miVec4& rect, float minimum, float maximum, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float));
	virtual void AddRangeSliderIntNoLimit(const miVec4& rect, int* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, int));
	virtual void AddRangeSliderFloatNoLimit(const miVec4& rect, float* (*onSelectObject)(miSceneObject*), void(*onValueChanged)(miSceneObject*, float));

	virtual void Show(bool);

	void OnSelectObject(miSceneObject*);
};

#endif