#ifndef _MI_MATH_H_
#define _MI_MATH_H_

#include <cmath>
#include <algorithm>

#define miInfinity std::numeric_limits<float>::infinity()
#define miEpsilon std::numeric_limits<float>::epsilon()

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

struct miVec4;

struct miVec2
{
	miVec2() :x(0.f), y(0.f) {}
	miVec2(float _x, float _y) :x(_x), y(_y) {}
	miVec2(float val) :x(val), y(val) {}
	float x, y;
	void set(float _x, float _y) { x = _x; y = _y; }
	miVec2 operator*(const miVec2& v)const { miVec2 r; r.x = x * v.x; r.y = y * v.y; return r; }
	miVec2 operator-(const miVec2& v)const { miVec2 r; r.x = x - v.x; r.y = y - v.y; return r; }
	miVec2 operator+(const miVec2& v)const { miVec2 r; r.x = x + v.x; r.y = y + v.y; return r; }
	miVec2 operator/(const miVec2& v)const { miVec2 r; r.x = x / v.x; r.y = y / v.y; return r; }
	void operator+=(const miVec2& v) { x += v.x; y += v.y; }
	void operator-=(const miVec2& v) { x -= v.x; y -= v.y; }
	void operator*=(const miVec2& v) { x *= v.x; y *= v.y; }
	void operator/=(const miVec2& v) { x /= v.x; y /= v.y; }
	bool operator==(const miVec2& v)const { if (x != v.x)return false; if (y != v.y)return false; return true; }
	float distance(const miVec2& from)const { return miVec2(x - from.x, y - from.y).length(); }
	float length()const { return std::sqrt(lengthSqrt()); }
	float lengthSqrt()const { return(x*x) + (y*y); }
};

struct miVec3
{
	miVec3() :x(0.f), y(0.f), z(0.f) {}
	miVec3(float _v) :x(_v), y(_v), z(_v) {}
	miVec3(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
	float x, y, z;
	void set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
	void set(float val) { x = y = z = val; }
	float* data() { return &x; }
	void operator+=(const miVec3& v) { x += v.x; y += v.y; z += v.z; }
	void operator-=(const miVec3& v) { x -= v.x; y -= v.y; z -= v.z; }
	void operator*=(const miVec3& v) { x *= v.x; y *= v.y; z *= v.z; }
	void operator/=(const miVec3& v) { x /= v.x; y /= v.y; z /= v.z; }
	float operator[](int index)const { return (&x)[index]; }
	float& operator[](int index) { return (&x)[index]; }
	miVec3 operator+(const miVec3& v)const { miVec3 r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; }
	miVec3 operator-(const miVec3& v)const { miVec3 r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; return r; }
	miVec3 operator*(const miVec3& v)const { miVec3 r; r.x = x*v.x; r.y = y*v.y; r.z = z*v.z; return r; }
	miVec3 operator/(const miVec3& v)const { miVec3 r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; return r; }
	bool operator==(const miVec3& v)const {
		if (x != v.x)return false;
		if (y != v.y)return false;
		if (z != v.z)return false;
		return true;
	}
	miVec3 cross(const miVec3& a)const {
		miVec3 out;
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
		return out;
	}
	void cross2(const miVec3& a, miVec3& out)const {
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	void	normalize2()
	{
		float len = std::sqrt(dot());
		if (len > 0)
			len = 1.0f / len;
		x *= len; y *= len; z *= len;
	}
	float	dot(const miVec3& V2)const { return (x*V2.x) + (y*V2.y) + (z*V2.z); }
	float	dot()const { return (x*x) + (y*y) + (z*z); }
	miVec3 operator-()const { miVec3 r; r.x = -x; r.y = -y; r.z = -z; return r; }
	float length() const { return std::sqrt(length2()); }
	float length2() const { return dot(*this); }

	void add(const miVec4&);
	miVec3 operator+(const miVec4& v)const;
};

struct miVec4
{
	miVec4() :x(0.f), y(0.f), z(0.f), w(0.f) {}
	miVec4(const miVec4& _v) :x(_v.x), y(_v.y), z(_v.z), w(_v.w) {}
	miVec4(const miVec3& _v) :x(_v.x), y(_v.y), z(_v.z), w(0.f) {}
	miVec4(float _v) :x(_v), y(_v), z(_v), w(_v) {}
	miVec4(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w) {}
	float x, y, z, w;

	const float* cdata()const { return &x; }
	float* data() { return &x; }
	float operator[](int index)const {  return (&x)[index]; }
	float& operator[](int index) {  return (&x)[index]; }

	void set(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
	void set(float val) { x = y = z = w = val; }
	void operator+=(const miVec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
	void operator-=(const miVec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
	void operator*=(const miVec4& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
	void operator/=(const miVec4& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; }

	miVec4 operator+(const miVec4& v)const { miVec4 r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; r.w = w + v.w; return r; }
	miVec4 operator-(const miVec4& v)const { miVec4 r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; r.w = w - v.w; return r; }
	miVec4 operator*(const miVec4& v)const { miVec4 r; r.x = x*v.x; r.y = y*v.y; r.z = z*v.z; r.w = w*v.w; return r; }
	miVec4 operator/(const miVec4& v)const { miVec4 r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; r.w = w / v.w; return r; }

	miVec4 operator-()const { miVec4 r; r.x = -x; r.y = -y; r.z = -z; r.w = -w; return r; }

	float	dot(const miVec4& V2)const { return (x*V2.x) + (y*V2.y) + (z*V2.z) + (w*V2.w); }
	float	dot()const { return (x*x) + (y*y) + (z*z) + (w*w); }
	void	normalize2()
	{
		float len = std::sqrt(dot());
		if (len > 0)
			len = 1.0f / len;
		x *= len; y *= len; z *= len; w *= len;
	}
	miVec4 cross1(const miVec4& a)const {
		return miVec4(
			(y * a.z) - (z * a.y),
			(z * a.x) - (x * a.z),
			(x * a.y) - (y * a.x),
			0.f);
	}
	void cross2(const miVec4& a, miVec4& out)const {
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	float lengthSqrt()const { return(x*x) + (y*y) + (z*z); }
	float length()const { return std::sqrt(lengthSqrt()); }
	float distance(const miVec4& from)const { return miVec4(x - from.x, y - from.y, z - from.z, 1.f).length(); }
	bool operator==(const miVec4& v)const {
		if (x != v.x)return false;
		if (y != v.y)return false;
		if (z != v.z)return false;
		if (w != v.w)return false;
		return true;
	}
};
inline miVec2 operator*(const float& s, const miVec2& v) { return v * s; }
inline miVec3 operator*(const float& s, const miVec3& v) { return v * s; }
inline miVec4 operator*(const float& s, const miVec4& v) { return v * s; }

struct miQuaternion
{
	float x, y, z, w;

	miQuaternion(){identity();}
	miQuaternion(const miQuaternion& o):x(o.x),y(o.y),z(o.z),w(o.w){}
	miQuaternion(float i1, float i2, float i3, float real):x(i1),y(i2),z(i3),w(real){}
	miQuaternion(const miVec3& _axis, const float& _angle){setRotation(_axis, _angle);}
	miQuaternion(float Euler_x, float Euler_y, float Euler_z){set(Euler_x, Euler_y, Euler_z);}
	miQuaternion(const miVec4& v) { set(v); }
	float* data() { return &x; }
	miVec4	get() const { return miVec4(x, y, z, w); }
	float	getReal() const { return w; }
	void set(float i1, float i2, float i3, float real) {x = i1;y = i2;z = i3;w = real;}
	void set(const miVec4& v) { set(v.x, v.y, v.z); }
	void set(float Euler_x, float Euler_y, float Euler_z) {
		Euler_x *= 0.5f;
		Euler_y *= 0.5f;
		Euler_z *= 0.5f;
		float c1 = cos(Euler_x);
		float c2 = cos(Euler_y);
		float c3 = cos(Euler_z);
		float s1 = sin(Euler_x);
		float s2 = sin(Euler_y);
		float s3 = sin(Euler_z);
		w = (c1 * c2 * c3) + (s1 * s2 * s3);
		x = (s1 * c2 * c3) - (c1 * s2 * s3);
		y = (c1 * s2 * c3) + (s1 * c2 * s3);
		z = (c1 * c2 * s3) - (s1 * s2 * c3);
	}
	void setRotation(const miVec3& axis, const float& _angle){
		float d = axis.length();
		float s = sin(_angle * 0.5f) / d;
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
		w = cos(_angle * 0.5f);
	}
	void setReal(float r) {w = r;}
	void identity() {x = 0.f;y = 0.f;z = 0.f;w = 1.f;}
	miQuaternion& operator=(const miQuaternion& o) {x = o.x;y = o.y;z = o.z;w = o.w;return *this;}
	miQuaternion operator*(const miQuaternion& q)const {
		return miQuaternion(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z);
	}
	void operator*=(const miQuaternion& q) {
		x = w * q.x + x * q.w + y * q.z - z * q.y;
		y = w * q.y + y * q.w + z * q.x - x * q.z;
		z = w * q.z + z * q.w + x * q.y - y * q.x;
		w = w * q.w - x * q.x - y * q.y - z * q.z;
	}
	bool operator!=(const miQuaternion& q)const {
		if (x != q.x) return true;
		if (y != q.y) return true;
		if (z != q.z) return true;
		if (w != q.w) return true;
		return false;
	}
	bool operator==(const miQuaternion& q)const {
		if (x != q.x) return false;
		if (y != q.y) return false;
		if (z != q.z) return false;
		if (w != q.w) return false;
		return true;
	}
	miQuaternion operator+(const miQuaternion& o) const {return miQuaternion(x + o.x,y + o.y,z + o.z,w + o.w);}
	miQuaternion operator-(const miQuaternion& o) const {return miQuaternion(x - o.x,y - o.y,z - o.z,w - o.w);}
	miQuaternion operator-() {x = -x;y = -y;z = -z;return miQuaternion(x, y, z, w);}
	float operator[](int index) const{return (&x)[index];}
	float& operator[](int index){return (&x)[index];}
	float	getLength() { return std::sqrt(x*x + y*y + z*z + w*w); }
	float length2() const { return dot(*this); }
	miQuaternion operator*(float s) const{return miQuaternion(s*x, s*y, s*z, s*w);}
	miQuaternion lerp(miQuaternion q1, miQuaternion q2, float time){
		const float scale = 1.0f - time;
		return (*this = (q1*scale) + (q2*time));
	}
	float dot(const miQuaternion& q2) const{return (x * q2.x) + (y * q2.y) + (z * q2.z) + (w * q2.w);}
	miQuaternion slerp(miQuaternion q1, miQuaternion q2, float time, float threshold){
		float angle = q1.dot(q2);
		// make sure we use the short rotation
		if (angle < 0.0f){
			q1 = q1 * -1.0f;
			angle *= -1.0f;
		}

		if (angle <= (1 - threshold)) // spherical interpolation
		{
			const float theta = acosf(angle);
			const float invsintheta = 1.f / (sinf(theta));
			const float scale = sinf(theta * (1.0f - time)) * invsintheta;
			const float invscale = sinf(theta * time) * invsintheta;
			return (*this = (q1*scale) + (q2*invscale));
		}
		else // linear interploation
			return lerp(q1, q2, time);
	}

	miQuaternion& normalize() {
		float len = this->getLength();
		if (len) {
			float len2 = 1.f / len;
			x *= len2;
			y *= len2;
			z *= len2;
			w *= len2;
		}
		return *this;
	}
	void invert(){x = -x;y = -y;z = -z;}
};

struct miMatrix{
	miVec4 m_data[4u];
	miMatrix(){identity();}
	miMatrix(const miMatrix& m){*this = m;}
	miMatrix(const miQuaternion& q){identity();setRotation(q);}
	miMatrix(float v){fill(v);}
	miMatrix(const miVec4& x, const miVec4& y, const miVec4& z, const miVec4& w) {m_data[0u] = x;m_data[1u] = y;m_data[2u] = z;m_data[3u] = w;}
	void fill(float v) {m_data[0u].set(v);m_data[1u].set(v); m_data[2u].set(v);m_data[3u].set(v);}
	void		zero() {fill(0.f);}
	void		identity() {
		auto * p = this->getPtr();
		p[0u] = 1.f;
		p[1u] = 0.f;
		p[2u] = 0.f;
		p[3u] = 0.f;

		p[4u] = 0.f;
		p[5u] = 1.f;
		p[6u] = 0.f;
		p[7u] = 0.f;

		p[8u] = 0.f;
		p[9u] = 0.f;
		p[10u] = 1.f;
		p[11u] = 0.f;

		p[12u] = 0.f;
		p[13u] = 0.f;
		p[14u] = 0.f;
		p[15u] = 1.f;

	}

	void setValue(float xx, float xy, float xz,
		float yx, float yy, float yz,
		float zx, float zy, float zz)
	{
		m_data[0].set(xx, xy, xz, 0.f);
		m_data[1].set(yx, yy, yz, 0.f);
		m_data[2].set(zx, zy, zz, 0.f);
	}

	void setRotation(const miQuaternion& q)
	{
		float d = q.length2();
		float s = 2.0f / d;
		float xs = q.x * s, ys = q.y * s, zs = q.z * s;
		float wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
		float xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
		float yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
		setValue(
			1.0f - (yy + zz), xy - wz, xz + wy,
			xy + wz, 1.0f - (xx + zz), yz - wx,
			xz - wy, yz + wx, 1.0f - (xx + yy));
	}

	void setTranslation(const miVec3& v) { m_data[3].set(v.x, v.y, v.z, 1.f); }
	void setTranslation(const miVec4& v) { m_data[3].set(v.x, v.y, v.z, 1.f); }

	float * getPtr() { return reinterpret_cast<float*>(&m_data); }
	float * getPtrConst()const { return (float*)&m_data; }
	miVec4&       operator[](int i) { return m_data[i]; }
	const miVec4& operator[](int i) const { return m_data[i]; }
	miMatrix operator+(const miMatrix& m) const {
		miMatrix out = *this;
		out[0u] += m[0u];
		out[1u] += m[1u];
		out[2u] += m[2u];
		out[3u] += m[3u];
		return out;
	}

	miMatrix operator-(const miMatrix& m) const {
		miMatrix out = *this;
		out[0u] -= m[0u];
		out[1u] -= m[1u];
		out[2u] -= m[2u];
		out[3u] -= m[3u];
		return out;
	}

	miMatrix operator*(const miMatrix& m) const{
		return miMatrix(
			m_data[0u] * m[0u].x + m_data[1u] * m[0u].y + m_data[2u] * m[0u].z + m_data[3u] * m[0u].w,
			m_data[0u] * m[1u].x + m_data[1u] * m[1u].y + m_data[2u] * m[1u].z + m_data[3u] * m[1u].w,
			m_data[0u] * m[2u].x + m_data[1u] * m[2u].y + m_data[2u] * m[2u].z + m_data[3u] * m[2u].w,
			m_data[0u] * m[3u].x + m_data[1u] * m[3u].y + m_data[2u] * m[3u].z + m_data[3u] * m[3u].w
		);
	}

	// возможно тут нужно по другому.
	miVec4 operator*(const miVec4& v) const {
		return miVec4
		(
			v.x * m_data[0].x + v.y * m_data[1].x + v.z * m_data[2].x + v.w * m_data[2].x,
			v.x * m_data[0].y + v.y * m_data[1].y + v.z * m_data[2].y + v.w * m_data[2].y,
			v.x * m_data[0].z + v.y * m_data[1].z + v.z * m_data[2].z + v.w * m_data[2].z,
			v.x * m_data[0].w + v.y * m_data[1].w + v.z * m_data[2].w + v.w * m_data[2].w
		);
	}

	miMatrix operator/(const miMatrix& m) const {
		miMatrix out = *this;
		out[0u] /= m[0u];
		out[1u] /= m[1u];
		out[2u] /= m[2u];
		out[3u] /= m[3u];
		return out;
	}

	miMatrix& operator+=(const miMatrix& m) {
		m_data[0u] += m[0u];
		m_data[1u] += m[1u];
		m_data[2u] += m[2u];
		m_data[3u] += m[3u];
		return *this;
	}

	miMatrix& operator-=(const miMatrix& m) {
		m_data[0u] -= m[0u];
		m_data[1u] -= m[1u];
		m_data[2u] -= m[2u];
		m_data[3u] -= m[3u];
		return *this;
	}

	miMatrix& operator*=(const miMatrix& m) {(*this) = (*this) * m;return *this;}
	miMatrix& operator/=(const miMatrix& m) {
		m_data[0u] /= m[0u];
		m_data[1u] /= m[1u];
		m_data[2u] /= m[2u];
		m_data[3u] /= m[3u];
		return *this;
	}

	void transpose(){
		miMatrix tmp;
		tmp[0u].x = this->m_data[0u].x; //0
		tmp[0u].y = this->m_data[1u].x; //1
		tmp[0u].z = this->m_data[2u].x; //2
		tmp[0u].w = this->m_data[3u].x; //3

		tmp[1u].x = this->m_data[0u].y; //4
		tmp[1u].y = this->m_data[1u].y; //5
		tmp[1u].z = this->m_data[2u].y; //6
		tmp[1u].w = this->m_data[3u].y; //7

		tmp[2u].x = this->m_data[0u].z; //8
		tmp[2u].y = this->m_data[1u].z; //9
		tmp[2u].z = this->m_data[2u].z; //10
		tmp[2u].w = this->m_data[3u].z; //11

		tmp[3u].x = this->m_data[0u].w; //12
		tmp[3u].y = this->m_data[1u].w; //13
		tmp[3u].z = this->m_data[2u].w; //14
		tmp[3u].w = this->m_data[3u].w; //15
		this->m_data[0u] = tmp[0u];
		this->m_data[1u] = tmp[1u];
		this->m_data[2u] = tmp[2u];
		this->m_data[3u] = tmp[3u];
	}

	//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse
	bool invert(){
		miMatrix mat;
		auto ptr = this->getPtr();
		for (unsigned column = 0; column < 4; ++column)
		{
			// Swap row in case our pivot point is not working
			auto column_data = m_data[column].data();
			if (column_data[column] == 0)
			{
				unsigned big = column;
				for (unsigned row = 0; row < 4; ++row)
				{
					auto row_data = m_data[row].data();
					auto big_data = m_data[big].data();
					if (fabs(row_data[column]) > fabs(big_data[column]))
						big = row;
				}
				// Print this is a singular matrix, return identity ?
				if (big == column)
				{
					//printf("Singular matrix\n");
				}
				// Swap rows                               
				else for (unsigned j = 0; j < 4; ++j)
				{
					auto big_data = m_data[big].data();
					std::swap(column_data[j], big_data[j]);

					auto other_column_data = mat.m_data[column].data();
					auto other_big_data = mat.m_data[big].data();
					std::swap(other_column_data[j], other_big_data[j]);
				}
			}

			// Set each row in the column to 0  
			for (unsigned row = 0; row < 4; ++row)
			{
				if (row != column)
				{
					auto row_data = m_data[row].data();
					float coeff = row_data[column] / column_data[column];
					if (coeff != 0)
					{
						for (unsigned j = 0; j < 4; ++j)
						{
							row_data[j] -= coeff * column_data[j];

							auto other_row_data = mat.m_data[row].data();
							auto other_column_data = mat.m_data[column].data();
							other_row_data[j] -= coeff * other_column_data[j];
						}
						// Set the element to 0 for safety
						row_data[column] = 0;
					}
				}
			}
		}

		// Set each element of the diagonal to 1
		for (unsigned row = 0; row < 4; ++row)
		{
			for (unsigned column = 0; column < 4; ++column)
			{
				auto other_row_data = mat.m_data[row].data();
				auto row_data = m_data[row].data();
				other_row_data[column] /= row_data[row];
			}
		}

		*this = mat;
		return true;
	}

	bool invert2() {
		miMatrix r0, r1, r2, r3;
		float a, det, invDet;
		float* mat = reinterpret_cast<float *>(this);

		det = mat[0 * 4 + 0] * mat[1 * 4 + 1] - mat[0 * 4 + 1] * mat[1 * 4 + 0];
		if (std::abs(det) < 1e-14) {
			return false;
		}
		miVec4 dd;

		invDet = 1.0f / det;

		r0[0][0] = mat[1 * 4 + 1] * invDet;
		r0[0][1] = -mat[0 * 4 + 1] * invDet;
		r0[1][0] = -mat[1 * 4 + 0] * invDet;
		r0[1][1] = mat[0 * 4 + 0] * invDet;
		r1[0][0] = r0[0][0] * mat[0 * 4 + 2] + r0[0][1] * mat[1 * 4 + 2];
		r1[0][1] = r0[0][0] * mat[0 * 4 + 3] + r0[0][1] * mat[1 * 4 + 3];
		r1[1][0] = r0[1][0] * mat[0 * 4 + 2] + r0[1][1] * mat[1 * 4 + 2];
		r1[1][1] = r0[1][0] * mat[0 * 4 + 3] + r0[1][1] * mat[1 * 4 + 3];
		r2[0][0] = mat[2 * 4 + 0] * r1[0][0] + mat[2 * 4 + 1] * r1[1][0];
		r2[0][1] = mat[2 * 4 + 0] * r1[0][1] + mat[2 * 4 + 1] * r1[1][1];
		r2[1][0] = mat[3 * 4 + 0] * r1[0][0] + mat[3 * 4 + 1] * r1[1][0];
		r2[1][1] = mat[3 * 4 + 0] * r1[0][1] + mat[3 * 4 + 1] * r1[1][1];
		r3[0][0] = r2[0][0] - mat[2 * 4 + 2];
		r3[0][1] = r2[0][1] - mat[2 * 4 + 3];
		r3[1][0] = r2[1][0] - mat[3 * 4 + 2];
		r3[1][1] = r2[1][1] - mat[3 * 4 + 3];

		det = r3[0][0] * r3[1][1] - r3[0][1] * r3[1][0];
		if (std::abs(det) < 1e-14) {
			return false;
		}

		invDet = 1.0f / det;

		a = r3[0][0];
		r3[0][0] = r3[1][1] * invDet;
		r3[0][1] = -r3[0][1] * invDet;
		r3[1][0] = -r3[1][0] * invDet;
		r3[1][1] = a * invDet;
		r2[0][0] = mat[2 * 4 + 0] * r0[0][0] + mat[2 * 4 + 1] * r0[1][0];
		r2[0][1] = mat[2 * 4 + 0] * r0[0][1] + mat[2 * 4 + 1] * r0[1][1];
		r2[1][0] = mat[3 * 4 + 0] * r0[0][0] + mat[3 * 4 + 1] * r0[1][0];
		r2[1][1] = mat[3 * 4 + 0] * r0[0][1] + mat[3 * 4 + 1] * r0[1][1];
		mat[2 * 4 + 0] = r3[0][0] * r2[0][0] + r3[0][1] * r2[1][0];
		mat[2 * 4 + 1] = r3[0][0] * r2[0][1] + r3[0][1] * r2[1][1];
		mat[3 * 4 + 0] = r3[1][0] * r2[0][0] + r3[1][1] * r2[1][0];
		mat[3 * 4 + 1] = r3[1][0] * r2[0][1] + r3[1][1] * r2[1][1];
		mat[0 * 4 + 0] = r0[0][0] - r1[0][0] * mat[2 * 4 + 0] - r1[0][1] * mat[3 * 4 + 0];
		mat[0 * 4 + 1] = r0[0][1] - r1[0][0] * mat[2 * 4 + 1] - r1[0][1] * mat[3 * 4 + 1];
		mat[1 * 4 + 0] = r0[1][0] - r1[1][0] * mat[2 * 4 + 0] - r1[1][1] * mat[3 * 4 + 0];
		mat[1 * 4 + 1] = r0[1][1] - r1[1][0] * mat[2 * 4 + 1] - r1[1][1] * mat[3 * 4 + 1];
		mat[0 * 4 + 2] = r1[0][0] * r3[0][0] + r1[0][1] * r3[1][0];
		mat[0 * 4 + 3] = r1[0][0] * r3[0][1] + r1[0][1] * r3[1][1];
		mat[1 * 4 + 2] = r1[1][0] * r3[0][0] + r1[1][1] * r3[1][0];
		mat[1 * 4 + 3] = r1[1][0] * r3[0][1] + r1[1][1] * r3[1][1];
		mat[2 * 4 + 2] = -r3[0][0];
		mat[2 * 4 + 3] = -r3[0][1];
		mat[3 * 4 + 2] = -r3[1][0];
		mat[3 * 4 + 3] = -r3[1][1];

		return true;
	}
};

namespace mimath 
{
	inline miVec4 mul(const miVec4& vec, const miMatrix& mat){
		return miVec4
		(
			mat[0u].x * vec.x + mat[1u].x * vec.y + mat[2u].x * vec.z + mat[3u].x * vec.w,
			mat[0u].y * vec.x + mat[1u].y * vec.y + mat[2u].y * vec.z + mat[3u].y * vec.w,
			mat[0u].z * vec.x + mat[1u].z * vec.y + mat[2u].z * vec.z + mat[3u].z * vec.w,
			mat[0u].w * vec.x + mat[1u].w * vec.y + mat[2u].w * vec.z + mat[3u].w * vec.w
		);
	}
	inline miVec3 mul(const miVec3& vec, const miMatrix& mat){
		return miVec3
		(
			mat[0u].x * vec.x + mat[1u].x * vec.y + mat[2u].x * vec.z + mat[3u].x,
			mat[0u].y * vec.x + mat[1u].y * vec.y + mat[2u].y * vec.z + mat[3u].y,
			mat[0u].z * vec.x + mat[1u].z * vec.y + mat[2u].z * vec.z + mat[3u].z
		);
	}
	inline miVec3 mulBasis(const miVec3& vec, const miMatrix& mat){
		return miVec3
		(
			mat[0u].x * vec.x + mat[1u].x * vec.y + mat[2u].x * vec.z,
			mat[0u].y * vec.x + mat[1u].y * vec.y + mat[2u].y * vec.z,
			mat[0u].z * vec.x + mat[1u].z * vec.y + mat[2u].z * vec.z
		);
	}
	const miVec4 miVec4FltMax = miVec4(FLT_MAX);
	const miVec4 miVec4FltMaxNeg = miVec4(-FLT_MAX);
}

struct miRay {
	miRay() {
		m_kz = 0;
		m_kx = 0;
		m_ky = 0;

		m_Sx = 0.f;
		m_Sy = 0.f;
		m_Sz = 0.f;
	}

	miVec4 m_origin;
	miVec4 m_end;
	miVec4 m_dir;
	miVec4 m_invDir;

	int m_kz;
	int m_kx;
	int m_ky;

	float m_Sx;
	float m_Sy;
	float m_Sz;

	int max_dim(const miVec4& v){
		return (v.x > v.y) ? ((v.x > v.z)
			? 0 : 2) : ((v.y > v.z) ? 1 : 2);
	}

	void update() {
		m_dir.x = m_end.x - m_origin.x;
		m_dir.y = m_end.y - m_origin.y;
		m_dir.z = m_end.z - m_origin.z;
		m_dir.normalize2();
		m_dir.w = 1.f;

		m_invDir.x = 1.f / m_dir.x;
		m_invDir.y = 1.f / m_dir.y;
		m_invDir.z = 1.f / m_dir.z;
		m_invDir.w = 1.f;

		m_kz = max_dim
		(
			miVec4
			(
				std::abs(m_dir.x),
				std::abs(m_dir.y),
				std::abs(m_dir.z),
				1.f
			)
		);

		m_kx = m_kz + 1;
		if (m_kx == 3)
			m_kx = 0;

		m_ky = m_kx + 1;
		if (m_ky == 3)
			m_ky = 0;

		auto dir_data = m_dir.cdata();
		if (dir_data[m_kz])
			std::swap(m_kx, m_ky);

		m_Sx = dir_data[m_kx] / dir_data[m_kz];
		m_Sy = dir_data[m_ky] / dir_data[m_kz];
		m_Sz = 1.f / dir_data[m_kz];
	}

	// line-line intersection
	float distanceToLine(const miVec4& lineP0, const miVec4& lineP1){
		miVec4 u = m_end - m_origin;
		miVec4 v = lineP1 - lineP0;
		miVec4 w = m_origin - lineP0;
		float a = u.dot();
		float b = u.dot(v);
		float c = v.dot();
		float d = u.dot(w);
		float e = v.dot(w);
		float D = a*c - b*b;
		float sc, tc;

		if (D < miEpsilon)
		{
			sc = 0.f;
			tc = (b>c ? d / b : e / c);
		}
		else
		{
			sc = (b*e - c*d) / D;
			tc = (a*e - b*d) / D;
		}

		miVec4 dP = w + (sc*u) - (tc*v);
		return std::sqrt(dP.dot());
	}

	bool planeIntersection(const miVec4& planePoint, const miVec4& planeNormal, miVec4& ip) {
		float det = (planeNormal.x*m_dir.x) + (planeNormal.y*m_dir.y) + (planeNormal.z*m_dir.z);
		
		if (std::fabs(det) < miEpsilon) return false;

		miVec4 v;
		v.x = planePoint.x - m_origin.x;
		v.y = planePoint.y - m_origin.y;
		v.z = planePoint.z - m_origin.z;

		float t = (planeNormal.x*v.x) + (planeNormal.y*v.y) + (planeNormal.z*v.z);

		t /= det;

		ip = m_origin + t * m_dir;

		return true;
	}
	/*
	public static Vector lineIntersection(Vector planePoint, Vector planeNormal, Vector linePoint, Vector lineDirection) {
    if (planeNormal.dot(lineDirection.normalize()) == 0) {
        return null;
    }

    double t = (planeNormal.dot(planePoint) - planeNormal.dot(linePoint)) / planeNormal.dot(lineDirection.normalize());
    return linePoint.plus(lineDirection.normalize().scale(t));
}
	*/
};

struct miAabb
{
	miAabb()
		:
		m_min(mimath::miVec4FltMax),
		m_max(mimath::miVec4FltMaxNeg)
	{}
	miAabb(const miVec4& min, const miVec4& max) :m_min(min), m_max(max) { }

	/*
	Transforming Axis-Aligned Bounding Boxes
	by Jim Arvo
	from "Graphics Gems", Academic Press, 1990
	*/
	void transform(miAabb* original, miMatrix* matrix, miVec4* position) {
		float  a, b;
		float  Amin[3], Amax[3];
		float  Bmin[3], Bmax[3];
		int    i, j;

		/*Copy box A into a min array and a max array for easy reference.*/
		Amin[0] = original->m_min.x;  Amax[0] = original->m_max.x;
		Amin[1] = original->m_min.y;  Amax[1] = original->m_max.y;
		Amin[2] = original->m_min.z;  Amax[2] = original->m_max.z;

		/* Take care of translation by beginning at T. */
		Bmin[0] = Bmax[0] = position->x;
		Bmin[1] = Bmax[1] = position->y;
		Bmin[2] = Bmax[2] = position->z;

		/* Now find the extreme points by considering the product of the */
		/* min and max with each component of M.  */
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				a = (float)(matrix->m_data[i][j] * Amin[j]);
				b = (float)(matrix->m_data[i][j] * Amax[j]);
				if (a < b)
				{
					Bmin[i] += a;
					Bmax[i] += b;
				}
				else
				{
					Bmin[i] += b;
					Bmax[i] += a;
				}
			}
		}
		/* Copy the result into the new box. */
		m_min.x = Bmin[0];  m_max.x = Bmax[0];
		m_min.y = Bmin[1];  m_max.y = Bmax[1];
		m_min.z = Bmin[2];  m_max.z = Bmax[2];
	}

	void add(const miVec4& point){
		if (point.x < m_min.x) m_min.x = point.x;
		if (point.y < m_min.y) m_min.y = point.y;
		if (point.z < m_min.z) m_min.z = point.z;

		if (point.x > m_max.x) m_max.x = point.x;
		if (point.y > m_max.y) m_max.y = point.y;
		if (point.z > m_max.z) m_max.z = point.z;
	}

	void add(const miVec3& point){
		if (point.x < m_min.x) m_min.x = point.x;
		if (point.y < m_min.y) m_min.y = point.y;
		if (point.z < m_min.z) m_min.z = point.z;

		if (point.x > m_max.x) m_max.x = point.x;
		if (point.y > m_max.y) m_max.y = point.y;
		if (point.z > m_max.z) m_max.z = point.z;
	}

	void add(const miAabb& box){
		if (box.m_min.x < m_min.x) m_min.x = box.m_min.x;
		if (box.m_min.y < m_min.y) m_min.y = box.m_min.y;
		if (box.m_min.z < m_min.z) m_min.z = box.m_min.z;

		if (box.m_max.x > m_max.x) m_max.x = box.m_max.x;
		if (box.m_max.y > m_max.y) m_max.y = box.m_max.y;
		if (box.m_max.z > m_max.z) m_max.z = box.m_max.z;
	}

	bool rayTest(const miRay& r){
		float t1 = (m_min.x - r.m_origin.x)*r.m_invDir.x;
		float t2 = (m_max.x - r.m_origin.x)*r.m_invDir.x;
		float t3 = (m_min.y - r.m_origin.y)*r.m_invDir.y;
		float t4 = (m_max.y - r.m_origin.y)*r.m_invDir.y;
		float t5 = (m_min.z - r.m_origin.z)*r.m_invDir.z;
		float t6 = (m_max.z - r.m_origin.z)*r.m_invDir.z;

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

		if (tmax < 0 || tmin > tmax) return false;

		return true;
	}

	void center(miVec4& v) const{
		v = miVec4(m_min + m_max);
		v *= 0.5f;
	}

	float radius(){
		auto c =  miVec4(m_min + m_max);
		c *= 0.5f;
		return c.distance(m_max);
	}
	void extent(miVec4& v) { v = miVec4(m_max - m_min); }
	bool isEmpty() const { 
		return ((m_min == mimath::miVec4FltMax) && (m_max == mimath::miVec4FltMaxNeg))
			|| (m_min == m_max);
	}
	void reset() { m_min = mimath::miVec4FltMax; m_max = mimath::miVec4FltMaxNeg; }

	miVec4 m_min;
	miVec4 m_max;
};

inline
void miVec3::add(const miVec4& v) {
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
}

inline
miVec3 miVec3::operator+(const miVec4& v)const { 
	miVec3 r; 
	r.x = x + v.x; 
	r.y = y + v.y; 
	r.z = z + v.z; 
	return r; 
}

#endif