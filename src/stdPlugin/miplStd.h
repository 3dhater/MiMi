#ifndef _MIPL_STD_H_
#define _MIPL_STD_H_

class miplStd : public miPlugin
{
	miSceneObject* m_newObjectPtr;
	void _destroyNewObject();
	void _dropNewObject();

	bool m_isLMBDown;

	miPluginGUI* m_gui_for_plane;
	miPluginGUI* m_gui_for_importOBJ;

public:
	miplStd();
	virtual ~miplStd();

	virtual const wchar_t* GetName();
	virtual const wchar_t* GetDescription();
	virtual const wchar_t* GetAuthor();

	virtual bool IsDebug();

	virtual void Init(miSDK* sdk);
	virtual int CheckVersion();

	virtual void OnCreateObject(unsigned int objectId);
	virtual void OnCursorMove(miSelectionFrust*, bool isCursorInGUI);
	virtual void OnLMBDown(miSelectionFrust*, bool isCursorInGUI);
	virtual void OnLMBUp(miSelectionFrust*, bool isCursorInGUI);
	virtual void OnCancel(miSelectionFrust*, bool isCursorInGUI);
	virtual void OnUpdate(miSelectionFrust*, bool isCursorInGUI);

	virtual void OnImport(const wchar_t* fileName, unsigned int id);

	miSDK* m_sdk;

	friend class miplPolygonObjectPlane;
};

#endif