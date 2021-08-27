#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "miShortcutManager.h"

#include "strings\string.h"
#include "strings\utils.h"

extern miApplication * g_app;

void miApplication::OnGizmoUVClick() {
	m_UVAabbOnClick = m_UVAabb;
	m_UVAabb.center(m_UVAabbCenterOnClick);
	m_UVAabbMoveOffset.set(0.f);
	m_UVAngle = 0.f;
}

void miApplication::CommandUVSelectAll() {
	m_UVAabb.reset();
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto obj = m_selectedObjects.m_data[i];
		obj->UVSelectAll(m_editMode, &m_UVAabb);
	}
	m_UVAabbOnClick = m_UVAabb;
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

void miApplication::UvSaveUVTemplate() {
	if (m_editorType != miEditorType::UV)
		return;

	if (!m_GUIManager->m_UVSaveTemplateSizeCombo->m_selectedItem)
		return;

	u32 size = m_GUIManager->m_UVSaveTemplateSizeCombo->m_selectedItem->GetID();

	yyImage image;
	image.m_width = size;
	image.m_height = size;
	image.m_pitch = image.m_width * 4;
	image.m_dataSize = image.m_pitch * image.m_height;
	image.m_data = (u8*)yyMemAlloc(image.m_dataSize);

	u8* ptr = image.m_data;
	for (u32 i = 0, sz = image.m_width * image.m_height; i < sz; ++i)
	{
		*ptr = 0; ++ptr;
		*ptr = 0; ++ptr;
		*ptr = 0; ++ptr;
		*ptr = 0; ++ptr;
	}

	auto _drawLine = [](yyImage* img, const v2f& p1, const v2f& p2) {
		f32 _ix = 1.f / img->m_width;
		f32 _iy = 1.f / img->m_height;

		f32 x1 = 0.f; if (p1.x != 0.f) x1 = p1.x / _ix;
		f32 x2 = 0.f; if (p2.x != 0.f) x2 = p2.x / _ix;
		f32 y1 = 0.f; if (p1.y != 0.f) y1 = p1.y / _iy;
		f32 y2 = 0.f; if (p2.y != 0.f) y2 = p2.y / _iy;


		const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
		if (steep)
		{
			std::swap(x1, y1);
			std::swap(x2, y2);
		}

		if (x1 > x2)
		{
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		const float dx = x2 - x1;
		const float dy = fabs(y2 - y1);

		float error = dx / 2.0f;
		const int ystep = (y1 < y2) ? 1 : -1;
		u32 y = (u32)y1;

		const u32 maxX = (u32)x2;

		for (u32 x = (u32)x1; x <= maxX; x++)
		{
			if (steep)
			{
				if (y < img->m_height && y >= 0 && x < img->m_width && x >= 0)
				{
					u32 i = (y * 4) + (x * img->m_pitch);
					img->m_data[i] = 0;
					img->m_data[i + 1] = 255;
					img->m_data[i + 2] = 0;
					img->m_data[i + 3] = 255;
				}
			}
			else
			{
				if (y < img->m_height && y >= 0 && x < img->m_width && x >= 0)
				{
					u32 i = (x * 4) + (y * img->m_pitch);
					img->m_data[i] = 0;
					img->m_data[i + 1] = 255;
					img->m_data[i + 2] = 0;
					img->m_data[i + 3] = 255;
				}
			}

			error -= dy;
			if (error < 0)
			{
				y += ystep;
				error += dx;
			}
		}

	};

	//_drawLine(&image, v2f(0.f, 0.f), v2f(1.f, 1.f));
	for (u32 i = 0; i < m_selectedObjects.m_size; ++i)
	{
		auto o = m_selectedObjects.m_data[i];
		if (o->m_objectType == miSceneObjectType::MeshObject)
		{
			u32 mc = o->GetMeshCount();
			for (u32 i2 = 0; i2 < mc; ++i2)
			{
				auto m = o->GetMesh(i2);
				if (m->m_first_polygon)
				{
					auto cp = m->m_first_polygon;
					auto lp = cp->m_left;
					while (true)
					{
						auto cv = cp->m_verts.m_head;
						auto lv = cv->m_left;
						while (true)
						{
							auto nv = cv->m_right;

							_drawLine(&image, cv->m_data.m_uv, nv->m_data.m_uv);

							if (cv == lv)
								break;
							cv = nv;
						}

						if (cp == lp)
							break;
						cp = cp->m_right;
					}
				}
			}
		}
	}

	auto path = yySaveFileDialog("Save image", "Save", "png");
	if (path)
	{
		yyString e = util::stringGetExtension(*path, false);
		yyStringA ea;
		ea += e.data();

		if (ea != "png")
		{
			path->append(".");
			path->append("png");
		}

		yySaveImage((const wchar_t*)path->data(), &image);
		yyDestroy(path);
	}
}