#ifndef _MI_GIZMO_H_
#define _MI_GIZMO_H_

class miGizmo
{
public:
	miGizmo();
	~miGizmo();
	
	yyResource* m_pivotModel;
	yyMaterial m_pivotModelMaterial;
	
	yyResource* m_X;
	yyResource* m_HeadX;
	Aabb m_XAabb;
	Aabb m_XAabbMod;
	Aabb m_HeadXAabb;
	Aabb m_HeadXAabbMod;
	bool m_isDrawAabbX;
	bool m_isDrawAabbHeadX;

	yyResource* m_Y;
	yyResource* m_HeadY;
	yyMaterial m_YMaterial;
	Aabb m_YAabb;
	Aabb m_YAabbMod;
	Aabb m_HeadYAabb;
	Aabb m_HeadYAabbMod;
	bool m_isDrawAabbY;
	bool m_isDrawAabbHeadY;

	yyResource* m_Z;
	yyResource* m_HeadZ;
	Aabb m_ZAabb;
	Aabb m_ZAabbMod;
	Aabb m_HeadZAabb;
	Aabb m_HeadZAabbMod;
	bool m_isDrawAabbZ;
	bool m_isDrawAabbHeadZ;

	yyResource* m_XZ;
	Aabb m_XZAabb;
	Aabb m_XZAabbMod;
	bool m_isDrawAabbXZ;

	yyResource* m_XY;
	Aabb m_XYAabb;
	Aabb m_XYAabbMod;
	bool m_isDrawAabbXY;

	yyResource* m_ZY;
	Aabb m_ZYAabb;
	Aabb m_ZYAabbMod;
	bool m_isDrawAabbZY;

	void Draw(miViewport*);
	bool Update(miViewport*);

	Mat4 m_S;
	Mat4 m_T;
	Mat4 m_W;
	Mat4 m_WVP;

};

#endif