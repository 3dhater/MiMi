#ifndef _MI_VISOBJ_H_
#define _MI_VISOBJ_H_

enum class miVisualObjectType
{
	Vertex,
	Edge,
	Polygon
};

// contain GPU models
class miVisualObject
{
public:
	miVisualObject() {}
	virtual ~miVisualObject() {}

	// create CPU buffers then create GPU models
	virtual void CreateNewGPUModels(miMesh*) = 0;
	
	// only for UV models
	virtual void CreateNewGPUModelsUV(miMesh*) = 0;

	// vertex and index buffer count
	virtual size_t GetBufferCount() = 0;
	
	virtual unsigned char* GetVertexBuffer(size_t index) = 0;
	
	//virtual void MarkBufferToRemap(size_t index) = 0;
	//virtual void RemapBuffers() = 0;

	// call this in miSceneObject::OnDraw()
	virtual void Draw(bool uv) = 0;

	virtual miVisualObjectType GetType() = 0;

	virtual Aabb GetAabb() = 0;
	virtual void UpdateAabb() = 0;

	virtual bool IsInSelectionFrust(miSelectionFrust* sf) = 0;
	virtual bool IsRayIntersect(yyRay* r, v4f* ip, float* d) = 0;

	// rebuild nodes with geometry
	virtual void OnTransform() = 0;
	
	// update some information
	// when select vertex/edge/polygon
	virtual void OnSelect(miEditMode) = 0;
};

#endif