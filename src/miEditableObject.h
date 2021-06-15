#ifndef _MI_EDITABLEOBJECT_H_
#define _MI_EDITABLEOBJECT_H_

class miEditableObject : public miSceneObject
{
	miSDK* m_sdk;
	miPlugin* m_plugin;

	miVisualObject* m_visualObject;

	miMeshBuilder<miDefaultAllocator<miPolygon>, miDefaultAllocator<miEdge>, miDefaultAllocator<miVertex>>
		* m_meshBuilder;

	friend class miSDKImpl;
public:
	miEditableObject(miSDK*, miPlugin*);
	virtual ~miEditableObject();

	virtual void OnDraw();
	virtual void OnUpdate(float dt);
	virtual void OnCreation(miPluginGUI*);
	virtual void OnCreationLMBDown();
	virtual void OnCreationLMBUp();
	virtual void OnCreationMouseMove();
	virtual void OnCreationEnd();
	virtual void OnConvertToEditableObject();

	virtual miPlugin* GetPlugin();
	virtual void DeleteSelectedObjects(miEditMode em);
	virtual int GetVisualObjectCount();
	virtual miVisualObject* GetVisualObject(int);
};

#endif