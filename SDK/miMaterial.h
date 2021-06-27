#ifndef __MI_MATERIAL_H__
#define __MI_MATERIAL_H__

#define miMaterialMaxMaps 10

class miSceneObject;
struct miMaterial
{
	miMaterial(){
		m_baseColor.set(1.f);
	}
	~miMaterial(){}

	miString m_name;

	struct map
	{
		map():m_GPUTexture(0){}
		~map() {}
		miString m_texturePath;
		void* m_GPUTexture;
	}
	m_maps[miMaterialMaxMaps];

	v4f m_baseColor;
};

#endif