#ifndef _MI_SCOBJ_H_
#define _MI_SCOBJ_H_

class miApplication;
class miPluginGUI;

enum miSceneObjectFlag
{
	miSceneObjectFlag_CanConvertToEditableObject = BIT(0)
};


// base class for all scene objects
// children classe must be implemented in plugins
class miSceneObject
{
protected:
	Aabb m_aabb;
	Aabb m_aabbTransformed;
	miString m_name;

	miSceneObject* m_parent;
	miList<miSceneObject*> m_children;
	
	Mat4 m_rotationScaleMatrix;
	Mat4 m_rotationScaleMatrixOnGizmoClick;
	Mat4 m_rotationOnlyMatrix; // i don't know how to get angles from Mat4(ScaleM*RotM)
	Mat4 m_rotationOnlyMatrixOnGizmoClick;  // so I will use this just for information
	Mat4 m_worldMatrix;
	Mat4 m_worldViewProjection;

	v4f m_localPosition;
	v4f m_globalPosition;

	v4f m_edgeColor;

	miPluginGUI* m_gui;

	bool m_isSelected;
	float m_distanceToCamera; // not implemented
	float m_cursorIntersectionPointDistance;
	v4f m_cursorIntersectionPoint;

	// will be set when user select object
	// it will be used in transformations
	//v4f m_selectionAabbOffset;
	v3f m_localPositionOnGizmoClick; // 
	
	u32 m_flags;

	friend class miApplication;
	friend class miGizmo;
	friend struct miViewport;
	friend class miVisualObjectImpl;
public:
	miSceneObject(){
		m_flags = 0;
		m_gui = 0;
		m_parent = 0;
		m_isSelected = false;
		m_distanceToCamera = 0.f;
		m_cursorIntersectionPointDistance = 0.f;
	}
	virtual ~miSceneObject() {}

	virtual void CopyBase(miSceneObject* other)
	{
		m_aabb = other->m_aabb;
		m_aabbTransformed = other->m_aabbTransformed;
		m_name = other->m_name;
		//m_parent = other->m_parent;
		//m_children = other->m_children;
		m_rotationScaleMatrix = other->m_rotationScaleMatrix;
		m_rotationScaleMatrixOnGizmoClick = other->m_rotationScaleMatrixOnGizmoClick;
		m_rotationOnlyMatrix = other->m_rotationOnlyMatrix;
		m_rotationOnlyMatrixOnGizmoClick = other->m_rotationOnlyMatrixOnGizmoClick;
		m_worldMatrix = other->m_worldMatrix;
		m_worldViewProjection = other->m_worldViewProjection;
		m_localPosition = other->m_localPosition;
		m_globalPosition = other->m_globalPosition;
		m_edgeColor = other->m_edgeColor;
		m_gui = other->m_gui;
		m_isSelected = other->m_isSelected;
		m_distanceToCamera = other->m_distanceToCamera;
		m_cursorIntersectionPointDistance = other->m_cursorIntersectionPointDistance;
		m_cursorIntersectionPoint = other->m_cursorIntersectionPoint;
		m_localPositionOnGizmoClick = other->m_localPositionOnGizmoClick;
		m_flags = other->m_flags;
	}

	virtual u32 GetFlags() { return m_flags; }

	virtual miPluginGUI* GetGui() { return m_gui; }
	virtual float GetDistanceToCamera() { return m_distanceToCamera; }
	virtual float GetDistanceToCursorIP() { return m_cursorIntersectionPointDistance; }
	virtual v4f* GetCursorIntersectionPoint() { return &m_cursorIntersectionPoint; }
	virtual bool IsSelected() { return m_isSelected; }
	
	virtual Mat4* GetRotationScaleMatrix() { return &m_rotationScaleMatrix; }
	virtual Mat4* GetRotationOnlyMatrix() { return &m_rotationOnlyMatrix; }
	//virtual Mat4* GetScaleMatrix() { return &m_scaleMatrix; }
	virtual Mat4* GetWorldMatrix() { return &m_worldMatrix; }

	virtual void UpdateTransform() {
		Mat4 T;
		T.setTranslation(m_localPosition);

		/*Mat4 S;
		S.setScale(m_scale);*/
		
		m_worldMatrix = T * m_rotationScaleMatrix;// *S;

		if (m_parent)
		{
			m_worldMatrix = m_parent->m_worldMatrix * m_worldMatrix;
		}

		m_globalPosition = m_worldMatrix.m_data[3];
	}

	virtual v4f* GetLocalPosition() { return &m_localPosition; }
	virtual v4f* GetGlobalPosition() { return &m_globalPosition; }

	virtual v4f* GetEdgeColor() { return &m_edgeColor; }
	virtual void SetEdgeColor(const v4f& c) { m_edgeColor = c; }

	virtual Aabb* GetAABB() { return &m_aabb; }
	virtual Aabb* GetAABBTransformed() { return &m_aabbTransformed; }
	virtual const miString& GetName() { return m_name; }

	virtual miList<miSceneObject*>* GetChildren() { return &m_children; }

	// must be called by application
	virtual void SetName(const wchar_t* newName) {
		assert(newName);
		m_name = newName;
	}

	virtual void SetParent(miSceneObject* o) {
		if (m_parent)
			m_parent->m_children.erase_first(this);

		m_parent = o;

		if (o)
			m_parent->m_children.push_back(this);
	}

	// when app need to draw this object
	virtual void OnDraw(miViewportDrawMode, miEditMode, float dt) = 0;
	virtual void OnUpdate(float dt) = 0;
	// when plugin create object
	virtual void OnCreation(miPluginGUI*) = 0; // when press button\menu command
	virtual void OnCreationLMBDown() = 0;
	virtual void OnCreationLMBUp() = 0;
	virtual void OnCreationMouseMove() = 0;
	virtual void OnCreationEnd() = 0;
	// you can remove/modify some meshes before convert
	virtual void OnConvertToEditableObject() = 0;

	virtual miPlugin* GetPlugin() = 0;

	// will call only for vertex/edge/polygon
	virtual void DeleteSelectedObjects (miEditMode em) = 0;

	virtual int GetVisualObjectCount() = 0;
	virtual miVisualObject* GetVisualObject(int) = 0;
	virtual int GetMeshCount() = 0;
	virtual miMesh* GetMesh(int) = 0;

	virtual bool IsVertexMouseHover(miSelectionFrust*) { return false; }
	virtual bool IsEdgeMouseHover(miSelectionFrust*) { return false; }

	virtual void UpdateAabb() {
		m_aabb.reset();
		for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
			m_aabb.add(GetVisualObject(i)->GetAabb());
		}
		//m_aabb.m_min += m_globalPosition;//no
		//m_aabb.m_max += m_globalPosition;//no
		// m_aabb must be in space center

		//Mat4 S;
		//S.setScale(m_localScale);
		Mat4 m = m_rotationScaleMatrix;// *S;

		m_aabbTransformed = m_aabb;
		m_aabbTransformed.transform(&m_aabb, &m, &m_globalPosition);
	}


	// select object
	//  or select one vertex/edge/polygon using miSelectionFrust
	virtual void SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf){
		switch (em)
		{
		default:
		case miEditMode::Object: {
			if (km == miKeyboardModifier::Alt)
			{
				m_isSelected = false;
			}
			else
			{
				m_isSelected = true;
			}
		}break;
		case miEditMode::Vertex:
		case miEditMode::Edge:
		case miEditMode::Polygon:
			break;
		}
	}
	// object/vertex/edge/polygon using miSelectionFrust
	//  select by rectangle
	virtual void Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
		switch (em)
		{
		default:
		case miEditMode::Object: {
			for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
			{
				if (GetVisualObject(i)->IsInSelectionFrust(sf))
				{
					if (km == miKeyboardModifier::Alt)
					{
						m_isSelected = false;
					}
					else
					{
						m_isSelected = true;
					}
					break;
				}
			}
		}break;
		case miEditMode::Vertex:
		case miEditMode::Edge:
		case miEditMode::Polygon:
			break;
		}
	}
	virtual void SelectAll(miEditMode em) {
		switch (em)
		{
		default:
		case miEditMode::Object:
			m_isSelected = true;
			break;
		case miEditMode::Vertex:
		case miEditMode::Edge:
		case miEditMode::Polygon:
			break;
		}
	}
	virtual void DeselectAll(miEditMode em) {
		switch (em)
		{
		default:
		case miEditMode::Object:
			m_isSelected = false;
			break;
		case miEditMode::Vertex:
		case miEditMode::Edge:
		case miEditMode::Polygon:
			break;
		}
	}
	virtual void InvertSelection(miEditMode em) {
		switch (em)
		{
		default:
		case miEditMode::Object:
			m_isSelected = m_isSelected ? false : true;
			break;
		case miEditMode::Vertex:
		case miEditMode::Edge:
		case miEditMode::Polygon:
			break;
		}
	}

	virtual bool IsRayIntersect(yyRay* r, v4f* ip, float* d) {
		for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
			if (GetVisualObject(i)->IsRayIntersect(r, ip, d))
				return true;
		}
		return false;
	}
};

#endif