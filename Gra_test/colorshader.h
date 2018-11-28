// colorshader.h : include file for color shader specific operations 
// The colorshader class is responsible for invoking the HLSL shaders for
// drawing the 3D models that are on the GPU.

#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

namespace graphics
{
	class colorshader
	{
	private:

		// This typedef must be exactly the same as the one in the vertex shader
		// as the model data needs to match the typedefs in the shader for proper rendering.
		struct MatrixBufferType
		{
			D3DXMATRIX world;
			D3DXMATRIX view;
			D3DXMATRIX projection;
		};
	public:
		colorshader(ID3D11Device *dev, HWND hWnd);
		colorshader(const colorshader& other) = delete;
		colorshader(colorshader&& other) = delete;
		~colorshader();

		colorshader& operator=(const colorshader& other) = delete;

		// The render function sets the shader parameters and then draws the prepared model vertices using the shader.
		bool Render(ID3D11DeviceContext *devcon,
			int indexcnt,
			D3DXMATRIX wordlmatrix,
			D3DXMATRIX viewmatrix,
			D3DXMATRIX projectionmatrix);
	private:
		bool InitializeShader(ID3D11Device *dev, HWND hWnd);
		void OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hWnd, WCHAR *path);

		bool SetShaderParameters(ID3D11DeviceContext *devcon,
			D3DXMATRIX worldMatrix,
			D3DXMATRIX viewMatrix,
			D3DXMATRIX projectionMatrix);
		void RenderShader(ID3D11DeviceContext *devcon, int indexcnt);
	private:
		ID3D11VertexShader* m_vertexShader{};
		ID3D11PixelShader* m_pixelShader{};
		ID3D11InputLayout* m_layout{};
		ID3D11Buffer* m_matrixBuffer{};
	};
}

