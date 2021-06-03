#ifndef _MI_VIEWPORT_H_
#define _MI_VIEWPORT_H_

#include "yy_gui.h"

#include "miViewportCamera.h"

struct miViewport
{
	miViewport(miViewportCameraType vct, const v4f& rect1_0);
	~miViewport();

	void Init();
	void Copy(miViewport*);

	void OnWindowSize();
	void OnDraw();
	yyVideoDriverAPI* m_gpu;

	bool m_isOnLeftBorder;
	bool m_isOnRightBorder;
	bool m_isOnTopBorder;
	bool m_isOnBottomBorder;

	miTriangle m_rayTestTiangles[2];

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
	void SetCameraType(miViewportCameraType);
	void SetViewportName(const wchar_t*);

	void UpdateAspect();

	void _frustum_cull(miSceneObject*);
	yyArraySimple<miSceneObject*> m_visibleObjects;

	yyGUIGroup* m_gui_group;
	yyGUIText*  m_gui_text_vpName;
	yyGUIButton* m_gui_button_resetCamera;
	//yyGUIButton* m_gui_button_viewport;
	void HideGUI();
	void ShowGUI();

	bool m_isDrawAabbs;

	s32 m_index;
	v4f m_creationRect; // 0;1
	v4f m_currentRect;
	v2f m_currentRectSize;
	bool m_isCursorInRect;

	enum DrawMode{
		Draw_Material,
		Draw_Wireframe,
		Draw_MaterialWireframe
	}m_drawMode;
	void SetDrawMode(DrawMode);


	bool m_drawGrid;
	void SetDrawGrid(bool);
	void _drawGrid();
	void _drawScene();
	void _drawSelectedObjectFrame();

	void ToggleDrawModeMaterial();
	void ToggleDrawModeWireframe();

	v4f GetCursorRayHitPosition(const v2f& cursorPosition);
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
	void HideGUI() {
		for (u16 i = 0, sz = m_viewports.size(); i < sz; ++i)
		{
			m_viewports[i]->HideGUI();
		}
	}

	void Add(const v4f& rect, miViewportCameraType vct) {
		miViewport* newViewport = new miViewport(vct, rect);
		//newViewport->m_creationRect = rect;
		//newViewport->Init();

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