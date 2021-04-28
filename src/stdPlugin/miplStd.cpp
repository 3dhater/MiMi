#include "miSDK.h"

#include "miplStd.h"

static bool g_isCreated = false;

extern "C" {
	MI_API miPlugin* MI_C_DECL miplCreatePlugin() {
		if (g_isCreated)
			return 0;
		g_isCreated = true;
		return new miplStd;
	}
}
MI_API void MI_C_DECL miplDestroyPlugin(miPlugin* plugin) {
	if (plugin)
	{
		delete plugin;
		g_isCreated = false;
	}
}

miplStd::miplStd() {

}

miplStd::~miplStd() {

}

const wchar_t* miplStd::GetName() {
	return L"Standart plugin";
}

const wchar_t* miplStd::GetDescription() {
	return L"Standart plugin";
}

const wchar_t* miplStd::GetAuthor() {
	return L"mimi developer";
}

bool miplStd::IsDebug() {
#ifdef MI_DEBUG
	return true;
#else
	return false;
#endif
}

miplDestroyPlugin_t miplStd::GetDestroyFunction(){
	return miplDestroyPlugin;
}

bool miplStd::Init(){

	return true;
}