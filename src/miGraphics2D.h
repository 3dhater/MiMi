#ifndef _MI_GRAPHICS2D_H_
#define _MI_GRAPHICS2D_H_

#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

class miGraphics2D 
{
	yyWindow* m_window;
	Gdiplus::Graphics* m_gdi;
	//Gdiplus::Graphics* m_gdiBuffer;
	//Gdiplus::Bitmap* m_gdi_bitmap;
	Gdiplus::SolidBrush* m_gdi_solidBrush_rectSelectFill;
	Gdiplus::SolidBrush* m_gdi_solidBrush_rectSelectBorder;
	Gdiplus::Pen* m_gdi_pen_rectSelect;
	ULONG_PTR          m_gdiplusToken;

public:
	miGraphics2D();
	virtual ~miGraphics2D();

	void Init(yyWindow* window);
	void UpdateClip();

	void DrawSelectionBox(const v2f& p1, const v2f& p2);

	void BeginDraw();
	void EndDraw();
};

#endif