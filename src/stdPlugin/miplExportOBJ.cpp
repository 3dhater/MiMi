//#define MI_EXPORTS
#include "miSDK.h"

#include "miplStd.h"

extern miSDK* g_sdk;
bool g_ExportOBJ_triangulate = false;
bool g_ExportOBJ_optimize = false;

void miplStd_ExportOBJ_Tiangulate_onClick(bool isChecked) {
	g_ExportOBJ_triangulate = isChecked;
}
void miplStd_ExportOBJ_Optimize_onClick(bool isChecked) {
	g_ExportOBJ_optimize = isChecked;
}

void miplStd_initGuiForExportOBJ(miPluginGUI* gui) {
	float X = 0.f;
	float Y = 0.f;
	gui->AddText(v2f(X, Y), L"Export OBJ parameters: ", 0, 0);

	Y += 15.f;
	gui->AddCheckBox(v2f(X, Y), L"Triangulate", miplStd_ExportOBJ_Tiangulate_onClick, g_ExportOBJ_triangulate, 0);
	Y += 15.f;
	gui->AddCheckBox(v2f(X, Y), L"Optimize", miplStd_ExportOBJ_Optimize_onClick, g_ExportOBJ_optimize, 0);
}



void miplStd_ExportOBJ(const wchar_t* fileName) {
	//wprintf(L"Export: %s\n", fileName);

	auto root = g_sdk->GetRootObject();
}