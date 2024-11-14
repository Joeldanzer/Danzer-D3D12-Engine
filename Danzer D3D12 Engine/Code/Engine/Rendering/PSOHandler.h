#pragma once
#include "../Core/D3D12Header.h"

#include "DirectX/include/directx/d3dx12.h"
#include <array>

class  D3D12Framework;
struct ID3D12Device;

class PSOHandler
{
public:
	enum INPUT_LAYOUTS :  uint8_t {
		IL_NONE,
		IL_INSTANCE_FORWARD,
		IL_INSTANCE_DEFFERED,
		IL_INSTANCE_SPRITE_2D,
		IL_INSTANCE_FONT_2D,
		IL_INSTANCE_RAY,
		IL_COUNT
	};

	enum SAMPLER_DESCS : uint8_t {
		SAMPLER_WRAP,
		SAMPLER_BORDER,
		SAMPLER_CLAMP,
		SAMPLER_COUNT
	};

	enum BLEND_DESC : uint8_t {
		BLEND_DEFAULT,
		BLEND_TRANSPARENT,
		BLEND_COUNT
	};

	enum RASTERIZER_DESC : uint8_t {
		RASTERIZER_DEFAULT,
		RASTERIZER_BACK,
		RASTERIZER_FRONT,
		RASTERIZER_NONE,
		RASTERIZER_COUNT
	};

	PSOHandler(D3D12Framework& framework);

	UINT CreateRootSignature(const UINT numberOfCBV, const UINT numberOfSRV, SAMPLER_DESCS sampler, D3D12_ROOT_SIGNATURE_FLAGS flags, std::wstring name);
	UINT CreatePSO(
		std::array<std::wstring, 2> shaderName, 
		BLEND_DESC				    blend, 
		RASTERIZER_DESC				rast, 
		D3D12_DEPTH_STENCIL_DESC	depth,
		DXGI_FORMAT					depthFormat,
		DXGI_FORMAT*				rtvFormats,
		const UINT					rtvCount,
		const UINT					rootSignature,
		INPUT_LAYOUTS				layout,
		std::wstring				name
	);

	ID3D12PipelineState* GetPipelineState(const UINT psoIndex) {
		if (psoIndex >= m_pipelineStates.size())
			return nullptr;
		return m_pipelineStates[psoIndex].Get();
	}
	ID3D12RootSignature* GetRootSignature(const UINT rsIndex) {
		if (rsIndex >= m_rootSignatures.size())
			return nullptr;
		return m_rootSignatures[rsIndex].Get();
	}

	CD3DX12_BLEND_DESC BlendDescs(BLEND_DESC blend) {
		return m_blendDescs[blend];
	}
	CD3DX12_RASTERIZER_DESC RastDescs(RASTERIZER_DESC rast) {
		return m_rastDescs[rast];
	}

private:
	void InitializeSamplerDescs();
	void InitializeInputLayouts();
	void InitializeBlendDescs();
	void InitializeRastDescs();

	std::vector<ComPtr<ID3D12PipelineState>> m_pipelineStates;
	std::vector<ComPtr<ID3D12RootSignature>> m_rootSignatures;

	std::array<std::vector<D3D12_INPUT_ELEMENT_DESC>, INPUT_LAYOUTS::IL_COUNT> m_inputLayouts;
	std::array<D3D12_STATIC_SAMPLER_DESC, SAMPLER_DESCS::SAMPLER_COUNT>		   m_samplerDescs;
	std::array<CD3DX12_BLEND_DESC, BLEND_DESC::BLEND_COUNT>					   m_blendDescs;
	std::array<CD3DX12_RASTERIZER_DESC, RASTERIZER_DESC::RASTERIZER_COUNT>	   m_rastDescs;

	ID3D12Device* m_device;
};

