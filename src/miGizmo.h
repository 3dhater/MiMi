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

	void Draw(miViewport*);
	bool Update(miViewport*);

	Mat4 m_S;
	Mat4 m_T;
	Mat4 m_W;
	Mat4 m_WVP;

};

#endif