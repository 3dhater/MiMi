#ifndef _MI_VIEWPORTCAMERA_H_
#define _MI_VIEWPORTCAMERA_H_

struct miCameraFrustum
{
	v4f m_planes[6u];

	bool PointInFrustum(const v4f& point)
	{
		for (s32 i = 0; i < 6; ++i)
		{
			if ((
				m_planes[i].x * point.x +
				m_planes[i].y * point.y +
				m_planes[i].z * point.z +
				m_planes[i].w)
				< 0.f)
				return false;
		}
		return true;
	}

	bool PointInFrustum(const miVec4& point)
	{
		
		if ((
			m_planes[0].x * point.x +
			m_planes[0].y * point.y +
			m_planes[0].z * point.z +
			m_planes[0].w)
			< 0.f)
			return false;
		if ((
			m_planes[1].x * point.x +
			m_planes[1].y * point.y +
			m_planes[1].z * point.z +
			m_planes[1].w)
			< 0.f)
			return false;
		if ((
			m_planes[2].x * point.x +
			m_planes[2].y * point.y +
			m_planes[2].z * point.z +
			m_planes[2].w)
			< 0.f)
			return false;
		if ((
			m_planes[3].x * point.x +
			m_planes[3].y * point.y +
			m_planes[3].z * point.z +
			m_planes[3].w)
			< 0.f)
			return false;
		if ((
			m_planes[4].x * point.x +
			m_planes[4].y * point.y +
			m_planes[4].z * point.z +
			m_planes[4].w)
			< 0.f)
			return false;
		if ((
			m_planes[5].x * point.x +
			m_planes[5].y * point.y +
			m_planes[5].z * point.z +
			m_planes[5].w)
			< 0.f)
			return false;
		return true;
	}

	bool SphereInFrustum(f32 radius, const miVec4& position)
	{
		for (u32 i = 0u; i < 6u; ++i)
		{
			if (m_planes[i].x * position.x +
				m_planes[i].y * position.y +
				m_planes[i].z * position.z +
				m_planes[i].w <= -radius)
				return false;
		}
		return true;
		/*float dist01 = std::min(DistanceToPlane(m_planes[0], position), DistanceToPlane(m_planes[1], position));
		float dist23 = std::min(DistanceToPlane(m_planes[2], position), DistanceToPlane(m_planes[3], position));
		float dist45 = std::min(DistanceToPlane(m_planes[4], position), DistanceToPlane(m_planes[5], position));
		return std::min(std::min(dist01, dist23), dist45) + radius;*/
	}

	enum FrustumSide
	{
		RIGHT = 0,		// The RIGHT side of the frustum
		LEFT = 1,		// The LEFT	 side of the frustum
		BOTTOM = 2,		// The BOTTOM side of the frustum
		TOP = 3,		// The TOP side of the frustum
		BACK = 4,		// The BACK	side of the frustum
		FRONT = 5			// The FRONT side of the frustum
	};

	void NormalizePlane(v4f& plane)
	{
		// Here we calculate the magnitude of the normal to the plane (point A B C)
		// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
		// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
		float magnitude = (float)sqrt(plane.x * plane.x +
			plane.y * plane.y +
			plane.z * plane.z);

		// Then we divide the plane's values by it's magnitude.
		// This makes it easier to work with.
		plane.x /= magnitude;
		plane.y /= magnitude;
		plane.z /= magnitude;
		plane.w /= magnitude;
	}

	void CalculateFrustum(Mat4& P, Mat4& V)
	{
		f32 *proj = P.getPtr();
		f32 *modl = V.getPtr();
		float   clip[16]; //clipping planes

		clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
		clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
		clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
		clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

		clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
		clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
		clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
		clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

		clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
		clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
		clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
		clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

		clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
		clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
		clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
		clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];


		//RIGHT 
		m_planes[RIGHT].x = (clip[3] - clip[0]);
		m_planes[RIGHT].y = (clip[7] - clip[4]);
		m_planes[RIGHT].z = (clip[11u] - clip[8]);
		m_planes[RIGHT].w = (clip[15u] - clip[12u]);
		NormalizePlane(m_planes[RIGHT]);

		//LEFT  
		m_planes[LEFT].x = (clip[3] + clip[0]);
		m_planes[LEFT].y = (clip[7] + clip[4]);
		m_planes[LEFT].z = (clip[11u] + clip[8]);
		m_planes[LEFT].w = (clip[15u] + clip[12u]);
		NormalizePlane(m_planes[LEFT]);

		//BOTTOM  
		m_planes[BOTTOM].x = (clip[3] + clip[1]);
		m_planes[BOTTOM].y = (clip[7] + clip[5]);
		m_planes[BOTTOM].z = (clip[11u] + clip[9]);
		m_planes[BOTTOM].w = (clip[15u] + clip[13u]);
		NormalizePlane(m_planes[BOTTOM]);

		//TOP  
		m_planes[TOP].x = (clip[3] - clip[1]);
		m_planes[TOP].y = (clip[7] - clip[5]);
		m_planes[TOP].z = (clip[11u] - clip[9]);
		m_planes[TOP].w = (clip[15u] - clip[13u]);
		NormalizePlane(m_planes[TOP]);

		//FAR  
		m_planes[BACK].x = (clip[3] - clip[2]);
		m_planes[BACK].y = (clip[7] - clip[6]);
		m_planes[BACK].z = (clip[11u] - clip[10u]);
		m_planes[BACK].w = (clip[15u] - clip[14u]);
		NormalizePlane(m_planes[BACK]);

		//NEAR  
		m_planes[FRONT].x = (clip[3] + clip[2]);
		m_planes[FRONT].y = (clip[7] + clip[6]);
		m_planes[FRONT].z = (clip[11u] + clip[10u]);
		m_planes[FRONT].w = (clip[15u] + clip[14u]);
		NormalizePlane(m_planes[FRONT]);
	}
};

struct miViewport;
struct miViewportCamera
{
	miViewportCamera(miViewport*, miViewportCameraType ct);
	~miViewportCamera();

	void Copy(miViewportCamera*);
	
	miViewport* m_viewport;
	miViewportCameraType m_type;

	// calculate matrices
	void Update();
	Mat4 m_viewMatrix;
	Mat4 m_projectionMatrix;
	Mat4 m_viewProjectionMatrix;
	Mat4 m_viewProjectionInvertMatrix;


	f32 m_near;
	f32 m_far;
	f32 m_fov;
	f32 m_aspect;

	v3f m_rotationPlatform;
	v4f m_positionPlatform; // w = height, zoom, mouse wheel value
	v3f m_positionCamera; // in world

	void Reset();
	void MoveToSelection();

	v4f m_creationRect;

	void PanMove();
	void Rotate();
	void RotateZ();
	void Zoom();
	void ChangeFOV();

	miCameraFrustum m_frust;
};

#endif