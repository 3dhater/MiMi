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
	miDefaultAllocator<miPolygon>* m_allocatorPolygon;
	miDefaultAllocator<miEdge>* m_allocatorEdge;
	miDefaultAllocator<miVertex>* m_allocatorVertex;
	void _destroyMesh();

	void _selectVertex(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectEdge(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectPolygon(miKeyboardModifier km, miSelectionFrust* sf);
	void _selectVerts_rectangle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	void _selectEdges_rectangle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	void _selectPolygons_rectangle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	void _selectAllVerts();
	void _selectAllEdges();
	void _selectAllPolygons();
	void _deselectAllPolygons();
	void _deselectAllVerts();
	void _deselectAllEdges();
	void _selectInvertVerts();
	void _selectInvertEdges();
	void _selectInvertPolygons();
	
	void _transformMove(const v3f& move_delta, bool isCancel);
	void _transformScale(Mat4* S, const v3f& C, bool isCancel);
	void _transformRotate(Mat4* R, const v3f& C, bool isCancel);

	void _deletePolygon(miPolygon*);

	yyArraySimple<miPair<miVertex*,v3f>> m_vertsForTransform;
	void _updateVertsForTransformArray(miEditMode em);

	void _callVisualObjectOnTransform();

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

	virtual miPlugin* GetPlugin() override;
	virtual void DeleteSelectedObjects(miEditMode em) override;
	virtual void RebuildVisualObjects() override;
	virtual int GetVisualObjectCount() override;
	virtual miVisualObject* GetVisualObject(int) override;
	
	virtual int GetMeshCount();
	virtual miMesh* GetMesh(int);

	virtual bool IsVertexMouseHover(miSelectionFrust* sf) override;
	virtual bool IsEdgeMouseHover(miSelectionFrust*) override;

	virtual bool IsVertexSelected()override;
	virtual bool IsEdgeSelected() override;
	virtual bool IsPolygonSelected() override;

	virtual void OnTransformVertex(miTransformMode, const v3f& move_delta, Mat4* scale, const v3f& center, bool isCancel) override;
	virtual void OnTransformEdge(miTransformMode, const v3f& move_delta, Mat4* scale, const v3f& center, bool isCancel) override;
	virtual void OnTransformPolygon(miTransformMode, const v3f& move_delta, Mat4* scale, const v3f& center, bool isCancel) override;

	virtual void OnSelect(miEditMode) override;
	virtual void OnSetEditMode(miEditMode) override;
	virtual void OnEndTransform(miEditMode) override;
};

#endif