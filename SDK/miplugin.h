#ifndef _MI_PLUGIN_H_
#define _MI_PLUGIN_H_

class miPlugin;

#define miplCreatePlugin_funcName "miplCreatePlugin"
typedef MI_API miPlugin* (MI_C_DECL*miplCreatePlugin_t)();
//#define miplDestroyPlugin_funcName "miplDestroyPlugin"
//typedef MI_API void (MI_C_DECL*miplDestroyPlugin_t)(miPlugin* plugin);

class miSDK;
class miSelectionFrust;
class miSceneObject;

// base class for all plugins
class miPlugin
{
public:
	miPlugin() {}
	virtual ~miPlugin() {}

	virtual const wchar_t* GetName() = 0;
	virtual const wchar_t* GetDescription() = 0;
	virtual const wchar_t* GetAuthor() = 0;

	// debug plugins must works only in debug app
	virtual bool IsDebug() = 0;

	// I want to have only 1 export function (miplCreatePlugin_t)
	// other functions can get through plugin
	//virtual miplDestroyPlugin_t GetDestroyFunction() = 0;

	// return true always
	virtual bool Init(miSDK* sdk) = 0;
	
	virtual void OnPopupCommand(unsigned int) = 0;
	virtual void OnCursorMove(miSelectionFrust*, bool isCursorInGUI) = 0;
	virtual void OnLMBDown(miSelectionFrust*, bool isCursorInGUI) = 0;
	virtual void OnLMBUp(miSelectionFrust*, bool isCursorInGUI) = 0;
	virtual void OnCancel(miSelectionFrust*, bool isCursorInGUI) = 0;
	virtual void OnUpdate(miSelectionFrust*, bool isCursorInGUI) = 0;
};

#endif