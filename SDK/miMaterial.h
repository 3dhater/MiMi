#ifndef __MI_MATERIAL_H__
#define __MI_MATERIAL_H__

#define miMaterialMaxMaps 10

class miSceneObject;
struct miMaterial
{
	miMaterial(){
		m_colorDiffuse.set(1.f);
		m_colorAmbient.set(0.f);
		m_colorSpecular.set(1.f);
		m_specularExponent = 10.f;
		m_refraction = 1.0f;
		m_opacity = 1.f;
	}
	~miMaterial(){}

	miString m_name;

	enum mapSlot {
		mapSlot_Diffuse = 0,
		mapSlot_Normal,
		mapSlot_SpecularHighlight,
		mapSlot_SpecularColor,
		mapSlot_AmbientColor,
		mapSlot_Alpha,
		mapSlot_Displacement,
		mapSlot_Reflection,
		mapSlot_Reserved1,
		mapSlot_Reserved2,
		mapSlot_Reserved3,
		mapSlot_Reserved4,
		mapSlot_Reserved5
	};

	struct map
	{
		map():m_GPUTexture(0){}
		~map() {}
		miString m_texturePath;
		void* m_GPUTexture;
	}
	m_maps[miMaterialMaxMaps];

	v4f m_colorDiffuse;
	v4f m_colorAmbient;
	v4f m_colorSpecular;

	f32 m_specularExponent;
	f32 m_refraction;
	f32 m_opacity;
};

#endif