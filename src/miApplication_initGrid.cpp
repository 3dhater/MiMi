#include "miApplication.h"
#include "miGUIManager.h"
#include "miViewport.h"
#include "yy_color.h"
#include "yy_gui.h"
#include "yy_model.h"

void miApplication::_initGrid() {
	auto _get_model = [&](s32 linesNum)->yyModel*{
		yyModel * model = yyCreate<yyModel>();
		model->m_stride = sizeof(yyVertexLineModel);
		model->m_vertexType = yyVertexType::LineModel;
		model->m_vCount = linesNum * 2;
		model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
		model->m_iCount = linesNum * 2;
		model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));
		return model;
	};

	yyColor colorBase(180, 180, 180, 255);
	v4f colorRed(1.f, 0.f, 0.f, 1.f);
	v4f colorGreen(0.f, 1.f, 0.f, 1.f);

	auto _build = [&](yyModel* model, f32 minimum, s32 linesNum, f32 step,
		const yyColor& half1Color, const yyColor& half2Color, const yyColor& specColor1, const yyColor& specColor2, s32 specColorIndex,
		miViewportCameraType ct
		) 
	{
		auto vertex = (yyVertexLineModel*)model->m_vertices;
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
				vertex->Position.set(pos, minimum, 0.f);
				vertex->Color = color;
				vertex++;
				vertex->Position.set(pos, std::abs(minimum), 0.f);
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
				vertex->Position.set(-minimum, pos, 0.f);
				vertex->Color = color;
				vertex++;
				vertex->Position.set(minimum, pos, 0.f);
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

		u16* index = (u16*)model->m_indices;
		for (s32 i = 0; i < model->m_iCount; ++i)
		{
			*index = (u16)i;
			index++;
		}
	};

	{
		const s32 linesNum = 22;
		auto model = _get_model(linesNum);

		_build(model, -5.f, linesNum, 1.f, colorBase, colorBase, ColorLime, ColorRed, 5, miViewportCameraType::Perspective);
		m_gridModel_perspective1 = m_gpu->CreateModel(model);

		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -5.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 5, miViewportCameraType::Perspective);
		m_gridModel_perspective2 = m_gpu->CreateModel(model);

		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);

		_build(model, -251.f, linesNum, 1.f, colorBase, colorBase, ColorLime, ColorRed, 251, miViewportCameraType::Top);
		m_gridModel_top1 = m_gpu->CreateModel(model);

		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Top);
		m_gridModel_top2 = m_gpu->CreateModel(model);

		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);

		_build(model, -2510.f, linesNum, 10.f, colorBase, colorBase, ColorLime, ColorRed, 251, miViewportCameraType::Top);
		m_gridModel_top1_10 = m_gpu->CreateModel(model);

		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -2510.f, linesNum, 10.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Top);
		m_gridModel_top2_10 = m_gpu->CreateModel(model);

		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);
		_build(model, -25100.f, linesNum, 100.f, colorBase, colorBase, ColorLime, ColorRed, 251, miViewportCameraType::Top);
		m_gridModel_top1_100 = m_gpu->CreateModel(model);
		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -25100.f, linesNum, 100.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Top);
		m_gridModel_top2_100 = m_gpu->CreateModel(model);
		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);

		_build(model, -251.f, linesNum, 1.f, colorBase, colorBase, ColorBlue, ColorRed, 251, miViewportCameraType::Front);
		m_gridModel_front1 = m_gpu->CreateModel(model);

		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Front);
		m_gridModel_front2 = m_gpu->CreateModel(model);

		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);

		_build(model, -2510.f, linesNum, 10.f, colorBase, colorBase, ColorBlue, ColorRed, 251, miViewportCameraType::Front);
		m_gridModel_front1_10 = m_gpu->CreateModel(model);

		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -2510.f, linesNum, 10.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Front);
		m_gridModel_front2_10 = m_gpu->CreateModel(model);

		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);
		_build(model, -25100.f, linesNum, 100.f, colorBase, colorBase, ColorBlue, ColorRed, 251, miViewportCameraType::Front);
		m_gridModel_front1_100 = m_gpu->CreateModel(model);
		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -25100.f, linesNum, 100.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Front);
		m_gridModel_front2_100 = m_gpu->CreateModel(model);
		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);

		_build(model, -251.f, linesNum, 1.f, colorBase, colorBase, ColorBlue, ColorLime, 251, miViewportCameraType::Left);
		m_gridModel_left1 = m_gpu->CreateModel(model);

		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Left);
		m_gridModel_left2 = m_gpu->CreateModel(model);

		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);

		_build(model, -2510.f, linesNum, 10.f, colorBase, colorBase, ColorBlue, ColorLime, 251, miViewportCameraType::Left);
		m_gridModel_left1_10 = m_gpu->CreateModel(model);

		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -2510.f, linesNum, 10.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Left);
		m_gridModel_left2_10 = m_gpu->CreateModel(model);

		yyDestroy(model);
	}
	{
		const s32 linesNum = 1002;
		auto model = _get_model(linesNum);
		_build(model, -25100.f, linesNum, 100.f, colorBase, colorBase, ColorBlue, ColorLime, 251, miViewportCameraType::Left);
		m_gridModel_left1_100 = m_gpu->CreateModel(model);
		colorBase = yyColor(150, 150, 150, 255);
		_build(model, -25100.f, linesNum, 100.f, colorBase, colorBase, colorBase, colorBase, 251, miViewportCameraType::Left);
		m_gridModel_left2_100 = m_gpu->CreateModel(model);
		yyDestroy(model);
	}
}
