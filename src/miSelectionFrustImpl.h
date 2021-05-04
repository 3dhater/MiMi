#ifndef _MI_SELFRUSTIMPL_H_
#define _MI_SELFRUSTIMPL_H_

/*
e   end
2---3
|   |
1---0
o   origin
*/
class miSelectionFrustImpl : public miSelectionFrust
{
public:
	miSelectionFrustImpl();
	virtual ~miSelectionFrustImpl();

	virtual void CreateWithAabb(const miAabb& aabb);
	virtual void CreateWithFrame(const miVec4& frame, const miVec4& vp_rect, const miMatrix& VP_invert);
	virtual bool PointInFrust(const miVec4& v)const;
	virtual bool LineInFrust(const miVec4& p1, const miVec4& p2)const ;
	virtual bool RayTest_MT(const miVec4& ray_origin, const miVec4& ray_end, const miVec4& v1, const miVec4& v2, const miVec4& v3)const;
};


#endif