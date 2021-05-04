#ifndef _MI_SELFRUST_H_
#define _MI_SELFRUST_H_

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

	miVec4 m_left[4];
	miVec4 m_top[4];
	miVec4 m_right[4];
	miVec4 m_bottom[4];
	miVec4 m_front[4];
	miVec4 m_back[4];

	// center
	miVec4 m_LC;
	miVec4 m_TC;
	miVec4 m_RC;
	miVec4 m_BC;
	miVec4 m_FrontC;
	miVec4 m_BackC;

	// normal
	miVec4 m_LN;
	miVec4 m_TN;
	miVec4 m_RN;
	miVec4 m_BN;
	miVec4 m_FrontN;
	miVec4 m_BackN;

	virtual void CreateWithAabb(const miAabb& aabb) = 0;
	virtual void CreateWithFrame(const miVec4& frame, const miVec4& vp_rect, const miMatrix& VP_invert) = 0;
	virtual bool PointInFrust(const miVec4& v)const = 0;
	virtual bool LineInFrust(const miVec4& p1, const miVec4& p2)const = 0;
	// using in LineInFrust
	virtual bool RayTest_MT(const miVec4& ray_origin, const miVec4& ray_end, const miVec4& v1, const miVec4& v2, const miVec4& v3)const = 0;
};


#endif