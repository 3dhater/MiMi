//#define MI_EXPORTS
#include "miSDK.h"

#include "miplStd.h"
#include <cwchar>

extern miSDK* g_sdk;

enum class OBJFaceType
{
	p,
	pu,
	pun,
	pn
};

struct OBJSimpleArr
{
	OBJSimpleArr() {
		sz = 0;
	}

	int data[0x100];
	unsigned int sz;

	void push_back(int v) { data[sz++] = v; }
	unsigned int size() { return sz; }
	void reset() { sz = 0; }
};

struct OBJFace
{
	OBJFace() {
		ft = OBJFaceType::pun;
	}

	OBJSimpleArr p, u, n;
	OBJFaceType ft;

	void reset()
	{
		ft = OBJFaceType::pun;
		p.reset();
		u.reset();
		n.reset();
	}
};

unsigned char * OBJNextLine(unsigned char * ptr);
unsigned char * OBJSkipSpaces(unsigned char * ptr);
unsigned char * OBJReadVec2(unsigned char * ptr, v2f& vec2);
unsigned char * OBJReadFloat(unsigned char * ptr, float& value);
unsigned char * OBJReadVec3(unsigned char * ptr, v3f& vec3);
unsigned char * OBJReadFace(unsigned char * ptr, OBJFace& f, char * s);
unsigned char * OBJReadWord(unsigned char * ptr, miString& str);

bool g_ImportOBJ_triangulate = false;

void miplStd_ImportOBJ_Tiangulate_onClick(bool isChecked) {
	g_ImportOBJ_triangulate = isChecked;
}

void miplStd_initGuiForImportOBJ(miPluginGUI* gui) {
	float X = 0.f;
	float Y = 0.f;
	gui->AddText(v2f(X, Y), L"Import OBJ parameters: ", 0, 0);

	Y += 15.f;

	gui->AddCheckBox(v2f(X,Y), L"Triangulate", miplStd_ImportOBJ_Tiangulate_onClick, false, 0);
}

void miplStd_ImportOBJ(const wchar_t* fileName) {
	assert(fileName);
	std::mbstate_t state = std::mbstate_t();
	std::size_t len = 1 + std::wcsrtombs(nullptr, &fileName, 0, &state);
	std::vector<char> mbstr(len);
	std::wcsrtombs(&mbstr[0], &fileName, mbstr.size(), &state);
	const char* fileNameA = &mbstr[0];

	auto material = g_sdk->CreateMaterial(L"New material");

	FILE* file = fopen(fileNameA, "rb");
	auto file_size = (size_t)g_sdk->FileSize(fileNameA);

	std::vector<unsigned char> file_byte_array((unsigned int)file_size + 2);

	unsigned char * ptr = file_byte_array.data();
	fread(ptr, 1, file_size, file);
	fclose(file);

	ptr[(unsigned int)file_size] = ' ';
	ptr[(unsigned int)file_size + 1] = 0;

	bool groupBegin = false;
	bool isModel = false;
	bool grpFound = false;

	v2f tcoords;
	v3f pos;
	v3f norm;

	std::vector<v3f> position;
	std::vector<v2f> uv;
	std::vector<v3f> normal;

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);

	//std::string name_word;
	miString tmp_word;
	miString curr_word;
	miString prev_word;

	OBJFace f;
	char s[0xff];

	int last_counter[3] = {0,0,0};
	
	std::unordered_map<std::string, unsigned int> map;
	std::string hash;

	miSDKImporterHelper importeHelper;
	miPolygonCreator triangulationPolygonCreator;

	while (*ptr)
	{
		switch (*ptr)
		{
		case '#':
		case 's':
		case 'l':
		case 'u'://usemtl
		case 'c'://curv
		case 'm'://mtllib
		case 'p'://parm
		case 'd'://deg
		case 'e'://end
			ptr = OBJNextLine(ptr);
			break;
		case 'v':
		{
			++ptr;
			if (groupBegin)
				groupBegin = false;
			switch (*ptr)
			{
			case 't':
				ptr = OBJReadVec2(++ptr, tcoords);
				uv.push_back(tcoords);
				++last_counter[1];
				break;
			case 'n':
				ptr = OBJReadVec3(++ptr, norm);
				normal.push_back(norm);
				++last_counter[2];
				break;
			default:
				ptr = OBJReadVec3(ptr, pos);
				position.push_back(pos);
				++last_counter[0];
				//newModel->m_aabb.add(pos);
				break;
			}
		}break;
		case 'f':
		{
			isModel = true;
			f.reset();
			ptr = OBJReadFace(++ptr, f, s);

			importeHelper.m_polygonCreator.Clear();

			if (!importeHelper.m_meshBuilder->m_isBegin)
			{
				importeHelper.m_meshBuilder->Begin();
			}

			bool weld = false;
			bool genNormals = true;

			for (size_t sz2 = f.p.size(), i2 = 0; i2 < sz2; ++i2)
			{
				auto index = i2;
				auto pos_index = f.p.data[index];
				

				if (pos_index < 0)
				{
					// если индекс отрицательный то он указывает на позицию относительно последнего элемента
					// -1 = последний элемент
					pos_index = last_counter[0] + pos_index + 1;
				}

				{
					hash.clear();
					hash += pos_index;

					// это я не помню зачем сделал
					// когда дойду до control вершин, станет ясно зачем это здесь
					auto it = map.find(hash);
					if (it == map.end())
					{
						map[hash] = pos_index;
					}
					else
					{
						weld = true;
					}
				}

				auto v = position[pos_index];

				//geometry_creator->AddPosition(v.x, v.y, v.z);
				v3f pcPos, pcNorm;
				v2f pcUV;

				pcPos = v;

				if (f.ft == OBJFaceType::pu || f.ft == OBJFaceType::pun)
				{
					auto uv_index = f.u.data[index];

					if (uv_index < 0)
					{
						uv_index = last_counter[1] + uv_index + 1;
					}

					auto u = uv[uv_index];
				//	geometry_creator->AddUV(u.x, u.y);
					pcUV.x = u.x;
					pcUV.y = 1.f - u.y;
				}

				if (f.ft == OBJFaceType::pn || f.ft == OBJFaceType::pun)
				{
					auto nor_index = f.n.data[index];

					if (nor_index < 0)
					{
						nor_index = last_counter[2] + nor_index + 1;
					}

					auto n = normal[nor_index];
				//	geometry_creator->AddNormal(n.x, n.y, n.z);
					pcNorm = n;
					genNormals = false;
				}
				importeHelper.m_polygonCreator.Add(pcPos, weld, false, pcNorm, pcUV);
			}

			if (g_ImportOBJ_triangulate && importeHelper.m_polygonCreator.Size() > 3)
			{
				u32 triCount = importeHelper.m_polygonCreator.Size() - 2;
				auto _positions = importeHelper.m_polygonCreator.GetPositions();
				auto _normals = importeHelper.m_polygonCreator.GetNormals();
				auto _tcoords = importeHelper.m_polygonCreator.GetTCoords();
				for (u32 i = 0; i < triCount; ++i)
				{
					triangulationPolygonCreator.Clear();
					triangulationPolygonCreator.Add(_positions[0].m_first, weld, false, _normals[0], _tcoords[0]);
					triangulationPolygonCreator.Add(_positions[i+1].m_first, weld, false, _normals[i+1], _tcoords[i+1]);
					triangulationPolygonCreator.Add(_positions[i+2].m_first, weld, false, _normals[i+2], _tcoords[i+2]);

					importeHelper.m_meshBuilder->AddPolygon(&triangulationPolygonCreator, genNormals);
				}
			}
			else
			{
				importeHelper.m_meshBuilder->AddPolygon(&importeHelper.m_polygonCreator, genNormals);
			}
		}break;
		case 'o':
		case 'g':
		{
			if (!groupBegin)
				groupBegin = true;
			else
			{
				ptr = OBJNextLine(ptr);
				break;
			}

			/*std::string tmp_word;
			ptr = OBJReadWord(++ptr, tmp_word);
			if (tmp_word.size())
			{
				if (!name_word.size())
					name_word = tmp_word;
			}*/
			ptr = OBJReadWord(++ptr, tmp_word);
			if (tmp_word.size())
			{
				prev_word = curr_word;
				curr_word = tmp_word;
			}

			if (grpFound)
			{
				if (importeHelper.m_meshBuilder->m_isBegin)
				{
					importeHelper.m_meshBuilder->End();
					g_sdk->CreateSceneObjectFromHelper(&importeHelper, prev_word.data(), material);
					importeHelper.Drop();
					importeHelper.Create();
					importeHelper.m_meshBuilder->Begin();
				}
			}
			grpFound = true;
		}break;
		default:
			++ptr;
			break;
		}
	}

	if (importeHelper.m_meshBuilder->m_isBegin)
	{
		importeHelper.m_meshBuilder->End();
		g_sdk->CreateSceneObjectFromHelper(&importeHelper, curr_word.data(), material);
		importeHelper.Drop();
	}

	g_sdk->UpdateSceneAabb();
}

unsigned char * OBJNextLine(unsigned char * ptr)
{
	while (*ptr)
	{
		if (*ptr == '\n')
		{
			ptr++;
			return ptr;
		}
		ptr++;
	}
	return ptr;
}

unsigned char * OBJReadVec2(unsigned char * ptr, v2f& vec2)
{
	ptr = OBJSkipSpaces(ptr);
	float x, y;
	if (*ptr == '\n')
	{
		ptr++;
	}
	else
	{
		ptr = OBJReadFloat(ptr, x);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, y);
		ptr = OBJNextLine(ptr);
		vec2.x = x;
		vec2.y = y;
	}
	return ptr;
}

unsigned char * OBJSkipSpaces(unsigned char * ptr)
{
	while (*ptr)
	{
		if (*ptr != '\t' && *ptr != ' ')
			break;
		ptr++;
	}
	return ptr;
}

unsigned char * OBJReadFloat(unsigned char * ptr, float& value)
{
	char str[32u];
	memset(str, 0, 32);
	char * p = &str[0u];
	while (*ptr) {
		if (!isdigit(*ptr) && (*ptr != '-') && (*ptr != '+')
			&& (*ptr != 'e') && (*ptr != 'E') && (*ptr != '.')) break;
		*p = *ptr;
		++p;
		++ptr;
	}
	value = (float)atof(str);
	return ptr;
}

unsigned char * OBJReadVec3(unsigned char * ptr, v3f& vec3) {
	ptr = OBJSkipSpaces(ptr);
	float x, y, z;
	if (*ptr == '\n') {
		ptr++;
	}
	else {
		ptr = OBJReadFloat(ptr, x);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, y);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, z);
		ptr = OBJNextLine(ptr);
		vec3.x = x;
		vec3.y = y;
		vec3.z = z;
	}
	return ptr;
}

unsigned char * OBJSkipSpace(unsigned char * ptr) {
	while (*ptr) {
		if (*ptr != ' ' && *ptr != '\t') break;
		ptr++;
	}
	return ptr;
}

unsigned char * OBJGetInt(unsigned char * p, int& i)
{
	char str[8u];
	memset(str, 0, 8);
	char * pi = &str[0u];

	while (*p)
	{
		/*if( *p == '-' )
		{
		++p;
		continue;
		}*/

		if (!isdigit(*p) && *p != '-') break;


		*pi = *p;
		++pi;
		++p;
	}
	i = atoi(str);
	return p;
}

unsigned char * OBJReadFace(unsigned char * ptr, OBJFace& f, char * s) {
	ptr = OBJSkipSpaces(ptr);
	if (*ptr == '\n')
	{
		ptr++;
	}
	else
	{
		while (true)
		{
			int p = 1;
			int u = 1;
			int n = 1;

			ptr = OBJGetInt(ptr, p);

			if (*ptr == '/')
			{
				ptr++;
				if (*ptr == '/')
				{
					ptr++;
					f.ft = OBJFaceType::pn;
					ptr = OBJGetInt(ptr, n);
				}
				else
				{
					ptr = OBJGetInt(ptr, u);
					if (*ptr == '/')
					{
						ptr++;
						f.ft = OBJFaceType::pun;
						ptr = OBJGetInt(ptr, n);
					}
					else
					{
						f.ft = OBJFaceType::pu;
					}
				}
			}
			else
			{
				f.ft = OBJFaceType::p;
			}
			f.n.push_back(n - 1);
			f.u.push_back(u - 1);
			f.p.push_back(p - 1);
			ptr = OBJSkipSpace(ptr);

			if (*ptr == '\r')
				break;
			else if (*ptr == '\n')
				break;
		}
	}
	return ptr;
}

unsigned char * OBJReadWord(unsigned char * ptr, miString& str)
{
	ptr = OBJSkipSpaces(ptr);
	str.clear();
	while (*ptr)
	{
		if (isspace(*ptr))
			break;
		str += (wchar_t)*ptr;
		ptr++;
	}
	return ptr;
}