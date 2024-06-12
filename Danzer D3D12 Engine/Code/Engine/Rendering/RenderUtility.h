#ifndef RENDER_UTILITY_H
#define RENDER_UTILITY_H

#include "DirectXTK12/Inc/DDSTextureLoader.h"
#include "DirectXTK12/Inc/DirectXHelpers.h"

#include <fstream>
#include <iostream>

// Used to easily gather vertex/index buffers and upload buffers
struct VertexIndexBufferInfo {
	ID3D12Resource* m_vBuffer = nullptr;
	ID3D12Resource* m_iBuffer = nullptr;

	ID3D12Resource* m_iBufferUpload = nullptr;
	ID3D12Resource* m_vBufferUpload = nullptr;
};

inline std::wstring ToWstring(std::string s) {
	return std::wstring(s.begin(), s.end());
}

// Load Vertex/Index buffers And Vertex/Index Upload Resources
inline VertexIndexBufferInfo GetIndexAndVertexBuffer(std::wstring modelName, unsigned int vertexSize, unsigned int indicesSize, 
	ID3D12Device* device) {
	HRESULT result;

	CD3DX12_HEAP_PROPERTIES heapDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_HEAP_PROPERTIES heapUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	VertexIndexBufferInfo bufferInfo;

	// Vertex Buffer
	{
		CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(vertexSize);

		result = device->CreateCommittedResource(
			&heapDefault,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&bufferInfo.m_vBuffer));
		CHECK_HR(result);

		
		bufferInfo.m_vBuffer->SetName(std::wstring(modelName + L" Vertex Buffer").c_str());

		result = device->CreateCommittedResource(
			&heapUpload,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&bufferInfo.m_vBufferUpload));
		CHECK_HR(result);

		bufferInfo.m_vBufferUpload->SetName(std::wstring(modelName + L" Vertex Buffer Upload").c_str());
	} // End of vertex buffer

	// IndexBuffers
	{
		CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(indicesSize);

		result = device->CreateCommittedResource(
			&heapDefault,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&bufferInfo.m_iBuffer));
		CHECK_HR(result);

		bufferInfo.m_iBuffer->SetName(std::wstring(modelName + L" Index Buffer").c_str());

		result = device->CreateCommittedResource(
			&heapUpload,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&bufferInfo.m_iBufferUpload));
		CHECK_HR(result);

		bufferInfo.m_iBufferUpload->SetName(std::wstring(modelName + L" Index Buffer Upload").c_str());
	} // End of index buffer

	return bufferInfo;
}


inline CD3DX12_RESOURCE_BARRIER LoadATextures(std::wstring file, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, ID3D12Resource** resource)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresourcedata = {};
	std::unique_ptr<UINT8[]> pointerData;

	HRESULT result = DirectX::LoadDDSTextureFromFile(
		device,
		file.c_str(),
		resource,
		pointerData,
		subresourcedata);
	CHECK_HR(result);

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(*resource, 0,
		static_cast<UINT>(subresourcedata.size()));

	CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ID3D12Resource* uploadBuffer;
	result = device->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));
	CHECK_HR(result);

	UpdateSubresources(
		cmdList,
		*resource,
		uploadBuffer,
		0, 0,
		static_cast<UINT>(subresourcedata.size()),
		subresourcedata.data());

	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		*resource,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// Return resource barrier so it's possible to upload multiple at the same time
	return transition;
}

//* Keep in mind when using this that Your destBuffer is set to COPY_DESTINATION and fromBuffer is GENERIC_READ
inline CD3DX12_RESOURCE_BARRIER SetSubresourceData(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* destBuffer, ID3D12Resource* fromBuffer, UINT* ptrData, UINT sizeOfData, UINT count, D3D12_RESOURCE_STATES newState) {
	
	D3D12_SUBRESOURCE_DATA data = {};
	data.pData = ptrData;
	data.RowPitch = sizeOfData * count;
	data.SlicePitch = data.RowPitch;

	UpdateSubresources(cmdList, destBuffer, fromBuffer, 0, 0, 1, &data);
	
	CD3DX12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier  = CD3DX12_RESOURCE_BARRIER::Transition(
		destBuffer, D3D12_RESOURCE_STATE_COPY_DEST, newState);

	return resourceBarrier;
}


#endif
