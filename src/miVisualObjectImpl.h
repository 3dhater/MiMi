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

		// miVertex, yyVertex___
		yyArraySimple<miPair<void*,void*>> m_ptrs;
	};
	
	yyResource* m_texture;

	yyArray<model_node_GPU*> m_nodes_GPU;
	yyArray<model_node_CPU*> m_nodes_CPU;
	yyArraySimple<u32> m_nodesForUpdate; // index array

	void _destroy();
	void _createSoftwareModel_polys();
	void _createSoftwareModel_edges();
	void _createSoftwareModel_verts();

	Aabb m_aabb;
	miMesh* m_mesh;
	
	miSceneObject* m_parentSceneObject;

	miVisualObjectType m_type;

	friend class miSDKImpl;

public:
	miVisualObjectImpl();
	virtual ~miVisualObjectImpl();

	virtual void CreateNewGPUModels(miMesh*);

	virtual size_t GetBufferCount();
	virtual unsigned char* GetVertexBuffer(size_t index) ;
	//virtual void MarkBufferToRemap(size_t index) ;
	//virtual void RemapBuffers() ;
	//virtual void UpdateCPUModelsOnly(miMesh*);
	//virtual void RemapAllBuffers();

	virtual void Draw();
	virtual Aabb GetAabb();
	virtual void UpdateAabb();

	virtual bool IsInSelectionFrust(miSelectionFrust* sf);

	virtual bool IsRayIntersect(yyRay* r, v4f* ip, float* d);
	virtual miVisualObjectType GetType();

	virtual void OnTransform();
	virtual void OnSelect(miEditMode);
};

#endif