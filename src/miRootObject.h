#ifndef _MI_ROOTOBJECT_H_
#define _MI_ROOTOBJECT_H_

class miRootObject : public miSceneObject
{
	miSDK* m_sdk;
	miPlugin* m_plugin;
public:
	miRootObject(miSDK*, miPlugin*);
	virtual ~miRootObject();

	virtual void OnDraw();
	virtual void OnUpdate(float dt);
	virtual void OnCreation();
	virtual void OnCreationLMBDown();
	virtual void OnCreationLMBUp();
	virtual void OnCreationMouseMove();
	virtual void OnCreationEnd();

	virtual miPlugin* GetPlugin();

	virtual int GetVisualObjectCount();
	virtual miVisualObject* GetVisualObject(int);

	/*virtual void SelectSingle(miSelectionFrust*);
	virtual void Select(miSelectionFrust*);
	virtual void Deselect(miSelectionFrust*);
	virtual void SelectAll();
	virtual void DeselectAll();*/

	//virtual bool IsRayIntersect(miRay*, miVec4* ip, float* d);
};

#endif