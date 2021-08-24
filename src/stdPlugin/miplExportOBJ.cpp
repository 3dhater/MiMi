//#define MI_EXPORTS
#include "miSDK.h"

#include "miplStd.h"

#include <map>

extern miSDK* g_sdk;
bool g_ExportOBJ_triangulate = false;
bool g_ExportOBJ_optimize = true;
bool g_ExportOBJ_writeNormals = true;
bool g_ExportOBJ_writeUVs = true;
bool g_ExportOBJ_createMTL = true;
bool g_ExportOBJ_onlySelected = true;
u32 g_ExportOBJ_vIndex = 0;
u32 g_ExportOBJ_vtIndex = 0;
u32 g_ExportOBJ_vnIndex = 0;

void miplStd_ExportOBJ_Tiangulate_onClick(bool isChecked) {
	g_ExportOBJ_triangulate = isChecked;
}
void miplStd_ExportOBJ_Optimize_onClick(bool isChecked) {
	g_ExportOBJ_optimize = isChecked;
}
void miplStd_ExportOBJ_WN_onClick(bool isChecked) {
	g_ExportOBJ_writeNormals = isChecked;
}
void miplStd_ExportOBJ_WUV_onClick(bool isChecked) {
	g_ExportOBJ_writeUVs = isChecked;
}
void miplStd_ExportOBJ_MTL_onClick(bool isChecked) {
	g_ExportOBJ_createMTL = isChecked;
}
void miplStd_ExportOBJ_onlySelected_onClick(bool isChecked) {
	g_ExportOBJ_onlySelected = isChecked;
}

void miplStd_initGuiForExportOBJ(miPluginGUI* gui) {
	float X = 0.f;
	float Y = 0.f;
	gui->AddText(v2f(X, Y), L"Export OBJ parameters: ", 0, 0);

	Y += 15.f;
	gui->AddCheckBox(v2f(X, Y), L"Triangulate", miplStd_ExportOBJ_Tiangulate_onClick, g_ExportOBJ_triangulate, 0);
	Y += 15.f;
	gui->AddCheckBox(v2f(X, Y), L"Optimize", miplStd_ExportOBJ_Optimize_onClick, g_ExportOBJ_optimize, 0);
	Y += 15.f;
	gui->AddCheckBox(v2f(X, Y), L"Write Normals", miplStd_ExportOBJ_WN_onClick, g_ExportOBJ_writeNormals, 0);
	Y += 15.f;
	gui->AddCheckBox(v2f(X, Y), L"Write Texture Coordinates", miplStd_ExportOBJ_WUV_onClick, g_ExportOBJ_writeUVs, 0);
	Y += 15.f;
	gui->AddCheckBox(v2f(X, Y), L"Create Material", miplStd_ExportOBJ_MTL_onClick, g_ExportOBJ_createMTL, 0);
	Y += 15.f;
	
	gui->AddCheckBox(v2f(200.f, 0.f), L"Only Selected", miplStd_ExportOBJ_onlySelected_onClick, g_ExportOBJ_onlySelected, 0);
}

class OBJWriter
{
	miStringA str_for_map;
public:
	OBJWriter() {}
	~OBJWriter() {}

	void WriteF( miStringA& f_str, u32 vI, u32 vtI, u32 vnI)
	{
		f_str += vI;
		if (g_ExportOBJ_writeUVs && g_ExportOBJ_writeNormals)
		{
			f_str += "/";
			f_str += vtI;
			f_str += "/";
			f_str += vnI;
		}
		else if (g_ExportOBJ_writeUVs && !g_ExportOBJ_writeNormals)
		{
			f_str += "/";
			f_str += vtI;
		}
		else if (!g_ExportOBJ_writeUVs && g_ExportOBJ_writeNormals)
		{
			f_str += "/";
			f_str += "/";
			f_str += vnI;
		}

		f_str += " ";
	}

	void WritePosition(
		const v3f& position,
		miStringA& v_str
	)
	{
		v_str += "v ";
		v_str.append_float(position.x);
		v_str += " ";
		v_str.append_float(position.y);
		v_str += " ";
		v_str.append_float(position.z);
		v_str += "\r\n";
	}

	void WriteUV(
		const v2f& UVs,
		miStringA& vt_str
		)
	{
		vt_str += "vt ";
		vt_str.append_float(UVs.x);
		vt_str += " ";
		vt_str.append_float(UVs.y);
		vt_str += " ";
		vt_str.append_float(0.f);
		vt_str += "\r\n";
	}

	void WriteNormal(
		const v3f& normal,
		miStringA& vn_str
	)
	{
		vn_str += "vn ";
		vn_str.append_float(normal.x);
		vn_str += " ";
		vn_str.append_float(normal.y);
		vn_str += " ";
		vn_str.append_float(normal.z);
		vn_str += "\r\n";
	}

	void WriteData(
		const v3f& position, 
		const v2f& UV, 
		const v3f& normal, 
		miStringA& v_str,
		miStringA& vt_str,
		miStringA& vn_str
	) 
	{
		WritePosition(position, v_str);

		if (g_ExportOBJ_writeUVs)
			WriteUV(UV, vt_str);

		if (g_ExportOBJ_writeNormals)
			WriteNormal(normal, vn_str);
	}

	void WriteObject(FILE* file,miSceneObject* object)
	{

		auto objectType = object->GetObjectType();
		if (objectType == miSceneObjectType::MeshObject)
		{
			bool good = true;

			if (g_ExportOBJ_onlySelected)
				good = object->IsSelected();

			auto meshCount = object->GetMeshCount();
			if (meshCount && good)
			{
				miStringA stra;
				stra = object->GetName().data();
				fprintf(file, "\r\no %s \r\n", stra.data());

				miStringA v_str;  v_str.reserve(0xffff);
				miStringA vn_str;  vn_str.reserve(0xffff);
				miStringA vt_str;  vt_str.reserve(0xffff);
				miStringA f_str;  f_str.reserve(0xffff);

				u32 v_count = 0;
				u32 vn_count = 0;
				u32 vt_count = 0;
				u32 f_count = 0;

				auto pivot = object->GetGlobalPosition();

				for (u32 i = 0; i < meshCount; ++i)
				{
					auto mesh = object->GetMesh(i);

					if (!mesh->m_first_polygon)
						continue;

					auto cp = mesh->m_first_polygon;
					auto lp = cp->m_left;
					while (true)
					{
						if (g_ExportOBJ_triangulate)
						{
							auto vertex_1 = cp->m_verts.m_head;
							auto vertex_3 = vertex_1->m_right;
							auto vertex_2 = vertex_3->m_right;
							while (true)
							{
								f_str += "f ";
								++f_count;

								auto vertex = vertex_1;

								v3f position = math::mulBasis(vertex->m_data.m_vertex->m_position, *object->GetWorldMatrix()) + *pivot;
								v2f UVs = vertex->m_data.m_uv;
								v3f normals = vertex->m_data.m_normal;

								if (g_ExportOBJ_optimize)
								{
									WriteOptimize(position, UVs, normals, v_count, vt_count, vn_count, f_str, v_str, vn_str, vt_str);
								}
								else
								{
									++g_ExportOBJ_vIndex;
									++g_ExportOBJ_vtIndex;
									++g_ExportOBJ_vnIndex;
									++v_count;
									++vt_count;
									++vn_count;

									WriteData(position, UVs, normals, v_str, vt_str, vn_str);
									WriteF(f_str, g_ExportOBJ_vIndex, g_ExportOBJ_vtIndex, g_ExportOBJ_vnIndex);
								}

								vertex = vertex_2;

								position = math::mulBasis(vertex->m_data.m_vertex->m_position, *object->GetWorldMatrix()) + *pivot;
								UVs = vertex->m_data.m_uv;
								normals = vertex->m_data.m_normal;

								if (g_ExportOBJ_optimize)
								{
									WriteOptimize(position, UVs, normals, v_count, vt_count, vn_count, f_str, v_str, vn_str, vt_str);
								}
								else
								{
									++g_ExportOBJ_vIndex;
									++g_ExportOBJ_vtIndex;
									++g_ExportOBJ_vnIndex;
									++v_count;
									++vt_count;
									++vn_count;

									WriteData(position, UVs, normals, v_str, vt_str, vn_str);
									WriteF(f_str, g_ExportOBJ_vIndex, g_ExportOBJ_vtIndex, g_ExportOBJ_vnIndex);
								}

								vertex = vertex_3;

								position = math::mulBasis(vertex->m_data.m_vertex->m_position, *object->GetWorldMatrix()) + *pivot;
								UVs = vertex->m_data.m_uv;
								normals = vertex->m_data.m_normal;

								if (g_ExportOBJ_optimize)
								{
									WriteOptimize(position, UVs, normals, v_count, vt_count, vn_count, f_str, v_str, vn_str, vt_str);
								}
								else
								{
									++g_ExportOBJ_vIndex;
									++g_ExportOBJ_vtIndex;
									++g_ExportOBJ_vnIndex;
									++v_count;
									++vt_count;
									++vn_count;

									WriteData(position, UVs, normals, v_str, vt_str, vn_str);
									WriteF(f_str, g_ExportOBJ_vIndex, g_ExportOBJ_vtIndex, g_ExportOBJ_vnIndex);
								}

								f_str += "\r\n";

								vertex_2 = vertex_2->m_right;
								vertex_3 = vertex_3->m_right;
								if (vertex_2 == vertex_1)
									break;
							}
						}
						else
						{
							f_str += "f ";
							++f_count;

							auto cv = cp->m_verts.m_head;
							auto lv = cv->m_left;
							while (true)
							{
								v3f position = math::mulBasis(cv->m_data.m_vertex->m_position, *object->GetWorldMatrix()) + *pivot;
								v2f UVs = cv->m_data.m_uv;
								v3f normals = cv->m_data.m_normal;

								if (g_ExportOBJ_optimize)
								{
									WriteOptimize(position, UVs, normals, v_count, vt_count, vn_count, f_str, v_str, vn_str, vt_str);
								}
								else
								{
									++g_ExportOBJ_vIndex;
									++g_ExportOBJ_vtIndex;
									++g_ExportOBJ_vnIndex;

									++v_count;
									++vt_count;
									++vn_count;

									WriteData(position, UVs, normals, v_str, vt_str, vn_str);
									WriteF(f_str, g_ExportOBJ_vIndex, g_ExportOBJ_vtIndex, g_ExportOBJ_vnIndex);
								}

								if (cv == lv)
									break;
								cv = cv->m_right;
							}
							f_str += "\r\n";
						}

						if (cp == lp)
							break;
						cp = cp->m_right;
					}
				}

				fprintf(file, "%s \r\n", v_str.data());
				fprintf(file, "# %u vertices \r\n\r\n", v_count);

				if (g_ExportOBJ_writeUVs)
				{
					fprintf(file, "%s \r\n", vt_str.data());
					fprintf(file, "# %u texture coordinates \r\n\r\n", vt_count);
				}

				if (g_ExportOBJ_writeNormals)
				{
					fprintf(file, "%s \r\n", vn_str.data());
					fprintf(file, "# %u normals \r\n\r\n", vn_count);
				}

				fprintf(file, "%s \r\n", f_str.data());
				fprintf(file, "# %u faces \r\n\r\n", f_count);
			}
		}

		auto children = object->GetChildren();
		if (children->m_head)
		{
			auto c = children->m_head;
			auto l = c->m_left;
			while (true)
			{
				WriteObject(file, c->m_data);
				if (c == l)
					break;
				c = c->m_right;
			}
		}
	}

	void WriteOptimize(
		const v3f& position, 
		const v2f& UVs, 
		const v3f& normals,
		u32& v_count,
		u32& vt_count,
		u32& vn_count,
		miStringA& f_str,
		miStringA& v_str,
		miStringA& vn_str,
		miStringA& vt_str
		)
	{
		u32 vI = 0;
		u32 vtI = 0;
		u32 vnI = 0;

		str_for_map.clear();
		str_for_map.append_float(position.x);
		str_for_map.append_float(position.y);
		str_for_map.append_float(position.z);

		auto itP = map_position.find(str_for_map.data());
		if (itP == map_position.end())
		{
			++g_ExportOBJ_vIndex;
			++v_count;
			map_position.insert(std::pair<const char*, u32>(str_for_map.data(), g_ExportOBJ_vIndex));
			WritePosition(position, v_str);
			vI = g_ExportOBJ_vIndex;
		}
		else
		{
			vI = itP->second;
		}

		if (g_ExportOBJ_writeUVs)
		{
			str_for_map.clear();
			str_for_map.append_float(UVs.x);
			str_for_map.append_float(UVs.y);

			auto itU = map_UV.find(str_for_map.data());
			if (itU == map_UV.end())
			{
				++g_ExportOBJ_vtIndex;
				++vt_count;
				map_UV.insert(std::pair<const char*, u32>(str_for_map.data(), g_ExportOBJ_vtIndex));
				WriteUV(UVs, vt_str);
				vtI = g_ExportOBJ_vtIndex;
			}
			else
			{
				vtI = itU->second;
			}
		}

		if (g_ExportOBJ_writeNormals)
		{
			str_for_map.clear();
			str_for_map.append_float(normals.x);
			str_for_map.append_float(normals.y);
			str_for_map.append_float(normals.z);

			auto itN = map_normal.find(str_for_map.data());
			if (itN == map_normal.end())
			{
				++g_ExportOBJ_vnIndex;
				++vn_count;
				map_normal.insert(std::pair<const char*, u32>(str_for_map.data(), g_ExportOBJ_vnIndex));
				WriteNormal(normals, vn_str);
				vnI = g_ExportOBJ_vnIndex;
			}
			else
			{
				vnI = itN->second;
			}
		}

		WriteF(f_str, vI, vtI, vnI);
	}

	std::map<std::string, u32> map_position;
	std::map<std::string, u32> map_UV;
	std::map<std::string, u32> map_normal;
};





void miplStd_ExportOBJ(const wchar_t* fileName) {
	//wprintf(L"Export: %s\n", fileName);
	g_ExportOBJ_vIndex = 0;
	g_ExportOBJ_vtIndex = 0;
	g_ExportOBJ_vnIndex = 0;

	std::mbstate_t state = std::mbstate_t();
	std::size_t len = 1 + std::wcsrtombs(nullptr, &fileName, 0, &state);
	std::vector<char> mbstr(len);
	std::wcsrtombs(&mbstr[0], &fileName, mbstr.size(), &state);
	const char* fileNameA = &mbstr[0];

	FILE* file = fopen(fileNameA, "wb");

	fprintf(file, "# Pipa3D Wavefront OBJ Exporter v1.0\r\n\r\n");

	OBJWriter o;
	
	o.WriteObject(file, g_sdk->GetRootObject());

	fclose(file);
}