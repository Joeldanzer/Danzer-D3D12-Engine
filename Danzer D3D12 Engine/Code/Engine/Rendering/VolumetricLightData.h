#pragma once

class ConstantBufferData;
class BufferHandler;

class VolumetricLightData
{
public:
	VolumetricLightData(const uint32_t steps = 100, const float scattering = 0.35f, const float strength = 1.5f) :
		m_data({
			steps,
			scattering,
			strength
		}),
		m_constantBuffer(nullptr)
	{}

	void InitializeConstantBuffer(BufferHandler& bufferHandler);

	void SetNewData(const uint32_t steps, const float scattering, const float strength);

	ConstantBufferData* GetBuffer() {
		return m_constantBuffer;
	}

private:
	struct Data {
		uint32_t m_numberOfSteps;
		float	 m_gScattering;
		float    m_scatteringStrength;
	} m_data;

	ConstantBufferData* m_constantBuffer;

};

