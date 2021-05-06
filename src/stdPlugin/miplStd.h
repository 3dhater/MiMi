#ifndef _MIPL_STD_H_
#define _MIPL_STD_H_

class miplStd : public miPlugin
{
	miSceneObject* m_newObjectPtr;
	void _destroyNewObject();
	void _saveNewObject();
public:
	miplStd();
	virtual ~miplStd();

	virtual const wchar_t* GetName();
	virtual const wchar_t* GetDescription();
	virtual const wchar_t* GetAuthor();

	virtual bool IsDebug();

	virtual bool Init(miSDK* sdk);

	virtual void OnPopupCommand(unsigned int);
	virtual void OnCursorMove(miSelectionFrust*);
	virtual void OnLMBDown(miSelectionFrust*);
	virtual void OnLMBUp(miSelectionFrust*);
	virtual void OnCancel(miSelectionFrust*);
	virtual void OnUpdate(miSelectionFrust*);

	miSDK* m_sdk;

	friend class miplPolygonObjectPlane;
};

#endif