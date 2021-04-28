#ifndef _MI_PLUGIN_H_
#define _MI_PLUGIN_H_

#define miplCreatePlugin_funcName "miplCreatePlugin"
#define miplDestroyPlugin_funcName "miplDestroyPlugin"
typedef MI_API miPlugin* (MI_C_DECL*miplCreatePlugin_t)();
typedef MI_API void (MI_C_DECL*miplDestroyPlugin_t)(miPlugin* plugin);

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

	virtual miplDestroyPlugin_t GetDestroyFunction() = 0;

	// return true always
	virtual bool Init() = 0;
};

#endif