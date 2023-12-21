#pragma once
#include <vector>
#include "Core/D3D12Header.h"
#include "DirectX-Headers-main/include/directx/d3d12.h"

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

private:
	friend class ModelEffectHandler; //

	UINT m_model;
	std::vector<UINT> m_textures;

	ComPtr<ID3D12PipelineState> m_pipelineState; // Holds all the information that will be sent to shader
	ComPtr<ID3D12RootSignature> m_rootSignature;
};