#ifndef _MI_GUIMANAGER_H_
#define _MI_GUIMANAGER_H_

#include "yy_gui.h"

//struct miGUIShowMenuInfo

struct miGUIMainMenuItem 
{
	miGUIMainMenuItem() {
		m_button = 0;
		//m_text = 0;
		//m_onClick = 0;
	}
	~miGUIMainMenuItem() {
	}
	yyGUIButton* m_button;
	//yyGUIText* m_text;
	v4f m_activeArea_noActive;
	v4f m_activeArea_Active;
	//yyGUICallback m_onClick;
};

struct miGUIMainMenuMenuGroup
{
	miGUIMainMenuMenuGroup(s32 mainButtonID) {
		m_mainButtonID = mainButtonID;
	};
	~miGUIMainMenuMenuGroup() {};

	void addButton(const wchar_t* text, const v4f& rect, s32 id, yyGUIDrawGroup* dg);

	yyArraySmall<yyGUIButton*> m_buttons;
	s32 m_mainButtonID;
};

#define g_buttonID_File 0
#define g_buttonID_File_NewScene 2
#define g_buttonID_Settings 1

class miGUIManager
{
	yyVideoDriverAPI* m_gpu;

	yyGUIFont* m_fontDefault;

	f32 m_mainMenu_Y;
	miPluginGUIImpl* m_activePluginGUI;

	yyGUIDrawGroup* m_mainMenu_drawGroup;
	bool m_isMainMenuActive;
	s32 m_hoveredMenuItemID;
	
	yyArraySmall<miGUIMainMenuMenuGroup*> m_mainMenu_menus;
	miGUIMainMenuMenuGroup* _addMainMenuItem(const wchar_t* text, //const v4f& buildRect, 
		const v4f& uvregion1, const v4f& uvregion2, const v4f& uvregion3,
		s32 id, yyGUICallback onClick);

public:
	miGUIManager();
	~miGUIManager();

	enum Font {
		Default
	};

	yyGUIFont* GetFont(Font);
	void ShowMenu(s32);
	void HideMenu();

	void ShowImportMenu(miPluginGUI* gui);

	friend struct miGUIMainMenuMenuGroup;
	bool m_isMainMenuInCursor;
	yyGUIGroup* m_mainMenu_group;
	v4f m_mainMenu_group_actRect_noActive;
	v4f m_mainMenu_group_actRect_Active;
	yyArraySmall<miGUIMainMenuItem*> m_mainMenu_items;
	s32 m_selectedMenuItemID;
	yyGUIPictureBox* m_mainMenu_backgroundPB;
	yyGUIPictureBox* m_mainMenu_windowBackgroundPB;
	
	yyGUIButton*  m_button_add;
	yyGUIButton*  m_button_import;
	yyGUITextInput* m_textInput_rename;
	
	yyGUIButton*  m_button_importWindow_import;
	
	yyGUIButton*  m_button_selectByName;

	void UpdateTransformModeButtons();
	yyGUIButtonGroup* m_buttonGroup_transformMode;
	yyGUIButton*  m_button_transformModeNoTransform;
	yyGUIButton*  m_button_transformModeMove;
	yyGUIButton*  m_button_transformModeScale;
	yyGUIButton*  m_button_transformModeRotate;
	yyGUIButton*  m_button_transformModeScaleLocal;
	yyGUIButton*  m_button_transformModeRotateLocal;
	yyGUIButton*  m_button_editModeVertex;
	yyGUIButton*  m_button_editModeEdge;
	yyGUIButton*  m_button_editModePolygon;
	
	yyGUIButtonGroup* m_buttonGroup_rightSide;
	yyGUIButton*  m_button_objectCommonParams;
	yyGUIButton*  m_button_objectObjectParams;
	yyGUIButton*  m_button_materials;
	yyGUIButton*  m_button_UV;
	
	yyGUIGroup*   m_gui_group_commonParams;
	yyGUIDrawGroup* m_gui_drawGroup_commonParams;
	yyGUIText*    m_gui_group_commonParams_text_Position;
	yyGUIRangeSlider*    m_gui_group_commonParams_range_PositionX;
	yyGUIRangeSlider*    m_gui_group_commonParams_range_PositionY;
	yyGUIRangeSlider*    m_gui_group_commonParams_range_PositionZ;
	f32 m_default_value_float;
	// call this when select/deselect object
	void SetCommonParamsRangePosition();
	v3f m_commonParams_range_Position_many;
	v3f m_commonParams_range_Position_many_onClick;

	yyGUIGroup*   m_gui_group_materials;
	yyGUIDrawGroup* m_gui_drawGroup_materials;
	yyGUIListBox* m_gui_listbox_materials;
	yyGUIListBox* m_gui_listbox_maps;
	yyGUIPictureBox* m_gui_pictureBox_map;
	void OnNewMaterial(miMaterial*);
	void DeleteSelectedMaterial();
	void AssignSelectedMaterial();
	void LoadNewImageForMaterial();
	void DeleteImageFromMaterial();
	void UpdateMaterialMapPictureBox();

	yyGUIComboBox* m_editorTypeCombo;
	yyGUIComboBoxItem* m_editorTypeComboItem_3D;
	yyGUIComboBoxItem* m_editorTypeComboItem_UV;
	
	yyGUIDrawGroup* m_gui_drawGroup_UV;
	yyGUIGroup*   m_gui_group_UV;
};

#endif