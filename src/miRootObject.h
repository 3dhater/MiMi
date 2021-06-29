#ifndef _MI_ROOTOBJECT_H_
#define _MI_ROOTOBJECT_H_

class miRootObject : public miSceneObject
{
	miSDK* m_sdk;
	miPlugin* m_plugin;
public:
	miRootObject(miSDK*, miPlugin*);
	virtual ~miRootObject();

	virtual void OnDraw(miViewportDrawMode, miEditMode, float dt);
	virtual void OnUpdate(float dt);
	virtual void OnCreation(miPluginGUI*);
	virtual void OnCreationLMBDown();
	virtual void OnCreationLMBUp();
	virtual void OnCreationMouseMove();
	virtual void OnCreationEnd();
	virtual void OnConvertToEditableObject();

	virtual miPlugin* GetPlugin();

	virtual int GetVisualObjectCount();
	virtual miVisualObject* GetVisualObject(int);

	virtual int GetMeshCount();
	virtual miMesh* GetMesh(int);

	virtual void DeleteSelectedObjects(miEditMode em);
	virtual void RebuildVisualObjects(bool) override;
};

#endif