#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "miShortcutManager.h"

extern miApplication * g_app;

void miApplication::OnGizmoUVClick() {
	m_UVAabbOnClick = m_UVAabb;
	m_UVAabb.center(m_UVAabbCenterOnClick);
	m_UVAabbMoveOffset.set(0.f);
	m_UVAngle = 0.f;
}

bool miApplication::IsMouseFocusInUVEditor() {
	if (!m_viewportInMouseFocus)
		return false;
	return m_viewportInMouseFocus->m_cameraType == miViewportCameraType::UV;
}

void miApplication::UVTransformAccept() {
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto obj = m_selectedObjects.m_data[i];
		if (obj->m_isUVSelected)
			obj->UVTransformAccept();
	}
}

void miApplication::UVTransformCancel() {
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto obj = m_selectedObjects.m_data[i];
		if (obj->m_isUVSelected)
		{
			obj->UVTransformCancel(m_gizmoModeUV, m_UVAabb, m_UVAabbCenterOnClick);
			obj->RebuildVisualObjects(false);
		}
	}
	m_UVAabbOnClick = m_UVAabb;
}

void miApplication::UVTransform()
{
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto obj = m_selectedObjects.m_data[i];
		if (obj->m_isUVSelected)
		{
			obj->UVTransform(m_gizmoModeUV, m_keyboardModifier, m_inputContext->m_mouseDelta, m_activeViewportLayout->m_activeViewport->m_activeCamera->m_positionPlatform.w);
			obj->RebuildVisualObjects(false);
		}
	}
}

void miApplication::UvMakePlanar(bool useScreenPlane) {
	m_UVAabb.reset();
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto obj = m_selectedObjects.m_data[i];
		if (obj->m_isUVSelected)
		{
			obj->UVMakePlanar(useScreenPlane);
			obj->UVUpdateAAABB(&m_UVAabb);
			obj->RebuildVisualObjects(false);
		}
	}
	m_UVAabbOnClick = m_UVAabb;
}

void miApplication::UvFlattenMapping() {
	m_UVAabb.reset();
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto obj = m_selectedObjects.m_data[i];
		if (obj->m_isUVSelected)
		{
			obj->UvFlattenMapping();
			obj->UVUpdateAAABB(&m_UVAabb);
			obj->RebuildVisualObjects(false);
		}
	}
	m_UVAabbOnClick = m_UVAabb;
}