#ifndef _MI_EDITABLEOBJECT_H_
#define _MI_EDITABLEOBJECT_H_

class miEditableObject : public miSceneObject
{
	miSDK* m_sdk;
	miPlugin* m_plugin;

	miVisualObject* m_visualObject_polygon;
	miVisualObject* m_visualObject_vertex;
	miVisualObject* m_visualObject_edge;

	//miMeshBuilder<miDefaultAllocator<miPolygon>, miDefaultAllocator<miEdge>, miDefaultAllocator<miVertex>>* m_meshBuilder;
	miMesh* m_mesh;

	void _selectVertex(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectVerts_rectangle(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectEdge(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectPolygon(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectEdges_rectangle(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectPolygons_rectangle(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectAllVerts();
	void _selectAllEdges();
	void _selectAllPolygons();
	void _deselectAllPolygons();
	void _deselectAllVerts();
	void _deselectAllEdges();
	void _selectInvertVerts();
	void _selectInvertEdges();
	void _selectInvertPolygons();

	friend class miApplication;
	friend class miSDKImpl;
public:
	miEditableObject(miSDK*, miPlugin*);
	virtual ~miEditableObject();

	virtual void OnDraw(miViewportDrawMode, miEditMode, float dt);
	virtual void OnUpdate(float dt);
	virtual void OnCreation(miPluginGUI*);
	virtual void OnCreationLMBDown();
	virtual void OnCreationLMBUp();
	virtual void OnCreationMouseMove();
	virtual void OnCreationEnd();
	virtual void OnConvertToEditableObject();

	virtual void SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	virtual void Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	virtual void SelectAll(miEditMode em);
	virtual void DeselectAll(miEditMode em);
	virtual void InvertSelection(miEditMode em);

	virtual miPlugin* GetPlugin();
	virtual void DeleteSelectedObjects(miEditMode em);

	virtual int GetVisualObjectCount();
	virtual miVisualObject* GetVisualObject(int);
	
	virtual int GetMeshCount();
	virtual miMesh* GetMesh(int);

	virtual bool IsVertexMouseHover(miSelectionFrust* sf) override;
	virtual bool IsEdgeMouseHover(miSelectionFrust*) override;

	virtual bool IsVertexSelected()override;
	virtual bool IsEdgeSelected() override;
	virtual bool IsPolygonSelected() override;
};

#endif