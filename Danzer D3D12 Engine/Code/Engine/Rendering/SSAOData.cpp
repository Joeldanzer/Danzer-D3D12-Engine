#include "stdafx.h"
#include "SSAOData.h"

#include "TextureHandler.h"
#include "Buffers/BufferHandler.h"

void SSAOData::GenerateRandomTexture(TextureHandler& textureHandler, BufferHandler& bufferHandler, const uint16_t numberOfSamples, const uint16_t noiseSize)
{
	std::uniform_real_distribution<float> randomFloats(-1.0, 1.0);
	std::default_random_engine rng;

	//std::vector<Vect4f> m_kernelSamples;
	m_kernelSamples.reserve(numberOfSamples);
	for (UINT i = 0; i < numberOfSamples; i++)
	{
		Vect4f sample = {
			randomFloats(rng),
			randomFloats(rng),
			randomFloats(rng),
			1.0f
		};

		sample.Normalize();
		sample *= randomFloats(rng);

		float scale = (float)i / numberOfSamples;
		scale = Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		m_kernelSamples.emplace_back(sample);
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

	// Generate a custom texture and fetch the offsetID for texture slotting.
	const uint32_t id = textureHandler.CreateCustomTexture(&ssaoNoise[0], static_cast<UINT>(sqrt(ssaoNoise.size())), L"ssaoNoise");
	textureHandler.LoadAllCreatedTexuresToGPU();
	m_textureOffset = textureHandler.GetTextureData(id).m_offsetID;

	// Samples is not gonna update so we only need to set the buffer data once.
	ConstantBufferData* sampleBuffer = bufferHandler.CreateBufferData(sizeof(Vect4f) * m_kernelSamples.size());
	sampleBuffer->UpdateBufferData(reinterpret_cast<uint16_t*>(&m_kernelSamples[0]));
	m_samplesBufferOffset = sampleBuffer->OffsetID();

	ConstantBufferData* dataBuffer = bufferHandler.CreateBufferData(sizeof(Data));
	m_data = {
		static_cast<uint32_t>(m_kernelSamples.size()),
		{WindowHandler::WindowData().m_w / sqrtf((float)m_kernelSamples.size()), WindowHandler::WindowData().m_h / sqrtf((float)m_kernelSamples.size())}
	};
	dataBuffer->UpdateBufferData(reinterpret_cast<uint16_t*>(&m_data));
	m_countBufferOffset = dataBuffer->OffsetID();
}
