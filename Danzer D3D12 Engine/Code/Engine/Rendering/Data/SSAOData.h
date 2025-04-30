#pragma once

class TextureHandler;
class BufferHandler;

class SSAOData
{
public:
	SSAOData() : m_countBufferOffset(UINT32_MAX), m_textureID(UINT32_MAX), m_samplesBufferOffset(UINT32_MAX) {}
	void GenerateRandomTexture(TextureHandler& textureHandler, BufferHandler& bufferHandler, const uint16_t numberOfSamples, const uint16_t noiseSize);

	const uint32_t NoiseTextureID() {
		return m_textureID;
	}
	const uint32_t GetBufferOffset() {
		return m_samplesBufferOffset;
	}
	const uint32_t GetCountOffset() {
		return m_countBufferOffset;
	}


private:
	struct Data {
		uint32_t m_count;
		Vect2f   m_noiseScale;
		float m_trashOne;
		float m_trashTwo[15];
	}m_data;

	std::vector<Vect4f> m_kernelSamples;
	std::vector<Vect4f> m_ssaoNoise;

	uint32_t m_textureID;
	uint32_t m_samplesBufferOffset;
	uint32_t m_countBufferOffset;
};

