#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miViewportCamera.h"
#include "miPluginGUIImpl.h"
#include "miSDKImpl.h"
#include "miEditableObject.h"
#include "miVisualObjectImpl.h"

extern miApplication * g_app;

void miEditableObject::OnDrawUV() {
	if (m_visualObject_vertex) m_visualObject_vertex->Draw(true);
	if (m_visualObject_edge) m_visualObject_edge->Draw(true);
	if (m_visualObject_polygon) m_visualObject_polygon->Draw(true);
}

void miEditableObject::_updateUVSelectionArray(Aabb* aabb) {
	m_selectedUV.clear();
	m_isUVSelected = false;
	/*{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
				{
					cv->m_data.m_flags ^= miPolygon::_vertex_data::flag_isSelected;
				}
				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}*/

	auto cp = m_mesh->m_first_polygon;
	auto lp = cp->m_left;
	while (true)
	{
		auto cv = cp->m_verts.m_head;
		auto lv = cv->m_left;
		while (true)
		{
			if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
			{
				m_selectedUV.push_back(cv);
				aabb->add(v3f(cv->m_data.m_uv.x, 0.f, cv->m_data.m_uv.y));
				m_isUVSelected = true;
			}

			if (cv == lv)
				break;
			cv = cv->m_right;
		}

		if (cp == lp)
			break;
		cp = cp->m_right;
	}
}

void miEditableObject::UpdateUVSelection(miEditMode em, Aabb* aabb) {
	if (g_app->m_editorType != miEditorType::UV)
		return;

	m_isUVSelected = false;
	m_selectedUV.clear();

	if (!m_mesh->m_first_polygon)
		return;

	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
				{
					cv->m_data.m_flags ^= miPolygon::_vertex_data::flag_isSelected;
				}
				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}

	if (em == miEditMode::Polygon)
	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			if (cp->IsSelected())
			{
				auto cv = cp->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					cv->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
					m_selectedUV.push_back(cv);
					aabb->add(v3f(cv->m_data.m_uv.x,0.f, cv->m_data.m_uv.y));
					m_isUVSelected = true;

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}

			if (cp == lp)
				break;
			cp = cp->m_right;
		}

//		this->RebuildUVModel();
	}
	else if (em == miEditMode::Edge)
	{
		auto ce = m_mesh->m_first_edge;
		auto le = ce->m_left;
		while (true)
		{
			if (ce->IsSelected())
			{
				if (ce->m_polygon1)
				{
					auto vNode = ce->m_polygon1->m_verts.find(ce->m_vertex1);
					if (vNode)
					{
						vNode->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
						m_selectedUV.push_back(vNode);
						aabb->add(v3f(vNode->m_data.m_uv.x, 0.f, vNode->m_data.m_uv.y));
						m_isUVSelected = true;
					}
					vNode = ce->m_polygon1->m_verts.find(ce->m_vertex2);
					if (vNode)
					{
						vNode->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
						m_selectedUV.push_back(vNode);
						aabb->add(v3f(vNode->m_data.m_uv.x, 0.f, vNode->m_data.m_uv.y));
						m_isUVSelected = true;
					}
				}

				if (ce->m_polygon2)
				{
					auto vNode = ce->m_polygon2->m_verts.find(ce->m_vertex1);
					if (vNode)
					{
						vNode->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
						m_selectedUV.push_back(vNode);
						aabb->add(v3f(vNode->m_data.m_uv.x, 0.f, vNode->m_data.m_uv.y));
						m_isUVSelected = true;
					}
					vNode = ce->m_polygon2->m_verts.find(ce->m_vertex2);
					if (vNode)
					{
						vNode->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
						m_selectedUV.push_back(vNode);
						aabb->add(v3f(vNode->m_data.m_uv.x, 0.f, vNode->m_data.m_uv.y));
						m_isUVSelected = true;
					}
				}
			}

			if (ce == le)
				break;
			ce = ce->m_right;
		}
	}
	else if (em == miEditMode::Vertex)
	{
		auto cv = m_mesh->m_first_vertex;
		auto lv = cv->m_left;
		while (true)
		{
			if (cv->IsSelected())
			{
				auto cp = cv->m_polygons.m_head;
				auto lp = cp->m_left;
				while (true)
				{
					auto vNode = cp->m_data->m_verts.find(cv);
					if (vNode)
					{
						vNode->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
						m_selectedUV.push_back(vNode);
						aabb->add(v3f(vNode->m_data.m_uv.x, 0.f, vNode->m_data.m_uv.y));
						m_isUVSelected = true;
					}

					if (cp == lp)
						break;
					cp = cp->m_right;
				}
			}

			if (cv == lv)
				break;
			cv = cv->m_right;
		}
	}
}

void miEditableObject::UVTransformCancel(miGizmoUVMode gm, const Aabb& currAabb, const v4f& aabbCenterOnClick) {
	switch (gm)
	{
	case miGizmoUVMode::NoTransform:
		break;
	case miGizmoUVMode::LeftTop:
	case miGizmoUVMode::RightTop:
	case miGizmoUVMode::LeftBottom:
	case miGizmoUVMode::RightBottom:
		break;
	case miGizmoUVMode::Top:
	case miGizmoUVMode::Left:
	case miGizmoUVMode::Right:
	case miGizmoUVMode::Bottom: {
		f32 a = -g_app->m_UVAngle;
		auto s = std::sin(a);
		auto c = std::cos(a);
		for (u32 i = 0; i < m_selectedUV.m_size; ++i)
		{
			auto x = m_selectedUV.m_data[i]->m_data.m_uv.x - g_app->m_UVAabbCenterOnClick.x;
			auto y = m_selectedUV.m_data[i]->m_data.m_uv.y - g_app->m_UVAabbCenterOnClick.z;

			m_selectedUV.m_data[i]->m_data.m_uv.x = x * c - y * s;
			m_selectedUV.m_data[i]->m_data.m_uv.y = x * s + y * c;

			m_selectedUV.m_data[i]->m_data.m_uv.x += g_app->m_UVAabbCenterOnClick.x;
			m_selectedUV.m_data[i]->m_data.m_uv.y += g_app->m_UVAabbCenterOnClick.z;
		}
	}break;
	case miGizmoUVMode::Center:
		for (u32 i = 0; i < m_selectedUV.m_size; ++i)
		{
			m_selectedUV.m_data[i]->m_data.m_uv.x -= g_app->m_UVAabbMoveOffset.x;
			m_selectedUV.m_data[i]->m_data.m_uv.y -= g_app->m_UVAabbMoveOffset.y;
		}
		break;
	default:
		break;
	}
}

void miEditableObject::UVTransform(miGizmoUVMode gm, miKeyboardModifier km, const v2f& md, f32 w) {
	switch (gm)
	{
	case miGizmoUVMode::NoTransform:
		break;
	case miGizmoUVMode::LeftTop:
	case miGizmoUVMode::RightTop:
	case miGizmoUVMode::LeftBottom:
	case miGizmoUVMode::RightBottom:{
		f32 v = 0.001f * w;
		if (km == miKeyboardModifier::Alt)
			v *= 0.01f;

		f32 mDeltaX = md.x;
		f32 mDeltaY = md.y;

		if (gm == miGizmoUVMode::LeftBottom)
		{
			mDeltaX = -md.x;
		}
		else if (gm == miGizmoUVMode::RightTop)
		{
			mDeltaY = -md.y;
		}
		else if (gm == miGizmoUVMode::LeftTop)
		{
			mDeltaX = -md.x;
			mDeltaY = -md.y;
		}

		auto X = mDeltaX * v;
		auto Y = mDeltaY * v;

		v2f off;
		if (gm == miGizmoUVMode::RightBottom)
		{
			off.x = g_app->m_UVAabbOnClick.m_min.x;
			off.y = g_app->m_UVAabbOnClick.m_min.z;
		}
		else if (gm == miGizmoUVMode::LeftBottom)
		{
			off.x = g_app->m_UVAabbOnClick.m_max.x;
			off.y = g_app->m_UVAabbOnClick.m_min.z;
		}
		else if (gm == miGizmoUVMode::RightTop)
		{
			off.x = g_app->m_UVAabbOnClick.m_min.x;
			off.y = g_app->m_UVAabbOnClick.m_max.z;
		}
		else if (gm == miGizmoUVMode::LeftTop)
		{
			off.x = g_app->m_UVAabbOnClick.m_max.x;
			off.y = g_app->m_UVAabbOnClick.m_max.z;
		}

		for (u32 i = 0; i < m_selectedUV.m_size; ++i)
		{
			m_selectedUV.m_data[i]->m_data.m_uv.x -= off.x;
			m_selectedUV.m_data[i]->m_data.m_uv.y -= off.y;

			m_selectedUV.m_data[i]->m_data.m_uv.x *= 1.0f + X;
			m_selectedUV.m_data[i]->m_data.m_uv.y *= 1.0f + Y;

			m_selectedUV.m_data[i]->m_data.m_uv.x += off.x;
			m_selectedUV.m_data[i]->m_data.m_uv.y += off.y;
		}
		//g_app->m_UVAabbMoveOffset.x += X;
		//g_app->m_UVAabbMoveOffset.y += Y;
		g_app->m_UVAabb.reset();
		this->UVUpdateAAABB(&g_app->m_UVAabb);
	}break;
	case miGizmoUVMode::Top:
	case miGizmoUVMode::Left:
	case miGizmoUVMode::Right:
	case miGizmoUVMode::Bottom: {
		f32 v = 0.001f * w;
		if (km == miKeyboardModifier::Alt)
			v *= 0.01f;
		auto a = md.x * v;

		auto s = std::sin(a);
		auto c = std::cos(a);
		//printf("a:%f s:%f  c:%f\n", a,  s, c);

		for (u32 i = 0; i < m_selectedUV.m_size; ++i)
		{
			auto x = m_selectedUV.m_data[i]->m_data.m_uv.x - g_app->m_UVAabbCenterOnClick.x;
			auto y = m_selectedUV.m_data[i]->m_data.m_uv.y - g_app->m_UVAabbCenterOnClick.z;

			m_selectedUV.m_data[i]->m_data.m_uv.x = x * c - y * s;
			m_selectedUV.m_data[i]->m_data.m_uv.y = x * s + y * c;

			m_selectedUV.m_data[i]->m_data.m_uv.x += g_app->m_UVAabbCenterOnClick.x;
			m_selectedUV.m_data[i]->m_data.m_uv.y += g_app->m_UVAabbCenterOnClick.z;
		}

		g_app->m_UVAngle += a;
	}break;
	case miGizmoUVMode::Center:
	{
		f32 v = 0.001f * w;
		if (km == miKeyboardModifier::Alt)
			v *= 0.01f;
		auto X = md.x * v;
		auto Y = md.y * v;
		for (u32 i = 0; i < m_selectedUV.m_size; ++i)
		{
			m_selectedUV.m_data[i]->m_data.m_uv.x += X;
			m_selectedUV.m_data[i]->m_data.m_uv.y += Y;
		}
		g_app->m_UVAabbMoveOffset.x += X;
		g_app->m_UVAabbMoveOffset.y += Y;
	}break;
	default:
		break;
	}
}

void miEditableObject::UVSelect(miSelectionFrust* sf, miKeyboardModifier km, miEditMode em, Aabb* aabb) {
	//m_isUVSelected = false;

	

	if (km != miKeyboardModifier::Alt && km != miKeyboardModifier::Ctrl)
	{
		if(g_app->m_editMode != miEditMode::Object )
			DeselectAll(g_app->m_editMode);

		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
					cv->m_data.m_flags ^= miPolygon::_vertex_data::flag_isSelected;
				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}

	
	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{

				if (sf->PointInFrust(v4f(cv->m_data.m_uv.x, 0.f, cv->m_data.m_uv.y, 0.f)))
				{
					if (km == miKeyboardModifier::Alt)
					{
					//	printf("desel\n");
						if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
							cv->m_data.m_flags ^= miPolygon::_vertex_data::flag_isSelected;
					}
					else
					{
						cv->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
	//					m_selectedUV.push_back(cv);
	//					m_isUVSelected = true;
						aabb->add(v3f(cv->m_data.m_uv.x, 0.f, cv->m_data.m_uv.y));
					}
				}

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}

	if (em == miEditMode::Polygon)
	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto vertex_1 = cp->m_verts.m_head;
			auto vertex_3 = vertex_1->m_right;
			auto vertex_2 = vertex_3->m_right;
			while (true) {
				if (sf->LineInFrust(v4f(vertex_1->m_data.m_uv.x, 0.f, vertex_1->m_data.m_uv.y, 0.f), v4f(vertex_2->m_data.m_uv.x, 0.f, vertex_2->m_data.m_uv.y, 0.f)))
				{
					if (km == miKeyboardModifier::Alt)
					{
						cp->Deselect();
						cp->DeselectUVs();
					}
					else
					{
						cp->Select();
						cp->SelectUVs();
					}
					break;
				}
				if (sf->LineInFrust(v4f(vertex_1->m_data.m_uv.x, 0.f, vertex_1->m_data.m_uv.y, 0.f), v4f(vertex_3->m_data.m_uv.x, 0.f, vertex_3->m_data.m_uv.y, 0.f)))
				{
					if (km == miKeyboardModifier::Alt)
					{
						cp->Deselect();
						cp->DeselectUVs();
					}
					else
					{
						cp->Select();
						cp->SelectUVs();
					}
					break;
				}
				if (sf->LineInFrust(v4f(vertex_2->m_data.m_uv.x, 0.f, vertex_2->m_data.m_uv.y, 0.f), v4f(vertex_3->m_data.m_uv.x, 0.f, vertex_3->m_data.m_uv.y, 0.f)))
				{
					if (km == miKeyboardModifier::Alt)
					{
						cp->Deselect();
						cp->DeselectUVs();
					}
					else
					{
						cp->Select();
						cp->SelectUVs();
					}
					break;
				}

				miTriangle tri;
				tri.v1.set(vertex_1->m_data.m_uv.x, 0.f, vertex_1->m_data.m_uv.y, 0.f);
				tri.v2.set(vertex_2->m_data.m_uv.x, 0.f, vertex_2->m_data.m_uv.y, 0.f);
				tri.v3.set(vertex_3->m_data.m_uv.x, 0.f, vertex_3->m_data.m_uv.y, 0.f);
				tri.update();

				f32 T, U, V, W;
				T = U = V = W = 0.f;
				if (tri.rayTest_MT(sf->m_data.m_ray1, true, T, U, V, W))
				{
					if (km == miKeyboardModifier::Alt)
					{
						cp->Deselect();
						cp->DeselectUVs();
					}
					else
					{
						cp->Select();
						cp->SelectUVs();
					}
					break;
				}
				if (tri.rayTest_MT(sf->m_data.m_ray2, true, T, U, V, W))
				{
					if (km == miKeyboardModifier::Alt)
					{
						cp->Deselect();
						cp->DeselectUVs();
					}
					else
					{
						cp->Select();
						cp->SelectUVs();
					}
					break;
				}
				if (tri.rayTest_MT(sf->m_data.m_ray3, true, T, U, V, W))
				{
					if (km == miKeyboardModifier::Alt)
					{
						cp->Deselect();
						cp->DeselectUVs();
					}
					else
					{
						cp->Select();
						cp->SelectUVs();
					}
					break;
				}
				if (tri.rayTest_MT(sf->m_data.m_ray4, true, T, U, V, W))
				{
					if (km == miKeyboardModifier::Alt)
					{
						cp->Deselect();
						cp->DeselectUVs();
					}
					else
					{
						cp->Select();
						cp->SelectUVs();
					}
					break;
				}
				if (tri.rayTest_MT(sf->m_data.m_ray5, true, T, U, V, W))
				{
					if (km == miKeyboardModifier::Alt)
					{
						cp->Deselect();
						cp->DeselectUVs();
					}
					else
					{
						cp->Select();
						cp->SelectUVs();
					}
					break;
				}

				vertex_2 = vertex_2->m_right;
				vertex_3 = vertex_3->m_right;
				if (vertex_2 == vertex_1)
					break;
			}

			if (cp == lp)
				break;
			cp = cp->m_right;
		}

		OnSelect(em);
		g_app->UpdateSelectionAabb();
	}
	_updateUVSelectionArray(aabb);
	RebuildVisualObjects(true);
	RebuildUVModel();
}

void miEditableObject::RebuildUVModel() {
	auto voc = GetVisualObjectCount();
	for (int i = 0; i < voc; ++i)
	{
		auto vo = (miVisualObjectImpl*)GetVisualObject(i);
		vo->CreateNewGPUModelsUV(m_mesh);
	}
}

void miEditableObject::UVUpdateAAABB(Aabb* aabb) {
	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if(cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
					aabb->add(v3f(cv->m_data.m_uv.x, 0.f, cv->m_data.m_uv.y));

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}
}

void miEditableObject::UVMakePlanar(bool useScreenPlane) {
	Mat4 VP;
	Aabb aabb3d;
	{
		v3f n;
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			bool good = false;

			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
				{
					good = true;
					aabb3d.add(cv->m_data.m_vertex->m_position);
				}
				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			
			if(good)
				n += cp->GetFaceNormalCalculateNew();

			if (cp == lp)
				break;
			cp = cp->m_right;
		}
		n.normalize2();
		if (n.x == 0.f) n.x = 0.0001f;
		if (n.y == 0.f) n.y = 0.0001f;
		if (n.z == 0.f) n.z = 0.0001f;

		if (useScreenPlane)
		{
			Mat4 V = g_app->m_activeViewportLayout->m_viewports[1]->m_activeCamera->m_viewMatrix;
			Mat4 P = g_app->m_activeViewportLayout->m_viewports[1]->m_activeCamera->m_projectionMatrix;
			VP = P * V;
		}
		else
		{
			Mat4 V;
			Mat4 P;
			math::makeLookAtRHMatrix(V, n, v4f(), v4f(0.f, 1.f, 0.f, 0.f));
			math::makeOrthoRHMatrix(P, 100.f, 100.f, -1000.f, 1000.f);
			VP = P * V;
		}
	}

	v4f center3d;
	aabb3d.center(center3d);
	Aabb aabb2d;
	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
				{
					v4f point = cv->m_data.m_vertex->m_position - center3d;
					point.w = 1.f;
					point = math::mul(point, VP);
					
					cv->m_data.m_uv.x = point.x / point.w;
					cv->m_data.m_uv.y = -point.y / point.w;

					aabb2d.add(v3f(cv->m_data.m_uv.x, cv->m_data.m_uv.y, 0.f));
				}

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}

	// move uv
	// min.x to 0.f
	{
		f32 minX = aabb2d.m_min.x;
		f32 minY = aabb2d.m_min.y;
		//printf("minX: %f\n", minX);
		//printf("minY: %f\n", minY);
		aabb2d.reset();
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
				{
					cv->m_data.m_uv.x -= minX;
					cv->m_data.m_uv.y -= minY;
					aabb2d.add(v3f(cv->m_data.m_uv.x, cv->m_data.m_uv.y, 0.f));
				}

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
		
	}

	// now fit
	f32 multX = 1.f;
	f32 multY = 1.f;
	if (aabb2d.m_max.x > 0.f)
		multX = 1.f / aabb2d.m_max.x;
	if (aabb2d.m_max.y > 0.f)
		multY = 1.f / aabb2d.m_max.y;

	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
				{
					if (multX < multY)
					{
						cv->m_data.m_uv.x *= multX;
						cv->m_data.m_uv.y *= multX;
					}
					else
					{
						cv->m_data.m_uv.x *= multY;
						cv->m_data.m_uv.y *= multY;
					}
				}

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}
}

void miEditableObject::UvFlattenMapping() {
	//Mat4 VP;
	Mat4 V;
	{
		v3f n;
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			bool good = false;

			Aabb aabb3d;
			auto cv = cp->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
				{
					//point += cv->m_data.m_normal;
					aabb3d.add(cv->m_data.m_vertex->m_position);
					good = true;
				}
				if (cv == lv)
					break;
				cv = cv->m_right;
			}

			if (good)
			{
				v4f center3d;
				aabb3d.center(center3d);
				n = cp->GetFaceNormalCalculateNew();
				n.normalize2();
				if (n.x == 0.f) n.x = 0.0001f;
				if (n.y == 0.f) n.y = 0.0001f;
				if (n.z == 0.f) n.z = 0.0001f;

				math::makeLookAtRHMatrix(V, v4f(), n, v4f(0.f, 1.f, 0.f, 0.f));

				cv = cp->m_verts.m_head;
				lv = cv->m_left;
				while (true)
				{
					if (cv->m_data.m_flags & miPolygon::_vertex_data::flag_isSelected)
					{
						v4f point2 = cv->m_data.m_vertex->m_position;
						point2 = math::mul(point2 - center3d, V) + center3d;

						cv->m_data.m_uv.x = point2.x;
						cv->m_data.m_uv.y = point2.y;




					}
					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}

			if (cp == lp)
				break;
			cp = cp->m_right;
		}

	}
}
