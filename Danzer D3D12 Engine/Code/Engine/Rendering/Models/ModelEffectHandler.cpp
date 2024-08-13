#include "stdafx.h"
#include "ModelEffectHandler.h"
#include "Rendering/Screen Rendering/GBuffer.h"
#include "Rendering/PSOHandler.h"
#include "Core/WindowHandler.h"

#include "DirectX/include/directx/d3dx12.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>


ModelEffectHandler::ModelEffectHandler(D3D12Framework& framework, PSOHandler& psoHandler)
{
	m_framework = &framework;
	m_psoHandler = &psoHandler;

	CD3DX12_RESOURCE_DESC dtDesc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		WindowHandler::WindowData().m_w,
		WindowHandler::WindowData().m_h,
		1,
		1,
		DXGI_FORMAT_R32_FLOAT,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_NONE
	);

	CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_DEFAULT);

	CHECK_HR(m_framework->GetDevice()->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&dtDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&m_depthTexture)
	));

	// Copy Depth Texture used in Model Effect Rendering
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(framework.CbvSrvHeap().GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(framework.CbvSrvHeap().m_handleCurrentOffset * framework.CbvSrvHeap().DESCRIPTOR_SIZE());
	m_framework->GetDevice()->CreateShaderResourceView(m_depthTexture.Get(), nullptr, handle);
	m_depthTexture->SetName(L"Model Effect Depth Texture");
	m_depthTextureOffset = framework.CbvSrvHeap().m_handleCurrentOffset;
	framework.CbvSrvHeap().m_handleCurrentOffset++;
}
ModelEffectHandler::~ModelEffectHandler(){
	m_framework  = nullptr;
	m_psoHandler = nullptr;
	m_modelEffects.clear();
}

ModelEffect ModelEffectHandler::CreateModelEffect(std::array<std::wstring, 2> shaders, const UINT model, void* bufferData, const UINT sizeOfData, std::vector<UINT> textures, bool transparent)
{	
	m_modelEffects.emplace_back(ModelEffectData(model, textures, bufferData ? true : false, *m_framework));
	ModelEffectData& effectData = m_modelEffects[m_modelEffects.size() - 1];

	if (bufferData) {
		memcpy(&effectData.m_bufferData, bufferData, sizeOfData);
	}
	
	effectData.m_rs  = m_psoHandler->CreateRootSignature(2, (UINT)textures.size() + 1, PSOHandler::SAMPLER_DESC_WRAP, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, L"Model Effect RS: " + std::to_wstring(m_modelEffects.size() - 1));
	
	std::array<DXGI_FORMAT, GBUFFER_COUNT> formats = GBuffer::GBufferFormats();
	effectData.m_pso = m_psoHandler->CreatePSO(
		shaders,
		transparent ? m_psoHandler->BlendDescs(PSOHandler::BLEND_TRANSPARENT) : m_psoHandler->BlendDescs(PSOHandler::BLEND_DEFAULT),
		transparent ? m_psoHandler->RastDescs(PSOHandler::RASTERIZER_NONE) : m_psoHandler->RastDescs(PSOHandler::RASTERIZER_FRONT),
		CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
		DXGI_FORMAT_D32_FLOAT,
		&formats[0],
		GBUFFER_COUNT,
		effectData.m_rs,
		PSOHandler::INPUT_LAYOUT_INSTANCE_DEFFERED,
		L"Model Effect PSO: " + std::to_wstring(m_modelEffects.size() - 1)
	);

	return ModelEffect(UINT(m_modelEffects.size()));
}
