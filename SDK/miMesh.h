#ifndef _MI_MESH_H_
#define _MI_MESH_H_

struct miEdge;
struct miPolygon;

struct miVertex 
{
	miVertex() {
		m_left = 0;
		m_right = 0;
	//	m_flags = 0;
	}

	miVertex* m_left;
	miVertex* m_right;

	miVec3 m_position;
	miVec3 m_normal;

	miList<miPolygon*> m_polygons;
	miList<miEdge*> m_edges;
	/*enum
	{
		flag_isSelected = 1
	};
	int m_flags;*/
};

struct miEdge
{
	miEdge() {
		m_left = 0;
		m_right = 0;

		m_vertex1 = 0;
		m_vertex2 = 0;

		m_polygon1 = 0;
		m_polygon2 = 0;
	//	m_flags = 0;
	}

	miEdge* m_left;
	miEdge* m_right;

	miVertex * m_vertex1;
	miVertex * m_vertex2;

	miPolygon * m_polygon1;
	miPolygon * m_polygon2;
	
	/*enum
	{
		flag_isSelected = 1
	};
	int m_flags;*/
};

struct miPolygon{
	miPolygon() {
		m_left = 0;
		m_right = 0;
	//	m_flags = 0;
	}
	miPolygon* m_left;
	miPolygon* m_right;
	
	miList<miVertex*> m_verts;
	miList<miEdge*> m_edges;
	/*enum
	{
		flag_isSelected = 1
	};
	int m_flags;*/
};


struct miMesh
{
	miPolygon* m_first_polygon;
	miEdge* m_first_edge;
	miVertex* m_first_vertex;
};

#endif