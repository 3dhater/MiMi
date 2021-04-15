#ifndef _MI_VIEWPORT_H_
#define _MI_VIEWPORT_H_

#include "yy_gui.h"

#include "miViewportCamera.h"

struct miViewport
{
	miViewport();
	~miViewport();

	void OnWindowSize();
	void OnDraw(yyVideoDriverAPI* gpu);

	miViewportCamera* m_cameraPerspective;
	miViewportCamera* m_activeCamera;

	v4f m_creationRect;
	v4f m_currentRect;
	v4f m_currentRectInPixels;
	v2f m_currentRectSize;

};

struct miViewportLayout
{
	miViewportLayout() {}
	~miViewportLayout() {
		for (u16 i = 0, sz = m_viewports.size(); i < sz; ++i)
		{
			delete m_viewports[i];
		}
	}
	
	yyArraySmall<miViewport*> m_viewports;
	miViewport* m_activeViewport;

	void Add(const v4f& rect) {
		miViewport* newViewport = new miViewport;
		newViewport->m_creationRect = rect;
		m_activeViewport = newViewport;
		m_viewports.push_back(newViewport);
	}
};

#define miViewportLayout_Full 0
#define miViewportLayout_Standart 1
#define miViewportLayout_Count 2

#endif