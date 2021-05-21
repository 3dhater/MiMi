#ifndef _MI_SCOBJ_H_
#define _MI_SCOBJ_H_

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
public:
	miSceneObject():m_parent(0) {}
	virtual ~miSceneObject() {}
	
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
};

#endif