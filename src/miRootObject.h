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
};

#endif