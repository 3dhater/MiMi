#ifndef _MIPL_PO_PLANE_H_
#define _MIPL_PO_PLANE_H_

class miplPolygonObjectPlane : public miSceneObject
{
	miSDK* m_sdk;
	miPlugin* m_plugin;
	miplStd* m_pluginImpl;
	miVec2 m_size;

	miVec3 m_firstPoint;
	miAabb m_creationAabb;
	bool m_isLMBDown;

	miPolygonCreator* m_polygonCreator;
	miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>> * m_meshBuilder;

	miVisualObject* m_visualObject;
public:
	miplPolygonObjectPlane(miSDK*, miPlugin*);
	virtual ~miplPolygonObjectPlane();

	virtual void OnDraw();
	virtual void OnUpdate(float dt);
	virtual void OnCreation();
	virtual void OnCreationLMBDown();
	virtual void OnCreationLMBUp();
	virtual void OnCreationMouseMove();

	virtual miPlugin* GetPlugin();
};

#endif