#ifndef _MI_GIZMO_H_
#define _MI_GIZMO_H_

class miGizmo
{
public:
	miGizmo();
	~miGizmo();
	
	yyResource* m_pivotModel;

	void Draw();
	bool Update();

	Mat4 m_W;
	Mat4 m_WVP;
};

#endif