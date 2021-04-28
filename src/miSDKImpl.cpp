#include "../SDK/miSDK.h"
#include "miApplication.h"
#include "miSDKImpl.h"
#include "miVisualObjectImpl.h"

extern miApplication * g_app;

miSDKImpl::miSDKImpl() {
}

miSDKImpl::~miSDKImpl() {
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