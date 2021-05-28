//#define MI_EXPORTS
#include "miSDK.h"

#include "miplStd.h"

void miplStd_ImportOBJ_Tiangulate_onClick(bool isChecked) {
	if (isChecked)
	{
		printf("1\n");
	}
	else
	{
		printf("0\n");
	}
}

void miplStd_initGuiForImportOBJ(miPluginGUI* gui) {
	float X = 0.f;
	float Y = 0.f;
	gui->AddText(miVec2(X, Y), L"Import parameters: ", 0);

	Y += 15.f;

	gui->AddCheckBox(miVec2(X,Y), L"Triangulate", miplStd_ImportOBJ_Tiangulate_onClick, false);
}

