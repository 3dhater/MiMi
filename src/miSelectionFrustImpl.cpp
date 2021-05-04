#include "yy.h"
#include "miApplication.h"
#include "miSDK.h"
#include "miSDKImpl.h"
#include "miSelectionFrustImpl.h"

extern miApplication * g_app;

miSelectionFrustImpl::miSelectionFrustImpl(){}
miSelectionFrustImpl::~miSelectionFrustImpl(){}

void miSelectionFrustImpl::CreateWithAabb(const miAabb& aabb){
	m_top[0].set(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_top[1].set(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_top[2].set(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z, 1.f);
	m_top[3].set(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z, 1.f);

	m_right[0].set(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_right[1].set(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_right[2].set(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z, 1.f);
	m_right[3].set(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z, 1.f);

	m_bottom[0].set(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_bottom[1].set(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_bottom[2].set(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z, 1.f);
	m_bottom[3].set(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z, 1.f);

	m_left[0].set(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_left[1].set(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_left[2].set(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z, 1.f);
	m_left[3].set(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z, 1.f);

	m_front[0].set(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z, 1.f);
	m_front[1].set(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z, 1.f);
	m_front[2].set(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z, 1.f);
	m_front[3].set(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z, 1.f);

	m_back[0].set(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_back[1].set(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z, 1.f);
	m_back[2].set(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z, 1.f);
	m_back[3].set(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z, 1.f);

	miVec4 e1, e2;

	e1 = m_right[1] - m_right[0];
	e2 = m_right[2] - m_right[0];
	e1.cross2(e2, m_RN);
	m_RC = m_right[0] + m_right[1] + m_right[2] + m_right[3];
	m_RC *= 0.25;

	e1 = m_bottom[1] - m_bottom[0];
	e2 = m_bottom[2] - m_bottom[0];
	e1.cross2(e2, m_BN);
	m_BC = m_bottom[0] + m_bottom[1] + m_bottom[2] + m_bottom[3];
	m_BC *= 0.25;

	e1 = m_top[1] - m_top[0];
	e2 = m_top[2] - m_top[0];
	e1.cross2(e2, m_TN);
	m_TC = m_top[0] + m_top[1] + m_top[2] + m_top[3];
	m_TC *= 0.25;

	e1 = m_left[1] - m_left[0];
	e2 = m_left[2] - m_left[0];
	e1.cross2(e2, m_LN);
	m_LC = m_left[0] + m_left[1] + m_left[2] + m_left[3];
	m_LC *= 0.25;

	e1 = m_front[1] - m_front[0];
	e2 = m_front[2] - m_front[0];
	e1.cross2(e2, m_FrontN);
	m_FrontC = m_front[0] + m_front[1] + m_front[2] + m_front[3];
	m_FrontC *= 0.25;

	e1 = m_back[1] - m_back[0];
	e2 = m_back[2] - m_back[0];
	e1.cross2(e2, m_BackN);
	m_BackC = m_back[0] + m_back[1] + m_back[2] + m_back[3];
	m_BackC *= 0.25;
}

void miSelectionFrustImpl::CreateWithFrame(const miVec4& frame, const miVec4& vp_rect, const miMatrix& VP_invert){
	// get 4 rays from screen
	miRay ray1, ray2, ray3, ray4;

	g_app->m_sdk->GetRayFromScreen(&ray1, miVec2(frame.x, frame.y), vp_rect, VP_invert);
	g_app->m_sdk->GetRayFromScreen(&ray2, miVec2(frame.z, frame.y), vp_rect, VP_invert);
	g_app->m_sdk->GetRayFromScreen(&ray3, miVec2(frame.x, frame.w), vp_rect, VP_invert);
	g_app->m_sdk->GetRayFromScreen(&ray4, miVec2(frame.z, frame.w), vp_rect, VP_invert);

	ray1.update();
	ray2.update();
	ray3.update();
	ray4.update();

	m_top[0] = ray1.m_origin;
	m_top[1] = ray2.m_origin;
	m_top[2] = ray1.m_end;
	m_top[3] = ray2.m_end;

	m_right[0] = ray2.m_origin;
	m_right[1] = ray4.m_origin;
	m_right[2] = ray2.m_end;
	m_right[3] = ray4.m_end;

	m_bottom[0] = ray4.m_origin;
	m_bottom[1] = ray3.m_origin;
	m_bottom[2] = ray4.m_end;
	m_bottom[3] = ray3.m_end;

	m_left[0] = ray3.m_origin;
	m_left[1] = ray1.m_origin;
	m_left[2] = ray3.m_end;
	m_left[3] = ray1.m_end;

	miVec4 e1, e2;

	e1 = m_right[1] - m_right[0];
	e2 = m_right[2] - m_right[0];
	e1.cross2(e2, m_RN);
	m_RC = m_right[0] + m_right[1] + m_right[2] + m_right[3];
	m_RC *= 0.25;

	e1 = m_bottom[1] - m_bottom[0];
	e2 = m_bottom[2] - m_bottom[0];
	e1.cross2(e2, m_BN);
	m_BC = m_bottom[0] + m_bottom[1] + m_bottom[2] + m_bottom[3];
	m_BC *= 0.25;

	e1 = m_top[1] - m_top[0];
	e2 = m_top[2] - m_top[0];
	e1.cross2(e2, m_TN);
	m_TC = m_top[0] + m_top[1] + m_top[2] + m_top[3];
	m_TC *= 0.25;

	e1 = m_left[1] - m_left[0];
	e2 = m_left[2] - m_left[0];
	e1.cross2(e2, m_LN);
	m_LC = m_left[0] + m_left[1] + m_left[2] + m_left[3];
	m_LC *= 0.25;
}

bool miSelectionFrustImpl::PointInFrust(const miVec4& v)const{
	if (m_TN.dot(m_TC - v) < 0.f) return false;
	if (m_BN.dot(m_BC - v) < 0.f) return false;
	if (m_RN.dot(m_RC - v) < 0.f) return false;
	if (m_LN.dot(m_LC - v) < 0.f) return false;
	return true;
}

bool miSelectionFrustImpl::LineInFrust(const miVec4& p1, const miVec4& p2)const{
	// обе точки за пределами
	if (m_TN.dot(m_TC - p1) < 0.f && m_TN.dot(m_TC - p2) < 0.f) return false;
	if (m_BN.dot(m_BC - p1) < 0.f && m_BN.dot(m_BC - p2) < 0.f) return false;
	if (m_RN.dot(m_RC - p1) < 0.f && m_RN.dot(m_RC - p2) < 0.f) return false;
	if (m_LN.dot(m_LC - p1) < 0.f && m_LN.dot(m_LC - p2) < 0.f) return false;

	// обе точки внутри
	if (m_TN.dot(m_TC - p1) > 0.f && m_TN.dot(m_TC - p2) > 0.f)
	{
		if (m_RN.dot(m_RC - p1) > 0.f && m_RN.dot(m_RC - p2) > 0.f)
		{
			if (m_BN.dot(m_BC - p1) > 0.f && m_BN.dot(m_BC - p2) > 0.f)
			{
				if (m_LN.dot(m_LC - p1) > 0.f && m_LN.dot(m_LC - p2) > 0.f)
				{
					return true;
				}
			}
		}
	}

	// try ray-triangle
	if (RayTest_MT(p1, p2, m_top[0], m_top[1], m_top[2])) return true;
	if (RayTest_MT(p1, p2, m_top[0], m_top[2], m_top[3])) return true;
	if (RayTest_MT(p1, p2, m_left[0], m_left[1], m_left[2])) return true;
	if (RayTest_MT(p1, p2, m_left[0], m_left[2], m_left[3])) return true;
	if (RayTest_MT(p1, p2, m_right[0], m_right[1], m_right[2])) return true;
	if (RayTest_MT(p1, p2, m_right[0], m_right[2], m_right[3])) return true;
	if (RayTest_MT(p1, p2, m_bottom[0], m_bottom[1], m_bottom[2])) return true;
	if (RayTest_MT(p1, p2, m_bottom[0], m_bottom[2], m_bottom[3])) return true;

	return false;
}

bool miSelectionFrustImpl::RayTest_MT(const miVec4& ray_origin, const miVec4& ray_end, const miVec4& v1, const miVec4& v2, const miVec4& v3)const{
	miVec4 e1 = v2 - v1;
	miVec4 e2 = v3 - v1;
	miVec4 ray_dir = ray_end - ray_origin;
	ray_dir.normalize2();
	ray_dir.w = 1.f;
	miVec4  pvec;
	ray_dir.cross2(e2, pvec);
	float det = e1.dot(pvec);

	if (std::fabs(det) < Epsilon) return false;

	miVec4 tvec(
		ray_origin.x - v1.x,
		ray_origin.y - v1.y,
		ray_origin.z - v1.z,
		0.f);

	f32 inv_det = 1.f / det;
	f32 U = tvec.dot(pvec) * inv_det;

	if (U < 0.f || U > 1.f)
		return false;

	miVec4  qvec;
	tvec.cross2(e1, qvec);
	f32 V = ray_dir.dot(qvec) * inv_det;

	if (V < 0.f || U + V > 1.f)
		return false;

	f32 T = e2.dot(qvec) * inv_det;

	//if( T < Epsilon ) return false;

	return true;
}

