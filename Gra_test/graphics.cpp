#include "stdafx.h"
#include "graphics.h"

namespace graphics
{
	graphics::graphics(HWND hWnd, INT screenWidth, INT screenHeight) :
		m_d3d(hWnd, screenWidth, screenHeight, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)
	{
		m_Model = std::make_unique<model>(m_d3d.getDevice());
		m_ColorShader = std::make_unique<colorshader>(m_d3d.getDevice(), hWnd);

		// Set the initial position of the camera.
		m_Camera.SetPosition(0.0f, 0.0f, -10.0f);
	}

	graphics::~graphics()
	{
	}

	// Render method begins with clearing the scene to black.
	// After that it calls the Render function for the camera object to create
	// a view matrix based on the camera's location that was set in the constructor.
	// Once the view matrix is created we get a copy of it from the camera class.
	// We also get copies of the world and projection matrix from the D3DClass object.
	// Then the ModelClass::Render function is called to put the green triangle model geometry
	// on the graphics pipeline. 
	// With the vertices now prepared we call the color shader to draw the vertices
	// using the model information and the three matrices for positioning each vertex.
	// The green triangle is now drawn to the back buffer.
	// With that the scene is complete and we call EndScene to display it to the screen.
	bool graphics::Render()
	{
		D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
		bool result;


		// Clear the buffers to begin the scene.
		m_d3d.BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

		// Generate the view matrix based on the camera's position.
		m_Camera.Render();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		m_Camera.GetViewMatrix(viewMatrix);
		m_d3d.GetWorldMatrix(worldMatrix);
		m_d3d.GetProjectionMatrix(projectionMatrix);

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Model->Render(m_d3d.GetDeviceContext());

		// Render the model using the color shader.
		result = m_ColorShader->Render(m_d3d.GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
		if (!result)
		{
			return false;
		}

		// Present the rendered scene to the screen.
		m_d3d.EndScene();

		return true;
	}
}