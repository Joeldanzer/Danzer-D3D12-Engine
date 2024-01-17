#pragma once
#include <vector>
#include "Core/D3D12Header.h"
#include "../3rdParty/DirectX-Headers-main/include/directx/d3d12.h"
#include "Core/MathDefinitions.h"

class ModelEffectData {
public:
	ModelEffectData()  = delete;
	~ModelEffectData() = default;
	explicit ModelEffectData(const ModelEffectData&) = default;
	explicit ModelEffectData(const UINT model, std::vector<UINT> textures) : 
		m_model(model), m_textures(textures)
	{}

	ID3D12PipelineState* GetEffectPSO(){
		return m_pipelineState.Get();
	}
	ID3D12RootSignature* GetEffectRSO() {
		return m_rootSignature.Get();
	}
	const UINT ModelID() {
		return m_model;
	}

	const std::vector<UINT>& GetTextures() {
		return m_textures;
	}

	std::vector<Mat4f>& GetTransforms() {
		return m_transforms;
	}
private:
	friend class ModelEffectHandler; //

	UINT m_model;
	std::vector<UINT> m_textures;
	std::vector<Mat4f> m_transforms;

	ComPtr<ID3D12PipelineState> m_pipelineState; // Holds all the information that will be sent to shader
	ComPtr<ID3D12RootSignature> m_rootSignature;
};