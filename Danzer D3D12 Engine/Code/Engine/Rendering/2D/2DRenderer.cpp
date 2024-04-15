#include "stdafx.h"
#include "2DRenderer.h"

#include "Rendering/PSOHandler.h"
#include "Core/FrameResource.h"
#include "../VertexAndTextures.h"
#include "../../Core/WindowHandler.h"
#include "../../Core/D3D12Framework.h"
#include "../RenderUtility.h"

Renderer2D::~Renderer2D(){}

void Renderer2D::Init(D3D12Framework& framework, PSOHandler& psoHandler)
{
	DXGI_FORMAT format[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	CD3DX12_DEPTH_STENCIL_DESC depth(D3D12_DEFAULT);
	depth.DepthEnable = false;
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				 D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	m_rs  = psoHandler.CreateRootSignature(1, 1, PSOHandler::SAMPLER_DESC_CLAMP, flags, L"Renderer 2D Root Signature");
	m_pso = psoHandler.CreatePSO(
		{L"Shaders/uiVertexShader.cso", L"Shaders/uiPixelShader.cso"},
		psoHandler.BlendDescs(PSOHandler::BLEND_TRANSPARENT),
		psoHandler.RastDescs(PSOHandler::RASTERIZER_BACK),
		depth,
		DXGI_FORMAT_UNKNOWN,
		&format[0],
		1,
		m_rs,
		PSOHandler::INPUT_LAYOUT_INSTANCE_SPRITE_2D,
		L"Renderer 2D PSO"
	);

	m_framework = &framework;
	
	m_windowBuffer.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_windowBuffer.FetchData(), sizeof(WindowBuffer::Data));
	m_spriteSheetBuffer.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_spriteSheetBuffer.FetchData(), sizeof(WindowBuffer::Data));
	CreateUIVertexAndIndexBuffers(framework);
}

void Renderer2D::UpdateDefaultUIBuffers(UINT frameIndex)
{
	//WindowBuffer::Data data;
	//data.m_windowSize.x = (float)WindowHandler::GetWindowData().m_width  / 2;
	//data.m_windowSize.y = (float)WindowHandler::GetWindowData().m_height / 2;
	//
	//m_windowBuffer.UpdateBuffer(frameIndex, &data);
	
	//ID3D12DescriptorHeap* descHeaps[] = { m_windowBuffer.GetDescriptorHeap(frameIndex) };
	//m_commandList->SetDescriptorHeaps(_countof(descHeaps), &descHeaps[0]);
	//m_commandList->SetGraphicsRootDescriptorTable(0, m_windowBuffer.GetDescriptorHeap(frameIndex)->GetGPUDescriptorHandleForHeapStart());
}

void Renderer2D::RenderUI(ID3D12GraphicsCommandList* cmdList, std::vector<SpriteData>& sprites, UINT frameIndex, std::vector<TextureHandler::Texture> textures)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_framework->CbvSrvHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize					= m_framework->CbvSrvHeap().DESCRIPTOR_SIZE();

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	cmdList->IASetIndexBuffer(&m_indexBufferView);

	WindowBuffer::Data data;
	data.m_windowSize.x = (float)WindowHandler::WindowData().m_w / 2.f;
	data.m_windowSize.y = (float)WindowHandler::WindowData().m_h / 2.f;

	m_windowBuffer.UpdateBuffer(&data, frameIndex);

	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cbvSrvHeapStart, m_windowBuffer.OffsetID() + frameIndex, cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);

	for (auto& sprite : sprites) {
		const SpriteData::Sheet sheet = sprite.GetSheet();

		sprite.UpdateInstanceBuffer(frameIndex);
		
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvHeapStart, textures[sheet.m_texture].m_offsetID, cbvSrvDescSize);
		cmdList->SetGraphicsRootDescriptorTable(1, srvHandle);
	
		cmdList->IASetVertexBuffers(1, 1, &sprite.GetInstanceBuffer().GetBufferView(frameIndex));
		cmdList->DrawIndexedInstanced(6, sprite.GetInstances().size(), 0, 0, 0);
	}
}

void Renderer2D::RenderFontUI(ID3D12GraphicsCommandList* cmdList, std::vector<Font>& fonts, UINT frameIndex, std::vector<TextureHandler::Texture> textures)
{
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	for (auto& font : fonts) {
		const Font::Data& fontData = font.GetData();
	
		if (!font.GetInstances().empty()) {
	
			//m_uiBuffer.UpdateBuffer(sheet.m_instances, frameIndex);
			font.UpdateInstanceBuffer(frameIndex);
			cmdList->IASetIndexBuffer(&m_indexBufferView);
			cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	
			//ID3D12DescriptorHeap* descHeaps[] = { textures[fontData.m_texture - 1].m_textureDescriptorHeap.Get() };
			//m_commandList->SetDescriptorHeaps(_countof(descHeaps), &descHeaps[0]);
			//m_commandList->SetGraphicsRootDescriptorTable(1, descHeaps[0]->GetGPUDescriptorHandleForHeapStart());
	
			cmdList->DrawInstanced(6, font.GetInstances().size(), 0, 0);
		}
	}
}

void Renderer2D::CreateUIVertexAndIndexBuffers(D3D12Framework& framework)
{
	//framework.ResetCommandListAndAllocator(nullptr, L"Renderer2D: Line 78");
	std::array<Vertex, 6> verticies = {
		// Position						           UV
		Vertex{ Vect4f(-(1.f),  (1.f), 0.f, 1.f), Vect2f(0.f, 0.f)},
		Vertex{ Vect4f((1.f),  -(1.f), 0.f, 1.f), Vect2f(1.f, 1.f)},
		Vertex{ Vect4f(-(1.f), -(1.f), 0.f, 1.f), Vect2f(0.f, 1.f)},
		Vertex{ Vect4f(-(1.f),  (1.f), 0.f, 1.f), Vect2f(0.f, 0.f)},
		Vertex{ Vect4f((1.f),   (1.f), 0.f, 1.f), Vect2f(1.f, 0.f)},
		Vertex{ Vect4f((1.f),  -(1.f), 0.f, 1.f), Vect2f(1.f, 1.f)}
	};

	std::array<UINT, 6> indices = {
		 0,  1,  2,  3,  4,  5
	};

	VertexIndexBufferInfo bufferInfo = GetIndexAndVertexBuffer(L"Default",verticies.size() * sizeof(Vertex),
		indices.size() * sizeof(UINT), framework.GetDevice());

	CD3DX12_RESOURCE_BARRIER resourceBarriers[2];

	// Update subresource data to vertex buffer and add the data to a buffer view
	{
		m_vertexBuffer = bufferInfo.m_vBuffer;

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<UINT*>(verticies.data());
		vertexData.RowPitch = sizeof(Vertex) * verticies.size();
		vertexData.SlicePitch = vertexData.RowPitch;

		UpdateSubresources(framework.InitCmdList(), m_vertexBuffer.Get(), bufferInfo.m_vBufferUpload, 0, 0, 1, &vertexData);
		resourceBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	// Update subresource data to index buffer and add the data to a buffer view
	{
		m_indexBuffer = bufferInfo.m_iBuffer;

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<UINT*>(indices.data());
		indexData.RowPitch = sizeof(unsigned int) * indices.size();
		indexData.SlicePitch = indexData.RowPitch;

		UpdateSubresources(m_framework->InitCmdList(), m_indexBuffer.Get(), bufferInfo.m_iBufferUpload, 0, 0, 1, &indexData);
		resourceBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
	}

	m_framework->InitCmdList()->ResourceBarrier(2, &resourceBarriers[0]);

	//framework.ExecuteCommandList();

	{
		m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
		m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_indexBufferView.SizeInBytes = sizeof(unsigned int) * indices.size();

		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes = sizeof(Vertex) * verticies.size();;
	}
}
