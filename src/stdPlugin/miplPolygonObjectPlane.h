#ifndef _MIPL_PO_PLANE_H_
#define _MIPL_PO_PLANE_H_

class miplPolygonObjectPlane : public miSceneObject
{
	miSDK* m_sdk;
	miPlugin* m_plugin;
	miplStd* m_pluginImpl;
	v2f m_size;

	v3f m_firstPoint;
	v3f m_secondPoint;
	Aabb m_creationAabb;
	bool m_isLMBDown;

	miPolygonCreator* m_polygonCreator;
	miMeshBuilder<miPoolAllocator<miPolygon>, miPoolAllocator<miEdge>, miPoolAllocator<miVertex>> * m_meshBuilder;

	miViewportCameraType m_viewportCameraType;

	miVisualObject* m_visualObject;

	void _generate();
	bool m_needToCreateNewGPUBuffers;

	friend void miplStd_plane_sliderXSegments_onValueChanged(miSceneObject* object, int x);
	friend void miplStd_plane_sliderYSegments_onValueChanged(miSceneObject* object, int y);

public:
	miplPolygonObjectPlane(miSDK*, miPlugin*);
	virtual ~miplPolygonObjectPlane();

	virtual void OnDraw();
	virtual void OnUpdate(float dt);
	virtual void OnCreation(miPluginGUI*);
	virtual void OnCreationLMBDown();
	virtual void OnCreationLMBUp();
	virtual void OnCreationMouseMove();
	virtual void OnCreationEnd();
	virtual void OnConvertToEditableObject();

	virtual miPlugin* GetPlugin();
	virtual void DeleteSelectedObjects(miEditMode em);
	virtual int GetVisualObjectCount();
	virtual miVisualObject* GetVisualObject(int);

	int m_x_segments;
	int m_y_segments;

	friend class miplStd;
};

#endif