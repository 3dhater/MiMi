#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "yy_color.h"
#include "yy_gui.h"

void miApplication::_initGrid() {
	auto _get_mesh = [&](s32 linesNum)->yyMesh*{
		yyMesh * mesh = yyCreate<yyMesh>();
		mesh->m_stride = sizeof(yyVertexLine);
		mesh->m_vertexType = yyMeshVertexType::Line;
		mesh->m_vCount = linesNum * 2;
		mesh->m_vertices = (u8*)yyMemAlloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_iCount = linesNum * 2;
		mesh->m_indices = (u8*)yyMemAlloc(mesh->m_iCount * sizeof(u16));
		return mesh;
	};

	yyColor colorBase(180, 180, 180, 255);
	v4f colorRed(1.f, 0.f, 0.f, 1.f);
	v4f colorGreen(0.f, 1.f, 0.f, 1.f);

	auto _build = [&](yyMesh* mesh, f32 minimum, s32 linesNum, f32 step,
		const yyColor& half1Color, const yyColor& half2Color, const yyColor& specColor1, const yyColor& specColor2, s32 specColorIndex,
		miViewportCameraType ct
		) 
	{
		auto vertex = (yyVertexLine*)mesh->m_vertices;
		f32 pos = minimum;

		const s32 halfNum = linesNum / 2;

		for (s32 i = 0; i < halfNum; ++i)
		{
			v4f color = half1Color.getV4f();

			if (i == specColorIndex)
				color = specColor1.getV4f();

			switch (ct)
			{
			case miViewportCameraType::Left:
			case miViewportCameraType::Right:
				vertex->Position.set(0.f, minimum, pos);
				vertex->Color = color;
				vertex++;
				vertex->Position.set(0.f, std::abs(minimum), pos);
				vertex->Color = color;
				vertex++;
				break;
			case miViewportCameraType::Perspective:
			case miViewportCameraType::Top:
			case miViewportCameraType::Bottom:
				vertex->Position.set(pos, 0.f, minimum);
				vertex->Color = color;
				vertex++;
				vertex->Position.set(pos, 0.f, std::abs(minimum));
				vertex->Color = color;
				vertex++;
				break;
			case miViewportCameraType::Front:
			case miViewportCameraType::Back:
				vertex->Position.set(pos, minimum, 0.f);
				vertex->Color = color;
				vertex++;
				vertex->Position.set(pos, std::abs(minimum), 0.f);
				vertex->Color = color;
				vertex++;
				break;
			default:
				break;
			}

			pos += step;
		}
		
		pos = minimum;
		for (s32 i = 0; i < halfNum; ++i)
		{
			v4f color = half2Color.getV4f();
			 
			if (i == specColorIndex)
				color = specColor2.getV4f();

			switch (ct)
			{
			case miViewportCameraType::Left:
			case miViewportCameraType::Right:
				vertex->Position.set(0.f, pos, -minimum);
				vertex->Color = color;
				vertex++;
				vertex->Position.set(0.f, pos, minimum);
				vertex->Color = color;
				vertex++;
				break;
			case miViewportCameraType::Perspective:
			case miViewportCameraType::Top:
			case miViewportCameraType::Bottom:
				vertex->Position.set(-minimum, 0.f, pos);
				vertex->Color = color;
				vertex++;
				vertex->Position.set(minimum, 0.f, pos);
				vertex->Color = color;
				vertex++;
				break;
			case miViewportCameraType::Front:
			case miViewportCameraType::Back:
				vertex->Position.set(-minimum, pos, 0.f);
				vertex->Color = color;
				vertex++;
				vertex->Position.set(minimum, pos, 0.f);
				vertex->Color = color;
				vertex++;
				break;
			default:
				break;
			}

			pos += step;
		}

		u16* index = (u16*)mesh->m_indices;
		for (u32 i = 0; i < mesh->m_iCount; ++i)
		{
			*index = (u16)i;
			index++;
		}
	};

	{
		const s32 linesNum = 22;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -5.f, linesNum, 1.f, colorBase, colorBase, ColorLime, ColorRed, 5, miViewportCameraType::Perspective);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_perspective1 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -5.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 5, miViewportCameraType::Perspective);

		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_perspective2 = yyCreateGPUMesh(&mi);
		}
		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, ColorLime, ColorRed, 251, miViewportCameraType::Top);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_top1 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Top);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_top2 = yyCreateGPUMesh(&mi);
		}
		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -2510.f, linesNum, 10.f, colorBase, colorBase, ColorLime, ColorRed, 251, miViewportCameraType::Top);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_top1_10 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -2510.f, linesNum, 10.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Top);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_top2_10 = yyCreateGPUMesh(&mi);
		}

		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);
		_build(mesh, -25100.f, linesNum, 100.f, colorBase, colorBase, ColorLime, ColorRed, 251, miViewportCameraType::Top);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_top1_100 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -25100.f, linesNum, 100.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Top);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_top2_100 = yyCreateGPUMesh(&mi);
		}

		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, ColorBlue, ColorRed, 251, miViewportCameraType::Front);

		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_front1 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Front);

		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_front2 = yyCreateGPUMesh(&mi);
		}

		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -2510.f, linesNum, 10.f, colorBase, colorBase, ColorBlue, ColorRed, 251, miViewportCameraType::Front);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_front1_10 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -2510.f, linesNum, 10.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Front);

		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_front2_10 = yyCreateGPUMesh(&mi);
		}

		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);
		_build(mesh, -25100.f, linesNum, 100.f, colorBase, colorBase, ColorBlue, ColorRed, 251, miViewportCameraType::Front);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_front1_100 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -25100.f, linesNum, 100.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Front);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_front2_100 = yyCreateGPUMesh(&mi);
		}
		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, ColorBlue, ColorLime, 251, miViewportCameraType::Left);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_left1 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Left);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_left2 = yyCreateGPUMesh(&mi);
		}

		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -2510.f, linesNum, 10.f, colorBase, colorBase, ColorBlue, ColorLime, 251, miViewportCameraType::Left);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_left1_10 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -2510.f, linesNum, 10.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Left);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_left2_10 = yyCreateGPUMesh(&mi);
		}

		yyDestroy(mesh);
	}
	{
		const s32 linesNum = 1002;
		auto mesh = _get_mesh(linesNum);
		_build(mesh, -25100.f, linesNum, 100.f, colorBase, colorBase, ColorBlue, ColorLime, 251, miViewportCameraType::Left);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_left1_100 = yyCreateGPUMesh(&mi);
		}

		colorBase = yyColor(150, 150, 150, 255);
		_build(mesh, -25100.f, linesNum, 100.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Left);
		
		{
			yyGPUMeshInfo mi;
			mi.m_meshPtr = mesh;
			m_gridModel_left2_100 = yyCreateGPUMesh(&mi);
		}

		yyDestroy(mesh);
	}
}
