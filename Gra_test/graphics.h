// graphics.h : include file for main graphics specific operations 
// Graphics class is the main class that is used to render the scene
// by invoking all the needed class objects for the project.
#pragma once

#include "d3d.h"
#include "camera.h"
#include "model.h"
#include "colorshader.h"
#include <memory>

namespace graphics
{
	constexpr BOOL FULL_SCREEN = false;
	constexpr BOOL VSYNC_ENABLED = false;
	constexpr FLOAT SCREEN_DEPTH = 1000.0f;
	constexpr FLOAT SCREEN_NEAR = 0.1f;

	class graphics
	{
	public:
		graphics(HWND hWnd, INT screenWidth, INT screenHeight);
		~graphics();
		bool Render();
	private:
		d3d m_d3d;
		std::unique_ptr<model> m_Model{};
		std::unique_ptr<colorshader> m_ColorShader{};
		camera m_Camera{};
	};
}
