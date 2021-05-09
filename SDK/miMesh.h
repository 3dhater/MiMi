#ifndef _MI_MESH_H_
#define _MI_MESH_H_

#include <unordered_map>
#include <string>

struct miEdge;
struct miPolygon;

#include "miPackOn.h"
struct miVertex 
{
	miVertex() {
		m_left = 0;
		m_right = 0;
		m_flags = 0;
	}
	~miVertex() {}
	unsigned char m_flags;
	enum { flag_isSelected = 1, };

	miVertex* m_left;
	miVertex* m_right;

	miVec3 m_position;
	half m_normal[3];


	miList<miPolygon*> m_polygons;
	miList<miEdge*> m_edges;
};
#include "miPackOff.h"

#include "miPackOn.h"
struct miEdge
{
	miEdge() {
		m_left = 0;
		m_right = 0;

		m_vertex1 = 0;
		m_vertex2 = 0;

		m_polygon1 = 0;
		m_polygon2 = 0;
		m_flags = 0;
	}
	~miEdge() {}
	unsigned char m_flags;
	enum { flag_isSelected = 1, };

	miEdge* m_left;
	miEdge* m_right;

	miVertex * m_vertex1;
	miVertex * m_vertex2;

	miPolygon * m_polygon1;
	miPolygon * m_polygon2;
};
#include "miPackOff.h"

#include "miPackOn.h"
struct miPolygon
{
	miPolygon() {
		m_left = 0;
		m_right = 0;
		m_flags = 0;
	}
	~miPolygon() {}
	unsigned char m_flags;
	enum { flag_isSelected = 1, };

	miPolygon* m_left;
	miPolygon* m_right;
	
	miList<miVertex*> m_verts;
	miList<miEdge*> m_edges;
};
#include "miPackOff.h"

// create miPolygonCreator by yourself
// use it when you create new model
// don't forget to call Clear() before\after every polygon
class miPolygonCreator
{
	int m_size;
	int m_allocated;
	miVec3* m_positions;
	miVec3* m_normals;

	void _reallocate(int size) {
		miVec3* newPositions = new miVec3[size];
		miVec3* newNormals = new miVec3[size];

		for (int i = 0; i < m_size; ++i)
		{
			newPositions[i] = m_positions[i];
			newNormals[i] = m_normals[i];
		}

		delete[] m_positions;
		delete[] m_normals;

		m_positions = newPositions;
		m_normals = newNormals;

		m_allocated = size;
	}

public:
	miPolygonCreator():m_size(0), m_allocated(0), m_positions(0),m_normals(0){}
	~miPolygonCreator() {
		if (m_positions) delete[] m_positions;
		if (m_normals) delete[] m_normals;
	}

	void Clear() { m_size = 0; }
	int Size() { return m_size; }

	void Add(const miVec3& position, const miVec3& normal) {
		if (m_size == m_allocated)
		{
			_reallocate(m_allocated + (int)std::ceil(((2.f + (float)m_allocated) * 0.5f)));
		}
		m_positions[m_size] = position;
		m_normals[m_size] = normal;
		++m_size;
	}

	miVec3* GetPositions() { return m_positions; }
	miVec3* GetNormals() { return m_normals; }
};

struct miSkeleton {

};

struct miMesh
{
	miMesh() {
		m_first_polygon = 0;
		m_first_edge = 0;
		m_first_vertex = 0;
		m_skeleton = 0;
	}
	miPolygon* m_first_polygon;
	miEdge* m_first_edge;
	miVertex* m_first_vertex;
	miSkeleton* m_skeleton;
};

template<typename _polygon_allocator_type, typename _edge_allocator_type, typename _vertex_allocator_type>
struct miMeshBuilder
{
	// polyCount/edgeCount/vertexCount for miPoolAllocator
	miMeshBuilder(int polyCount, int edgeCount, int vertexCount) {
		m_allocatorPolygon = (_polygon_allocator_type*)miMalloc(sizeof(_polygon_allocator_type));
		m_allocatorEdge = (_edge_allocator_type*)miMalloc(sizeof(_edge_allocator_type));
		m_allocatorVertex = (_vertex_allocator_type*)miMalloc(sizeof(_vertex_allocator_type));
		new(m_allocatorPolygon)_polygon_allocator_type(polyCount);
		new(m_allocatorEdge)_edge_allocator_type(edgeCount);
		new(m_allocatorVertex)_vertex_allocator_type(vertexCount);
	}

	~miMeshBuilder() {
		DeleteMesh();

		m_allocatorPolygon->~_polygon_allocator_type();
		m_allocatorEdge->~_edge_allocator_type();
		m_allocatorVertex->~_vertex_allocator_type();
		miFree(m_allocatorPolygon);
		miFree(m_allocatorEdge);
		miFree(m_allocatorVertex);
	}

	void DeleteMesh() {
		_delete_edges();
		if (m_mesh.m_first_polygon)
		{
			auto p = m_mesh.m_first_polygon;
			auto last = p->m_left;
			while (true) {
				auto next = p->m_right;
				p->~miPolygon();
				m_allocatorPolygon->Deallocate(p);
				if (p == last)
					break;
				p = next;
			}
		}


		if (m_mesh.m_first_vertex)
		{
			auto v = m_mesh.m_first_vertex;
			auto last = v->m_left;
			while (true) {
				auto next = v->m_right;
				v->~miVertex();
				m_allocatorVertex->Deallocate(v);
				if (v == last)
					break;
				v = next;
			}
		}
	}

	miMesh m_mesh;

	_polygon_allocator_type* m_allocatorPolygon;
	_edge_allocator_type* m_allocatorEdge;
	_vertex_allocator_type* m_allocatorVertex;

	std::unordered_map<std::string, miVertex*> m_weldMap;
	std::string m_vertsMapHash;

	void AddPolygon(miPolygonCreator* pc, bool weld) {
		auto polygonVertexCount = pc->Size();
		if (polygonVertexCount < 3)
			return;


		miPolygon* newPolygon = m_allocatorPolygon->Allocate();
		new(newPolygon)miPolygon();


		auto positions = pc->GetPositions();
		auto normals = pc->GetNormals();

		for (int i = 0; i < polygonVertexCount; ++i)
		{
			miVertex* newVertex = 0;
			
			if (weld)
			{
				_set_hash(&positions[i]);

				auto find_result = m_weldMap.find(m_vertsMapHash);
				if (find_result == m_weldMap.end())
				{
					newVertex = m_allocatorVertex->Allocate();
					new(newVertex)miVertex();
					newVertex->m_position = positions[i];
					
					newVertex->m_normal[0] = normals[i].x;
					newVertex->m_normal[1] = normals[i].y;
					newVertex->m_normal[2] = normals[i].z;

					m_weldMap[m_vertsMapHash] = newVertex;
					_add_vertex_to_list(newVertex);
				}
				else
				{
					newVertex = find_result->second;
				}
			}
			else
			{
				newVertex = m_allocatorVertex->Allocate();
				new(newVertex)miVertex();
				newVertex->m_position = positions[i];
				
				newVertex->m_normal[0] = normals[i].x;
				newVertex->m_normal[1] = normals[i].y;
				newVertex->m_normal[2] = normals[i].z;

				_add_vertex_to_list(newVertex);
			}

			newVertex->m_polygons.push_back(newPolygon);

			newPolygon->m_verts.push_back(newVertex);
		}

		if (!m_mesh.m_first_polygon)
		{
			m_mesh.m_first_polygon = newPolygon;
			m_mesh.m_first_polygon->m_right = m_mesh.m_first_polygon;
			m_mesh.m_first_polygon->m_left = m_mesh.m_first_polygon;
		}
		else
		{
			auto last = m_mesh.m_first_polygon->m_left;
			last->m_right = newPolygon;
			newPolygon->m_left = last;
			newPolygon->m_right = m_mesh.m_first_polygon;
			m_mesh.m_first_polygon->m_left = newPolygon;
		}
	}
	void CreateEdges() {
		if (m_mesh.m_first_edge)
			_delete_edges();
		if (!m_mesh.m_first_polygon)
			return;
		auto current_polygon = m_mesh.m_first_polygon;
		auto last_polygon = current_polygon->m_left;
		while (true) {
			auto next_polygon = current_polygon->m_right;
			
			auto current_vertex = next_polygon->m_verts.m_head;
			auto next_vertex = current_vertex->m_right;
			auto last_vertex = current_vertex->m_left;
			while (true) {
				miVertex* v1 = current_vertex->m_data;
				miVertex* v2 = next_vertex->m_data;

				// пусть вершина с адресом значение которого меньше
				//  будет на первом месте.
				if (v2 < v1)
				{
					v1 = next_vertex->m_data;
					v2 = current_vertex->m_data;
				}

				miEdge* newEdge = nullptr;
				if (v1->m_edges.m_head) // find exist edge
				{
					// я ищу рёбра в списке у вершины, так как v1 и v2
					//  отсортированы по значению адреса, и v1 всегда
					//   будет иметь возможность хранить уже ранее 
					//    созданное ребро.
					auto current_edge = v1->m_edges.m_head;
					auto last_edge = current_edge->m_left;
					while (true) {

						if (current_edge->m_data->m_vertex1 == v1
							&& current_edge->m_data->m_vertex2 == v2)
						{
							newEdge = current_edge->m_data;
							break;
						}

						if (current_edge == last_edge)
							break;
						current_edge = current_edge->m_right;
					}
				}

				if (!newEdge)
				{
					newEdge = m_allocatorEdge->Allocate();
					new(newEdge)miEdge();
					newEdge->m_vertex1 = v1;
					newEdge->m_vertex2 = v2;
					_add_edge_to_list(newEdge);
				}

				if (!newEdge->m_polygon1)
					newEdge->m_polygon1 = current_polygon;
				else 
					newEdge->m_polygon2 = current_polygon;

				current_polygon->m_edges.push_back(newEdge);
				v1->m_edges.push_back(newEdge);
				v2->m_edges.push_back(newEdge);

				if (current_vertex == last_vertex)
					break;

				current_vertex = next_vertex;
				next_vertex = next_vertex->m_right;
			}
			

			if (current_polygon == last_polygon)
				break;
			current_polygon = next_polygon;
		}
	}
private:
	void _set_hash(miVec3* position) {
		m_vertsMapHash.clear();
		char * ptr = (char*)position;
		if (ptr[0] == 0) m_vertsMapHash += (ptr[0] + 1); else m_vertsMapHash += ptr[0];
		if (ptr[1] == 0) m_vertsMapHash += (ptr[1] + 1); else m_vertsMapHash += ptr[1];
		if (ptr[2] == 0) m_vertsMapHash += (ptr[2] + 1); else m_vertsMapHash += ptr[2];
		if (ptr[3] == 0) m_vertsMapHash += (ptr[3] + 1); else m_vertsMapHash += ptr[3];
		if (ptr[4] == 0) m_vertsMapHash += (ptr[4] + 1); else m_vertsMapHash += ptr[4];
		if (ptr[5] == 0) m_vertsMapHash += (ptr[5] + 1); else m_vertsMapHash += ptr[5];
		if (ptr[6] == 0) m_vertsMapHash += (ptr[6] + 1); else m_vertsMapHash += ptr[6];
		if (ptr[7] == 0) m_vertsMapHash += (ptr[7] + 1); else m_vertsMapHash += ptr[7];
		if (ptr[8] == 0) m_vertsMapHash += (ptr[8] + 1); else m_vertsMapHash += ptr[8];
		if (ptr[9] == 0) m_vertsMapHash += (ptr[9] + 1); else m_vertsMapHash += ptr[9];
		if (ptr[10] == 0) m_vertsMapHash += (ptr[10] + 1); else m_vertsMapHash += ptr[10];
		if (ptr[11] == 0) m_vertsMapHash += (ptr[11] + 1); else m_vertsMapHash += ptr[11];
	}
	void _add_vertex_to_list(miVertex* newVertex) {
		if (!m_mesh.m_first_vertex)
		{
			m_mesh.m_first_vertex = newVertex;
			m_mesh.m_first_vertex->m_right = m_mesh.m_first_vertex;
			m_mesh.m_first_vertex->m_left = m_mesh.m_first_vertex;
		}
		else
		{
			auto last = m_mesh.m_first_vertex->m_left;
			last->m_right = newVertex;
			newVertex->m_left = last;
			newVertex->m_right = m_mesh.m_first_vertex;
			m_mesh.m_first_vertex->m_left = newVertex;
		}
	}
	void _add_edge_to_list(miEdge* newEdge) {
		if (!m_mesh.m_first_edge)
		{
			m_mesh.m_first_edge = newEdge;
			m_mesh.m_first_edge->m_right = m_mesh.m_first_edge;
			m_mesh.m_first_edge->m_left = m_mesh.m_first_edge;
		}
		else
		{
			auto last = m_mesh.m_first_edge->m_left;
			last->m_right = newEdge;
			newEdge->m_left = last;
			newEdge->m_right = m_mesh.m_first_edge;
			m_mesh.m_first_edge->m_left = newEdge;
		}
	}
	void _delete_edges() {
		if (m_mesh.m_first_polygon)
		{
			auto p = m_mesh.m_first_polygon;
			auto last = p->m_left;
			while (true) {
				auto next = p->m_right;
				p->m_edges.clear();
				if (p == last)
					break;
				p = next;
			}
		}
		if (m_mesh.m_first_vertex)
		{
			auto v = m_mesh.m_first_vertex;
			auto last = v->m_left;
			while (true) {
				auto next = v->m_right;
				v->m_edges.clear();
				if (v == last)
					break;
				v = next;
			}
		}
		if (m_mesh.m_first_edge)
		{
			auto e = m_mesh.m_first_edge;
			auto last = e->m_left;
			while (true) {
				auto next = e->m_right;
				e->~miEdge();
				m_allocatorEdge->Deallocate(e);
				if (e == last)
					break;
				e = next;
			}
			m_mesh.m_first_edge = 0;
		}
	}
};

#endif