#pragma once

class TextureHandler;
class BufferHandler;

class SSAOData
{
public:
	SSAOData() : m_countBufferOffset(UINT32_MAX), m_textureOffset(UINT32_MAX), m_samplesBufferOffset(UINT32_MAX) {}
	void GenerateRandomTexture(TextureHandler& textureHandler, BufferHandler& bufferHandler, const uint16_t numberOfSamples, const uint16_t noiseSize);

	const uint32_t GetTextureOffset() {
		return m_textureOffset;
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

	uint32_t m_textureOffset;
	uint32_t m_samplesBufferOffset;
	uint32_t m_countBufferOffset;
};

