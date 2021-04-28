#ifndef _MI_VISOBJIMPL_H_
#define _MI_VISOBJIMPL_H_

class miVisualObjectImpl : public miVisualObject
{
	yyResource* m_modelGPU;
	miMatrix m_transform;
public:
	miVisualObjectImpl();
	virtual ~miVisualObjectImpl();

	virtual miMatrix* GetTransform();
	virtual void Draw();
};

#endif