#pragma once
#include "Core/D3D12Header.h"
//#include "../3rdParty/DirectX-Headers-main/include/directx/d3d12.h"
#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"

#include "Models/Material.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <array>

class DirectX12Framework;

class TextureHandler
{
public:
	TextureHandler() = delete;
	TextureHandler(DirectX12Framework& framework);
	~TextureHandler();

	void Init(DirectX12Framework& framework, bool loadAllTexures = false);

	struct Texture {
		std::wstring m_texturePath;
		bool m_cubeMap = false;

		ComPtr<ID3D12Resource>		 m_textureBuffer;
		ComPtr<ID3D12DescriptorHeap> m_textureDescriptorHeap;
	};

	void LoadAllExistingTextures();
	void LoadAllCreatedTexuresToGPU();

	std::vector<UINT> CreateMultipleTextures(std::string* paths, UINT numOfTextures);
	//std::array<UINT, 3> CreateMultipleTextures(std::string paths[3]);

	Material CreateMaterial(std::string textures[3], float metallic, float m_roughness, float m_emissive, float color[4]);
	UINT CreateTexture(std::wstring file, bool isCubeMap = false);
	UINT GetTexture(std::wstring texturePath);

	std::vector<Texture>& GetTextures() { return m_textures; }
	Texture& GetTextureData(UINT texture) {
		texture = texture > 0 ? texture : 1;
		return m_textures[texture - 1];
	}
	ID3D12DescriptorHeap* GetDescriptorHeap(UINT id) { return m_textures[id].m_textureDescriptorHeap.Get(); }
	std::wstring GetCorrectPathAndName(std::wstring path);

private:

	CD3DX12_RESOURCE_BARRIER LoadTextures(std::wstring file, ID3D12Resource** textureBuffer, bool isCubeMap);

	std::vector<Texture> m_textures;
	std::vector<Texture> m_tempTextures;

	std::vector<CD3DX12_RESOURCE_BARRIER> m_resourceBarriers;	
	DirectX12Framework& m_framework;
};

