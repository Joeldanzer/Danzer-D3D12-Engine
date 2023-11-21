#pragma once
#include "../Core/D3D12Header.h"

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"
#include <array>

class D3D12Framework;
struct ID3D12Device;

enum PIPELINE_STATES {
	PIPELINE_STATE_MODELS,
	PIPELINE_STATE_GBUFFER,
	PIPELINE_STATE_DIRECTIONAL_LIGHT,
	PIPELINE_STATE_TRANSPARENT,
	PIPELINE_STATE_UI,
	PIPELINE_STATE_FONT,
	PIPELINE_STATE_SKYBOX,
	// Only visible in Debug
	PIPELINE_STATE_AABB_WIREFRAME,
	PIPELINE_STATE_RAY_WIREFRAME,
	PIPELINE_COUNT
};

enum ROOTSINGATURE_STATES {
	ROOTSIGNATURE_STATE_LIGHT,
	ROOTSIGNATURE_STATE_GBUFFER,
	ROOTSIGNATURE_STATE_DEFAULT,
	ROOTSIGNATURE_STATE_UI,
	ROOTSIGNATURE_COUNT
};

class PipelineStateHandler
{
public:
	PipelineStateHandler();
	~PipelineStateHandler();

	void Init(D3D12Framework& framework);

	ID3D12PipelineState* GetPSO(PIPELINE_STATES state) { return m_PSObjects[state].Get(); }
	ID3D12RootSignature* GetRootSignature(ROOTSINGATURE_STATES state) { return m_rootSignatures[state].Get(); }

private:
	// Pre made InputLayouts. 
	enum INPUT_LAYOUTS {
		INPUT_LAYOUT_INSTANCE_MODEL,
		INPUT_LAYOUT_INSTANCE_GBUFFER,
		INPUT_LAYOUT_INSTANCE_SPRITE_2D,
		INPUT_LAYOUT_INSTANCE_FONT_2D,
		INPUT_LAYOUT_INSTANCE_AABB,
		INPUT_LAYOUT_INSTANCE_RAY,
		INPUT_LAYOUT_COUNT
	};

	enum SAMPLER_DESCS {
		SAMPLER_DESC_WRAP,
		SAMPLER_DESC_BORDER,
		SAMPLER_DESC_CLAMP,
		SAMPLER_DESC_COUNT
	};

	void CreateLightRootSignature(ID3D12Device* device);
	void CreateGBufferRootSingature(ID3D12Device* device);
	void CreateDefaultRootSingature(ID3D12Device* device);
	void CreateUIRootSignature(ID3D12Device* device);

	void CreateUIPSO(ID3D12Device* device);
	void CreateFontPSO(ID3D12Device* device);
	void CreateSkyboxPSO(ID3D12Device* device);
	void CreateGBufferPSO(ID3D12Device* device);
	void CreateTransparentPSO(ID3D12Device* device);
	void CreateAABBWireframePSO(ID3D12Device* device);
	void CreateRayWireframePSO(ID3D12Device* device);
	void CreateDirectionalLightPSO(ID3D12Device* device);
	void CreateModelPSO(ID3D12Device* framework); 
	

	void InitializeSamplerDescs();
	void InitializeInputLayouts();

	std::array<ComPtr<ID3D12PipelineState>, PIPELINE_COUNT> m_PSObjects;
	std::array<ComPtr<ID3D12RootSignature>, ROOTSIGNATURE_COUNT> m_rootSignatures;

	std::array<std::vector<D3D12_INPUT_ELEMENT_DESC>, INPUT_LAYOUT_COUNT> m_inputLayouts;
	std::array<D3D12_STATIC_SAMPLER_DESC, SAMPLER_DESC_COUNT> m_samplerDescs;
};

