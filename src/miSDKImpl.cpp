#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miVisualObjectImpl.h"

extern miApplication * g_app;

miSDKImpl::miSDKImpl() {
}

miSDKImpl::~miSDKImpl() {
	for (u16 i = 0, sz = m_objectCategories.size(); i < sz; ++i)
	{
		delete m_objectCategories[i];
	}
}

miVisualObject* miSDKImpl::CreateVisualObject() {
	return new miVisualObjectImpl;
}

void miSDKImpl::DestroyVisualObject(miVisualObject* o) {
	delete o;
}

void* miSDKImpl::AllocateMemory(unsigned int size) {
	return yyMemAlloc(size);
}

void  miSDKImpl::FreeMemory(void* ptr) {
	yyMemFree(ptr);
}

miObjectCategory* miSDKImpl::_getObjectCategory(const wchar_t* category) {
	miObjectCategory* cat = 0;
	yyStringW str = category;
	for (u16 i = 0, sz = m_objectCategories.size(); i < sz; ++i)
	{
		if (str == m_objectCategories[i]->m_categoryName)
		{
			cat = m_objectCategories[i];
			break;
		}
	}
	if (!cat)
	{
		cat = new miObjectCategory;
		cat->m_categoryName = category;
		m_objectCategories.push_back(cat);
	}
	return cat;
}
unsigned int miSDKImpl::RegisterNewObject(miPlugin* plugin, const wchar_t* category, const wchar_t* objectName) {
	auto cat = this->_getObjectCategory(category);

	unsigned int id = g_app->m_miCommandID_for_plugins_count;
	++g_app->m_miCommandID_for_plugins_count;

	cat->AddObject(objectName, id + miCommandID_for_plugins);

	g_app->m_pluginCommandID.Add(id, miPluginCommandIDMapNode(plugin, id));

	return id;
}