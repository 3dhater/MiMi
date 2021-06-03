#ifndef _MI_GIZMO_H_
#define _MI_GIZMO_H_

class miGizmo
{
public:
	miGizmo();
	~miGizmo();
	
	yyResource* m_pivotModel;
	yyMaterial m_pivotModelMaterial;
	
	yyResource* m_moveModelY;
	yyResource* m_moveModelHeadY;
	yyMaterial m_moveModelYMaterial;
	Aabb m_moveBodyModelYAabb;
	Aabb m_moveBodyModelYAabbMod;
	Aabb m_moveHeadModelYAabb;
	Aabb m_moveHeadModelYAabbMod;
	bool m_isDrawAabbMoveBodyY;
	bool m_isDrawAabbMoveHeadY;

	yyResource* m_moveModelX;
	yyResource* m_moveModelHeadX;
	Aabb m_moveBodyModelXAabb;
	Aabb m_moveBodyModelXAabbMod;
	Aabb m_moveHeadModelXAabb;
	Aabb m_moveHeadModelXAabbMod;
	bool m_isDrawAabbMoveBodyX;
	bool m_isDrawAabbMoveHeadX;

	yyResource* m_moveModelZ;
	yyResource* m_moveModelHeadZ;
	Aabb m_moveBodyModelZAabb;
	Aabb m_moveBodyModelZAabbMod;
	Aabb m_moveHeadModelZAabb;
	Aabb m_moveHeadModelZAabbMod;
	bool m_isDrawAabbMoveBodyZ;
	bool m_isDrawAabbMoveHeadZ;

	void Draw();
	bool Update();

	Mat4 m_S;
	Mat4 m_T;
	Mat4 m_W;
	Mat4 m_WVP;

	// depends on camera platformposition.W and selection aabb center.destance(something)
	// m_zoomScaleValue = platformposition.W; always when reset camera or moveToSelection
	// calculate new when zoom or rotate
	f32 m_zoomScaleValue;
	void SetZoomScaleValue(f32);
	// save distance between camera and sel. aabb center. only on reset or moveToSelection
	f32 m_zoomScaleValueCameraDistance; 
	void ClaculateZoomScaleValue(miViewportCamera*);
};

#endif