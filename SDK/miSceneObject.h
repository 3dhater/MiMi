#ifndef _MI_SCOBJ_H_
#define _MI_SCOBJ_H_

class miApplication;

// base class for all scene objects
// children classed must be implemented in plugins
class miSceneObject
{
protected:
	miAabb m_aabb;
	miAabb m_aabbTransformed;
	miString m_name;

	miSceneObject* m_parent;
	miList<miSceneObject*> m_children;
	
	miMatrix m_rotationMatrix;
	miMatrix m_worldMatrix;

	miVec4 m_localPosition;
	miVec4 m_globalPosition;

	miVec4 m_edgeColor;

	bool m_isSelected;
	float m_distanceToCamera; // not implemented
	float m_cursorIntersectionPointDistance;
	miVec4 m_cursorIntersectionPoint;
	friend class miApplication;
public:
	miSceneObject(){
		m_parent = 0;
		m_isSelected = false;
		m_distanceToCamera = 0.f;
		m_cursorIntersectionPointDistance = 0.f;
	}
	virtual ~miSceneObject() {}

	virtual float GetDistanceToCamera() { return m_distanceToCamera; }
	virtual float GetDistanceToCursorIP() { return m_cursorIntersectionPointDistance; }
	virtual miVec4* GetCursorIntersectionPoint() { return &m_cursorIntersectionPoint; }
	virtual bool IsSelected() { return m_isSelected; }
	
	virtual miMatrix* GetRotationMatrix() { return &m_rotationMatrix; }
	virtual miMatrix* GetWorldMatrix() { return &m_worldMatrix; }

	virtual void UpdateTransform() {
		miMatrix T;
		T.setTranslation(m_localPosition);
		
		m_worldMatrix = T * m_rotationMatrix;

		if (m_parent)
		{
			m_worldMatrix = m_parent->m_worldMatrix * m_worldMatrix;
		}

		m_globalPosition = m_worldMatrix.m_data[3];
	}

	virtual miVec4* GetLocalPosition() { return &m_localPosition; }
	virtual miVec4* GetGlobalPosition() { return &m_globalPosition; }
	
	virtual miVec4* GetEdgeColor() { return &m_edgeColor; }
	virtual void SetEdgeColor(const miVec4& c) { m_edgeColor = c; }

	virtual miAabb* GetAABB() { return &m_aabb; }
	virtual miAabb* GetAABBTransformed() { return &m_aabbTransformed; }
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
	virtual void OnDraw() = 0;
	virtual void OnUpdate(float dt) = 0;
	// when plugin create object
	virtual void OnCreation() = 0; // when press button\menu command
	virtual void OnCreationLMBDown() = 0;
	virtual void OnCreationLMBUp() = 0;
	virtual void OnCreationMouseMove() = 0;
	virtual void OnCreationEnd() = 0;

	virtual miPlugin* GetPlugin() = 0;


	virtual int GetVisualObjectCount() = 0;
	virtual miVisualObject* GetVisualObject(int) = 0;
	virtual void UpdateAabb() {
		m_aabb.reset();
		for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
			m_aabb.add(GetVisualObject(i)->GetAabb());
		}
		//m_aabb.m_min += m_globalPosition;//no
		//m_aabb.m_max += m_globalPosition;//no
		// m_aabb must be in space center
	}


	// select object
	//  or select one vertex/edge/polygon using miSelectionFrust
	virtual void SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf) {
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
			for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
			{
				GetVisualObject(i)->SelectSingle(em, km, sf);
			}
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
			for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
			{
				GetVisualObject(i)->Select(em, km, sf);
			}
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
			for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
			{
				GetVisualObject(i)->SelectAll();
			}
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
			for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
			{
				GetVisualObject(i)->DeselectAll();
			}
			break;
		}
	}

	virtual bool IsRayIntersect(miRay* r, miVec4* ip, float* d) {
		for (int i = 0, sz = GetVisualObjectCount(); i < sz; ++i)
		{
			if (GetVisualObject(i)->IsRayIntersect(r, ip, d))
				return true;
		}
		return false;
	}
};

#endif