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

	miVertex* m_left;
	miVertex* m_right;

	v3f m_position;

	unsigned char m_flags;
	enum { 
		flag_isSelected = BIT(0),
		flag_User1 = BIT(1),
		flag_User2 = BIT(2),
	};

	bool IsSelected() { return (m_flags & flag_isSelected) == flag_isSelected; }
	void Select() { m_flags |= flag_isSelected; }
	void Deselect() { if (m_flags & flag_isSelected) m_flags ^= flag_isSelected; }

	void CopyData(miVertex* other)
	{
		m_position = other->m_position;
		m_flags = other->m_flags;
	}


	miList<miPolygon*> m_polygons;
	miList<miEdge*> m_edges;

	bool IsOnEdge();
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
	enum 
	{ 
		flag_isSelected = BIT(0),
		flag_User1 = BIT(1),
	};
	
	bool IsSelected(){return (m_flags & flag_isSelected) == flag_isSelected;}
	void Select() { m_flags |= flag_isSelected; }
	void Deselect() { if(m_flags & flag_isSelected) m_flags ^= flag_isSelected;}
	
	void CopyData(miEdge* other) {
		m_flags = other->m_flags;
	}

	miEdge* m_left;
	miEdge* m_right;

	miVertex * m_vertex1;
	miVertex * m_vertex2;

	miPolygon * m_polygon1;
	miPolygon * m_polygon2;
};
#include "miPackOff.h"

inline
bool miVertex::IsOnEdge()
{
	auto c = m_edges.m_head;
	auto l = c->m_left;
	while (true)
	{
		if (!c->m_data->m_polygon1)
			return true;

		if (!c->m_data->m_polygon2)
			return true;

		if (c == l)
			break;
		c = c->m_right;
	}
	return false;
}

#include "miPackOn.h"
struct miPolygon
{
	miPolygon() {
		m_left = 0;
		m_right = 0;
		m_flags = 0;
	}
	~miPolygon() {
	}

	unsigned char m_flags;
	enum { flag_isSelected = 1, };

	void CopyData(miPolygon* other) {
		m_flags = other->m_flags;
	}

	miPolygon* m_left;
	miPolygon* m_right;
	
	//miList3<miVertex*, v2f, v3f> m_verts;
	struct _vertex_data {
		_vertex_data() :m_vertex(0), m_flags(0) {}
		_vertex_data(miVertex* v):m_vertex(v),m_flags(0){}
		_vertex_data(miVertex* v, const v2f& uv, const v3f& normal, u32 flags)
			:
			m_vertex(v), 
			m_uv(uv),
			m_normal(normal),
			m_flags(flags)
		{}

		bool operator==(const _vertex_data& other) {
			return m_vertex == other.m_vertex;
		}
		
		enum { flag_isSelected = 1, };

		miVertex* m_vertex;
		v2f m_uv;
		v3f m_normal;
		u32 m_flags;
	};
	miList<_vertex_data> m_verts;

	miList<miEdge*> m_edges;

	bool IsSelected() { return (m_flags & flag_isSelected) == flag_isSelected; }
	void Select() { m_flags |= flag_isSelected; }
	void Deselect() { if (m_flags & flag_isSelected) m_flags ^= flag_isSelected; }

	void SelectUVs() {
		auto curV = m_verts.m_head;
		auto lastV = curV->m_left;
		while (true)
		{
			curV->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
			if (curV == lastV)
				break;
			curV = curV->m_right;
		}
	}

	miListNode<_vertex_data>* FindVertex(miVertex* v) {
		auto curV = m_verts.m_head;
		auto lastV = curV->m_left;
		while (true)
		{
			if (curV->m_data.m_vertex == v)
				break;
			if (curV == lastV)
				break;
			curV = curV->m_right;
		}
		return curV;
	}

	bool IsVisible() {
		auto vertex_1 = m_verts.m_head;
		auto vertex_3 = vertex_1->m_right;
		auto vertex_2 = vertex_3->m_right;
		while (true)
		{
			auto a = vertex_2->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
			auto b = vertex_3->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;

			v3f n;
			a.cross2(b, n);
			
			f32 area = 0.5f * sqrt(n.dot());
			
			if (area > 0.001)
				return true;

			
			vertex_2 = vertex_2->m_right;
			vertex_3 = vertex_3->m_right;
			if (vertex_3 == vertex_1)
				break;
		}
		return false;
	}

	void CalculateNormal() {
		{
			auto vertex_1 = m_verts.m_head;
			//vertex_1->m_data3.set(0.f);
			vertex_1->m_data.m_normal.set(0.f);

			auto vertex_3 = vertex_1->m_right;
			auto vertex_2 = vertex_3->m_right;
			while (true) {
				//vertex_2->m_data3.set(0.f);
				vertex_2->m_data.m_normal.set(0.f);
				//vertex_3->m_data3.set(0.f);
				vertex_3->m_data.m_normal.set(0.f);
				// ===============================
				vertex_2 = vertex_2->m_right;
				vertex_3 = vertex_3->m_right;

				if (vertex_2 == vertex_1)
					break;
			}
		}
		{
			auto vertex_1 = m_verts.m_head;
			auto vertex_3 = vertex_1->m_right;
			auto vertex_2 = vertex_3->m_right;
			while (true) {
				//auto e1 = vertex_2->m_data1->m_position - vertex_1->m_data1->m_position;
				auto e1 = vertex_2->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
				//auto e2 = vertex_3->m_data1->m_position - vertex_1->m_data1->m_position;
				auto e2 = vertex_3->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
				auto n = e1.cross(e2);
				//vertex_1->m_data3 -= n;
				vertex_1->m_data.m_normal -= n;
				//vertex_2->m_data3 -= n;
				vertex_2->m_data.m_normal -= n;
				//vertex_3->m_data3 -= n;
				vertex_3->m_data.m_normal -= n;

				// ===============================
				vertex_2 = vertex_2->m_right;
				vertex_3 = vertex_3->m_right;

				if (vertex_2 == vertex_1)
					break;
			}
		}
		{
			auto vertex_1 = m_verts.m_head;
			//vertex_1->m_data3.normalize2();
			vertex_1->m_data.m_normal.normalize2();

			auto vertex_3 = vertex_1->m_right;
			auto vertex_2 = vertex_3->m_right;
			while (true) {
				//vertex_2->m_data3.normalize2();
				vertex_2->m_data.m_normal.normalize2();
				//vertex_3->m_data3.normalize2();
				vertex_3->m_data.m_normal.normalize2();
				// ===============================
				vertex_2 = vertex_2->m_right;
				vertex_3 = vertex_3->m_right;

				if (vertex_2 == vertex_1)
					break;
			}
		}
	}

	v3f GetFaceNormal()
	{
		v3f n;
		auto curV = m_verts.m_head;
		auto lastV = curV->m_left;
		while (true)
		{
			//n += curV->m_data3;
			n += curV->m_data.m_normal;
			if (curV == lastV)
				break;
			curV = curV->m_right;
		}
		n.normalize2();
		return n;
	}

	void Flip() {
		m_verts.reverse();
	}

	void FixOrder(f32 lineLineCollisionLen) {
		// only if 4 or more vertices
		u32 num = 0;
		auto curV = m_verts.m_head;
		auto lastV = curV->m_left;
		while (true)
		{
			++num;
			if (curV == lastV)
				break;
			curV = curV->m_right;
		}
		if (num < 4)
			return;

		for (u32 i = 0; i < num; ++i)
		{
			curV = m_verts.m_head;
			lastV = curV->m_left;
			while (true)
			{
				//miPair<miListNode3<miVertex*, v2f, v3f>*, miListNode3<miVertex*, v2f, v3f>*> current;
				miPair<miListNode<_vertex_data>*, miListNode<_vertex_data>*> current;
				//miPair<miListNode3<miVertex*, v2f, v3f>*, miListNode3<miVertex*, v2f, v3f>*> prev;
				miPair<miListNode<_vertex_data>*, miListNode<_vertex_data>*> prev;

				current.m_first = curV;
				current.m_second = current.m_first->m_right;

				prev.m_second = current.m_first;
				prev.m_first = prev.m_second->m_left;

				auto tgCurr = current.m_second->m_right;
				auto tgLast = tgCurr->m_left;
				while (true)
				{
					//miPair<miListNode3<miVertex*, v2f, v3f>*, miListNode3<miVertex*, v2f, v3f>*> target;
					miPair<miListNode<_vertex_data>*, miListNode<_vertex_data>*> target;
					target.m_first = tgCurr;
					target.m_second = tgCurr->m_right;

					if (target.m_first == prev.m_first)
						break;

					yyRay r;
					//r.m_origin = current.m_first->m_data1->m_position;
					r.m_origin = current.m_first->m_data.m_vertex->m_position;
					//r.m_end = current.m_second->m_data1->m_position;
					r.m_end = current.m_second->m_data.m_vertex->m_position;

					//f32 d = r.distanceToLine(target.m_first->m_data1->m_position, target.m_second->m_data1->m_position);
					f32 d = r.distanceToLine(target.m_first->m_data.m_vertex->m_position, target.m_second->m_data.m_vertex->m_position);
					if (d < lineLineCollisionLen)
					{
						//auto tmp = target.m_first->m_data1;
						auto tmp = target.m_first->m_data.m_vertex;
						//target.m_first->m_data1 = current.m_second->m_data1;
						target.m_first->m_data.m_vertex = current.m_second->m_data.m_vertex;
						//current.m_second->m_data1 = tmp;
						current.m_second->m_data.m_vertex = tmp;
					}

					if (tgCurr == tgLast)
						break;

					tgCurr = tgCurr->m_right;
				}


				if (curV == lastV)
					break;
				curV = curV->m_right;
			}
		}
	}
};
#include "miPackOff.h"

struct miTriangle
{
	miTriangle() {}
	miTriangle(const v4f& _v1, const v4f& _v2, const v4f& _v3)
		:
		v1(_v1),
		v2(_v2),
		v3(_v3)
	{
		update();
	}

	miTriangle(const v3f& _v1, const v3f& _v2, const v3f& _v3)
		:
		v1(_v1),
		v2(_v2),
		v3(_v3)
	{
		update();
	}

	v4f v1;
	v4f v2;
	v4f v3;
	//v4f faceNormal;
	v4f normal1;
	v4f normal2;
	v4f normal3;
	v4f e1;
	v4f e2;
	v4f t1;
	v4f t2;
	v4f t3;

	void update()
	{
		e1 = v4f(v2.x - v1.x,
			v2.y - v1.y,
			v2.z - v1.z,
			0.f);
		e2 = v4f(v3.x - v1.x,
			v3.y - v1.y,
			v3.z - v1.z,
			0.f);
		//	e1.cross(e2, faceNormal);
	}

	void center(v4f& out)
	{
		out = (v1 + v2 + v3) * 0.3333333f;
	}

	bool rayTest_MT(const yyRay& ray, bool withBackFace, float& T, float& U, float& V, float& W)
	{
		v4f  pvec;
		ray.m_direction.cross2(e2, pvec);
		float det = e1.dot(pvec);

		if (withBackFace)
		{
			if (std::fabs(det) < Epsilon)
				return false;
		}
		else
		{
			if (det < Epsilon && det > -Epsilon)
				return false;
		}

		v4f tvec(
			ray.m_origin.x - v1.x,
			ray.m_origin.y - v1.y,
			ray.m_origin.z - v1.z,
			0.f);

		float inv_det = 1.f / det;
		U = tvec.dot(pvec) * inv_det;

		if (U < 0.f || U > 1.f)
			return false;

		v4f  qvec;
		tvec.cross2(e1, qvec);
		V = ray.m_direction.dot(qvec) * inv_det;

		if (V < 0.f || U + V > 1.f)
			return false;

		T = e2.dot(qvec) * inv_det;

		if (T < Epsilon) return false;

		W = 1.f - U - V;
		return true;
	}

	bool rayTest_Watertight(const yyRay& ray, bool withBackFace, float& T, float& U, float& V, float& W)
	{
		v1.w = 1.f;
		v2.w = 1.f;
		v3.w = 1.f;
		const auto A = v2 - ray.m_origin;
		const auto B = v3 - ray.m_origin;
		const auto C = v1 - ray.m_origin;

		const float Ax = A[ray.m_kx] - (ray.m_Sx * A[ray.m_kz]);
		const float Ay = A[ray.m_ky] - (ray.m_Sy * A[ray.m_kz]);
		const float Bx = B[ray.m_kx] - (ray.m_Sx * B[ray.m_kz]);
		const float By = B[ray.m_ky] - (ray.m_Sy * B[ray.m_kz]);
		const float Cx = C[ray.m_kx] - (ray.m_Sx * C[ray.m_kz]);
		const float Cy = C[ray.m_ky] - (ray.m_Sy * C[ray.m_kz]);

		U = (Cx * By) - (Cy * Bx);
		V = (Ax * Cy) - (Ay * Cx);
		W = (Bx * Ay) - (By * Ax);

		if (U == 0.f || V == 0.f || W == 0.f)
		{
			double CxBy = (double)Cx * (double)By;
			double CyBx = (double)Cy * (double)Bx;
			U = (float)(CxBy - CyBx);

			double AxCy = (double)Ax * (double)Cy;
			double AyCx = (double)Ay * (double)Cx;
			V = (float)(AxCy - AyCx);

			double BxAy = (double)Bx * (double)Ay;
			double ByAx = (double)By * (double)Ax;
			W = (float)(BxAy - ByAx);
		}

		if (withBackFace)
		{
			if ((U<0.f || V<0.f || W < 0.f) &&
				(U>0.f || V>0.f || W > 0.f))
				return false;
		}
		else
		{
			if (U<0.f || V<0.f || W<0.f)
				return false;
		}

		float det = U + V + W;

		if (det == 0.f)
			return false;

		const float Az = ray.m_Sz * A[ray.m_kz];
		const float Bz = ray.m_Sz * B[ray.m_kz];
		const float Cz = ray.m_Sz * C[ray.m_kz];
		const float Ts = (U*Az) + (V*Bz) + (W*Cz);

		if (!withBackFace) // CULL
		{
			if (Ts < 0.f || Ts > Infinity *det)
				return false;
		}
		else
		{
			if (det < 0.f && (Ts >= 0.f || Ts < Infinity * det))
				return false;
			else if (det > 0.f && (Ts <= 0.f || Ts > Infinity * det))
				return false;
		}

		const float invDet = 1.f / det;
		U = U*invDet;
		V = V*invDet;
		W = W*invDet;
		T = Ts*invDet;
		if (T < Epsilon)
			return false;
		return true;
	}
};

// create miPolygonCreator by yourself
// use it when you create new model
// don't forget to call Clear() before\after every polygon
class miPolygonCreator
{
	int m_size;
	int m_allocated;
	miPair<v3f,u8>* m_positions;
	v3f* m_normals;
	v2f* m_tcoords;

	void _reallocate(int size) {
		miPair<v3f, u8>* newPositions = new miPair<v3f, u8>[size];
		v3f* newNormals = new v3f[size];
		v2f* newtCoords = new v2f[size];

		for (int i = 0; i < m_size; ++i)
		{
			newPositions[i] = m_positions[i];
			newNormals[i] = m_normals[i];
			newtCoords[i] = m_tcoords[i];
		}

		delete[] m_positions;
		delete[] m_normals;
		delete[] m_tcoords;

		m_positions = newPositions;
		m_normals = newNormals;
		m_tcoords = newtCoords;

		m_allocated = size;
	}

public:
	miPolygonCreator():m_size(0), m_allocated(0), m_positions(0),m_normals(0), m_tcoords(0){}
	~miPolygonCreator() {
		if (m_positions) delete[] m_positions;
		if (m_normals) delete[] m_normals;
		if (m_tcoords) delete[] m_tcoords;
	}

	void Clear() { m_size = 0; }
	int Size() { return m_size; }

	enum
	{
		flag_weld = BIT(0),
		flag_selected = BIT(1)
	};

	void Add(const v3f& position, bool weld, bool selected, const v3f& normal, const v2f& tCoords) {
		if (m_size == m_allocated)
		{
			_reallocate(m_allocated + (int)std::ceil(((2.f + (float)m_allocated) * 0.5f)));
		}
		u8 flags = 0;
		if (weld)
			flags |= flag_weld;
		if(selected)
			flags |= flag_selected;

		m_positions[m_size] = miPair<v3f, u8>(position, flags);
		m_normals[m_size] = normal;
		m_tcoords[m_size] = tCoords;
		++m_size;
	}

	miPair<v3f, u8>* GetPositions() { return m_positions; }
	v3f* GetNormals() { return m_normals; }
	v2f* GetTCoords() { return m_tcoords; }
};

struct miSkeleton {

};



// m_mesh = miCreate<miMesh>();
// miDestroy(m_mesh);
struct miMesh
{
	miMesh() {
		m_first_polygon = 0;
		m_first_edge = 0;
		m_first_vertex = 0;
		m_skeleton = 0;
		
		m_vertexCount = 0;
		m_edgeCount = 0;
		m_polygonCount = 0;
		m_uvCount = 0;
	}
	miPolygon* m_first_polygon;
	miEdge* m_first_edge;
	miVertex* m_first_vertex;
	miSkeleton* m_skeleton;

	u32 m_vertexCount;
	u32 m_edgeCount;
	u32 m_polygonCount;
	u32 m_uvCount;

	void UpdateCounts() {
		m_vertexCount = 0;
		m_edgeCount = 0;
		m_polygonCount = 0;
		m_uvCount = 0;
		{
			auto c = m_first_vertex;
			if (c)
			{
				auto l = c->m_left;
				while (true)
				{
					++m_vertexCount;
					if (c == l)
						break;
					c = c->m_right;
				}
			}
		}
		{
			auto c = m_first_edge;
			if (c)
			{
				auto l = c->m_left;
				while (true)
				{
					++m_edgeCount;
					if (c == l)
						break;
					c = c->m_right;
				}
			}
		}
		{
			auto c = m_first_polygon;
			if (c)
			{
				auto l = c->m_left;
				while (true)
				{
					++m_polygonCount;
					auto cv = c->m_verts.m_head;
					auto lv = cv->m_left;
					while (true)
					{
						++m_uvCount;
						if (cv == lv)
							break;
						cv = cv->m_right;
					}
					if (c == l)
						break;
					c = c->m_right;
				}
			}
		}
	}

	// must be for each polygon.
	// array contain first miUVChannel* for each channel
	//miArray<miUVChannelNode*> m_UVChannels;

	template<typename _polygon_allocator_type, typename _edge_allocator_type, typename _vertex_allocator_type>
	void CreateEdges(_polygon_allocator_type* pa, _edge_allocator_type* ea, _vertex_allocator_type* va) {
		if (m_first_edge)
			_delete_edges(ea);
		if (!m_first_polygon)
			return;

		auto current_polygon = m_first_polygon;
		auto last_polygon = current_polygon->m_left;
		while (true) {
			auto next_polygon = current_polygon->m_right;

			auto current_vertex = current_polygon->m_verts.m_head;
			auto next_vertex = current_vertex->m_right;
			auto last_vertex = current_vertex->m_left;
			while (true) {
				miVertex* v1 = current_vertex->m_data.m_vertex;
				miVertex* v2 = next_vertex->m_data.m_vertex;

				// пусть вершина с адресом значение которого меньше
				//  будет на первом месте.
				if (v2 < v1)
				{
					v1 = next_vertex->m_data.m_vertex;
					v2 = current_vertex->m_data.m_vertex;
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
					newEdge = ea->Allocate();
					new(newEdge)miEdge();
					newEdge->m_vertex1 = v1;
					newEdge->m_vertex2 = v2;
					
					v1->m_edges.push_back(newEdge);
					v2->m_edges.push_back(newEdge);
					
					/*{
						auto n1 = v1->m_edges.find(newEdge);
						auto n2 = v2->m_edges.find(newEdge);
						printf("E[%u] N1[%u] N2[%u]\n", (u32)newEdge, (u32)n1, (u32)n2);
					}*/

					_add_edge_to_list(newEdge);
				}

				if (!newEdge->m_polygon1)
					newEdge->m_polygon1 = current_polygon;
				else if (!newEdge->m_polygon2)
					newEdge->m_polygon2 = current_polygon;
				if (newEdge->m_polygon1 == newEdge->m_polygon2)
					newEdge->m_polygon2 = 0;

				current_polygon->m_edges.push_back(newEdge);
				

				if (current_vertex == last_vertex)
					break;

				current_vertex = next_vertex;
				next_vertex = next_vertex->m_right;
			}


			if (current_polygon == last_polygon)
				break;
			current_polygon = next_polygon;
		}

		// Try to sort edges in vertices
		{
			miArray<miEdge*> sortedEdges;
			miArray<miEdge*> sortedEdges2;

			auto current_vertex = m_first_vertex;
			auto last_vertex = current_vertex->m_left;
			while (true)
			{
				sortedEdges.clear();
				sortedEdges2.clear();

				u32 count = 0;
				auto current_edge = current_vertex->m_edges.m_head;
				if (current_edge)
				{

					auto last_edge = current_edge->m_left;
					while (true)
					{

						if (current_edge->m_data->m_flags & miEdge::flag_User1)
							current_edge->m_data->m_flags ^= miEdge::flag_User1;

						sortedEdges.push_back(current_edge->m_data);

						if (current_edge == last_edge)
							break;
						current_edge = current_edge->m_right;
					}

					miEdge* lastEdge = 0;
					for (u32 i = 0, sz = sortedEdges.size(); i < sz; ++i)
					{
						if (i == 0)
						{
							sortedEdges2.push_back(sortedEdges.m_data[i]);
							sortedEdges.m_data[i]->m_flags |= miEdge::flag_User1;
							lastEdge = sortedEdges.m_data[i];
						}
						else
						{
							miEdge* thatEdge = 0;
							for (u32 i2 = 1, sz2 = sortedEdges.size(); i2 < sz2; ++i2)
							{
								auto e = sortedEdges.m_data[i2];
								if ((e->m_flags & miEdge::flag_User1) == 0)
								{
									if (lastEdge->m_polygon1) {
										if (e->m_polygon1) {
											if (e->m_polygon1 == lastEdge->m_polygon1) {
												thatEdge = e;
												break;
											}
										}
										if (e->m_polygon2) {
											if (e->m_polygon2 == lastEdge->m_polygon1) {
												thatEdge = e;
												break;
											}
										}
									}
									if (lastEdge->m_polygon2) {
										if (e->m_polygon1) {
											if (e->m_polygon1 == lastEdge->m_polygon2) {
												thatEdge = e;
												break;
											}
										}
										if (e->m_polygon2) {
											if (e->m_polygon2 == lastEdge->m_polygon2) {
												thatEdge = e;
												break;
											}
										}
									}
								}
							}
							if (thatEdge)
							{
								sortedEdges2.push_back(thatEdge);
								thatEdge->m_flags |= miEdge::flag_User1;
								lastEdge = thatEdge;
							}
							else
							{
								sortedEdges2.push_back(sortedEdges.m_data[i]);
								sortedEdges.m_data[i]->m_flags |= miEdge::flag_User1;
								lastEdge = sortedEdges.m_data[i];
							}
						}
					}

					current_edge = current_vertex->m_edges.m_head;
					for (size_t i = 0, sz = sortedEdges2.size(); i < sz; ++i)
					{
						current_edge->m_data = sortedEdges2.m_data[i];
						current_edge = current_edge->m_right;
					}
				}

				if (current_vertex == last_vertex)
					break;
				current_vertex = current_vertex->m_right;
			}
		}
	}

	template<typename _edge_allocator_type>
	void _delete_edges(_edge_allocator_type * ea) {
		if (m_first_polygon)
		{
			auto p = m_first_polygon;
			auto last = p->m_left;
			while (true) {
				auto next = p->m_right;
				p->m_edges.clear();
				if (p == last)
					break;
				p = next;
			}
		}
		if (m_first_vertex)
		{
			auto v = m_first_vertex;
			auto last = v->m_left;
			while (true) {
				auto next = v->m_right;
				v->m_edges.clear();
				if (v == last)
					break;
				v = next;
			}
		}
		if (m_first_edge)
		{
			auto e = m_first_edge;
			auto last = e->m_left;
			while (true) {
				auto next = e->m_right;
				e->~miEdge();
				ea->Deallocate(e);
				if (e == last)
					break;
				e = next;
			}
			m_first_edge = 0;
		}
	}
	void _add_edge_to_list(miEdge* newEdge) {
		if (!m_first_edge)
		{
			m_first_edge = newEdge;
			m_first_edge->m_right = m_first_edge;
			m_first_edge->m_left = m_first_edge;
		}
		else
		{
			auto last = m_first_edge->m_left;
			last->m_right = newEdge;
			newEdge->m_left = last;
			newEdge->m_right = m_first_edge;
			m_first_edge->m_left = newEdge;
		}
	}
	void _add_vertex_to_list(miVertex* newVertex) {
		if (!m_first_vertex)
		{
			m_first_vertex = newVertex;
			m_first_vertex->m_right = m_first_vertex;
			m_first_vertex->m_left = m_first_vertex;
		}
		else
		{
			auto last = m_first_vertex->m_left;
			last->m_right = newVertex;
			newVertex->m_left = last;
			newVertex->m_right = m_first_vertex;
			m_first_vertex->m_left = newVertex;
		}
	}
	void _add_polygon_to_list(miPolygon* newPolygon) {
		if (!m_first_polygon)
		{
			m_first_polygon = newPolygon;
			m_first_polygon->m_right = m_first_polygon;
			m_first_polygon->m_left = m_first_polygon;
		}
		else
		{
			auto last = m_first_polygon->m_left;
			last->m_right = newPolygon;
			newPolygon->m_left = last;
			newPolygon->m_right = m_first_polygon;
			m_first_polygon->m_left = newPolygon;
		}
	}
	
	void _remove_edge_from_list(miEdge* o) {
		auto l = o->m_left;
		auto r = o->m_right;
		l->m_right = r;
		r->m_left = l;

		if (o == m_first_edge)
			m_first_edge = o->m_right;
		if (o == m_first_edge)
			m_first_edge = 0;
	}
	void _remove_vertex_from_list(miVertex* o) {
		auto l = o->m_left;
		auto r = o->m_right;
		l->m_right = r;
		r->m_left = l;

		if (o == m_first_vertex)
			m_first_vertex = o->m_right;
		if (o == m_first_vertex)
			m_first_vertex = 0;
	}
	void _remove_polygon_from_list(miPolygon* o) {
		auto l = o->m_left;
		auto r = o->m_right;
		l->m_right = r;
		r->m_left = l;

		if (o == m_first_polygon)
			m_first_polygon = o->m_right;
		if (o == m_first_polygon)
			m_first_polygon = 0;
	}
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
		m_isBegin = false;
		m_mesh = miCreate<miMesh>();
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
		if (!m_mesh)
			return;
		m_mesh->_delete_edges(m_allocatorEdge);
		if (m_mesh->m_first_polygon)
		{
			auto p = m_mesh->m_first_polygon;
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


		if (m_mesh->m_first_vertex)
		{
			auto v = m_mesh->m_first_vertex;
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
		miDestroy(m_mesh);
		m_mesh = 0;
	}

	miMesh* m_mesh;

	_polygon_allocator_type* m_allocatorPolygon;
	_edge_allocator_type* m_allocatorEdge;
	_vertex_allocator_type* m_allocatorVertex;

	std::unordered_map<std::string, miVertex*> m_weldMap;
	std::string m_vertsMapHash;

	//v4f m_position;
	Aabb m_aabb;

	bool m_isBegin;
	void Begin() {
		if (m_isBegin) return;
		m_isBegin = true;
		m_weldMap.clear();
	}
	void End(){
		m_isBegin = false;
		CreateEdges();
	}
	void AddPolygon(miPolygonCreator* pc,/* bool weld,*/ bool triangulate, bool genNormals) {
		auto polygonVertexCount = pc->Size();
		if (polygonVertexCount < 3)
			return;

		if(!m_mesh)
			m_mesh = miCreate<miMesh>();

		miPolygon* newPolygon = m_allocatorPolygon->Allocate();
		new(newPolygon)miPolygon();
		

		if (!m_mesh->m_first_polygon)
		{
			m_mesh->m_first_polygon = newPolygon;
			m_mesh->m_first_polygon->m_right = m_mesh->m_first_polygon;
			m_mesh->m_first_polygon->m_left = m_mesh->m_first_polygon;
		}
		else
		{
			auto last = m_mesh->m_first_polygon->m_left;
			last->m_right = newPolygon;
			newPolygon->m_left = last;
			newPolygon->m_right = m_mesh->m_first_polygon;
			m_mesh->m_first_polygon->m_left = newPolygon;
		}

		auto positions = pc->GetPositions();
		auto normals = pc->GetNormals();
		auto tCoords = pc->GetTCoords();

		for (int i = 0; i < polygonVertexCount; ++i)
		{
			miVertex* newVertex = 0;
			m_aabb.add(positions[i].m_first);

			if (positions[i].m_second & miPolygonCreator::flag_weld)
			{
				_set_hash(&positions[i].m_first);

				auto find_result = m_weldMap.find(m_vertsMapHash);
				if (find_result == m_weldMap.end())
				{
					newVertex = m_allocatorVertex->Allocate();
					new(newVertex)miVertex();
					newVertex->m_position = positions[i].m_first;

					m_weldMap[m_vertsMapHash] = newVertex;
					m_mesh->_add_vertex_to_list(newVertex);
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
				newVertex->m_position = positions[i].m_first;

				m_mesh->_add_vertex_to_list(newVertex);
			}

			if (positions[i].m_second & miPolygonCreator::flag_selected)
				newVertex->m_flags |= miVertex::flag_isSelected;

			// add newPolygon to vertex and newVertex to polygon
			// only once
			if (newVertex->m_polygons.find(newPolygon) == 0)
			{
				newVertex->m_polygons.push_back(newPolygon);
				miPolygon::_vertex_data newVD;
				newVD.m_vertex = newVertex;
				newVD.m_uv = tCoords[i];
				newVD.m_normal = normals[i];
				newPolygon->m_verts.push_back(newVD);
			}
		}
	}
	void CreateEdges() {
		m_mesh->CreateEdges(m_allocatorPolygon, m_allocatorEdge, m_allocatorVertex);
	}
private:
	void _set_hash(v3f* position) {
		m_vertsMapHash.clear();
		char * ptr = (char *)position->data();
		char bytes[13];
		bytes[0] = ptr[0];
		bytes[1] = ptr[1];
		bytes[2] = ptr[2];
		bytes[3] = ptr[3];
		bytes[4] = ptr[4];
		bytes[5] = ptr[5];
		bytes[6] = ptr[6];
		bytes[7] = ptr[7];
		bytes[8] = ptr[8];
		bytes[9] = ptr[9];
		bytes[10] = ptr[10];
		bytes[11] = ptr[11];
		if (bytes[0] == 0) bytes[0] = 1;
		if (bytes[1] == 0) bytes[1] = 1;
		if (bytes[2] == 0) bytes[2] = 1;
		if (bytes[3] == 0) bytes[3] = 1;
		if (bytes[4] == 0) bytes[4] = 1;
		if (bytes[5] == 0) bytes[5] = 1;
		if (bytes[6] == 0) bytes[6] = 1;
		if (bytes[7] == 0) bytes[7] = 1;
		if (bytes[8] == 0) bytes[8] = 1;
		if (bytes[9] == 0) bytes[9] = 1;
		if (bytes[10] == 0) bytes[10] = 1;
		if (bytes[11] == 0) bytes[11] = 1;
		bytes[12] = 0;
		m_vertsMapHash = bytes;
	}
};

#endif