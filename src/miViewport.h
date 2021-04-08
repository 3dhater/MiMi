#ifndef _MI_VIEWPORT_H_
#define _MI_VIEWPORT_H_

#include "yy_gui.h"

#include "miViewportCamera.h"

struct miViewport
{
	miViewport();
	~miViewport();

	miViewportCamera* m_cameraPerspective;
	miViewportCamera* m_activeCamera;
};

#endif