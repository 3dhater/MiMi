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

	virtual void CreateWithAabb(const Aabb& aabb);
	virtual void CreateWithFrame(const v4f& frame, const v4f& vp_rect, const Mat4& VP_invert);
	virtual bool PointInFrust(const v4f& v)const;
	virtual bool LineInFrust(const v4f& p1, const v4f& p2)const ;
	virtual bool RayTest_MT(const v4f& ray_origin, const v4f& ray_end, const v4f& v1, const v4f& v2, const v4f& v3)const;
};


#endif