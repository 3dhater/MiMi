#include "../SDK/miSDK.h"
#include "miApplication.h"

#include "miGraphics2D.h"

extern miApplication * g_app;

miGraphics2D::miGraphics2D() {
	//m_gdi_bitmap = 0;
	m_gdi = 0;
	m_gdiplusToken = 0;
	m_gdi_solidBrush_rectSelectFill = 0;
	m_gdi_solidBrush_rectSelectBorder = 0;
	m_gdi_pen_rectSelect = 0;
}

miGraphics2D::~miGraphics2D() {
	if (m_gdi)
	{
		//if (m_gdi_bitmap) delete m_gdi_bitmap;
		if (m_gdi_pen_rectSelect) delete m_gdi_pen_rectSelect;
		if (m_gdi_solidBrush_rectSelectFill) delete m_gdi_solidBrush_rectSelectFill;
		if (m_gdi_solidBrush_rectSelectBorder) delete m_gdi_solidBrush_rectSelectBorder;
		//delete m_gdi;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}
}

void miGraphics2D::Init(yyWindow* window) {
	m_window = window;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	
	//m_gdi_bitmap = new Gdiplus::Bitmap((s32)window->m_currentSize.x, (s32)window->m_currentSize.y);

	m_gdi = new Gdiplus::Graphics(window->m_dc);
	//m_gdiBuffer = Gdiplus::Graphics::FromImage(m_gdi_bitmap);

	m_gdi->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighSpeed);
	m_gdi->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighSpeed);
	m_gdi_solidBrush_rectSelectFill = new Gdiplus::SolidBrush(Gdiplus::Color(60, 255, 255, 255));
	m_gdi_solidBrush_rectSelectBorder = new Gdiplus::SolidBrush(Gdiplus::Color(200, 255, 255, 255));
	m_gdi_pen_rectSelect = new Gdiplus::Pen(m_gdi_solidBrush_rectSelectBorder);
	m_gdi_pen_rectSelect->SetWidth(2.f);
	m_gdi_pen_rectSelect->SetDashStyle(Gdiplus::DashStyleDash);
	//m_gdi_pen_rectSelect->Set
}

void miGraphics2D::UpdateClip() {
	auto & vprect = g_app->m_activeViewportLayout->m_activeViewport->m_currentRect;
	
	delete m_gdi;
	m_gdi = new Gdiplus::Graphics(m_window->m_dc);

	//delete m_gdi_bitmap;
	//m_gdi_bitmap = new Gdiplus::Bitmap((s32)m_window->m_currentSize.x, (s32)m_window->m_currentSize.y);
	//m_gdiBuffer = Gdiplus::Graphics::FromImage(m_gdi_bitmap);

	m_gdi->SetClip(Gdiplus::Rect(vprect.x, vprect.y, vprect.z - vprect.x, vprect.w - vprect.y));
}

void miGraphics2D::DrawSelectionBox(const v2f& p1, const v2f& p2) {

	Aabb aabb;
	aabb.add(v3f(p1.x + 1.f, p1.y + 1.f, 0.f));
	aabb.add(v3f(p2.x - 1.f, p2.y - 1.f, 0.f));
	/*m_gdi->FillRectangle(m_gdi_solidBrush_rectSelectFill, aabb.m_min.x, aabb.m_min.y,
		aabb.m_max.x - aabb.m_min.x, aabb.m_max.y - aabb.m_min.y);*/
	
	m_gdi->DrawLine(m_gdi_pen_rectSelect, (s32)p1.x, (s32)p1.y, (s32)p2.x, (s32)p1.y);
	m_gdi->DrawLine(m_gdi_pen_rectSelect, (s32)p1.x, (s32)p1.y, (s32)p1.x, (s32)p2.y);
	m_gdi->DrawLine(m_gdi_pen_rectSelect, (s32)p1.x, (s32)p2.y, (s32)p2.x, (s32)p2.y);
	m_gdi->DrawLine(m_gdi_pen_rectSelect, (s32)p2.x, (s32)p1.y, (s32)p2.x, (s32)p2.y);
}

void miGraphics2D::BeginDraw() {
	//SwapBuffers(m_window->m_dc);
	//m_gdi->Clear(Gdiplus::Color(0, 255, 255, 255));
}

void miGraphics2D::EndDraw() {
	//m_gdi->DrawImage(m_gdi_bitmap, 0, 0, (s32)m_window->m_currentSize.x, (s32)m_window->m_currentSize.y);
	//SwapBuffers(m_window->m_dc);
}