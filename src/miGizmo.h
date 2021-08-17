#ifndef _MI_GIZMO_H_
#define _MI_GIZMO_H_

class miGizmo
{
	void _reset_variables();
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

	yyGPUMesh* m_pivotModel;
	
	yyGPUMesh* m_rotateX;
	yyGPUMesh* m_rotateY;
	yyGPUMesh* m_rotateZ;
	yyGPUMesh* m_rotateScreen;
	yySprite*   m_rotateSprite;

	yyGPUMesh* m_X;
	yyGPUMesh* m_Y;
	yyGPUMesh* m_Z;

	yyGPUMesh* m_HeadMoveX;
	yyGPUMesh* m_HeadMoveY;
	yyGPUMesh* m_HeadMoveZ;

	yyGPUMesh* m_HeadScaleX;
	yyGPUMesh* m_HeadScaleY;
	yyGPUMesh* m_HeadScaleZ;

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

	yyGPUMesh* m_XZ;
	Aabb m_XZAabb;
	Aabb m_XZAabbMod;
	bool m_isDrawAabbXZ;

	yyGPUMesh* m_XY;
	Aabb m_XYAabb;
	Aabb m_XYAabbMod;
	bool m_isDrawAabbXY;

	yyGPUMesh* m_ZY;
	Aabb m_ZYAabb;
	Aabb m_ZYAabbMod;
	bool m_isDrawAabbZY;

	void Draw(miViewport*);
	//bool Update(miViewport*);
	void Update(miViewport*);
	void OnStartFrame();
	void OnEndFrame();

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

	v3f m_moveDelta;
	v3f m_var_move_old;
	v3f m_var_move;
	v3f m_var_move_onEscape; // just for moving verts back to position 
	v3f m_var_scale;
	v3f m_var_scale2;
	v4f m_var_rotate;
	f32 m_var_rotate_snap;
	Mat4 m_rotateScreenMatrix;

	v3f m_position;

	//v4f m_selectionAabbCenterOnClick;
	void OnClick();
	void OnRelease();
	void OnEscape();
};

#endif