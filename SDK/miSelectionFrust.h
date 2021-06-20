#ifndef _MI_SELFRUST_H_
#define _MI_SELFRUST_H_

struct miSelectionFrustData {
	v4f m_left[4];
	v4f m_top[4];
	v4f m_right[4];
	v4f m_bottom[4];
	v4f m_front[4];
	v4f m_back[4];

	// center
	v4f m_LC;
	v4f m_TC;
	v4f m_RC;
	v4f m_BC;
	v4f m_FrontC;
	v4f m_BackC;

	// normal
	v3f m_LN;
	v3f m_TN;
	v3f m_RN;
	v3f m_BN;
	v3f m_FrontN;
	v3f m_BackN;

	// only for CreateWithFrame 
	yyRay m_ray1, m_ray2, m_ray3, m_ray4, m_ray5;
};

/*
e   end
2---3
|   |
1---0
o   origin
*/
class miSelectionFrust
{
public:
	miSelectionFrust() {}
	virtual ~miSelectionFrust() {}

	miSelectionFrustData m_data;

	virtual void CreateWithAabb(const Aabb& aabb) = 0;
	virtual void CreateWithFrame(const v4f& frame, const v4f& vp_rect, const Mat4& VP_invert) = 0;
	virtual bool PointInFrust(const v4f& v)const = 0;
	virtual bool LineInFrust(const v4f& p1, const v4f& p2)const = 0;
	// using in LineInFrust
	virtual bool RayTest_MT(const v4f& ray_origin, const v4f& ray_end, const v4f& v1, const v4f& v2, const v4f& v3)const = 0;
};


#endif