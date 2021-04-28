#ifndef _MI_SDKIMPL_H_
#define _MI_SDKIMPL_H_

class miSDKImpl : public miSDK
{
public:
	miSDKImpl();
	virtual ~miSDKImpl();

	virtual miVisualObject* CreateVisualObject();
	virtual void DestroyVisualObject(miVisualObject*);

	virtual void* AllocateMemory(unsigned int size);
	virtual void  FreeMemory(void*);
};

#endif