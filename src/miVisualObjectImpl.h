#ifndef _MI_VISOBJIMPL_H_
#define _MI_VISOBJIMPL_H_

class miVisualObjectImpl : public miVisualObject
{
	struct node {
		node() {
			m_modelGPU = 0;
			m_modelCPU = 0;
			m_remap = false;
		}
		~node() {
			if (m_modelGPU) yyMegaAllocator::Destroy(m_modelGPU);
			if (m_modelCPU) yyMegaAllocator::Destroy(m_modelCPU);
		}
		yyResource* m_modelGPU;
		yyModel* m_modelCPU;
		bool m_remap;
	};
	
	yyResource* m_texture;

	yyArray<node*> m_nodes_polygons;
	yyArray<node*> m_nodes_edges;
	void _destroy();
	void _createSoftwareModel_polys();
	void _createSoftwareModel_edges();
	void _createSoftwareModel_verts();

	miAabb m_aabb;
	miMesh* m_mesh;

public:
	miVisualObjectImpl();
	virtual ~miVisualObjectImpl();

	virtual void CreateNewGPUModels(miMesh*);
	virtual size_t GetBufferCount();
	virtual unsigned char* GetVertexBuffer(size_t index) ;
	virtual void MarkBufferToRemap(size_t index) ;
	virtual void RemapBuffers() ;

	virtual void Draw(miMatrix*);
	virtual miAabb GetAabb();
};

#endif