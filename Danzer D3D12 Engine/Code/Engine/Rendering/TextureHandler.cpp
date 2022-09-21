#include "stdafx.h"
#include "TextureHandler.h"
#include "Core/DirectX12Framework.h"

#include "../3rdParty/DirectXTK12-main/Inc/DDSTextureLoader.h"
#include "../3rdParty/DirectXTK12-main/Inc/DirectXHelpers.h"

#include <filesystem>
#include <fstream>


TextureHandler::~TextureHandler()
{
	for (UINT i = 0; i < m_textures.size(); i++)
	{
		m_textures[i].m_textureDescriptorHeap->Release();
		m_textures[i].m_textureBuffer->Release();
	}
}

void TextureHandler::Init(DirectX12Framework& framework, bool loadAllTexures)
{
	m_framework = framework;
	if (loadAllTexures) {
		LoadAllExistingTextures();
	}
}

//* Creates and loads every texture to be available to the gpu from the beginning
void TextureHandler::LoadAllExistingTextures()
{
	std::string path = "Sprites/";

	m_framework.ResetCommandListAndAllocator(nullptr);

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::wstring file = entry.path().c_str();
		CreateTexture(file);
	}

	LoadAllCreatedTexuresToGPU();
}

//* This is/should always be called after creating 1 or multiple textures,
void TextureHandler::LoadAllCreatedTexuresToGPU()
{
	if (!m_resourceBarriers.empty()) {
		m_framework.GetCommandList()->ResourceBarrier(m_resourceBarriers.size(), &m_resourceBarriers[0]);

		m_framework.ExecuteCommandList();
		m_framework.WaitForPreviousFrame();

		HRESULT result;

		for (UINT i = 0; i < m_tempTextures.size(); i++) {
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.NumDescriptors = 1;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			result = m_framework.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_tempTextures[i].m_textureDescriptorHeap));
			CHECK_HR(result);

			m_tempTextures[i].m_textureDescriptorHeap->SetName(m_tempTextures[i].m_texturePath.c_str());

			DirectX::CreateShaderResourceView(
				m_framework.GetDevice(), m_tempTextures[i].m_textureBuffer.Get(),
				m_tempTextures[i].m_textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
				m_tempTextures[i].m_cubeMap
			);

			m_textures.emplace_back(m_tempTextures[i]);
		}

		m_tempTextures.clear();
		m_resourceBarriers.clear();
	}
}

std::vector<UINT> TextureHandler::CreateMultipleTextures(std::string* textures, UINT numOfTextures)
{
	std::vector<UINT> newTextures;

	for (UINT i = 0; i < numOfTextures; i++)
	{
		newTextures.emplace_back(CreateTexture({ textures[i].begin(), textures[i].end() }));
	}

	LoadAllCreatedTexuresToGPU();

	return newTextures; 
}

//std::array<UINT, 3> TextureHandler::CreateMultipleTextures(std::string paths[3])
//{
//	std::array<UINT, 3> newTextures;
//
//	for (UINT i = 0; i < 3; i++)
//	{
//		newTextures[i] = CreateTexture({paths[i].begin(), paths[i].end()});
//	}
//
//	LoadAllCreatedTexuresToGPU();
//
//	return newTextures;
//}

//* Create Texture expects CommandList to have already been resetted
//* before use. LoadAllCreatedTexuresToGPU also needs to be called directly after this,
//* CreateMultipleTextures loads all to GPU automatically.
UINT TextureHandler::CreateTexture(std::wstring file, bool isCubeMap)
{	
	file = GetCorrectPathAndName(file);
	for (UINT i = 0; i < m_textures.size(); i++)
	{
		if (file == m_textures[i].m_texturePath)
			return i + 1;
	}
		
	if (!m_framework.CmdListIsRecording()) {
		m_framework.ResetCommandListAndAllocator(nullptr);
	}

	Texture texture;
	texture.m_texturePath = file;
	texture.m_cubeMap	  = isCubeMap;
	CD3DX12_RESOURCE_BARRIER resource = LoadTextures(file, &texture.m_textureBuffer, isCubeMap);
	m_resourceBarriers.emplace_back(resource);
	m_tempTextures.emplace_back(texture);

	return m_textures.size() + m_tempTextures.size();
}


UINT TextureHandler::GetTexture(std::wstring texturePath)
{
	texturePath = GetCorrectPathAndName(texturePath);
	for (UINT i = 0; i < m_textures.size(); i++)
	{
		if (texturePath == m_textures[i].m_texturePath)
			return i + 1;
	}

	return 0;
}

std::wstring TextureHandler::GetCorrectPathAndName(std::wstring path)
{
	if (path.empty()) {
		path = L"Sprites/defaultTexture.dds";
	}

	// Check if filepath has .dds, if not change it.
	size_t n = path.find(L".dds");
	if (n == std::wstring::npos) {
		size_t strPos = path.find_first_of(L".");
		path.replace(strPos + 1, strPos + 3, L"dds");
	}

	if (path.find(L"Sprites/") == std::wstring::npos) {
		std::wstring add(L"Sprites/");
		add += path;
		path = add;
	}

	// last check if the file actually exists
	std::ifstream exists(path.c_str());
	if (!exists.good()) {
		path = L"Sprites/defaultTexture.dds";
	}
	exists.close();

	return path;
}

CD3DX12_RESOURCE_BARRIER TextureHandler::LoadTextures(std::wstring file, ID3D12Resource** textureBuffer, bool isCubeMap)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresourcedata = {};
	std::unique_ptr<UINT8[]> pointerData;

	HRESULT result = DirectX::LoadDDSTextureFromFile(
		m_framework.GetDevice(),
		file.c_str(),
		textureBuffer,
		pointerData,
		subresourcedata,
		0, 
		nullptr, 
		&isCubeMap);
	CHECK_HR(result);

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(*textureBuffer, 0,
		static_cast<UINT>(subresourcedata.size()));

	CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ID3D12Resource* uploadBuffer;
	result = m_framework.GetDevice()->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));
	CHECK_HR(result);

	UpdateSubresources(
		m_framework.GetCommandList(),
		*textureBuffer,
		uploadBuffer,
		0, 0,
		static_cast<UINT>(subresourcedata.size()),
		subresourcedata.data());

	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		*textureBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// Return resourc barrier so it's possible to upload multiple at the same time
	return transition;
}
