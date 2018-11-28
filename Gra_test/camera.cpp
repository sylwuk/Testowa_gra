#include "stdafx.h"
#include "camera.h"

namespace graphics
{
	void camera::SetPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		m_positionX = x;
		m_positionY = y;
		m_positionZ = z;
	}

	void camera::SetRotation(FLOAT x, FLOAT y, FLOAT z)
	{
		m_rotationX = x;
		m_rotationY = y;
		m_rotationZ = z;
	}

	D3DXVECTOR3 camera::GetPosition()
	{
		return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
	}

	D3DXVECTOR3 camera::GetRotation()
	{
		return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
	}

	// The Render function uses the position and rotation of the camera to build and update the view matrix.
	// First the variables for up, position, rotation, and so forth are set.
	// Then at the origin of the scene the camera is rotated based on the x, y, and z rotation of the camera.
	// Once it is properly rotated when then translate the camera to the position in 3D space.
	// With the correct values in the position, lookAt, and up D3DXMatrixLookAtLH function is used
	// to create the view matrix representing the current camera rotation and translation. 
	void camera::Render()
	{
		D3DXVECTOR3 up, position, lookAt;
		float yaw, pitch, roll;
		D3DXMATRIX rotationMatrix;

		// Setup the vector that points upwards.
		up.x = 0.0f;
		up.y = 1.0f;
		up.z = 0.0f;

		// Setup the position of the camera in the world.
		position.x = m_positionX;
		position.y = m_positionY;
		position.z = m_positionZ;

		// Setup where the camera is looking by default.
		lookAt.x = 0.0f;
		lookAt.y = 0.0f;
		lookAt.z = 1.0f;

		// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
		pitch = m_rotationX * 0.0174532925f;
		yaw = m_rotationY * 0.0174532925f;
		roll = m_rotationZ * 0.0174532925f;

		// Create the rotation matrix from the yaw, pitch, and roll values.
		D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

		// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
		D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
		D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

		// Translate the rotated camera position to the location of the viewer.
		lookAt = position + lookAt;

		// Finally create the view matrix from the three updated vectors.
		D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
	}

	// After the Render function has been called to create the view matrix
	// we can provide the updated view matrix to calling functions using this GetViewMatrix function.
	// The view matrix will be one of the three main matrices used in the HLSL vertex shader.
	void camera::GetViewMatrix(D3DXMATRIX& viewMatrix)
	{
		viewMatrix = m_viewMatrix;
	}
}