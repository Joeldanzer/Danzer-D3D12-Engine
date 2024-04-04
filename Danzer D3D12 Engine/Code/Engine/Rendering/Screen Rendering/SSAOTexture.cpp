#include "stdafx.h"
#include "SSAOTexture.h"

#include "Core/WindowHandler.h"
#include "Core/D3D12Framework.h"
#include "Core/MathDefinitions.h"
#include "Rendering/TextureHandler.h"

#include <random>

void SSAOTexture::InitializeSSAO(D3D12Framework& framework, TextureHandler& textureHandler, const UINT numberOfSamples, const UINT noiseSize)
{
	std::uniform_real_distribution<float> randomFloats(-1.0, 1.0);
	std::default_random_engine rng;

	std::vector<Vect3f> kernelSamples;
	kernelSamples.reserve(numberOfSamples);
	for (UINT i = 0; i < numberOfSamples; i++)
	{
		Vect3f sample = {
			randomFloats(rng),
			randomFloats(rng),
			randomFloats(rng)
		};

		sample.Normalize();
		sample *= randomFloats(rng);

		float scale = (float)i / numberOfSamples;
		scale = Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		kernelSamples.emplace_back(sample);
	}

	std::vector<Vect4f> ssaoNoise;
	ssaoNoise.reserve(noiseSize);
	for (UINT i = 0; i < noiseSize; i++)
	{
		Vect4f noise = {
			randomFloats(rng),
			randomFloats(rng),
			randomFloats(rng),
			1.0f
		};
		ssaoNoise.emplace_back(noise);
	}
	
	m_textureID = textureHandler.CreateCustomTexture(&ssaoNoise[0], static_cast<UINT>(sqrt(ssaoNoise.size())), L"ssaoNoise");
	textureHandler.LoadAllCreatedTexuresToGPU();
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

	m_noiseScaleBuffer.UpdateBuffer(&m_noiseScaleData, frameIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE noiseScaleHandle(cbvSrvHeapStart, m_noiseScaleBuffer.OffsetID(), cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(3, noiseScaleHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cbvSrvHeapStart, textureHandler.GetTextureData(m_textureID).m_offsetID, cbvSrvDescSize);
	cmdList->SetGraphicsRootDescriptorTable(7, srvHandle);

	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawInstanced(3, 1, 0, 0);
}

void SSAOTexture::SetBufferData(D3D12Framework& framework, const std::vector<Vect3f>& samples)
{
	for (UINT i = 0; i < 32; i++) {
		m_bufferDataOne.m_samples[i] = { samples[i].x, samples[i].y, samples[i].z, 1.0f };
		m_bufferDataTwo.m_samples[i] = { samples[i + 32].x, samples[i + 32].y, samples[i + 32].z, 1.0f};
	}	
	m_noiseScaleData.m_windowSize = { WindowHandler::WindowData().m_w / sqrtf((float)samples.size()), WindowHandler::WindowData().m_h / sqrtf((float)samples.size()) };
	
	m_noiseScaleBuffer.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_noiseScaleBuffer.FetchData(), sizeof(Vect2f));
	m_bufferOne.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_bufferOne.FetchData(), sizeof(SSAOBuffer::Data));
	m_bufferTwo.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_bufferTwo.FetchData(), sizeof(SSAOBuffer::Data));
}
