#ifndef _MI_SDKIMPL_H_
#define _MI_SDKIMPL_H_



struct miObjectCategory_object
{
	yyStringW m_objectName;
	u32 m_popupIndex;
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

	void AddObject(const wchar_t* objectName, u32 popupIndexID) {
		miObjectCategory_object* newObj = new miObjectCategory_object;
		newObj->m_objectName = objectName;
		newObj->m_popupIndex = popupIndexID;
		m_objects.push_back(newObj);
	}

	yyStringW m_categoryName;
	
	yyArraySmall<miObjectCategory_object*> m_objects;
};

class miSDKImpl : public miSDK
{
	yyArraySmall<miObjectCategory*> m_objectCategories;
	miObjectCategory* _getObjectCategory(const wchar_t* category);
public:
	miSDKImpl();
	virtual ~miSDKImpl();

	virtual miVisualObject* CreateVisualObject();

	virtual miViewportCameraType GetActiveViewportCameraType();

	virtual miKeyboardModifier GetKeyboardModifier();
	virtual miCursorBehaviorMode GetCursorBehaviorModer();
	virtual void SetCursorBehaviorModer(miCursorBehaviorMode);
	virtual miVec2 GetCursorPosition2D();
	virtual miVec3 GetCursorPosition3D();
	virtual miVec3 GetCursorPosition3DFirstClick();
	virtual miEditMode GetEditMode();
	virtual void SetEditMode(miEditMode);

	virtual unsigned int  RegisterNewObject(miPlugin* plugin, const wchar_t* category, const wchar_t* objectName);
	virtual void GetRayFromScreen(miRay* ray, const miVec2& coords, const miVec4& viewportRect, const miMatrix& VPInvert);

	virtual void SetActivePlugin(miPlugin*);

	virtual void AddObjectToScene(miSceneObject*, const wchar_t* name);
	virtual void RemoveObjectFromScene(miSceneObject*);

	friend class miApplication;
};

#endif