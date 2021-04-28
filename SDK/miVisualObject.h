#ifndef _MI_VISOBJ_H_
#define _MI_VISOBJ_H_

class miVisualObject
{
public:
	miVisualObject() {}
	virtual ~miVisualObject() {}

	// call this in miSceneObject::OnDraw()
	virtual void Draw() = 0;

	virtual miMatrix* GetTransform() = 0;
};

#endif