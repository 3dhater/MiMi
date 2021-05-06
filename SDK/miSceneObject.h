#ifndef _MI_SCOBJ_H_
#define _MI_SCOBJ_H_

//inline
//miListNode<miSceneObject*>* miSceneObject_children_onAllocate() {
//	//return new miListNode<miSceneObject*>;
//	miListNode<miSceneObject*>* node = (miListNode<miSceneObject*>*)miSingleton<miSDK>::s_instance->AllocateMemory(sizeof(miListNode<miSceneObject*>));
//	//new(ptr) _type(a1, a2);
//	new(node)miListNode<miSceneObject*>();
//	return node;
//}
//
//inline
//void miSceneObject_children_onDeallocate(miListNode<miSceneObject*>* o) {
//	assert(o);
//	delete o;
//}

// base class for all scene objects
// children classed must be implemented in plugins
class miSceneObject
{
protected:
	miAabb m_aabb;
	miString m_name;

	miSceneObject* m_parent;
	miList<miSceneObject*> m_children;
public:
	miSceneObject() {
	//	m_children.m_onAllocate = miSceneObject_children_onAllocate;
	//	m_children.m_onDeallocate = miSceneObject_children_onDeallocate;
	}
	virtual ~miSceneObject() {}

	virtual miAabb* GetAABB() { return &m_aabb; }

	// must be called by application
	virtual void SetName(const char* newName) {
		assert(newName);
		m_name = newName;
	}
	// must be called by application
	virtual void SetParent(miSceneObject* o) {
		if (m_parent)
			m_parent->m_children.erase_first(this);

		m_parent = o;

		if (o)
			m_parent->m_children.push_back(o);
	}

	// when app need to draw this object
	virtual void OnDraw() = 0;
	// when plugin create object
	virtual void OnCreationLMBDown() = 0;
	virtual void OnCreationLMBUp() = 0;
	virtual void OnCreationMouseMove() = 0;
	

	virtual miPlugin* GetPlugin() = 0;
};

#endif