#ifndef _MIPL_STD_H_
#define _MIPL_STD_H_

void miplStd_ImportOBJ(const wchar_t* fileName);
void miplStd_ExportOBJ(const wchar_t* fileName);

class miplStd : public miPlugin
{
	miSceneObject* m_newObjectPtr;
	void _destroyNewObject();
	void _dropNewObject();

	bool m_isLMBDown;

	miPluginGUI* m_gui_for_plane;
	miPluginGUI* m_gui_for_importOBJ;
	miPluginGUI* m_gui_for_exportOBJ;

public:
	miplStd();
	virtual ~miplStd();

	virtual const wchar_t* GetName() override;
	virtual const wchar_t* GetDescription() override;
	virtual const wchar_t* GetAuthor() override;

	virtual bool IsDebug() override;

	virtual void Init(miSDK* sdk) override;
	virtual int CheckVersion() override;

	virtual void OnCreateObject(unsigned int objectId) override;
	virtual void OnCursorMove(miSelectionFrust*, bool isCursorInGUI) override;
	virtual void OnLMBDown(miSelectionFrust*, bool isCursorInGUI) override;
	virtual void OnLMBUp(miSelectionFrust*, bool isCursorInGUI) override;
	virtual void OnCancel(miSelectionFrust*, bool isCursorInGUI) override;
	virtual void OnUpdate(miSelectionFrust*, bool isCursorInGUI) override;

	virtual void OnImportExport(const wchar_t* fileName, unsigned int id) override;
	virtual void OnShiftGizmo(miGizmoMode, miEditMode, miSceneObject*) override;

	miSDK* m_sdk;

	friend class miplPolygonObjectPlane;
};

#endif