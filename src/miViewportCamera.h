#ifndef _MI_VIEWPORTCAMERA_H_
#define _MI_VIEWPORTCAMERA_H_

enum class miViewportCameraType
{
	Perspective,
	Left,
	Right,
	Top,
	Bottom,
	Front,
	Back
};

struct miViewportCamera
{
	miViewportCamera();
	~miViewportCamera();
	
	miViewportCameraType m_type;

	// calculate matrices
	void Update();
	Mat4 m_viewMatrix;
	Mat4 m_projectionMatrix;

	f32 m_near;
	f32 m_far;
	f32 m_fov;
	f32 m_aspect;

	v3f m_rotationPlatform;
	v4f m_positionPlatform; // w = height, zoom, mouse wheel value
	v3f m_positionCamera; // in world

	void Reset();

	v4f m_creationRect;

	void PanMove();
	void Rotate();
	void RotateZ();
	void Zoom();
	void ChangeFOV();
};

#endif