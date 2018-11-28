#include "stdafx.h"
#include "model.h"

namespace graphics
{
	model::model(ID3D11Device *device)
	{
		// Initialize the vertex and index buffer that hold the geometry for the triangle.
		if (!InitializeBuffers(device))
		{
			throw "Unable to initialize buffers.";
		}

	}

	model::~model()
	{
		ShutdownBuffers();
	}

	// The InitializeBuffers function is where vertex and index buffers are created.
	// Usually you would read in a model and create the buffers from that data file.
	bool model::InitializeBuffers(ID3D11Device *dev)
	{
		VertexType *vertices;
		ULONG *indices;
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;
		// First create two temporary arrays to hold the vertex and index data 
		// that we will use later to populate the final buffers with.

		// Set the number of vertices in the vertex array.
		vertexcnt = 3;

		// Set the number of indices in the index array.
		indexcnt = 3;

		// Create the vertex array.
		vertices = new VertexType[vertexcnt];
		if (!vertices)
		{
			return false;
		}

		// Create the index array.
		indices = new ULONG[indexcnt];
		if (!indices)
		{
			return false;
		}

		// NOTE: hard coded triangle.
		// Order of vertices is very important.
		// They have to be placed in the clockwise order to be visible.
		// If they are put counter clockwise they will not be drawn due to back face culling.

		// Load the vertex array with data.
		vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
		vertices[0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

		vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
		vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

		vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
		vertices[2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

		// Load the index array with data.
		indices[0] = 0;  // Bottom left.
		indices[1] = 1;  // Top middle.
		indices[2] = 2;  // Bottom right.

		// First fill out a description of the buffer.
		// In the description the ByteWidth (size of the buffer) and the BindFlags (type of buffer) 
		// are what you need to ensure are filled out correctly.
		// After the description is filled out you need to also fill out a subresource pointer
		// which will point to either your vertex or index array you previously created. 
		// With the description and subresource pointer you can call CreateBuffer using the D3D device
		// and it will return a pointer to the new buffer.

		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexcnt;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now create the vertex buffer.
		result = dev->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexbuff);
		if (FAILED(result))
		{
			return false;
		}

		// Set up the description of the static index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexcnt;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = dev->CreateBuffer(&indexBufferDesc, &indexData, &indexbuff);
		if (FAILED(result))
		{
			return false;
		}

		// Release the arrays now that the vertex and index buffers have been created and loaded.
		delete[] vertices;
		vertices = nullptr;

		delete[] indices;
		indices = nullptr;

		return true;
	}

	// The purpose of this function is to set the vertex buffer and index buffer as active
	// on the input assembler in the GPU.
	// Once the GPU has an active vertex buffer it can then use the shader to render that buffer.
	// This function also defines how those buffers should be drawn such as
	// triangles, lines, fans, and so forth.
	// For now we set the vertex buffer and index buffer as active on the input assembler
	// and tell the GPU that the buffers should be drawn as triangles 
	// using the IASetPrimitiveTopology DirectX function.
	void model::Render(ID3D11DeviceContext *devcon)
	{
		UINT stride{}, offset{};

		// Set vertex buffer stride and offset.
		stride = sizeof(VertexType);
		offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		devcon->IASetVertexBuffers(0, 1, &vertexbuff, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		devcon->IASetIndexBuffer(indexbuff, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void model::ShutdownBuffers()
	{
		// Release the index buffer.
		if (indexbuff)
		{
			indexbuff->Release();
			indexbuff = nullptr;
		}

		// Release the vertex buffer.
		if (vertexbuff)
		{
			vertexbuff->Release();
			vertexbuff = nullptr;
		}

		return;
	}

	int model::GetIndexCount()
	{
		return indexcnt;
	}
}
