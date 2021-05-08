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

	yyArray<node*> m_nodes;
	void _destroy();

	miAabb m_aabb;

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