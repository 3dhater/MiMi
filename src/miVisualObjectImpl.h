#ifndef _MI_VISOBJIMPL_H_
#define _MI_VISOBJIMPL_H_

class miVisualObjectImpl : public miVisualObject
{
	struct node {
		node() {
			m_modelGPU_p = 0;
			m_modelCPU_p = 0;
			m_texture = 0;
			m_remap = false;
		}
		~node() {
			if (m_modelGPU_p) yyMegaAllocator::Destroy(m_modelGPU_p);
			if (m_modelCPU_p) yyMegaAllocator::Destroy(m_modelCPU_p);
		}
		yyResource* m_modelGPU_p;
		yyResource* m_texture;
		yyModel* m_modelCPU_p;
		bool m_remap;
	};

	yyArray<node*> m_nodes;
	void _destroy();
	void _createSoftwareModel_polys();

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