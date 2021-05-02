#ifndef _MI_SELFRUST_H_
#define _MI_SELFRUST_H_

/*
e   end
2---3
|   |
1---0
o   origin
*/
struct miSelectionFrust
{
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

	void createWithAabb(const Aabb& aabb);
	void createWithFrame(const v4f& frame, const v4f& vp_rect, const Mat4& VP_invert);
	bool pointInFrust(const v4f& v)const;
	bool lineInFrust(const v4f& p1, const v4f& p2)const;
	bool rayTest_MT(const v4f& ray_origin, const v4f& ray_end, const v4f& v1, const v4f& v2, const v4f& v3)const;
};


#endif