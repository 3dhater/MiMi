#ifndef _MI_SDKIMPL_H_
#define _MI_SDKIMPL_H_


struct miObjectCategory_object
{
	yyStringW m_objectName;
	u32 m_popupIndex;
	u32 m_objectID;
	miPlugin* m_plugin;
};

struct miObjectCategory
{
	miObjectCategory() {}
	~miObjectCategory() {
		for (u16 i = 0, sz = m_objects.size(); i < sz; ++i)
		{
			delete m_objects[i];
		}
	}

	void AddObject(
		miPlugin* p,
		const wchar_t* objectName, 
		u32 popupIndexID, 
		unsigned int objectID) 
	{
		miObjectCategory_object* newObj = new miObjectCategory_object;
		newObj->m_objectName = objectName;
		newObj->m_popupIndex = popupIndexID;
		newObj->m_objectID = objectID;
		newObj->m_plugin = p;
		m_objects.push_back(newObj);
	}

	yyStringW m_categoryName;
	
	yyArraySmall<miObjectCategory_object*> m_objects;
};

struct miImporter
{
	miImporter() {
		m_plugin = 0;
		m_gui = 0;
		m_popupIndex = 0;
		m_importerID = 0;
	}
	~miImporter() {}
	
	miPlugin* m_plugin;
	miPluginGUI* m_gui;
	std::vector<yyStringW> m_extensions;
	
	yyStringW m_title;

	u32 m_popupIndex;
	u32 m_importerID;
};

class miSDKImpl : public miSDK
{
	yyArraySmall<miObjectCategory*> m_objectCategories;
	miObjectCategory* _getObjectCategory(const wchar_t* category);
	
	yyArraySmall<miImporter*> m_importers;
public:
	miSDKImpl();
	virtual ~miSDKImpl();

	virtual miPluginGUI* CreatePluginGUI(miPluginGUIType);
	virtual miVisualObject* CreateVisualObject(miSceneObject* parent);

	virtual miViewportCameraType GetActiveViewportCameraType();

	virtual miKeyboardModifier GetKeyboardModifier();
	virtual miCursorBehaviorMode GetCursorBehaviorModer();
	virtual void SetCursorBehaviorModer(miCursorBehaviorMode);
	virtual miVec2 GetCursorPosition2D();
	virtual miVec3 GetCursorPosition3D();
	virtual miVec3 GetCursorPosition3DFirstClick();
	virtual miEditMode GetEditMode();
	virtual void SetEditMode(miEditMode);

	virtual void UpdateSceneAabb();

	virtual void RegisterNewObject(miPlugin* plugin, unsigned int objectID, const wchar_t* category, const wchar_t* objectName);
	virtual void RegisterImporter(miPlugin*, unsigned int id, const wchar_t* title, const wchar_t* extensions, miPluginGUI* gui);

	virtual void GetRayFromScreen(miRay* ray, const miVec2& coords, const miVec4& viewportRect, const miMatrix& VPInvert);

	virtual void SetActivePlugin(miPlugin*);

	virtual void AddObjectToScene(miSceneObject*, const wchar_t* name);
	virtual void RemoveObjectFromScene(miSceneObject*);
	virtual void CreateSceneObjectFromHelper(miSDKImporterHelper*, const wchar_t* name);

	virtual size_t FileSize(const char* fileName);

	friend class miApplication;
	friend void window_callbackOnCommand(s32 commandID);
};

#endif