#ifndef _MI_VISOBJIMPL_H_
#define _MI_VISOBJIMPL_H_

class miVisualObjectImpl : public miVisualObject
{
	struct model_node_GPU {
		model_node_GPU() {
			m_modelGPU = 0;
		}
		~model_node_GPU() {
			if (m_modelGPU) yyMegaAllocator::Destroy(m_modelGPU);
		}
		yyResource* m_modelGPU;
	};
	struct model_node_CPU {
		model_node_CPU() {
			m_modelCPU = 0;
		}
		~model_node_CPU() {
			if (m_modelCPU) yyMegaAllocator::Destroy(m_modelCPU);
		}
		yyModel* m_modelCPU;
	};
	
	yyResource* m_texture;

	yyArray<model_node_GPU*> m_nodes_polygons_GPU;
	yyArray<model_node_CPU*> m_nodes_polygons_CPU;
	
	yyArray<model_node_GPU*> m_nodes_edges_GPU;
	yyArray<model_node_CPU*> m_nodes_edges_CPU;

	yyArray<model_node_GPU*> m_nodes_verts_GPU;
	yyArray<model_node_CPU*> m_nodes_verts_CPU;

	void _destroy();
	void _createSoftwareModel_polys();
	void _createSoftwareModel_edges();
	void _createSoftwareModel_verts();

	miAabb m_aabb;
	miMesh* m_mesh;
	
	miSceneObject* m_parentSceneObject;

	friend class miSDKImpl;

public:
	miVisualObjectImpl();
	virtual ~miVisualObjectImpl();

	virtual void CreateNewGPUModels(miMesh*);
	virtual size_t GetBufferCount();
	virtual unsigned char* GetVertexBuffer(size_t index) ;
	//virtual void MarkBufferToRemap(size_t index) ;
	virtual void RemapBuffers() ;
	//virtual void UpdateCPUModelsOnly(miMesh*);
	//virtual void RemapAllBuffers();

	virtual void Draw(miMatrix*);
	virtual miAabb GetAabb();
	
	virtual bool IsInSelectionFrust(miSelectionFrust* sf);
	virtual void SelectSingle(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	virtual void Select(miEditMode em, miKeyboardModifier km, miSelectionFrust* sf);
	virtual void SelectAll();
	virtual void DeselectAll();
	virtual bool IsRayIntersect(miRay* r, miVec4* ip, float* d);
};

#endif