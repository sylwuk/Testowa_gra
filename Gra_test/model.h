// model.h : include file for model specific operations 
// Model class is responsible for encapsulating the geometry for 3D models.
#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

namespace graphics
{
	class model
	{
	private:
		// Take note that this typedef must match the layout in the ColorShaderClass.
		struct VertexType
		{
			D3DXVECTOR3 position;
			D3DXVECTOR4 color;
		};
	public:
		model() = delete;
		model(ID3D11Device *device);
		model(const model& other);
		~model();

		// The Render function puts the model geometry on the video card
		// to prepare it for drawing by the color shader.
		void Render(ID3D11DeviceContext *devcon);
		int GetIndexCount();
	private:
		bool InitializeBuffers(ID3D11Device *dev);
		void ShutdownBuffers();
	
		// The private variables in the model class are the vertex and index buffer
		// as well as two integers to keep track of the size of each buffer.
		// Note that all DirectX 11 buffers generally use the generic ID3D11Buffer type
		// and are more clearly identified by a buffer description when they are first created.
		ID3D11Buffer *vertexbuff{}, *indexbuff{};
		INT vertexcnt{}, indexcnt{};
	};
}
