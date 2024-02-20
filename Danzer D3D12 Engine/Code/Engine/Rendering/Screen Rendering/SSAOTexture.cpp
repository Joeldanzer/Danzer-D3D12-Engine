#include "stdafx.h"
#include "SSAOTexture.h"

#include "Core/WindowHandler.h"
#include "Core/D3D12Framework.h"
#include "Core/MathDefinitions.h"
#include "Rendering/TextureHandler.h"

#include <random>

void SSAOTexture::InitializeSSAO(D3D12Framework& framework, TextureHandler& textureHandler, const UINT numberOfSamples, const UINT noiseSize)
{
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine rng;

	std::vector<Vect3f> kernelSamples;
	kernelSamples.reserve(numberOfSamples);
	for (UINT i = 0; i < numberOfSamples; i++)
	{
		Vect3f sample = {
			randomFloats(rng) * 2.0f - 1.0f,
			randomFloats(rng) * 2.0f - 1.0f,
			randomFloats(rng)
		};

		sample.Normalize();
		sample *= randomFloats(rng);

		float scale = (float)i / numberOfSamples;
		scale = Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		kernelSamples.emplace_back(sample);
	}

	std::vector<Vect3f> ssaoNoise;
	ssaoNoise.reserve(noiseSize);
	for (UINT i = 0; i < noiseSize; i++)
	{
		Vect3f noise = {
			randomFloats(rng) * 2.0f - 1.0f,
			randomFloats(rng) * 2.0f - 1.0f,
			0.0f 
		};
		ssaoNoise.emplace_back(noise);
	}
	
	m_textureID = textureHandler.CreateCustomTexture(&ssaoNoise[0], ssaoNoise.size() * sizeof(Vect3f), L"ssaoNoise");
	SetBufferData(framework, kernelSamples);
}

void SSAOTexture::RenderTexture(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& handle, const UINT frameIndex)
{

}

void SSAOTexture::RenderSSAO(ID3D12GraphicsCommandList* cmdList, DescriptorHeapWrapper& srvWrapper, TextureHandler& textureHandler, const UINT frameIndex)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = srvWrapper.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	const UINT cbvSrvDescSize = srvWrapper.DESCRIPTOR_SIZE();

	m_bufferOne.UpdateBuffer(&m_bufferDataOne, frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE bufferHandleOne(cbvSrvHeapStart, m_bufferOne.OffsetID(), cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(1, bufferHandleOne);

	m_bufferTwo.UpdateBuffer(&m_bufferDataTwo, frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE bufferHandleTwo(cbvSrvHeapStart, m_bufferTwo.OffsetID(), cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(2, bufferHandleTwo);

	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvHeapStart, textureHandler.GetTextureData(m_textureID).m_offsetID, cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(6, srvHandle);

	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawInstanced(3, 1, 0, 0);
}

void SSAOTexture::SetBufferData(D3D12Framework& framework, const std::vector<Vect3f>& samples)
{
	for (UINT i = 0; i < 32; i++) {
		m_bufferDataOne.m_samples[i] = samples[i];
		m_bufferDataTwo.m_samples[i] = samples[i + 32];
	}
	
	m_bufferDataOne.m_noiseScale = { WindowHandler::WindowData().m_w / sqrtf((float)samples.size()), WindowHandler::WindowData().m_h / sqrtf((float)samples.size()) };
	m_bufferOne.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_bufferOne.FetchData(), sizeof(SSAOBuffer::Data));
	m_bufferTwo.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_bufferTwo.FetchData(), sizeof(SSAOBuffer::Data));
}
