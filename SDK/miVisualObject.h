#ifndef _MI_VISOBJ_H_
#define _MI_VISOBJ_H_

// contain GPU models
class miVisualObject
{
public:
	miVisualObject() {}
	virtual ~miVisualObject() {}

	// create CPU beffers then create GPU models
	virtual void CreateNewGPUModels(miMesh*) = 0;

	// vertex and index beffuer count
	virtual size_t GetBufferCount() = 0;
	
	virtual unsigned char* GetVertexBuffer(size_t index) = 0;
	
	virtual void MarkBufferToRemap(size_t index) = 0;

	virtual void RemapBuffers() = 0;

	// call this in miSceneObject::OnDraw()
	virtual void Draw(miMatrix*) = 0;

	virtual miAabb GetAabb() = 0;
};

#endif