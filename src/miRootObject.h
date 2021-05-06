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
	virtual void OnCreationLMBDown();
	virtual void OnCreationLMBUp();
	virtual void OnCreationMouseMove();

	virtual miPlugin* GetPlugin();
};

#endif