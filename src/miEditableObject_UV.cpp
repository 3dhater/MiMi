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
				m_selectedUV.push_back(miPair<miListNode<miPolygon::_vertex_data>*, v2f>(cv, cv->m_data.m_uv));
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
					m_selectedUV.push_back(miPair<miListNode<miPolygon::_vertex_data>*, v2f>(cv, cv->m_data.m_uv));
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
						m_selectedUV.push_back(miPair<miListNode<miPolygon::_vertex_data>*, v2f>(vNode, vNode->m_data.m_uv));
						aabb->add(v3f(vNode->m_data.m_uv.x, 0.f, vNode->m_data.m_uv.y));
						m_isUVSelected = true;
					}
					vNode = ce->m_polygon1->m_verts.find(ce->m_vertex2);
					if (vNode)
					{
						vNode->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
						m_selectedUV.push_back(miPair<miListNode<miPolygon::_vertex_data>*, v2f>(vNode, vNode->m_data.m_uv));
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
						m_selectedUV.push_back(miPair<miListNode<miPolygon::_vertex_data>*, v2f>(vNode, vNode->m_data.m_uv));
						aabb->add(v3f(vNode->m_data.m_uv.x, 0.f, vNode->m_data.m_uv.y));
						m_isUVSelected = true;
					}
					vNode = ce->m_polygon2->m_verts.find(ce->m_vertex2);
					if (vNode)
					{
						vNode->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
						m_selectedUV.push_back(miPair<miListNode<miPolygon::_vertex_data>*, v2f>(vNode, vNode->m_data.m_uv));
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
						m_selectedUV.push_back(miPair<miListNode<miPolygon::_vertex_data>*, v2f>(vNode, vNode->m_data.m_uv));
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
	m_UVScale.set(1.f, 1.f);
}

void miEditableObject::UVSelectAll(miEditMode em, Aabb* aabb) {
	m_selectedUV.clear();

	auto cp = m_mesh->m_first_polygon;
	auto lp = cp->m_left;
	while (true)
	{
		auto cv = cp->m_verts.m_head;
		auto lv = cv->m_left;
		while (true)
		{
			cv->m_data.m_flags |= miPolygon::_vertex_data::flag_isSelected;
			m_selectedUV.push_back(miPair<miListNode<miPolygon::_vertex_data>*, v2f>(cv, cv->m_data.m_uv));
			aabb->add(v3f(cv->m_data.m_uv.x, 0.f, cv->m_data.m_uv.y));
			m_isUVSelected = true;

			if (cv == lv)
				break;
			cv = cv->m_right;
		}

		if (em == miEditMode::Polygon)
			cp->Select();

		if (cp == lp)
			break;
		cp = cp->m_right;
	}
	
	_updateUVSelectionArray(aabb);
	RebuildUVModel();
	RebuildVisualObjects(true);

	m_UVScale.set(1.f, 1.f);
}

void miEditableObject::UVTransformAccept() {
	for (u32 i = 0; i < m_selectedUV.m_size; ++i)
	{
		m_selectedUV.m_data[i].m_second.x = m_selectedUV.m_data[i].m_first->m_data.m_uv.x;
		m_selectedUV.m_data[i].m_second.y = m_selectedUV.m_data[i].m_first->m_data.m_uv.y;
	}
	m_UVScale.set(1.f, 1.f);
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
			auto x = m_selectedUV.m_data[i].m_first->m_data.m_uv.x - g_app->m_UVAabbCenterOnClick.x;
			auto y = m_selectedUV.m_data[i].m_first->m_data.m_uv.y - g_app->m_UVAabbCenterOnClick.z;

			m_selectedUV.m_data[i].m_first->m_data.m_uv.x = x * c - y * s;
			m_selectedUV.m_data[i].m_first->m_data.m_uv.y = x * s + y * c;

			m_selectedUV.m_data[i].m_first->m_data.m_uv.x += g_app->m_UVAabbCenterOnClick.x;
			m_selectedUV.m_data[i].m_first->m_data.m_uv.y += g_app->m_UVAabbCenterOnClick.z;
		}
	}break;
	case miGizmoUVMode::Center:
		for (u32 i = 0; i < m_selectedUV.m_size; ++i)
		{
			m_selectedUV.m_data[i].m_first->m_data.m_uv.x -= g_app->m_UVAabbMoveOffset.x;
			m_selectedUV.m_data[i].m_first->m_data.m_uv.y -= g_app->m_UVAabbMoveOffset.y;
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

		//auto X = mDeltaX * v;
		//auto Y = mDeltaY * v;
		
		m_UVScale.x += mDeltaX * v;
		m_UVScale.y += mDeltaY * v;


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
			//m_selectedUV.m_data[i].m_first->m_data.m_uv.x -= off.x;
			//m_selectedUV.m_data[i].m_first->m_data.m_uv.y -= off.y;

			m_selectedUV.m_data[i].m_first->m_data.m_uv.x = (m_selectedUV.m_data[i].m_second.x - off.x) * m_UVScale.x;
			m_selectedUV.m_data[i].m_first->m_data.m_uv.y = (m_selectedUV.m_data[i].m_second.y - off.y) * m_UVScale.y;

			m_selectedUV.m_data[i].m_first->m_data.m_uv.x += off.x;
			m_selectedUV.m_data[i].m_first->m_data.m_uv.y += off.y;
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
			auto x = m_selectedUV.m_data[i].m_first->m_data.m_uv.x - g_app->m_UVAabbCenterOnClick.x;
			auto y = m_selectedUV.m_data[i].m_first->m_data.m_uv.y - g_app->m_UVAabbCenterOnClick.z;

			m_selectedUV.m_data[i].m_first->m_data.m_uv.x = x * c - y * s;
			m_selectedUV.m_data[i].m_first->m_data.m_uv.y = x * s + y * c;

			m_selectedUV.m_data[i].m_first->m_data.m_uv.x += g_app->m_UVAabbCenterOnClick.x;
			m_selectedUV.m_data[i].m_first->m_data.m_uv.y += g_app->m_UVAabbCenterOnClick.z;
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
			m_selectedUV.m_data[i].m_first->m_data.m_uv.x += X;
			m_selectedUV.m_data[i].m_first->m_data.m_uv.y += Y;
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
	{
		auto cp = m_mesh->m_first_polygon;
		auto lp = cp->m_left;
		while (true)
		{
			if (cp->m_flags & miPolygon::flag_User1)
				cp->m_flags ^= miPolygon::flag_User1;
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}


	// find groups
	struct polygon_group
	{
		miArray<miPolygon*> m_polygons;
		Aabb m_aabbUV;
		Aabb m_aabbUV_new;
	};
	miArray<polygon_group*> groups;
	groups.reserve(0x1000);
	{
	beginGroup:;
		polygon_group* currGroup = 0;
		{
			f32 angleDot = 0.3f;

			auto cp = m_mesh->m_first_polygon;
			auto lp = cp->m_left;
			while (true)
			{
				if ((cp->m_flags & miPolygon::flag_User1) == 0)
				{
					if (cp->IsSelected())
					{
						cp->m_flags |= miPolygon::flag_User1;

						if (!currGroup)
						{
							currGroup = new polygon_group;
							currGroup->m_polygons.reserve(0x1000);
							groups.push_back(currGroup);
						}

						currGroup->m_polygons.push_back(cp);

						u32 index = 0;
					beginPolygon:;
						bool good = false;
						for (u32 sz = currGroup->m_polygons.m_size; index < sz; index++)
						{
							auto p = currGroup->m_polygons.m_data[index];
							auto n1 = p->GetFaceNormalCalculateNew();

							auto cv = p->m_verts.m_head;
							auto lv = cv->m_left;
							while (true)
							{
								auto cvp = cv->m_data.m_vertex->m_polygons.m_head;
								auto lvp = cvp->m_left;
								while (true)
								{
									if ((cvp->m_data->m_flags & miPolygon::flag_User1) == 0)
									{
										if (cvp->m_data->IsSelected())
										{
											auto n2 = cvp->m_data->GetFaceNormalCalculateNew();
											if (n1.dot(n2) > angleDot)
											{
												good = true;

												// check all normals 
												for (u32 i2 = 0; i2 < currGroup->m_polygons.m_size; ++i2)
												{
													auto p2 = currGroup->m_polygons.m_data[i2];
													auto norm = p2->GetFaceNormalCalculateNew();
													if (norm.dot(n2) < angleDot)
													{
														good = false;
														break;
													}
												}

												if (good)
												{
													cvp->m_data->m_flags |= miPolygon::flag_User1;
													currGroup->m_polygons.push_back(cvp->m_data);
												}
											}
										}
									}
									if (cvp == lvp)
										break;
									cvp = cvp->m_right;
								}

								if (cv == lv)
									break;
								cv = cv->m_right;
							}
						}
						if (good)
						{
							goto beginPolygon;
						}
						else
						{
							goto beginGroup;
						}
					}
				}

				if (cp == lp)
					break;
				cp = cp->m_right;
			}
		}
	}

	if (!groups.m_size)
		return;

	{
		//printf("Groups: %u\n", groups.m_size);
		for (u32 i = 0; i < groups.m_size; ++i)
		{
			auto g = groups.m_data[i];

			v3f n;
			for (u32 i = 0; i < g->m_polygons.m_size; ++i)
			{
				auto cp = g->m_polygons.m_data[i];

				n += cp->GetFaceNormalCalculateNew();
			}

			n.normalize2();
			if (n.x == 0.f) n.x = 0.0001f;
			if (n.y == 0.f) n.y = 0.0001f;
			if (n.z == 0.f) n.z = 0.0001f;
			Mat4 V;
			v3f up = v4f(0.f, 1.f, 0.f, 0.f);
			math::makeLookAtRHMatrix(V, v4f(), n, up);

			for (u32 i = 0; i < g->m_polygons.m_size; ++i)
			{
				auto cp = g->m_polygons.m_data[i];

				auto cv = cp->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
				{
					v4f point2 = cv->m_data.m_vertex->m_position;
					point2 = math::mul(point2, V);

					cv->m_data.m_uv.x = point2.x;
					cv->m_data.m_uv.y = -point2.y;

					g->m_aabbUV.add(v3f(cv->m_data.m_uv.x, cv->m_data.m_uv.y, 0.f));
					g->m_aabbUV_new.add(v3f(cv->m_data.m_uv.x, cv->m_data.m_uv.y, 0.f));

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}

		}
	}

	// sort
	struct _pred
	{
		bool operator() (polygon_group* a, polygon_group* b) const
		{
			v4f ae;
			v4f be;
			a->m_aabbUV.extent(ae);
			b->m_aabbUV.extent(be);

			f32 aarea = ae.x * ae.y;
			f32 barea = be.x * be.y;

			return aarea < barea;
		}
	};
	groups.sort_insertion(_pred());

	Aabb aabbAll;
	f32 spacing = 0.02f;

	miList<v2f> points; // where to place this group
	for (u32 i = 0; i < groups.m_size; ++i)
	{
		auto g = groups.m_data[i];
		v2f where;

		bool addNewPoints = true;
		if (i == 0)
		{
			//	g->m_aabbUV_new.m_max -= g->m_aabbUV_new.m_min;
			//	g->m_aabbUV_new.m_min -= g->m_aabbUV_new.m_min;
		}
		else
		{
			// I have points where to place group.
			// point is new m_min (left top corner).
			// I need that point, 
			//  1. that not make aabbAll larger
			//  2. if all points make aabbAll larger
			//     then that point, that make aabbAll more like square

			auto cp = points.m_head;
			if (cp)
			{
				bool found_1 = false;
				bool found_2 = false;  // I need first good result for this
				v2f where_1; // not make larger
				v2f where_2; // make larger
				auto lp = cp->m_left;
				miListNode<v2f>* eraseNode = 0;
				Aabb newAabb;
				while (true)
				{
					newAabb = g->m_aabbUV_new;

					newAabb.m_max -= newAabb.m_min;
					newAabb.m_min -= newAabb.m_min;

					newAabb.m_min.x += cp->m_data.x;
					newAabb.m_min.y += cp->m_data.y;
					newAabb.m_max.x += cp->m_data.x;
					newAabb.m_max.y += cp->m_data.y;
					/*printf("CP: %f %f\n", cp->m_data.x, cp->m_data.y);
					printf("NEW AABB MIN: %f %f\n", newAabb.m_min.x, newAabb.m_min.y);
					printf("NEW AABB MAX: %f %f\n", newAabb.m_max.x, newAabb.m_max.y);
					printf("ALL AABB MIN: %f %f\n", aabbAll.m_min.x, aabbAll.m_min.y);
					printf("ALL AABB MAX: %f %f\n", aabbAll.m_max.x, aabbAll.m_max.y);*/
					if (newAabb.m_max.x > aabbAll.m_max.x
						|| newAabb.m_max.y > aabbAll.m_max.y)
					{
						if (!found_2)
						{
							if (aabbAll.m_max.x > aabbAll.m_max.y)
							{
								if (newAabb.m_max.y >= aabbAll.m_max.y)
								{
									found_2 = true;
									where_2 = cp->m_data;
									eraseNode = cp;
								}
							}
							else
							{
								if (newAabb.m_max.x >= aabbAll.m_max.x)
								{
									found_2 = true;
									where_2 = cp->m_data;
									eraseNode = cp;
								}
							}
						}
					}
					else
					{
						found_1 = true;
						where_1 = cp->m_data;
						eraseNode = cp;
						break;
					}

					if (cp == lp)
						break;
					cp = cp->m_right;
				}

				if (found_1)
				{
					where = where_1;
				}
				else
				{
					where = where_2;
				}
				points.erase_first(where);

				// check if newAabb intersect other AABBs
				f32 intersectionOffset = 0.f;
				for (u32 i2 = 0; i2 < i; ++i2)
				{

					auto g2 = groups.m_data[i2];
					if (
						(newAabb.m_min.x <= g2->m_aabbUV_new.m_max.x && newAabb.m_max.x >= g2->m_aabbUV_new.m_min.x)
						&&
						(newAabb.m_min.y <= g2->m_aabbUV_new.m_max.y && newAabb.m_max.y >= g2->m_aabbUV_new.m_min.y)
						)
					{
						//f32 iox = g2->m_aabbUV_new.m_max.x - newAabb.m_min.x;
						f32 ioy = g2->m_aabbUV_new.m_max.y - newAabb.m_min.y;
						//	printf("Intersect [%f %f] !!!\n", iox,ioy);
						if (ioy > intersectionOffset)
							intersectionOffset = ioy;
					}
				}
				where.y += intersectionOffset;
			}
		}

		g->m_aabbUV_new.m_max -= g->m_aabbUV_new.m_min;
		g->m_aabbUV_new.m_min -= g->m_aabbUV_new.m_min;

		g->m_aabbUV_new.m_min.x += where.x - spacing;
		g->m_aabbUV_new.m_min.y += where.y - spacing;
		g->m_aabbUV_new.m_max.x += where.x + spacing;
		g->m_aabbUV_new.m_max.y += where.y + spacing;
		//printf("WHERE:  %f %f\n", where.x, where.y);
		aabbAll.add(g->m_aabbUV_new);

		if (addNewPoints)
		{
			v2f point1, point2;
			point1.set(g->m_aabbUV_new.m_max.x + spacing, g->m_aabbUV_new.m_min.y);
			point2.set(g->m_aabbUV_new.m_min.x, g->m_aabbUV_new.m_max.y + spacing);
			//	printf("P1:  %f %f\n", point1.x, point1.y);
			//	printf("P2:  %f %f\n\n", point2.x, point2.y);

			points.push_back(point1);
			points.push_back(point2);
		}
	}

	// everything with spacing. I need to move everything. Make only positive numbers
	{
		f32 x = aabbAll.m_min.x;
		f32 y = aabbAll.m_min.y;
		aabbAll.m_min.x -= x;
		aabbAll.m_min.y -= y;
		for (u32 i = 0; i < groups.m_size; ++i)
		{
			auto g = groups.m_data[i];
			if (x < 0.f)
			{
				g->m_aabbUV_new.m_min.x -= x;
				g->m_aabbUV_new.m_max.x -= x;
			}

			if (y < 0.f)
			{
				g->m_aabbUV_new.m_min.y -= y;
				g->m_aabbUV_new.m_max.y -= y;
			}
		}
	}

	for (u32 i = 0; i < groups.m_size; ++i)
	{
		auto g = groups.m_data[i];
		v3f vector = g->m_aabbUV_new.m_min - g->m_aabbUV.m_min;

		for (u32 i2 = 0; i2 < g->m_polygons.m_size; ++i2)
		{
			auto p = g->m_polygons.m_data[i2];
			auto cv = p->m_verts.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				cv->m_data.m_uv.x += vector.x;
				cv->m_data.m_uv.y += vector.y;

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
		}
	}

	bool fit = true;
	if (fit)
	{
		f32 multX = 1.f;
		f32 multY = 1.f;
		if (aabbAll.m_max.x > 0.f)
			multX = 1.f / aabbAll.m_max.x;
		if (aabbAll.m_max.y > 0.f)
			multY = 1.f / aabbAll.m_max.y;

		for (u32 i = 0; i < groups.m_size; ++i)
		{
			auto g = groups.m_data[i];
			v3f vector = g->m_aabbUV_new.m_min - g->m_aabbUV.m_min;

			for (u32 i2 = 0; i2 < g->m_polygons.m_size; ++i2)
			{
				auto p = g->m_polygons.m_data[i2];
				auto cv = p->m_verts.m_head;
				auto lv = cv->m_left;
				while (true)
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

					if (cv == lv)
						break;
					cv = cv->m_right;
				}
			}
		}
	}

	for (u32 i = 0; i < groups.m_size; ++i)
	{
		auto g = groups.m_data[i];
		delete g;
	}
}
