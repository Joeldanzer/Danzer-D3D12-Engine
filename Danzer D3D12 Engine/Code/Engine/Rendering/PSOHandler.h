#pragma once
#include "../Core/D3D12Header.h"

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"
#include <array>

class  D3D12Framework;
struct ID3D12Device;

class PSOHandler
{
public:
	enum INPUT_LAYOUTS {
		INPUT_LAYOUT_NONE,
		INPUT_LAYOUT_INSTANCE_FORWARD,
		INPUT_LAYOUT_INSTANCE_DEFFERED,
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

	enum BLEND_DESC {
		BLEND_DEFAULT,
		BLEND_TRANSPARENT,
		BLEND_COUNT
	};

	enum RASTERIZER_DESC {
		RASTERIZER_BACK,
		RASTERIZER_FRONT,
		RASTERIZER_NONE,
		RASTERIZER_COUNT
	};


	PSOHandler(D3D12Framework& framework);

	UINT CreateRootSignature(const UINT numberOfCBV, const UINT numberOfSRV, SAMPLER_DESCS sampler, D3D12_ROOT_SIGNATURE_FLAGS flags, std::wstring name);
	UINT CreatePSO(
		std::array<std::wstring, 2> shaderName, 
		D3D12_BLEND_DESC            blend, 
		D3D12_RASTERIZER_DESC		rast, 
		D3D12_DEPTH_STENCIL_DESC	depth,
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

	std::array<std::vector<D3D12_INPUT_ELEMENT_DESC>, INPUT_LAYOUTS::INPUT_LAYOUT_COUNT> m_inputLayouts;
	std::array<D3D12_STATIC_SAMPLER_DESC, SAMPLER_DESCS::SAMPLER_DESC_COUNT>		     m_samplerDescs;
	std::array<CD3DX12_BLEND_DESC, BLEND_DESC::BLEND_COUNT>								 m_blendDescs;
	std::array<CD3DX12_RASTERIZER_DESC, RASTERIZER_DESC::RASTERIZER_COUNT>			     m_rastDescs;

	ID3D12Device* m_device;
};

