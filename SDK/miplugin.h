#ifndef _MI_PLUGIN_H_
#define _MI_PLUGIN_H_

class miPlugin;

#define miplCreatePlugin_funcName "miplCreatePlugin"
typedef MI_API miPlugin* (MI_C_DECL*miplCreatePlugin_t)();

class miSDK;
class miSelectionFrust;
class miSceneObject;

// base class for all plugins
class miPlugin
{
public:
	miPlugin() {}
	virtual ~miPlugin() {}

	virtual const wchar_t* GetName() { return L"Plugin name"; }
	virtual const wchar_t* GetDescription() { return L"Plugin description"; }
	virtual const wchar_t* GetAuthor() { return L"Plugin author"; }

	// debug plugins must works only in debug app
	virtual bool IsDebug() {
#ifdef MI_DEBUG
		return true;
#else
		return false;
#endif
	}

	virtual void Init(miSDK* sdk) = 0;

	// return MI_SDK_VERSION
	virtual int CheckVersion() {
		return MI_SDK_VERSION;
	}
	
	virtual void OnCreateObject(unsigned int objectId) {}
	virtual void OnCursorMove(miSelectionFrust*, bool isCursorInGUI) {}
	virtual void OnLMBDown(miSelectionFrust*, bool isCursorInGUI) {}
	virtual void OnLMBUp(miSelectionFrust*, bool isCursorInGUI) {}
	virtual void OnCancel(miSelectionFrust*, bool isCursorInGUI) {}
	virtual void OnUpdate(miSelectionFrust*, bool isCursorInGUI) {}

	virtual void OnImportExport(const wchar_t* fileName, unsigned int id) {}
	
	// when hold shift and first click on gizmo
	// will be called on each selected object
	virtual void OnClickGizmo(miKeyboardModifier, miGizmoMode, miEditMode, miSceneObject*) {}

	// return: new object
	virtual miSceneObject* OnDuplicate(miSceneObject* object, bool isMirror, miAxis axis) { return 0; }
};

#endif