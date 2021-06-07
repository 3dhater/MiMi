#ifndef _MI_GIZMO_H_
#define _MI_GIZMO_H_

class miGizmo
{
public:
	miGizmo();
	~miGizmo();
	
	f32 m_gizmo_arrow_body_size;
	f32 m_gizmo_head_size;
	f32 m_gizmo_head_len;
	f32 m_gizmo_2pl_sz;
	f32 m_gizmo_rot_sz;
	f32 m_gizmo_rot_sz_screen;
	f32 m_gizmo_rot_sz_mn;
	f32 m_gizmo_rot_sz_mx;

	yyMaterial m_commonMaterial;
	yyMaterial m_pivotModelMaterial;

	yyResource* m_pivotModel;
	
	yyResource* m_rotateX;
	yyResource* m_rotateY;
	yyResource* m_rotateZ;
	yyResource* m_rotateScreen;
	yySprite*   m_rotateSprite;

	yyResource* m_X;
	yyResource* m_Y;
	yyResource* m_Z;

	yyResource* m_HeadMoveX;
	yyResource* m_HeadMoveY;
	yyResource* m_HeadMoveZ;

	yyResource* m_HeadScaleX;
	yyResource* m_HeadScaleY;
	yyResource* m_HeadScaleZ;

	Aabb m_XAabb;
	Aabb m_YAabb;
	Aabb m_ZAabb;

	Aabb m_XAabbMod;
	Aabb m_YAabbMod;
	Aabb m_ZAabbMod;


	Aabb m_HeadXAabb;
	Aabb m_HeadYAabb;
	Aabb m_HeadZAabb;

	Aabb m_HeadXAabbMod;
	Aabb m_HeadYAabbMod;
	Aabb m_HeadZAabbMod;

	Aabb m_HeadScaleXAabb;
	Aabb m_HeadScaleYAabb;
	Aabb m_HeadScaleZAabb;

	Aabb m_HeadScaleXAabbMod;
	Aabb m_HeadScaleYAabbMod;
	Aabb m_HeadScaleZAabbMod;

	bool m_isDrawAabbX;
	bool m_isDrawAabbY;
	bool m_isDrawAabbZ;

	bool m_isDrawAabbHeadX;
	bool m_isDrawAabbHeadY;
	bool m_isDrawAabbHeadZ;

	bool m_isDrawAabbScaleHeadX;
	bool m_isDrawAabbScaleHeadY;
	bool m_isDrawAabbScaleHeadZ;

	bool m_isRotationHoverX;
	bool m_isRotationHoverY;
	bool m_isRotationHoverZ;
	bool m_isRotationHoverScreen;

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

	v2f m_2d_point;
	bool m_isIn2d;
	f32 m_size_2d;

	yyColor m_color_x;
	yyColor m_color_y;
	yyColor m_color_z;
};

#endif