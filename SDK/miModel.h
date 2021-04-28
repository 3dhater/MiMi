#ifndef _MI_MODEL_H_
#define _MI_MODEL_H_

struct miModelSourceVertex
{
	float m_position[3];
	float m_normal[3];
	float m_tcoords[2];
};

struct miModelSourceVertexColor
{
	float m_position[3];
	float m_color[3];
};

enum class miModelSourceType
{
	Triangles,  // use miModelSourceVertex
	Lines       // use miModelSourceVertexColor
};

struct miModelSource
{
	miModelSource() {
		m_type = miModelSourceType::Lines;
		m_inds = 0;
		m_verts = 0;
		m_vCount = 0;
		m_iCount = 0;
	}

	~miModelSource() {
	}

	miModelSourceType m_type;

	unsigned short * m_inds;
	unsigned char * m_verts;

	int m_vCount; // vertex count
	int m_iCount; // index count
};

#endif