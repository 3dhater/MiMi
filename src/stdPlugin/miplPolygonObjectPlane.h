#ifndef _MIPL_PO_PLANE_H_
#define _MIPL_PO_PLANE_H_

class miplPolygonObjectPlane : public miSceneObject
{
	miSDK* m_sdk;
	miPlugin* m_plugin;
	miplStd* m_pluginImpl;
	miVec2 m_size;
public:
	miplPolygonObjectPlane(miSDK*, miPlugin*);
	virtual ~miplPolygonObjectPlane();

	virtual void OnDraw();
	virtual void OnCreationLMBDown();
	virtual void OnCreationLMBUp();
	virtual void OnCreationMouseMove();
	//virtual void OnSetNewName(const char* newName);

	virtual miPlugin* GetPlugin();
};

#endif