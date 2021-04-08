#include "miApplication.h"
#include "miViewport.h"
#include "miViewportCamera.h"

extern miApplication * g_app;

miViewport::miViewport(){
	m_cameraPerspective = new miViewportCamera;
	m_activeCamera = m_cameraPerspective;
}

miViewport::~miViewport(){
	if (m_cameraPerspective) delete m_cameraPerspective;
}
