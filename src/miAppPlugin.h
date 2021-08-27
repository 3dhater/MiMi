#ifndef _MI_APPPLUGIN_H_
#define _MI_APPPLUGIN_H_

class miApplicationPlugin : public miPlugin
{
	miSceneObject* _duplicateEditableObject(miSceneObject*, bool isMirror, miAxis axis);
public:
	miApplicationPlugin();
	virtual ~miApplicationPlugin();
	virtual void Init(miSDK* sdk) override;

	virtual void OnClickGizmo(miKeyboardModifier, miGizmoMode, miEditMode, miSceneObject*) override;
	virtual miSceneObject* OnDuplicate(miSceneObject* object, bool isMirror, miAxis axis) override;

	static u32 m_objectType_editableObject;
};

#endif