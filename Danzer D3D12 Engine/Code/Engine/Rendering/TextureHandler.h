#pragma once
#include "Core/D3D12Header.h"
#include "DirectX/include/directx/d3dx12.h"

#include "Models/Material.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <array>

class D3D12Framework;

struct Texture {
	std::wstring m_texturePath;
	bool m_cubeMap = false;
	UINT m_offsetID = 0;

	ComPtr<ID3D12Resource> m_textureBuffer;
};

class TextureHandler
{
public:
	static constexpr uint16_t MaxMipLevels = 6;
	static constexpr uint16_t MinMipLevels = 1;
	static constexpr uint16_t BaseMipLevel = 4;

	TextureHandler() = delete;
	TextureHandler(D3D12Framework& framework);
	~TextureHandler();

	void Init(D3D12Framework& framework, bool loadAllTexures = false);

	void LoadAllExistingTextures();
	void LoadAllCreatedTexuresToGPU();

	std::vector<UINT> CreateMultipleTextures(std::string* paths, UINT numOfTextures);

	Material CreateMaterial(std::string textures[3], float metallic, float m_roughness, float m_emissive, float color[4]);
	UINT CreateTexture(std::wstring file, bool isCubeMap = false);
	UINT CreateCustomTexture(void* data, const UINT sizeOfData, std::wstring name);
	UINT GetTexture(std::wstring texturePath);

	std::vector<Texture>& GetTextures() { return m_textures; }
	Texture& GetTextureData(UINT texture) {
		texture = texture > 0 && texture < m_textures.size() ? texture : 0;
		return m_textures[texture];
	}
	//ID3D12DescriptorHeap* GetDescriptorHeap(UINT id) { return m_textures[id].m_textureDescriptorHeap.Get(); }
	std::wstring GetCorrectPathAndName(std::wstring path);

private:
	//UINT TextureExists(std::wstring file);

	CD3DX12_RESOURCE_BARRIER LoadTextures(std::wstring file, ID3D12Resource** textureBuffer, bool isCubeMap);
	CD3DX12_RESOURCE_BARRIER LoadTextures(void* data, const UINT sizeOfData, ID3D12Resource** textureBuffer);

	std::unordered_map<uint32_t, std::wstring> m_textureMap;
	std::vector<Texture> m_textures;
	std::vector<Texture> m_tempTextures;

	std::vector<CD3DX12_RESOURCE_BARRIER> m_resourceBarriers;	
	D3D12Framework& m_framework;
};

