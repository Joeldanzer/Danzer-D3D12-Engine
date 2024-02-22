#include "stdafx.h"
#include "TextureHandler.h"
#include "Core/FrameResource.h"
#include "Core/D3D12Framework.h"

#include "../3rdParty/DirectXTK12-main/Inc/DDSTextureLoader.h"
#include "../3rdParty/DirectXTK12-main/Inc/DirectXHelpers.h"

#include <filesystem>
#include <fstream>

TextureHandler::TextureHandler(D3D12Framework& framework) :
	m_framework(framework)
{
	LoadAllExistingTextures();
}
TextureHandler::~TextureHandler()
{
	for (UINT i = 0; i < m_textures.size(); i++)
	{
		m_textures[i].m_textureBuffer->Release();
	}
}

void TextureHandler::Init(D3D12Framework& framework, bool loadAllTexures)
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

	//m_framework.ResetCommandListAndAllocator(nullptr, L"TextureHandler: Line 42");

	bool isSkybox = true;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::wstring file = entry.path().c_str();
		if (file.find(L"Skybox") != std::wstring::npos)
			CreateTexture(file, isSkybox);
		else
			CreateTexture(file);
	}

	LoadAllCreatedTexuresToGPU();
}

//* This is/should always be called after creating 1 or multiple textures,
void TextureHandler::LoadAllCreatedTexuresToGPU()
{
	DescriptorHeapWrapper* srvWrapper = &m_framework.CbvSrvHeap();
	UINT offset = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(srvWrapper->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	if (!m_resourceBarriers.empty()) {
		m_framework.InitCmdList()->ResourceBarrier(m_resourceBarriers.size(), &m_resourceBarriers[0]);

		HRESULT result;

		cbvSrvHandle.Offset(srvWrapper->m_handleCurrentOffset * srvWrapper->DESCRIPTOR_SIZE());

		for (UINT i = 0; i < m_tempTextures.size(); i++) {
			DirectX::CreateShaderResourceView(
				m_framework.GetDevice(), 
				m_tempTextures[i].m_textureBuffer.Get(),
				cbvSrvHandle,
				m_tempTextures[i].m_cubeMap);

			// Offset the descriptor and save the offset value for later use in rendering.
			m_tempTextures[i].m_offsetID = srvWrapper->m_handleCurrentOffset;
			cbvSrvHandle.Offset(srvWrapper->DESCRIPTOR_SIZE());
			srvWrapper->m_handleCurrentOffset++; // += srvWrapper->DESCRIPTOR_SIZE();


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

Material TextureHandler::CreateMaterial(std::string textures[6], float metallic, float roughness, float emissive, float color[4])
{
	std::vector<UINT> IDs = CreateMultipleTextures(&textures[0], 6);

	Material material;
	material.m_albedo	    = IDs[0];
	material.m_normal		= IDs[1];
	material.m_metallicMap  = IDs[2];
	material.m_roughnessMap = IDs[3];
	material.m_heightMap    = IDs[4];
	material.m_aoMap	    = IDs[5];

	material.m_shininess = metallic;
	material.m_roughness = roughness;
	material.m_emissvie  = emissive;

	for (UINT i = 0; i < 4; i++)
		material.m_color[i] = color[i];
	
	return material;
}

//* OUTDATED INFORMATION
//* Create Texture expects CommandList to have already been resetted
//* before use. LoadAllCreatedTexuresToGPU also needs to be called directly after this,
//* CreateMultipleTextures loads all created textures to the GPU automatically.
UINT TextureHandler::CreateTexture(std::wstring file, bool isCubeMap)
{	
	file = GetCorrectPathAndName(file);
	for (UINT i = 0; i < m_textures.size(); i++)
	{
		if (file == m_textures[i].m_texturePath)
			return i + 1;
	}
		
	Texture texture;
	texture.m_texturePath = file;
	texture.m_cubeMap	  = isCubeMap;
	CD3DX12_RESOURCE_BARRIER resource = LoadTextures(file, &texture.m_textureBuffer, isCubeMap);
	m_resourceBarriers.emplace_back(resource);
	m_tempTextures.emplace_back(texture);

	return m_textures.size() + m_tempTextures.size();
}

UINT TextureHandler::CreateCustomTexture(void* data, const UINT sizeOfData, std::wstring name)
{
	//name = GetCorrectPathAndName(name);
	//for (UINT i = 0; i < m_textures.size(); i++)
	//{
	//	if (name == m_textures[i].m_texturePath)
	//		return i + 1;
	//}

	Texture texture;
	texture.m_texturePath = name;
	texture.m_cubeMap = false;
	CD3DX12_RESOURCE_BARRIER resource = LoadTextures(data, sizeOfData, &texture.m_textureBuffer);
	m_resourceBarriers.emplace_back(resource);
	m_tempTextures.emplace_back(texture);

	return m_textures.size() + m_tempTextures.size() - 1;
}


UINT TextureHandler::GetTexture(std::wstring texturePath)
{
	//texturePath = GetCorrectPathAndName(texturePath);
	for (UINT i = 0; i < m_textures.size(); i++)
	{
		if (texturePath == m_textures[i].m_texturePath)
			return i;
	}

	return 0;
}

std::wstring TextureHandler::GetCorrectPathAndName(std::wstring path)
{
	if (path.empty()) {
		path = L"Sprites/0aDefaultTexture.dds";
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
		path = L"Sprites/0aDefaultTexture.dds";
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
		m_framework.InitCmdList(),
		*textureBuffer,
		uploadBuffer,
		0, 0,
		static_cast<UINT>(subresourcedata.size()),
		subresourcedata.data()
	);

	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		*textureBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// Return resourc barrier so it's possible to upload multiple at the same time
	return transition;
}

CD3DX12_RESOURCE_BARRIER TextureHandler::LoadTextures(void* data, const UINT sizeOfData, ID3D12Resource** textureBuffer)
{
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels		   = 1;
	textureDesc.Format			   = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.Width			   = sizeOfData;
	textureDesc.Height			   = sizeOfData;
	textureDesc.Flags			   = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize   = 1;
	textureDesc.SampleDesc.Count   = 1;
	textureDesc.SampleDesc.Quality = 1;
	textureDesc.Dimension		   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	CD3DX12_HEAP_PROPERTIES heapType(D3D12_HEAP_TYPE_DEFAULT);

	CHECK_HR(m_framework.GetDevice()->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(textureBuffer))
	);

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(*textureBuffer, 0, 1);

	CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ID3D12Resource* uploadBuffer;
	CHECK_HR(m_framework.GetDevice()->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer))
	);

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData	   = data;
	textureData.RowPitch   = (sizeof(float) * 4) * sizeOfData;
	textureData.SlicePitch = textureData.RowPitch * sizeOfData;

	UpdateSubresources(
		m_framework.InitCmdList(),
		*textureBuffer,
		uploadBuffer,
		0, 0,
		1,
		&textureData
	);

	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		*textureBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	return transition;
}
