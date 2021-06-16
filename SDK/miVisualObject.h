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

	// create CPU beffers then create GPU models
	virtual void CreateNewGPUModels(miMesh*) = 0;

	// vertex and index beffuer count
	virtual size_t GetBufferCount() = 0;
	
	virtual unsigned char* GetVertexBuffer(size_t index) = 0;
	
	//virtual void MarkBufferToRemap(size_t index) = 0;
	//virtual void RemapBuffers() = 0;

	// call this in miSceneObject::OnDraw()
	virtual void Draw() = 0;

	virtual miVisualObjectType GetType() = 0;

	virtual Aabb GetAabb() = 0;

	virtual bool IsInSelectionFrust(miSelectionFrust* sf) = 0;
	virtual void SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) = 0;
	virtual void Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) = 0;
	virtual void SelectAll() = 0;
	virtual void DeselectAll() = 0;
	virtual void InvertSelection() = 0;

	virtual bool IsRayIntersect(yyRay* r, v4f* ip, float* d) = 0;
};

#endif