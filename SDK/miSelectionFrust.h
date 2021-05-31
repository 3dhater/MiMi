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
	v4f m_LN;
	v4f m_TN;
	v4f m_RN;
	v4f m_BN;
	v4f m_FrontN;
	v4f m_BackN;
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