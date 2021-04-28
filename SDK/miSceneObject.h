#ifndef _MI_SCOBJ_H_
#define _MI_SCOBJ_H_

class miSceneObject
{
public:
	miSceneObject(){}
	virtual ~miSceneObject() {}

	// app will call it first when will create new object
	virtual void OnCreate() = 0;

	// when app need to draw this object
	virtual void OnDraw() = 0;

	virtual miPlugin* GetPlugin() = 0;
};

#endif