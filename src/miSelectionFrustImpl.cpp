#include "yy.h"
#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"
#include "miSelectionFrustImpl.h"

extern miApplication * g_app;

miSelectionFrustImpl::miSelectionFrustImpl(){}
miSelectionFrustImpl::~miSelectionFrustImpl(){}

void miSelectionFrustImpl::CreateWithAabb(const Aabb& aabb){
	m_data.m_top[0].set(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_data.m_top[1].set(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_data.m_top[2].set(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z, 1.f);
	m_data.m_top[3].set(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z, 1.f);

	m_data.m_right[0].set(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_data.m_right[1].set(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_data.m_right[2].set(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z, 1.f);
	m_data.m_right[3].set(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z, 1.f);

	m_data.m_bottom[0].set(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_data.m_bottom[1].set(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_data.m_bottom[2].set(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z, 1.f);
	m_data.m_bottom[3].set(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z, 1.f);

	m_data.m_left[0].set(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_data.m_left[1].set(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_data.m_left[2].set(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z, 1.f);
	m_data.m_left[3].set(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z, 1.f);

	m_data.m_front[0].set(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z, 1.f);
	m_data.m_front[1].set(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z, 1.f);
	m_data.m_front[2].set(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z, 1.f);
	m_data.m_front[3].set(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z, 1.f);

	m_data.m_back[0].set(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_data.m_back[1].set(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_data.m_back[2].set(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_data.m_back[3].set(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z, 1.f);

	v3f e1, e2;

	e1 = m_data.m_right[1] - m_data.m_right[0];
	e2 = m_data.m_right[2] - m_data.m_right[0];
	e1.cross2(e2, m_data.m_RN);
	m_data.m_RC = m_data.m_right[0] + m_data.m_right[1] + m_data.m_right[2] + m_data.m_right[3];
	m_data.m_RC *= 0.25;

	e1 = m_data.m_bottom[1] - m_data.m_bottom[0];
	e2 = m_data.m_bottom[2] - m_data.m_bottom[0];
	e1.cross2(e2, m_data.m_BN);
	m_data.m_BC = m_data.m_bottom[0] + m_data.m_bottom[1] + m_data.m_bottom[2] + m_data.m_bottom[3];
	m_data.m_BC *= 0.25;

	e1 = m_data.m_top[1] - m_data.m_top[0];
	e2 = m_data.m_top[2] - m_data.m_top[0];
	e1.cross2(e2, m_data.m_TN);
	m_data.m_TC = m_data.m_top[0] + m_data.m_top[1] + m_data.m_top[2] + m_data.m_top[3];
	m_data.m_TC *= 0.25;

	e1 = m_data.m_left[1] - m_data.m_left[0];
	e2 = m_data.m_left[2] - m_data.m_left[0];
	e1.cross2(e2, m_data.m_LN);
	m_data.m_LC = m_data.m_left[0] + m_data.m_left[1] + m_data.m_left[2] + m_data.m_left[3];
	m_data.m_LC *= 0.25;

	e1 = m_data.m_front[1] - m_data.m_front[0];
	e2 = m_data.m_front[2] - m_data.m_front[0];
	e1.cross2(e2, m_data.m_FrontN);
	m_data.m_FrontC = m_data.m_front[0] + m_data.m_front[1] + m_data.m_front[2] + m_data.m_front[3];
	m_data.m_FrontC *= 0.25;

	e1 = m_data.m_back[1] - m_data.m_back[0];
	e2 = m_data.m_back[2] - m_data.m_back[0];
	e1.cross2(e2, m_data.m_BackN);
	m_data.m_BackC = m_data.m_back[0] + m_data.m_back[1] + m_data.m_back[2] + m_data.m_back[3];
	m_data.m_BackC *= 0.25;

	m_data.m_RN.normalize2();
	m_data.m_LN.normalize2();
	m_data.m_TN.normalize2();
	m_data.m_BN.normalize2();
	m_data.m_BackN.normalize2();
	m_data.m_FrontN.normalize2();
}

void miSelectionFrustImpl::CreateWithFrame(const v4f& frame, const v4f& vp_rect, const Mat4& VP_invert){
	// get 4 rays from screen
	

	g_app->m_sdk->GetRayFromScreen(&m_data.m_ray1, v2f(frame.x, frame.y), vp_rect, VP_invert);
	g_app->m_sdk->GetRayFromScreen(&m_data.m_ray2, v2f(frame.z, frame.y), vp_rect, VP_invert);
	g_app->m_sdk->GetRayFromScreen(&m_data.m_ray3, v2f(frame.x, frame.w), vp_rect, VP_invert);
	g_app->m_sdk->GetRayFromScreen(&m_data.m_ray4, v2f(frame.z, frame.w), vp_rect, VP_invert);

	m_data.m_ray1.update();
	m_data.m_ray2.update();
	m_data.m_ray3.update();
	m_data.m_ray4.update();

	m_data.m_top[0] = m_data.m_ray1.m_origin;
	m_data.m_top[1] = m_data.m_ray2.m_origin;
	m_data.m_top[2] = m_data.m_ray2.m_end;
	m_data.m_top[3] = m_data.m_ray1.m_end;

	m_data.m_right[0] = m_data.m_ray2.m_origin;
	m_data.m_right[1] = m_data.m_ray4.m_origin;
	m_data.m_right[2] = m_data.m_ray4.m_end;
	m_data.m_right[3] = m_data.m_ray2.m_end;

	m_data.m_bottom[0] = m_data.m_ray4.m_origin;
	m_data.m_bottom[1] = m_data.m_ray3.m_origin;
	m_data.m_bottom[2] = m_data.m_ray3.m_end;
	m_data.m_bottom[3] = m_data.m_ray4.m_end;

	m_data.m_left[0] = m_data.m_ray3.m_origin;
	m_data.m_left[1] = m_data.m_ray1.m_origin;
	m_data.m_left[2] = m_data.m_ray1.m_end;
	m_data.m_left[3] = m_data.m_ray3.m_end;

	m_data.m_front[0] = m_data.m_ray1.m_origin;
	m_data.m_front[1] = m_data.m_ray2.m_origin;
	m_data.m_front[2] = m_data.m_ray3.m_origin;
	m_data.m_front[3] = m_data.m_ray4.m_origin;

	m_data.m_back[0] = m_data.m_ray1.m_end;
	m_data.m_back[1] = m_data.m_ray2.m_end;
	m_data.m_back[2] = m_data.m_ray3.m_end;
	m_data.m_back[3] = m_data.m_ray4.m_end;

	v3f e1, e2;

	e1 = m_data.m_right[1] - m_data.m_right[3];
	e2 = m_data.m_right[2] - m_data.m_right[3];
	e1.cross2(e2, m_data.m_RN);
	m_data.m_RC = m_data.m_right[0] + m_data.m_right[1] + m_data.m_right[2] + m_data.m_right[3];
	m_data.m_RC *= 0.25;

	e1 = m_data.m_bottom[1] - m_data.m_bottom[3];
	e2 = m_data.m_bottom[2] - m_data.m_bottom[3];
	e1.cross2(e2, m_data.m_BN);
	m_data.m_BC = m_data.m_bottom[0] + m_data.m_bottom[1] + m_data.m_bottom[2] + m_data.m_bottom[3];
	m_data.m_BC *= 0.25;

	e1 = m_data.m_top[1] - m_data.m_top[3];
	e2 = m_data.m_top[2] - m_data.m_top[3];
	e1.cross2(e2, m_data.m_TN);
	m_data.m_TC = m_data.m_top[0] + m_data.m_top[1] + m_data.m_top[2] + m_data.m_top[3];
	m_data.m_TC *= 0.25;

	e1 = m_data.m_left[1] - m_data.m_left[3];
	e2 = m_data.m_left[2] - m_data.m_left[3];
	e1.cross2(e2, m_data.m_LN);
	m_data.m_LC = m_data.m_left[0] + m_data.m_left[1] + m_data.m_left[2] + m_data.m_left[3];
	m_data.m_LC *= 0.25;

	e1 = m_data.m_front[1] - m_data.m_front[0];
	e2 = m_data.m_front[2] - m_data.m_front[0];
	e1.cross2(e2, m_data.m_FrontN);
	m_data.m_FrontC = m_data.m_front[0] + m_data.m_front[1] + m_data.m_front[2] + m_data.m_front[3];
	m_data.m_FrontC *= 0.25;

	e1 = m_data.m_back[1] - m_data.m_back[0];
	e2 = m_data.m_back[2] - m_data.m_back[0];
	e1.cross2(e2, m_data.m_BackN);
	m_data.m_BackC = m_data.m_back[0] + m_data.m_back[1] + m_data.m_back[2] + m_data.m_back[3];
	m_data.m_BackC *= 0.25;

	m_data.m_ray5.m_origin = m_data.m_BackC;
	m_data.m_ray5.m_end = m_data.m_FrontC;
	m_data.m_ray5.update();

	m_data.m_RN.normalize2();
	m_data.m_LN.normalize2();
	m_data.m_TN.normalize2();
	m_data.m_BN.normalize2();
	m_data.m_BackN.normalize2();
	m_data.m_FrontN.normalize2();
}

bool miSelectionFrustImpl::PointInFrust(const v4f& v)const{
	if (m_data.m_TN.dot(m_data.m_TC - v) < 0.f) return false;
	if (m_data.m_BN.dot(m_data.m_BC - v) < 0.f) return false;
	if (m_data.m_RN.dot(m_data.m_RC - v) < 0.f) return false;
	if (m_data.m_LN.dot(m_data.m_LC - v) < 0.f) return false;
	return true;
}

bool miSelectionFrustImpl::LineInFrust(const v4f& p1, const v4f& p2)const{
	// обе точки за пределами
	if (m_data.m_TN.dot(m_data.m_TC - p1) < 0.f && m_data.m_TN.dot(m_data.m_TC - p2) < 0.f) return false;
	if (m_data.m_BN.dot(m_data.m_BC - p1) < 0.f && m_data.m_BN.dot(m_data.m_BC - p2) < 0.f) return false;
	if (m_data.m_RN.dot(m_data.m_RC - p1) < 0.f && m_data.m_RN.dot(m_data.m_RC - p2) < 0.f) return false;
	if (m_data.m_LN.dot(m_data.m_LC - p1) < 0.f && m_data.m_LN.dot(m_data.m_LC - p2) < 0.f) return false;

	// обе точки внутри
	if (m_data.m_TN.dot(m_data.m_TC - p1) > 0.f && m_data.m_TN.dot(m_data.m_TC - p2) > 0.f)
	{
		if (m_data.m_RN.dot(m_data.m_RC - p1) > 0.f && m_data.m_RN.dot(m_data.m_RC - p2) > 0.f)
		{
			if (m_data.m_BN.dot(m_data.m_BC - p1) > 0.f && m_data.m_BN.dot(m_data.m_BC - p2) > 0.f)
			{
				if (m_data.m_LN.dot(m_data.m_LC - p1) > 0.f && m_data.m_LN.dot(m_data.m_LC - p2) > 0.f)
				{
					return true;
				}
			}
		}
	}

	// try ray-triangle
	if (RayTest_MT(p1, p2, m_data.m_top[0], m_data.m_top[1], m_data.m_top[2])) return true;
	if (RayTest_MT(p1, p2, m_data.m_top[0], m_data.m_top[2], m_data.m_top[3])) return true;
	if (RayTest_MT(p1, p2, m_data.m_left[0], m_data.m_left[1], m_data.m_left[2])) return true;
	if (RayTest_MT(p1, p2, m_data.m_left[0], m_data.m_left[2], m_data.m_left[3])) return true;
	if (RayTest_MT(p1, p2, m_data.m_right[0], m_data.m_right[1], m_data.m_right[2])) return true;
	if (RayTest_MT(p1, p2, m_data.m_right[0], m_data.m_right[2], m_data.m_right[3])) return true;
	if (RayTest_MT(p1, p2, m_data.m_bottom[0], m_data.m_bottom[1], m_data.m_bottom[2])) return true;
	if (RayTest_MT(p1, p2, m_data.m_bottom[0], m_data.m_bottom[2], m_data.m_bottom[3])) return true;

	return false;
}

bool miSelectionFrustImpl::RayTest_MT(const v4f& ray_origin, const v4f& ray_end, const v4f& v1, const v4f& v2, const v4f& v3)const{
	v4f e1 = v2 - v1;
	v4f e2 = v3 - v1;
	v4f ray_dir = ray_end - ray_origin;
	ray_dir.normalize2();
	ray_dir.w = 1.f;
	v4f  pvec;
	ray_dir.cross2(e2, pvec);
	float det = e1.dot(pvec);

	if (std::fabs(det) < Epsilon) return false;

	v4f tvec(
		ray_origin.x - v1.x,
		ray_origin.y - v1.y,
		ray_origin.z - v1.z,
		0.f);

	f32 inv_det = 1.f / det;
	f32 U = tvec.dot(pvec) * inv_det;

	if (U < 0.f || U > 1.f)
		return false;

	v4f  qvec;
	tvec.cross2(e1, qvec);
	f32 V = ray_dir.dot(qvec) * inv_det;

	if (V < 0.f || U + V > 1.f)
		return false;

	//f32 T = e2.dot(qvec) * inv_det;
	//if( T < Epsilon ) return false;
	return true;
}

