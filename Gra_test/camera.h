// camera.h : include file for camera specific operations 
// The camera class will keep track of where the camera is and its current rotation.
// It will use the position and rotation information to generate a view matrix
// which will be passed into the HLSL shader for rendering.
#pragma once

#include <d3dx10math.h>

namespace graphics
{
	class camera
	{
	public:
		camera() {};
		camera(const camera& other) = delete;
		~camera() {};

		// The SetPosition and SetRotation functions will be used
		// to set the position and rotation of the camera object along x, y and z axis. 
		void SetPosition(FLOAT x, FLOAT y, FLOAT z);
		void SetRotation(FLOAT x, FLOAT y, FLOAT z);

		// The GetPosition and GetRotation functions return the location
		// and rotation of the camera to calling functions.
		D3DXVECTOR3 GetPosition();
		D3DXVECTOR3 GetRotation();

		// Render will be used to create the view matrix based on the position and rotation of the camera.
		void Render();

		// GetViewMatrix will be used to retrieve the view matrix from the camera object
		// so that the shaders can use it for rendering.
		void GetViewMatrix(D3DXMATRIX& viewMatrix);
	private:
		FLOAT m_positionX{}, m_positionY{}, m_positionZ{};
		FLOAT m_rotationX{}, m_rotationY{}, m_rotationZ{};
		D3DXMATRIX m_viewMatrix{};
	};
}

