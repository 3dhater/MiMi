#ifndef _MI_EDITABLEOBJECT_H_
#define _MI_EDITABLEOBJECT_H_

class miEditableObject : public miSceneObject
{
	miSDK* m_sdk;
	miPlugin* m_plugin;

	miVisualObject* m_visualObject_polygon;
	miVisualObject* m_visualObject_vertex;
	miVisualObject* m_visualObject_edge;

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


	yyArraySimple<miPair<miVertex*,v3f>> m_vertsForTransform;
	void _updateVertsForTransformArray(miEditMode em);

	void _updateModel(bool onlyEdge = false, bool updateCounts = true);

	void _createMeshFromTMPMesh_meshBuilder(bool saveSelection, bool weldSelected);

	miArray<miPair<miListNode<miPolygon::_vertex_data>*, v2f>> m_selectedUV;
	v2f m_UVScale;

	friend class miApplication;
	friend class miSDKImpl;
	friend void editableObjectGUI_tgweldButton_onSelectSecond(miSceneObject* o, miVertex* v1, miVertex* v2);
	friend void editableObjectGUI_movetoButton_onSelectSecond(miSceneObject* o, miVertex* v1, miVertex* v2);
	friend void editableObjectGUI_weldButton_onCancel();
	friend void editableObjectGUI_vertexChamferButton_onCancel();
	friend void editableObjectGUI_weldRange_onValueChanged(miSceneObject* obj, float* fptr);
	friend void editableObjectGUI_weldButton_onUncheck(s32 id);
	friend void editableObjectGUI_vertexChamferButton_onUncheck(s32 id);
	friend void editableObjectGUI_vertexChamferRange_onValueChanged(miSceneObject*obj, float* fptr);
	friend void editableObjectGUI_selectEdgeLoop_onClick(s32 id);
	friend void editableObjectGUI_selectEdgeRing_onClick(s32 id);
	friend void editableObjectGUI_selectEdgeBorder_onClick(s32 id);
	friend void editableObjectGUI_edgeChamferButton_onUncheck(s32 id);

public:
	miEditableObject(miSDK*, miPlugin*);
	virtual ~miEditableObject();

	virtual void OnDraw(miViewportDrawMode, miEditMode, float dt) override;
	virtual void OnDrawUV() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnCreation(miPluginGUI*) override;
	virtual void OnCreationLMBDown() override;
	virtual void OnCreationLMBUp() override;
	virtual void OnCreationMouseMove() override;
	virtual void OnCreationEnd() override;
	virtual void OnConvertToEditableObject() override;

	virtual void SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	virtual void Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	virtual void SelectAll(miEditMode em);
	virtual void DeselectAll(miEditMode em);
	virtual void InvertSelection(miEditMode em);

	virtual miPlugin* GetPlugin() override;
	virtual void DeleteSelectedObjects(miEditMode em) override;
	virtual void RebuildVisualObjects(bool onlyEditMode) override;
	virtual int GetVisualObjectCount() override;
	virtual miVisualObject* GetVisualObject(int) override;
	
	virtual int GetMeshCount();
	virtual miMesh* GetMesh(int);

	virtual miVertex* IsVertexMouseHover(miSelectionFrust* sf) override;
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

	/*virtual void CallVisualObjectOnTransform() override;
	virtual void CallVisualObjectOnSelect() override;*/

	miDefaultAllocator<miPolygon>* m_allocatorPolygon;
	miDefaultAllocator<miEdge>* m_allocatorEdge;
	miDefaultAllocator<miVertex>* m_allocatorVertex;
	miMesh* m_mesh;

	// always update rebuild edges after this
	void DeletePolygon(miPolygon*);
	
	void VertexConnect();
	void VertexBreak();
	bool VertexTargetWeld(miVertex* v1, miVertex* v2, miPolygon** polygonForDelete1, miPolygon** polygonForDelete2);
	void VertexMoveTo(miVertex* v1, miVertex* v2);

	void AttachObject(miEditableObject*);

	bool m_isWeld;
	f32 m_weldValue;
	
	miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>> * m_meshBuilderTmpModelPool;
	void CreateTMPModelWithPoolAllocator(s32 polygonNum, s32 edgeNum, s32 vertexNum);
	void DestroyTMPModelWithPoolAllocator();
	void OnWeld(bool createNewTMPModel);
	void OnWeldApply();
	void _createMeshFromTMPMesh();

	bool m_isVertexChamfer;
	f32 m_vertexChamferValue;
	bool m_addPolygonsWhenVertexChamfer;
	void OnVertexChamfer();
	void OnVertexChamferApply();

	void EdgeExtrude();
	void EdgeConnect();
	void EdgeBridge();

	bool m_isEdgeChamfer;
	f32 m_edgeChamferValue;
	bool m_addPolygonsWhenEdgeChamfer;
	void OnEdgeChamfer();
	void OnEdgeChamferApply();

	void PolygonFlip();
	void PolygonCalculateNormal(bool smooth);
	void PolygonDetachAsElement();
	void PolygonDetachAsObject();
	void PolygonDuplicate();
	void PolygonExtrude();

	void UpdateUVSelection(miEditMode, Aabb*);
	void _updateUVSelectionArray(Aabb*);
	virtual void UVUpdateAAABB(Aabb*) override;
	virtual void UVSelect(miSelectionFrust*, miKeyboardModifier, miEditMode, Aabb*) override;
	virtual void UVSelectAll(miEditMode, Aabb*) override;
	virtual void UVTransform(miGizmoUVMode, miKeyboardModifier, const v2f&, f32) override;
	virtual void UVTransformCancel(miGizmoUVMode gm, const Aabb& currAabb, const v4f& aabbCenterOnClick) override;
	virtual void UVTransformAccept() override;
	virtual void UVMakePlanar(bool useScreenPlane) override;
	virtual void UvFlattenMapping() override;
	void RebuildUVModel();

	virtual void ApplyPivotOffset() override;
	virtual void PivotToObjectCenter() override;
	virtual void PivotToSceneCenter() override;
};

#endif