#include "miSDK.h"

//miListNode<miSceneObject*>* miSceneObject_children_onAllocate() {
//	return new miListNode<miSceneObject*>;
//}
//
//void miSceneObject_children_onDeallocate(miListNode<miSceneObject*>* o) {
//	assert(o);
//	delete o;
//}

//miSceneObject::miSceneObject() {
//	m_children.m_onAllocate = miSceneObject_children_onAllocate;
//	m_children.m_onDeallocate = miSceneObject_children_onDeallocate;
//}

//miSceneObject::~miSceneObject(){}

//miAabb* miSceneObject::GetAABB(){ 
//	return &m_aabb; 
//}

//void miSceneObject::SetName(const char* newName) {
//	assert(newName);
//	m_name = newName;
//}

//void miSceneObject::SetParent(miSceneObject* o){
//
//
//	if (m_parent)
//		m_parent->m_children.erase_first(this);
//
//	m_parent = o;
//
//	if (o)
//		m_parent->m_children.push_back(o);
//}