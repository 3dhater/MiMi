#ifndef _MI_VIEWPORT_H_
#define _MI_VIEWPORT_H_

#include "yy_gui.h"

#include "miViewportCamera.h"

struct miViewport
{
	miViewport(miViewportCameraType vct);
	~miViewport();

	void Init();

	void OnWindowSize();
	void OnDraw(yyVideoDriverAPI* gpu);

	enum {
		Camera_Perspective = 0,
		Camera_Top,
		Camera_Bottom,
		Camera_Front,
		Camera_Back,
		Camera_Left,
		Camera_Right,
		Camera_count_,
	};

	miViewportCamera* m_camera[Camera_count_];
	miViewportCamera* m_activeCamera;

	miViewportCameraType m_cameraType;

	yyGUIGroup* m_gui_group;
	yyGUIText*  m_gui_text_vpName;
	void HideGUI();
	void ShowGUI();

	s32 m_index;
	v4f m_creationRect;
	v4f m_currentRect;
	v2f m_currentRectSize;
	bool m_isCursorInRect;
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

	void ShowGUI() {
		for (u16 i = 0, sz = m_viewports.size(); i < sz; ++i)
		{
			m_viewports[i]->ShowGUI();
		}
	}

	void Add(const v4f& rect, miViewportCameraType vct) {
		miViewport* newViewport = new miViewport(vct);
		newViewport->m_creationRect = rect;
		newViewport->Init();

		m_activeViewport = newViewport;
		m_viewports.push_back(newViewport);
		newViewport->m_index = m_viewports.size();
		newViewport->HideGUI();
	}
};

#define miViewportLayout_Full 0
#define miViewportLayout_Standart 1
#define miViewportLayout_Count 2

#endif