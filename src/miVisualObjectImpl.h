#ifndef _MI_VISOBJIMPL_H_
#define _MI_VISOBJIMPL_H_

class miVisualObjectImpl : public miVisualObject
{
	struct mesh_node_GPU {
		mesh_node_GPU() {
			m_meshGPU = 0;
		}
		~mesh_node_GPU() {
			if (m_meshGPU)
				yyDestroy(m_meshGPU);
		}
		yyGPUMesh* m_meshGPU;
	};
	struct mesh_node_CPU {
		mesh_node_CPU() {
			m_meshCPU = 0;
		}
		~mesh_node_CPU() {
			if (m_meshCPU)
				yyDestroy(m_meshCPU);
		}
		yyMesh* m_meshCPU;

		// miVertex, yyVertex___
		yyArraySimple<miPair<void*,void*>> m_ptrs;
	};

	struct mesh_node_UV_CPU {
		mesh_node_UV_CPU() {
			m_meshCPU = 0;
		}
		~mesh_node_UV_CPU() {
			if (m_meshCPU)
				yyDestroy(m_meshCPU);
		}
		yyMesh* m_meshCPU;

		yyArraySimple<void*> m_ptrs;
	};
	
	//yyResource* m_texture;

	yyArray<mesh_node_GPU*> m_nodes_GPU;
	yyArray<mesh_node_CPU*> m_nodes_CPU;
	yyArraySimple<u32> m_nodesForUpdate; // index array

	void _destroy();
	void _destroy_UV();
	void _createSoftwareModel_polys();
	void _createSoftwareModel_edges();
	void _createSoftwareModel_verts();

	Aabb m_aabb;
	miMesh* m_mesh;
	
	miSceneObject* m_parentSceneObject;

	miVisualObjectType m_type;

	// only points and lines
	mesh_node_GPU* m_node_UV_GPU;
	mesh_node_UV_CPU* m_node_UV_CPU;
	void _createSoftwareModel_polys_UV();
	void _createSoftwareModel_edges_UV();
	void _createSoftwareModel_verts_UV();

	friend class miSDKImpl;

public:
	miVisualObjectImpl();
	virtual ~miVisualObjectImpl();

	virtual void CreateNewGPUModels(miMesh*);
	virtual void CreateNewGPUModelsUV(miMesh*);

	virtual size_t GetBufferCount();
	virtual unsigned char* GetVertexBuffer(size_t index) ;
	//virtual void MarkBufferToRemap(size_t index) ;
	//virtual void RemapBuffers() ;
	//virtual void UpdateCPUModelsOnly(miMesh*);
	//virtual void RemapAllBuffers();

	virtual void Draw(bool uv);
	virtual Aabb GetAabb();
	virtual void UpdateAabb();

	virtual bool IsInSelectionFrust(miSelectionFrust* sf);

	virtual bool IsRayIntersect(yyRay* r, v4f* ip, float* d);
	virtual miVisualObjectType GetType();

	virtual void OnTransform();
	virtual void OnSelect(miEditMode);

};

#endif